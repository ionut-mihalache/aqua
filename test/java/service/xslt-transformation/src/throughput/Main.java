package throughput;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import com.sun.jna.Memory;
import org.w3c.dom.Document;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import consts.Constants;

interface LibDSP extends Library {
    LibDSP INSTANCE = Native.load("aqua", LibDSP.class);

    int aquaCreateServerConnectInfo();

    int aquaCreateServerCallInfo();

    void aquaInstall(int connectInfoHandle, int callInfoHandle, String strId, String version, int callQType);

    void aquaServerSend(int returnInfoHandle, Pointer data, int size);

    int receiveConnectRequest(int p_ConnectInfoHandle);

    void aquaServerRecv(Pointer data, int callInfoHandle);

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

class ProcessCallThread3 implements Runnable {
    private final int m_CallInfoHandle;
    private final int m_ServiceReturnInfoHandle;

    ProcessCallThread3(int p_CallInfoHandle,
            int p_ServiceReturnInfoHandle) {
        m_CallInfoHandle = p_CallInfoHandle;
        m_ServiceReturnInfoHandle = p_ServiceReturnInfoHandle;
    }

    public void run() {
        int cnt = 0;
        while (!Thread.currentThread().isInterrupted()) {
            // if (cnt == Main.MSG_NUMBER) {
            // break;
            // }
            try {
                Memory data = new Memory(Main.PAYLOAD_SIZE);
                LibDSP.INSTANCE.aquaServerRecv(data, m_CallInfoHandle);

                int dataLen = data.getInt(0);

                byte[] received = data.getByteArray(4, dataLen);

                data.close();

                Memory returnData = new Memory(dataLen);
                returnData.write(0, received, 0, dataLen);
                LibDSP.INSTANCE.aquaServerSend(m_ServiceReturnInfoHandle, returnData, dataLen);
                returnData.close();

                cnt++;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        // m_ConnectInfo.m_ReceiveDisconnectRequest.receiveDisconnectRequest(m_ConnectInfo);
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

        System.setProperty("javax.xml.transform.TransformerFactory", "net.sf.saxon.TransformerFactoryImpl");
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
    public static int MSG_NUMBER;

    private static int sm_GetQType(String p_Arg) {
        return switch (p_Arg.toUpperCase()) {
            case "SMB" -> Constants.SMBQ;
            case "EMB" -> Constants.EMBQ;
            case "QMB" -> Constants.QMBQ;
            case "HMB" -> Constants.HMBQ;
            case "MB" -> Constants.MBQ;
            case "DMB" -> Constants.DMBQ;
            case "HGB" -> Constants.HGBQ;
            case "GB" -> Constants.GBQ;
            default -> throw new IllegalArgumentException("Unknown queue type: " + p_Arg);
        };
    }

    private static int sm_GetPayloadSize(String p_Arg) {
        return switch (p_Arg.toUpperCase()) {
            case "SMB" -> Constants.SMB;
            case "EMB" -> Constants.EMB;
            case "QMB" -> Constants.QMB;
            case "HMB" -> Constants.HMB;
            case "MB" -> Constants.MB;
            case "DMB" -> Constants.DMB;
            case "HGB" -> Constants.HGB;
            case "GB" -> Constants.GB;
            default -> throw new IllegalArgumentException("Unknown payload type: " + p_Arg);
        };
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            PAYLOAD_SIZE = Constants.SMB;
            QTYPE = Constants.SMBQ;
            // MSG_NUMBER = 1000;
        } else {
            PAYLOAD_SIZE = sm_GetPayloadSize(args[0]);
            QTYPE = sm_GetQType(args[0]);
            // MSG_NUMBER = Integer.parseInt(args[1]);
        }

        try (ExecutorService executor = Executors.newFixedThreadPool(2 * THREAD_POOL_SIZE)) {

            int connectInfoHandle = LibDSP.INSTANCE.aquaCreateServerConnectInfo();
            int callInfoHandle = LibDSP.INSTANCE.aquaCreateServerCallInfo();

            // LibDSP.INSTANCE.aquaInstall(connectInfo, callInfo, "xslt-transformation",
            // "v0.0.2", QTYPE);
            LibDSP.INSTANCE.aquaInstall(connectInfoHandle, callInfoHandle, "xslt-transformation", "v0.0.2", QTYPE);

            // ConnectThread connectThread = new ConnectThread(connectInfo, connections);
            DisconnectThread disconnectThread = new DisconnectThread(connectInfoHandle);

            // connectThread.setName("ConnectThread");
            disconnectThread.setName("DisconnectThread");

            // connectThread.start();
            disconnectThread.start();

            while (!Thread.currentThread().isInterrupted()) {
                try {
                    int returnInfoHandle = LibDSP.INSTANCE.receiveConnectRequest(connectInfoHandle);

                    executor.submit(new ProcessCallThread3(callInfoHandle, returnInfoHandle));
                    // executor.submit(new ProcessCallThread3(received, returnInfoHandle));
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
