package main;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import org.w3c.dom.Document;

import com.sun.jna.Library;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

import calling.interfaces.Call;
import calling.return_package.*;

import consts.Constants;

interface LibDSP extends Library {
    LibDSP INSTANCE = (LibDSP) Native.load("aqua", LibDSP.class);

    int aquaCreateServerConnectInfo();

    int aquaCreateServerCallInfo();

    void aquaInstallV2(
            int connectInfoHandle,
            int callInfoHandle,
            String strId,
            String version,
            int callQType);

    void sendReturnV2(
            int p_ReturnInfoHandle,
            Pointer p_ReturnData);

    void aquaServerSend(int callInfoHandle, int returnInfoHandle, Pointer data, int size);

    int receiveConnectRequestV2(int p_ConnectInfoHandle);

    void aquaServerRecv(Pointer data, int callInfoHandle);

    int getReturnInfoQType(int p_ReturnInfoHandle);

    void receiveDisconnectRequest(int p_ConnectInfoHandle);
}

class DisconnectThread extends Thread {
    private final int m_ConnectInfoHandle;

    DisconnectThread(int p_ConnectInfoHandle) {
        m_ConnectInfoHandle = p_ConnectInfoHandle;
    }

    public void run() {
        while (!Thread.currentThread().isInterrupted()) {
            LibDSP.INSTANCE.receiveDisconnectRequest(m_ConnectInfoHandle);
        }
    }
}

class ProcessCallThread1 implements Runnable {
    private final byte[] m_CallData;
    private final int m_ReturnInfoHandle;

    ProcessCallThread1(byte[] p_CallData, int p_ReturnInfoHandle) {
        m_CallData = p_CallData;
        m_ReturnInfoHandle = p_ReturnInfoHandle;
    }

