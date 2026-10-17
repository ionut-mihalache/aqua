package main;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
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
import org.w3c.dom.Document;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

import calling.call_package.DMBCall;
import calling.call_package.EMBCall;
import calling.call_package.GBCall;
import calling.call_package.HGBCall;
import calling.call_package.SMBCall;
import calling.call_package.QMBCall;
import calling.call_package.HMBCall;
import calling.call_package.MBCall;
import calling.interfaces.Call;
import calling.return_package.*;

import consts.Constants;

interface LibDSP extends Library {
    LibDSP INSTANCE = (LibDSP) Native.load("aqua", LibDSP.class);

    // void aquaInstall(ServiceConnectInfo p_ConnectInfo, ServiceCallInfo
    // p_CallInfo, String p_StrId, String p_Version,
    // int p_CallQType);

    int aquaCreateConnectInfo();

    int aquaCreateCallInfo();

    void aquaInstallV2(
            int connectInfoHandle,
            int callInfoHandle,
            String strId,
            String version,
            int callQType);

    void sendReturnV2(
            int p_ReturnInfoHandle,
            Pointer p_ReturnData);

    int receiveConnectRequestV2(int p_ConnectInfoHandle);

    void receiveCallV2(
            Pointer p_CallData,
            int p_CallInfoHandle);

    int getReturnInfoQType(int p_ReturnInfoHandle);

    void receiveDisconnectRequest(int p_ConnectInfoHandle);
}

class DisconnectThread extends Thread {
    // private final ServiceConnectInfo m_ConnectInfo;
    private final int m_ConnectInfoHandle;

    // DisconnectThread(ServiceConnectInfo p_ConnectInfo) {
    // m_ConnectInfo = p_ConnectInfo;
    // }

    DisconnectThread(int p_ConnectInfoHandle) {
        m_ConnectInfoHandle = p_ConnectInfoHandle;
    }

    public void run() {
        while (!Thread.currentThread().isInterrupted()) {
            // m_ConnectInfo.m_ReceiveDisconnectRequest.receiveDisconnectRequest(m_ConnectInfo);
            LibDSP.INSTANCE.receiveDisconnectRequest(m_ConnectInfoHandle);
        }
    }
}

class ProcessCallThread1 implements Runnable {
    private final Call m_CallData;
    // private final ServiceReturnInfo m_ServiceReturnInfo;
    private final int m_ReturnInfoHandle;

    // ProcessCallThread1(Call p_CallData, ServiceReturnInfo p_ServiceReturnInfo) {
    // m_CallData = p_CallData;
    // m_ServiceReturnInfo = p_ServiceReturnInfo;
    // }
    ProcessCallThread1(Call p_CallData, int p_ReturnInfoHandle) {
        m_CallData = p_CallData;
        m_ReturnInfoHandle = p_ReturnInfoHandle;
    }

    public void run() {
        try {
            byte[] iiaData = Arrays.copyOfRange(m_CallData.getCallInfo(), 0, m_CallData.getMetadata().m_Size);

            Path xsltPath = Paths.get("transformations/transform_version_v7.xsl");
            byte[] xsltData = Files.readAllBytes(xsltPath);

            String result = mf_GetXmlTransformed(iiaData, xsltData);

            byte[] resByteArr = result.getBytes(StandardCharsets.UTF_8);

            // ServiceReturnInfo serviceReturnInfo =
            // m_Connections.get(m_CallData.getMetadata().m_ConnId);

            // long returnDataSegmentSize = (new SMBReturn()).size();
            // Memory nativeMem = new Memory(returnDataSegmentSize);

            // System.out.println("SMBReturn size: " + returnDataSegmentSize);

            // Call returnData = switch (m_ServiceReturnInfo.m_Q.m_Type) {
            // case Constants.SMBQ -> new SMBReturn();
            // case Constants.EMBQ -> new EMBReturn();
            // case Constants.QMBQ -> new QMBReturn();
            // case Constants.HMBQ -> new HMBReturn();
            // case Constants.MBQ -> new MBReturn();
            // case Constants.DMBQ -> new DMBReturn();
            // case Constants.HGBQ -> new HGBReturn();
            // case Constants.GBQ -> new GBReturn();
            // default -> {
            // System.err.println("Return queue type not recognized!");
            // yield null;
            // }
            // };
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
            // System.arraycopy(iiaData, 0, returnData.getCallInfo(), 0,
            // m_CallData.getMetadata().m_Size);
            returnData.getMetadata().m_ConnId = m_CallData.getMetadata().m_ConnId;

            returnData.write();
            // nativeMem.write(0, returnData.getPointer().getByteArray(0, (int)
            // returnDataSegmentSize), 0,
            // (int) returnDataSegmentSize);
            // LibDSP.INSTANCE.sendReturn(m_ServiceReturnInfo, returnData.getPointer());
            LibDSP.INSTANCE.sendReturnV2(m_ReturnInfoHandle, returnData.getPointer());

            // m_ConnectInfo.m_ReceiveDisconnectRequest.receiveDisconnectRequest(m_ConnectInfo);
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

        // ServiceConnectInfo connectInfo = new ServiceConnectInfo();
        // ServiceCallInfo callInfo = new ServiceCallInfo();
        // ConcurrentHashMap<Integer, ServiceReturnInfo> connections = new
        // ConcurrentHashMap<Integer, ServiceReturnInfo>();

        // LibDSP.INSTANCE.aquaInstall(connectInfo, callInfo, "xslt-transformation",
        // "v0.0.2", QTYPE);

        // connectInfo.read();
        // callInfo.read();

        int connectInfoHandle = LibDSP.INSTANCE.aquaCreateConnectInfo();
        int callInfoHandle = LibDSP.INSTANCE.aquaCreateCallInfo();

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
                // ServiceReturnInfo returnInfo = new ServiceReturnInfo();

                // connectInfo.m_ReceiveConnectRequest.receiveConnectRequest(returnInfo,
                // connectInfo);
                int returnInfoHandle = LibDSP.INSTANCE.receiveConnectRequestV2(connectInfoHandle);
                System.out.println("Got return handle " + returnInfoHandle);

                // returnInfo.read();

                // Pointer nativePtr = returnInfo.m_ResponseQueue.m_Data;
                // if (nativePtr == null) {
                // System.err.println("ConnectThread: m_Data pointer is NULL");
                // continue;
                // }

                // int size = new ConnectResponseInformation().size();

                // byte[] buf = nativePtr.getByteArray(0, size);

                // ConnectResponseInformation responseInfo = new ConnectResponseInformation();
                // responseInfo.getPointer().write(0, buf, 0, size);
                // responseInfo.read();
                // ConnectResponseInformation responseInfo = new ConnectResponseInformation(
                // returnInfo.m_ResponseQueue.m_Data.share(0));

                Call callData = switch (QTYPE) {
                    case Constants.SMBQ -> new SMBCall();
                    case Constants.EMBQ -> new EMBCall();
                    case Constants.QMBQ -> new QMBCall();
                    case Constants.HMBQ -> new HMBCall();
                    case Constants.MBQ -> new MBCall();
                    case Constants.DMBQ -> new DMBCall();
                    case Constants.HGBQ -> new HGBCall();
                    case Constants.GBQ -> new GBCall();
                    default -> {
                        System.err.println("Return queue type not recognized!");
                        yield null;
                    }
                };

                LibDSP.INSTANCE.receiveCallV2(callData.getPointer(), callInfoHandle);

                callData.read();

                executor.submit(new ProcessCallThread1(callData, returnInfoHandle));
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
