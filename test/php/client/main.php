<?php

enum QType: int
{
    case SMBQ = 0;
    case EMBQ = 1;
    case QMBQ = 2;
    case HMBQ = 3;
    case MBQ = 4;
    case DMBQ = 5;
    case HGBQ = 6;
    case GBQ = 7;
};

$arg = $argv[3] ?? "SMB";
$msgNr = $argv[4] ?? 1000;

$qType = match (strtoupper($arg)) {
    "SMB" => QType::SMBQ->value,
    "EMB" => QType::EMBQ->value,
    "QMB" => QType::QMBQ->value,
    "HMB" => QType::HMBQ->value,
    "MB"  => QType::MBQ->value,
    "DMB" => QType::DMBQ->value,
    "HGB" => QType::HGBQ->value,
    "GB"  => QType::GBQ->value,
    default => throw new InvalidArgumentException("Unknown queue type: $arg"),
};

$ffi = FFI::cdef("
    struct CallMetadata {
        uint32_t m_Size;
        uint32_t m_ConnId;
        bool m_DataReady;
    };

    struct DSPQueueMetadata {
        void *m_FullCond;
        void *m_EmptyCond;
        void *m_Lock;
        uint32_t *m_PushIdxPtr;
        uint32_t *m_PopIdxPtr;
        uint32_t *m_Size;
    };

    struct SMBCall {
        uint8_t m_CallInfo[1 << 16];
        struct CallMetadata m_Metadata;
    };

    struct EMBCall {
        uint8_t m_CallInfo[1 << 17];
        struct CallMetadata m_Metadata;
    };

    struct QMBCall {
        uint8_t m_CallInfo[1 << 18];
        struct CallMetadata m_Metadata;
    };

    struct HMBCall {
        uint8_t m_CallInfo[1 << 19];
        struct CallMetadata m_Metadata;
    };

    struct MBCall {
        uint8_t m_CallInfo[1 << 20];
        struct CallMetadata m_Metadata;
    };

    struct DMBCall {
        uint8_t m_CallInfo[1 << 21];
        struct CallMetadata m_Metadata;
    };

    struct HGBCall {
        uint8_t m_CallInfo[1 << 29];
        struct CallMetadata m_Metadata;
    };

    struct GBCall {
        uint8_t m_CallInfo[1 << 30];
        struct CallMetadata m_Metadata;
    };

    struct ConnectQueue {
        struct DSPQueueMetadata m_Metadata;
        struct ConnectRequest *m_Data;
    };

    struct DisconnectQueue {
        struct DSPQueueMetadata m_Metadata;
        struct ConnectRequest *m_Data;
    };

    struct DSPQueue {
        struct DSPQueueMetadata m_Metadata;
        void *m_Data;
        uint32_t m_MaxSize;
        int m_Type;
    };

    int32_t aquaCreateClientConnectInfo();
    int32_t aquaCreateClientCallInfo();
    int32_t aquaCreateClientReturnInfo();
    int32_t aquaCreateClientConnRequestInfo();

    void aquaDestroyClientConnectInfo(int32_t);
    void aquaDestroyClientCallInfo(int32_t);
    void aquaDestroyClientReturnInfo(int32_t);
    void aquaDestroyClientConnRequestInfo(int32_t);

    void aquaSetClientConnRequestInfo(int32_t connRequestHandle, const char *returnQName, uint32_t returnQSize, const char *reqResponseQName, uint32_t responseQSize, enum QType qType);

    void aquaConnectV2(int32_t returnInfoHandle, int32_t connectInfoHandle, int32_t connReqInfoHandle);

    void aquaDisconnectV2(int32_t connectInfoHandle, int32_t returnInfoHandle);

    void send(int32_t callInfoHandle, void *data);

    void sendV2(int32_t callInfoHandle, int32_t returnInfoHandle, void *data, uint32_t size);

    void recv(void *data, int32_t returnInfoHandle);

    int32_t setCallData(int p_Type, void *p_CallInfo, uint8_t *p_Data, uint32_t p_Size);
    int32_t setCallDataV2(int p_Type, void *p_CallInfo, uint8_t *p_Data, uint32_t p_Size, int32_t returnInfoHandle);

    void aquaInitConnectionV2(int32_t connectInfoHandle, int32_t callInfoHandle, const char *serviceStrId);
", "/root/aqua/build/lib/shared/libaqua.so");

function aquaCreateClientConnectInfo(FFI $p_Ffi): int
{
    return $p_Ffi->aquaCreateClientConnectInfo();
}

function aquaCreateClientCallInfo(FFI $p_Ffi): int
{
    return $p_Ffi->aquaCreateClientCallInfo();
}

function aquaCreateClientReturnInfo(FFI $p_Ffi): int
{
    return $p_Ffi->aquaCreateClientReturnInfo();
}

function aquaCreateClientConnRequestInfo(FFI $p_Ffi): int
{
    return $p_Ffi->aquaCreateClientConnRequestInfo();
}

function aquaSetClientConnRequestInfo(FFI $p_Ffi, int $connRequestHandle, string $returnQName, int $returnQSize, string $reqResponseQName, int $responseQSize, int $qType)
{
    $p_Ffi->aquaSetClientConnRequestInfo($connRequestHandle, $returnQName, $returnQSize, $reqResponseQName, $responseQSize, $qType);
}

function aquaInitConnectionV2(FFI $p_Ffi, int $connectInfoHandle, int $callInfoHandle, string $serviceStrId)
{
    $p_Ffi->aquaInitConnectionV2($connectInfoHandle, $callInfoHandle, $serviceStrId);
}

function aquaConnectV2(FFI $p_Ffi, int $returnInfoHandle, int $connectInfoHandle, int $connReqInfoHandle)
{
    $p_Ffi->aquaConnectV2($returnInfoHandle, $connectInfoHandle, $connReqInfoHandle);
}

function setCallData(FFI $p_Ffi, int $p_Type, $p_CallDataPtr, $p_DataBuffer, int $p_Size)
{
    return $p_Ffi->setCallData($p_Type, $p_CallDataPtr, $p_DataBuffer, $p_Size);
}

function setCallDataV2(FFI $p_Ffi, int $p_Type, $p_CallDataPtr, $p_DataBuffer, int $p_Size, int $returnInfoHandle): int
{
    return $p_Ffi->setCallDataV2($p_Type, $p_CallDataPtr, $p_DataBuffer, $p_Size, $returnInfoHandle);
}

function send(FFI $p_Ffi, int $callInfoHandle, $dataPtr)
{
    $p_Ffi->send($callInfoHandle, $dataPtr);
}

function sendV2(FFI $p_Ffi, int $callInfoHandle, int $returnInfoHandle, $data, int $size)
{
    $p_Ffi->sendV2($callInfoHandle, $returnInfoHandle, $data, $size);
}

function recv(FFI $p_Ffi, $dataPtr, int $returnInfoHandle)
{
    $p_Ffi->recv($dataPtr, $returnInfoHandle);
}

function aquaDisconnectV2($p_Ffi, int $connectInfoHandle, int $returnInfoHandle)
{
    $p_Ffi->aquaDisconnectV2($connectInfoHandle, $returnInfoHandle);
}

function measureFnExec(callable $p_Fn): float
{
    $start = microtime(true);

    $p_Fn();

    $end = microtime(true);

    return ($end - $start) * 1_000_000;

    $start = getrusage();
    // $p_Fn();
    // $end = getrusage();

    // $utime = ($end["ru_utime.tv_sec"] - $start["ru_utime.tv_sec"]) * 1_000_000;
    // $utime += ($end["ru_utime.tv_usec"] - $start["ru_utime.tv_usec"]);

    // $stime = ($end["ru_stime.tv_sec"] - $start["ru_stime.tv_sec"]) * 1_000_000;
    // $stime += ($end["ru_stime.tv_usec"] - $start["ru_stime.tv_usec"]);

    // return $utime + $stime; // CPU real microseconds
}

function getDataPtr($p_Ffi, int $p_Type)
{
    $data = null;

    switch ($p_Type) {
        case QType::SMBQ->value:
            $data = $p_Ffi->new("struct SMBCall");
            break;
        case QType::EMBQ->value:
            $data = $p_Ffi->new("struct EMBCall");
            break;
        case QType::QMBQ->value:
            $data = $p_Ffi->new("struct QMBCall");
            break;
        case QType::HMBQ->value:
            $data = $p_Ffi->new("struct HMBCall");
            break;
        case QType::MBQ->value:
            $data = $p_Ffi->new("struct MBCall");
            break;
        case QType::DMBQ->value:
            $data = $p_Ffi->new("struct DMBCall");
            break;
        case QType::HGBQ->value:
            $data = $p_Ffi->new("struct HGBCall");
            break;
        case QType::GBQ->value:
            $data = $p_Ffi->new("struct GBCall");
            break;
        default:
            echo "Call queue type not recognized.\n";
            $data = null;
    }

    if ($data) {
        $dataPtr = FFI::addr($data);

        return [$data, $dataPtr];
    }

    return [null, null];
}

// function getCallDataPtr($p_Ffi, $p_CallInfo)
// {
//     return getDataPtr($p_Ffi, $p_CallInfo->m_Q->m_Type);
// }

$benchmark = [];

$connectInfoHandle = aquaCreateClientConnectInfo($ffi);
$callInfoHandle = aquaCreateClientCallInfo($ffi);

// $connectInfo = $ffi->new("struct ClientConnectInfo");
// $connectInfoPtr = FFI::addr($connectInfo);

// $callInfo = $ffi->new("struct ClientCallInfo");
// $callInfoPtr = FFI::addr($callInfo);

// aquaInitConnection($ffi, $connectInfoPtr, $callInfoPtr, "xslt-transformation");
aquaInitConnectionV2($ffi, $connectInfoHandle, $callInfoHandle, "xslt-transformation");

// $returnInfo = $ffi->new("struct ClientReturnInfo");
// $returnInfoPtr = FFI::addr($returnInfo);
$returnInfoHandle = aquaCreateClientReturnInfo($ffi);

// $requestInfo = $ffi->new("struct ClientConnectRequestInformation");
$connReqInfoHandle = aquaCreateClientConnRequestInfo($ffi);

$uniqueId = uniqid("", true);

$returnQName = "return-q-" . $uniqueId;
// FFI::memset($requestInfo->m_ReturnQName, 0, 256);
// FFI::memcpy($requestInfo->m_ReturnQName, $returnQName, strlen($returnQName));
// $requestInfo->m_ReturnQSize = 1;

$responseQName = "response-q-" . $uniqueId;
// FFI::memset($requestInfo->m_RequestResponseQName, 0, 256);
// FFI::memcpy($requestInfo->m_RequestResponseQName, $responseQName, strlen($responseQName));
// $requestInfo->m_ResponseQSize = 1;

// $requestInfo->m_QType = $qType;

// $requestInfoPtr = FFI::addr($requestInfo);
aquaSetClientConnRequestInfo($ffi, $connReqInfoHandle, $returnQName, 1, $responseQName, 1, $qType);

// $benchmark["connect"] = measureFnExec(function () use ($ffi, $returnInfoPtr, $connectInfoPtr, $requestInfoPtr) {
//     aquaConnect($ffi, $returnInfoPtr, $connectInfoPtr, $requestInfoPtr);
// });
$benchmark["connect"] = measureFnExec(function () use ($ffi, $returnInfoHandle, $connectInfoHandle, $connReqInfoHandle) {
    aquaConnectV2($ffi, $returnInfoHandle, $connectInfoHandle, $connReqInfoHandle);
});

// [$callData, $callDataPtr] = getCallDataPtr($ffi, $callInfo);
// [$callData, $callDataPtr] = getDataPtr($ffi, $qType);

// $callData->m_Metadata->m_ConnId = $returnInfo->m_ConnectResponseInformation->m_Id;

$iiaData = '<?xml version="1.0" encoding="UTF-8"?>

<iias-get-response xmlns="https://github.com/erasmus-without-paper/ewp-specs-api-iias/blob/stable-v7/endpoints/get-response.xsd" xmlns:c="https://github.com/erasmus-without-paper/ewp-specs-types-contact/tree/stable-v1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="&#10;        https://github.com/erasmus-without-paper/ewp-specs-api-iias/blob/stable-v7/endpoints/get-response.xsd&#10;        https://raw.githubusercontent.com/erasmus-without-paper/ewp-specs-api-iias/stable-v7/endpoints/get-response.xsd&#10;    "><iia><partner><hei-id>upb.ro</hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26062</iia-id></partner><partner><hei-id>brussels.uni-foundation.eu</hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26063</iia-id><signing-contact><c:contact-name>Test contact</c:contact-name><c:email>test.contact@test.ro</c:email></signing-contact></partner><in-effect>false</in-effect><cooperation-conditions><student-studies-mobility-spec><sending-hei-id>brussels.uni-foundation.eu</sending-hei-id><sending-ounit-id>5</sending-ounit-id><receiving-hei-id>upb.ro</receiving-hei-id><receiving-ounit-id>ef711143-b1ab-4412-8493-3476d5ff962f</receiving-ounit-id><receiving-first-academic-year-id>2026/2027</receiving-first-academic-year-id><receiving-last-academic-year-id>2029/2030</receiving-last-academic-year-id><mobilities-per-year>2</mobilities-per-year><recommended-language-skill><language>is</language><cefr-level>B2</cefr-level><subject-area><isced-f-code>0007</isced-f-code><isced-clarification>Engineering, manufacturing and construction</isced-clarification></subject-area></recommended-language-skill><subject-area><isced-f-code>0819</isced-f-code><isced-clarification>Agriculture, not elsewhere classified</isced-clarification></subject-area><blended>false</blended></student-studies-mobility-spec></cooperation-conditions></iia></iias-get-response>';

// $iiaDataBuffer = FFI::new("uint8_t[" . (strlen($iiaData)) . "]");
// for ($i = 0; $i < strlen($iiaData); ++$i) {
//     $iiaDataBuffer[$i] = ord($iiaData[$i]);
// }

// setCallData($ffi, $qType, $callDataPtr, $iiaDataBuffer, strlen($iiaData));
// setCallDataV2($ffi, $qType, $callDataPtr, $iiaDataBuffer, strlen($iiaData), $returnInfoHandle);

// $benchmark["call"] = measureFnExec(function () use ($ffi, $callInfoPtr, $callDataPtr) {
//     callFn($ffi, $callInfoPtr, $callDataPtr);
// });

$benchmark["call"] = measureFnExec(function () use ($ffi, $callInfoHandle, $returnInfoHandle, $iiaData) {
    // send($ffi, $callInfoHandle, $callDataPtr);
    echo "Sending for " . $callInfoHandle . ' ' . $returnInfoHandle . ' ' . strlen($iiaData) . "\n";
    sendV2($ffi, $callInfoHandle, $returnInfoHandle, $iiaData, strlen($iiaData));
});


[$returnData, $returnDataPtr] = getDataPtr($ffi, $qType);

// $benchmark["return"] = measureFnExec(function () use ($ffi, $returnDataPtr, $returnInfoPtr) {
//     returnFn($ffi, $returnDataPtr, $returnInfoPtr);
// });
$benchmark["return"] = measureFnExec(function () use ($ffi, $returnDataPtr, $returnInfoHandle) {
    recv($ffi, $returnDataPtr, $returnInfoHandle);
});

$result = "";
for ($i = 0; $i < $returnData->m_Metadata->m_Size; ++$i) {
    $result = $result . chr($returnData->m_CallInfo[$i]);
}

echo $result . "\n";

// $requestInfoPtr = FFI::addr($returnInfo->m_ConnectResponseInformation);

// $benchmark["disconnect"] = measureFnExec(function () use ($ffi, $connectInfoPtr, $requestInfoPtr) {
//     aquaDisconnect($ffi, $connectInfoPtr, $requestInfoPtr);
// });
$benchmark["disconnect"] = measureFnExec(function () use ($ffi, $connectInfoHandle, $returnInfoHandle) {
    aquaDisconnectV2($ffi, $connectInfoHandle, $returnInfoHandle);
});

print_r($benchmark);

$writeToFile = false;

if (isset($argv[1]) && $argv[1] == "true") {
    $writeToFile = true;
}

if ($writeToFile) {
    $file = fopen("benchmark_results/clients/" . $argv[2] . "/client_benchmark_" . $uniqueId . ".csv", 'a');
    fputcsv($file, [
        $argv[2],
        $benchmark["connect"],
        $benchmark["call"],
        $benchmark["return"],
        $benchmark["disconnect"]
    ]);
    fclose($file);
}