    public void run() {
        try {
            Path xsltPath = Paths.get("transformations/transform_version_v7.xsl");
            byte[] xsltData = Files.readAllBytes(xsltPath);

            String result = mf_GetXmlTransformed(m_CallData, xsltData);

            byte[] resByteArr = result.getBytes(StandardCharsets.UTF_8);

            Call returnData = switch (LibDSP.INSTANCE.getReturnInfoQType(m_ReturnInfoHandle)) {
                case Constants.SMBQ -> new SMBReturn();
                case Constants.EMBQ -> new EMBReturn();
                case Constants.QMBQ -> new QMBReturn();
                case Constants.HMBQ -> new HMBReturn();
                case Constants.MBQ -> new MBReturn();
                case Constants.DMBQ -> new DMBReturn();
                case Constants.HGBQ -> new HGBReturn();
                case Constants.GBQ -> new GBReturn();
                default -> {
                    System.err.println("Return queue type not recognized!");
                    yield null;
                }
            };

            if (returnData == null) {
                System.err.println("Return data type not recognized!");
                return;
            }

            System.arraycopy(resByteArr, 0, returnData.getCallInfo(), 0,
                    resByteArr.length);
            returnData.getMetadata().m_Size = resByteArr.length;
            // returnData.getMetadata().m_ConnId = m_CallData.getMetadata().m_ConnId;

            returnData.write();

            LibDSP.INSTANCE.sendReturnV2(m_ReturnInfoHandle, returnData.getPointer());
            // LibDSP.INSTANCE.aquaServerSend(m_ReturnInfoHandle, m_ReturnInfoHandle, null,
            // m_ReturnInfoHandle);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Transform a xml file by means of a xslt file
     *
     * @param xmlBytes  The content of the xml file
     * @param xsltBytes The content of the xslt file
     * @return A xml useful to compute the hash code
     */
    private String mf_GetXmlTransformed(byte[] xmlBytes, byte[] xsltBytes) throws Exception {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        dbf.setNamespaceAware(true);

        DocumentBuilder db = dbf.newDocumentBuilder();
        Document document = db.parse(new ByteArrayInputStream(xmlBytes));

        System.setProperty("javax.xml.transform.TransformerFactory",
                "net.sf.saxon.TransformerFactoryImpl");
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory
                .newTransformer(new StreamSource(new ByteArrayInputStream(xsltBytes)));
        ByteArrayOutputStream output = new ByteArrayOutputStream();

        transformer.transform(new DOMSource(document), new StreamResult(output));

        return output.toString();
    }
}

public class Main {
    private static final int THREAD_POOL_SIZE = Runtime.getRuntime().availableProcessors();
    public static int PAYLOAD_SIZE;
    public static int QTYPE;

    private static int sm_GetQType(String p_Arg) {
        switch (p_Arg.toUpperCase()) {
            case "SMB":
                return Constants.SMBQ;
            case "EMB":
                return Constants.EMBQ;
            case "QMB":
                return Constants.QMBQ;
            case "HMB":
                return Constants.HMBQ;
            case "MB":
                return Constants.MBQ;
            case "DMB":
                return Constants.DMBQ;
            case "HGB":
                return Constants.HGBQ;
            case "GB":
                return Constants.GBQ;
            default:
                throw new IllegalArgumentException("Unknown queue type: " + p_Arg);
        }
    }

    private static int sm_GetPayloadSize(String p_Arg) {
        switch (p_Arg.toUpperCase()) {
            case "SMB":
                return Constants.SMB;
            case "EMB":
                return Constants.EMB;
            case "QMB":
                return Constants.QMB;
            case "HMB":
                return Constants.HMB;
            case "MB":
                return Constants.MB;
            case "DMB":
                return Constants.DMB;
            case "HGB":
                return Constants.HGB;
            case "GB":
                return Constants.GB;
            default:
                throw new IllegalArgumentException("Unknown payload type: " + p_Arg);
        }
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            PAYLOAD_SIZE = Constants.SMB;
            QTYPE = Constants.SMBQ;
        } else {
            PAYLOAD_SIZE = sm_GetPayloadSize(args[0]);
            QTYPE = sm_GetQType(args[0]);
        }

        ExecutorService executor = Executors.newFixedThreadPool(THREAD_POOL_SIZE);

        int connectInfoHandle = LibDSP.INSTANCE.aquaCreateServerConnectInfo();
        int callInfoHandle = LibDSP.INSTANCE.aquaCreateServerCallInfo();

        LibDSP.INSTANCE.aquaInstallV2(connectInfoHandle, callInfoHandle, "xslt-transformation", "v0.0.2", QTYPE);

        // ConnectThread connectThread = new ConnectThread(connectInfo, connections);
        // DisconnectThread disconnectThread = new DisconnectThread(connectInfo);
        DisconnectThread disconnectThread = new DisconnectThread(connectInfoHandle);

        // connectThread.setName("ConnectThread");
        disconnectThread.setName("DisconnectThread");

        // connectThread.start();
        disconnectThread.start();

        while (!Thread.currentThread().isInterrupted()) {
            try {
                int returnInfoHandle = LibDSP.INSTANCE.receiveConnectRequestV2(connectInfoHandle);

                Memory data = new Memory(65000);
                LibDSP.INSTANCE.aquaServerRecv(data, callInfoHandle);

                int dataLen = data.getInt(0);

                byte[] received = data.getByteArray(4, dataLen);

                executor.submit(new ProcessCallThread1(received, returnInfoHandle));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Transform a xml file by means of a xslt file
     *
     * @param xmlBytes  The content of the xml file
     * @param xsltBytes The content of the xslt file
     * @return A xml useful to compute the hash code
     */
    public static String s_GetXmlTransformed(byte[] xmlBytes, byte[] xsltBytes) throws Exception {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        dbf.setNamespaceAware(true);

        DocumentBuilder db = dbf.newDocumentBuilder();
        Document document = db.parse(new ByteArrayInputStream(xmlBytes));

        System.setProperty("javax.xml.transform.TransformerFactory", "net.sf.saxon.TransformerFactoryImpl");
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory
                .newTransformer(new StreamSource(new ByteArrayInputStream(xsltBytes)));
        ByteArrayOutputStream output = new ByteArrayOutputStream();

        transformer.transform(new DOMSource(document), new StreamResult(output));

        return output.toString();
    }
}
