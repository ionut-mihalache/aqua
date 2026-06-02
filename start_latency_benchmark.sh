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

    msgNumbers=(1000 4000 7000 10000 13000 16000 19000 22000 25000 28000)

    cd /home/ubuntu/dsp-library/test/c/client
    make clean
    make EXTRA_DEFINES=-D${payload}

    for msgCount in "${msgNumbers[@]}";
    do
        for i in $(seq 1 30);
        do
            make run QTYPE=${qType} MSG_COUNT=${msgCount}
        done
    done

    cd -

    kill $aquaPid
}

payloads=("USE_64K SMB" "USE_256K QMB" "USE_1M MB")
for payload in "${payloads[@]}";
do
    read -r size type <<< "${payload}"
    echo ${size} ${type}
done


# runAQUABenchmark USE_64K SMB
