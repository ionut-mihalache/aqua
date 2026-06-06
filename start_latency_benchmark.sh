runIceoryx2NoWaitBenchmark() {
    payload=$1

    rm /dev/shm/*

    cd /home/ubuntu/dsp-library/iceoryx2/examples/c/request_response_nowait_benchmark/src
    make clean
    make EXTRA_DEFINES=-D${payload}
    make run-server &
    iceoryx2Pid=$!
    sleep 2
    cd -

    outpath=/home/ubuntu/dsp-library/${payload}

    if [ ! -d "benchmark_results/clients/${clientsNr}" ]; then
        mkdir -p ${outpath}
    fi

    # msgNumbers=(1000 4000 7000 10000 13000 16000 19000 22000 25000 28000)
    msgNumbers=(1000 2000 4000 8000 10000)
    cd /home/ubuntu/dsp-library/iceoryx2/examples/c/request_response_nowait_benchmark/src

    echo "msg_count,P50,P90,P99,P99.9" > ${outpath}/iceoryx2_nowait.out
    for msgCount in "${msgNumbers[@]}";
    do
        for i in $(seq 1 30);
        do
            make run-client MSG_COUNT=${msgCount} OUTFILE=${outpath}/iceoryx2_nowait.out
        done
    done

    cd -

    kill ${iceoryx2Pid}
}

runIceoryx2Benchmark() {
    payload=$1

    rm /dev/shm/*

    cd /home/ubuntu/dsp-library/iceoryx2/examples/c/request_response_benchmark/src
    make clean
    make EXTRA_DEFINES=-D${payload}
    make run-server &
    iceoryx2Pid=$!
    sleep 2
    cd -

    outpath=/home/ubuntu/dsp-library/${payload}

    if [ ! -d "benchmark_results/clients/${clientsNr}" ]; then
        mkdir -p ${outpath}
    fi

    # msgNumbers=(1000 4000 7000 10000 13000 16000 19000 22000 25000 28000)
    msgNumbers=(1000 2000 4000 8000 10000)
    cd /home/ubuntu/dsp-library/iceoryx2/examples/c/request_response_benchmark/src

    echo "msg_count,P50,P90,P99,P99.9" > ${outpath}/iceoryx2.out
    for msgCount in "${msgNumbers[@]}";
    do
        for i in $(seq 1 30);
        do
            make run-client MSG_COUNT=${msgCount} OUTFILE=${outpath}/iceoryx2.out
        done
    done

    cd -

    kill ${iceoryx2Pid}
}

runECALBenchmark() {
    payload=$1

    rm /dev/shm/*

    cd /home/ubuntu/dsp-library/ecal
    make clean
    make EXTRA_DEFINES=-D${payload}
    make run-server &
    ecalPid=$!
    sleep 2
    cd -

    outpath=/home/ubuntu/dsp-library/${payload}

    if [ ! -d "benchmark_results/clients/${clientsNr}" ]; then
        mkdir -p ${outpath}
    fi

    # msgNumbers=(1000 4000 7000 10000 13000 16000 19000 22000 25000 28000)
    msgNumbers=(1000 2000 4000 8000 10000)
    cd /home/ubuntu/dsp-library/ecal

    echo "msg_count,P50,P90,P99,P99.9" > ${outpath}/ecal.out
    for msgCount in "${msgNumbers[@]}";
    do
        for i in $(seq 1 30);
        do
            make run-client MSG_COUNT=${msgCount} OUTFILE=${outpath}/ecal.out
        done
    done

    cd -

    kill ${ecalPid}
}

runAQUABenchmark() {
    payload=$1
    shift
    qType=$1

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ubuntu/dsp-library

    rm /dev/shm/*

    cd /home/ubuntu/dsp-library/test/c/service
    make clean
    make EXTRA_DEFINES=-D${payload}
    make run QTYPE=${qType} &
    aquaPid=$!
    sleep 2
    cd -

    outpath=/home/ubuntu/dsp-library/${payload}

    if [ ! -d "benchmark_results/clients/${clientsNr}" ]; then
        mkdir -p ${outpath}
    fi

    # msgNumbers=(1000 4000 7000 10000 13000 16000 19000 22000 25000 28000)
    msgNumbers=(1000 2000 4000 8000 10000)

    cd /home/ubuntu/dsp-library/test/c/client
    make clean
    make EXTRA_DEFINES=-D${payload}

    echo "msg_count,P50,P90,P99,P99.9" > ${outpath}/aqua.out
    for msgCount in "${msgNumbers[@]}";
    do
        for i in $(seq 1 30);
        do
            make run QTYPE=${qType} MSG_COUNT=${msgCount} OUTFILE=${outpath}/aqua.out
        done
    done

    cd -

    kill $aquaPid
}

payloads=(USE_64K USE_256K USE_1M)
for payload in "${payloads[@]}";
do
    # read -r size type <<< "${payload}"
    # echo ${size} ${type}
    runIceoryx2NoWaitBenchmark ${payload}
done

# payloads=(USE_64K USE_256K USE_1M)
# for payload in "${payloads[@]}";
# do
#     # read -r size type <<< "${payload}"
#     # echo ${size} ${type}
#     runIceoryx2Benchmark ${payload}
# done

# payloads=(USE_64K USE_256K USE_1M)
# for payload in "${payloads[@]}";
# do
#     # read -r size type <<< "${payload}"
#     # echo ${size} ${type}
#     runECALBenchmark ${payload}
# done

payloads=("USE_64K SMB" "USE_256K QMB" "USE_1M MB")
for payload in "${payloads[@]}";
do
    read -r size type <<< "${payload}"
    # echo ${size} ${type}
    runAQUABenchmark ${size} ${type}
done
