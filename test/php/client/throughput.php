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

class Constants
{
    public const int SMB = 1 << 16;
    public const int EMB = 1 << 17;
    public const int QMB = 1 << 18;
    public const int HMB = 1 << 19;
    public const int MB = 1 << 20;
    public const int DMB = 1 << 21;
    public const int HGB = 1 << 29;
    public const int GB = 1 << 30;
};

$arg = $argv[3] ?? "SMB";
$msgNr = $argv[2] ?? 1000;

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

$payloadSize = match (strtoupper($arg)) {
    "SMB" => Constants::SMB,
    "EMB" => Constants::EMB,
    "QMB" => Constants::QMB,
    "HMB" => Constants::HMB,
    "MB"  => Constants::MB,
    "DMB" => Constants::DMB,
    "HGB" => Constants::HGB,
    "GB"  => Constants::GB,
    default => throw new InvalidArgumentException("Unknown queue type: $arg"),
};

$ffi = FFI::cdef("
    int32_t aquaCreateClientConnectInfo();
    int32_t aquaCreateClientCallInfo();
    int32_t aquaCreateClientReturnInfo();
    int32_t aquaCreateClientConnRequestInfo();

    void aquaSetClientConnRequestInfo(int32_t connRequestHandle, const char *returnQName, uint32_t returnQSize, const char *reqResponseQName, uint32_t responseQSize, enum QType qType);

    void aquaConnect(int32_t returnInfoHandle, int32_t connectInfoHandle, int32_t connReqInfoHandle);

    void aquaDisconnect(int32_t connectInfoHandle, int32_t returnInfoHandle);

    void aquaClientSend(int32_t callInfoHandle, void *data, uint32_t size);

    void aquaClientRecv(void *data, int32_t returnInfoHandle);

    void aquaInitClientConnection(int32_t connectInfoHandle, int32_t callInfoHandle, const char *serviceStrId);
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

function aquaInitConnection(FFI $p_Ffi, int $connectInfoHandle, int $callInfoHandle, string $serviceStrId)
{
    $p_Ffi->aquaInitClientConnection($connectInfoHandle, $callInfoHandle, $serviceStrId);
}

function aquaConnect(FFI $p_Ffi, int $returnInfoHandle, int $connectInfoHandle, int $connReqInfoHandle)
{
    $p_Ffi->aquaConnect($returnInfoHandle, $connectInfoHandle, $connReqInfoHandle);
}

function send(FFI $p_Ffi, int $callInfoHandle, $data, int $size)
{
    $p_Ffi->aquaClientSend($callInfoHandle, $data, $size);
}

function recv(FFI $p_Ffi, $dataPtr, int $returnInfoHandle)
{
    $p_Ffi->aquaClientRecv($dataPtr, $returnInfoHandle);
}

function aquaDisconnect($p_Ffi, int $connectInfoHandle, int $returnInfoHandle)
{
    $p_Ffi->aquaDisconnect($connectInfoHandle, $returnInfoHandle);
}

function measureFnExec($p_Fn): float
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

function getDataPtr($p_Ffi, $p_Type)
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

function getCallDataPtr($p_Ffi, $p_CallInfo)
{
    return getDataPtr($p_Ffi, $p_CallInfo->m_Q->m_Type);
}

$benchmark = [];

$connectInfoHandle = aquaCreateClientConnectInfo($ffi);
$callInfoHandle = aquaCreateClientCallInfo($ffi);

aquaInitConnection($ffi, $connectInfoHandle, $callInfoHandle, "xslt-transformation");

$returnInfoHandle = aquaCreateClientReturnInfo($ffi);

$connReqInfoHandle = aquaCreateClientConnRequestInfo($ffi);

$uniqueId = uniqid("", true);

$returnQName = "return-q-" . $uniqueId;

$responseQName = "response-q-" . $uniqueId;

aquaSetClientConnRequestInfo($ffi, $connReqInfoHandle, $returnQName, 1, $responseQName, 1, $qType);

$benchmark["connect"] = measureFnExec(function () use ($ffi, $returnInfoHandle, $connectInfoHandle, $connReqInfoHandle) {
    aquaConnect($ffi, $returnInfoHandle, $connectInfoHandle, $connReqInfoHandle);
});

$iiaData = '<?xml version="1.0" encoding="UTF-8"?>

<iias-get-response xmlns="https://github.com/erasmus-without-paper/ewp-specs-api-iias/blob/stable-v7/endpoints/get-response.xsd" xmlns:c="https://github.com/erasmus-without-paper/ewp-specs-types-contact/tree/stable-v1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="&#10;        https://github.com/erasmus-without-paper/ewp-specs-api-iias/blob/stable-v7/endpoints/get-response.xsd&#10;        https://raw.githubusercontent.com/erasmus-without-paper/ewp-specs-api-iias/stable-v7/endpoints/get-response.xsd&#10;    "><iia><partner><hei-id>upb.ro</hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26062</iia-id></partner><partner><hei-id>brussels.uni-foundation.eu</hei-id><iia-id>9e01bc4f-f607-43c1-881a-8329c3e26063</iia-id><signing-contact><c:contact-name>Test contact</c:contact-name><c:email>test.contact@test.ro</c:email></signing-contact></partner><in-effect>false</in-effect><cooperation-conditions><student-studies-mobility-spec><sending-hei-id>brussels.uni-foundation.eu</sending-hei-id><sending-ounit-id>5</sending-ounit-id><receiving-hei-id>upb.ro</receiving-hei-id><receiving-ounit-id>ef711143-b1ab-4412-8493-3476d5ff962f</receiving-ounit-id><receiving-first-academic-year-id>2026/2027</receiving-first-academic-year-id><receiving-last-academic-year-id>2029/2030</receiving-last-academic-year-id><mobilities-per-year>2</mobilities-per-year><recommended-language-skill><language>is</language><cefr-level>B2</cefr-level><subject-area><isced-f-code>0007</isced-f-code><isced-clarification>Engineering, manufacturing and construction</isced-clarification></subject-area></recommended-language-skill><subject-area><isced-f-code>0819</isced-f-code><isced-clarification>Agriculture, not elsewhere classified</isced-clarification></subject-area><blended>false</blended></student-studies-mobility-spec></cooperation-conditions></iia></iias-get-response>';

$latencies = [];
$startTransfer = microtime(true);

for ($j = 0; $j < $msgNr; ++$j) {
    $latencies[] = measureFnExec(function () use ($ffi, $callInfoHandle, $iiaData, $returnInfoHandle, $payloadSize) {
        send($ffi, $callInfoHandle, $iiaData, strlen($iiaData));

        $returnData = $ffi->new("uint8_t[{$payloadSize}}]");

        recv($ffi, $returnData, $returnInfoHandle);
    });
}

$endTransfer = microtime(true);
$transferTime = $endTransfer - $startTransfer;
$throughput = $msgNr / $transferTime;

$avgLatency = array_sum($latencies) / count($latencies);
$minLatency = min($latencies);
$maxLatency = max($latencies);

echo "Average latency: $avgLatency μs\n";
echo "Min latency: $minLatency μs\n";
echo "Max latency: $maxLatency μs\n";
echo "Throughput: $throughput messages/sec\n";

$benchmark["disconnect"] = measureFnExec(function () use ($ffi, $connectInfoHandle, $returnInfoHandle) {
    aquaDisconnect($ffi, $connectInfoHandle, $returnInfoHandle);
});

$writeToFile = false;

if (isset($argv[1]) && $argv[1] == "true") {
    $writeToFile = true;
}

if ($writeToFile) {
    $file = fopen("benchmark_results/throughput/" . $msgNr . "/throughput_benchmark_" . $uniqueId . ".csv", 'a');
    fputcsv($file, [
        $msgNr,
        $payloadSize,
        $throughput,
        $minLatency,
        $maxLatency,
        $avgLatency
    ]);
    fclose($file);
}
