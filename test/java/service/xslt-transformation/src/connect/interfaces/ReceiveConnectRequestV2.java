package connect.interfaces;

import com.sun.jna.Callback;

import calling.return_package.ServiceReturnInfo;

public interface ReceiveConnectRequestV2 extends Callback {
    int receiveConnectRequest(ServiceReturnInfo p_ReturnInfo, int p_ConnectInfoHandle);
}
