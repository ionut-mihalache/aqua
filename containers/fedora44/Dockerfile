FROM fedora:44

ARG PHP_VERSION=84
ARG TARGETARCH

RUN dnf -y update && \
    dnf -y install gcc gcc-c++ clang clangd cmake git vim python3 python3-pip inkscape \
        https://rpms.remirepo.net/fedora/remi-release-44.rpm && \
    dnf -y install \
        php${PHP_VERSION} \
        php${PHP_VERSION}-php-cli \
        php${PHP_VERSION}-php-devel \
        php${PHP_VERSION}-php-pear \
        php${PHP_VERSION}-php-process && \
    dnf -y install curl tar gzip && \
        case "${TARGETARCH}" in \
            amd64) \
                JDK_ARCH="x64" ;; \
            arm64) \
                JDK_ARCH="aarch64" ;; \
            *) \
                echo "Unsupported architecture: ${TARGETARCH}" >&2; \
                exit 1 ;; \
        esac && \
        curl -L -o /tmp/jdk.tar.gz \
        https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.10%2B7/OpenJDK21U-jdk_${JDK_ARCH}_linux_hotspot_21.0.10_7.tar.gz && \
        mkdir -p /opt/java && \
        tar -xzf /tmp/jdk.tar.gz -C /opt/java --strip-components=1 && \
        ln -sf /opt/java/bin/java /usr/local/bin/java && \
        ln -sf /opt/java/bin/javac /usr/local/bin/javac && \
        ln -sf /opt/remi/php${PHP_VERSION}/root/usr/bin/php /usr/local/bin/php && \
        ln -sf /opt/remi/php${PHP_VERSION}/root/usr/bin/phpize /usr/local/bin/phpize && \
        ln -sf /opt/remi/php${PHP_VERSION}/root/usr/bin/php-config /usr/local/bin/php-config && \
    dnf clean all

ENV PATH="/opt/remi/php${PHP_VERSION}/root/usr/bin:/opt/remi/php${PHP_VERSION}/root/usr/sbin:${PATH}"

CMD ["bash", "start.sh", "-D"]
