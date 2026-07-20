FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

ARG PHP_VERSION=8.4

RUN apt-get update && \
  apt-get upgrade -y && \
  apt-get install gcc g++ clang vim cmake software-properties-common git-all \
  python3-venv openjdk-21-jre-headless openjdk-21-jdk-headless inkscape -y && \
  add-apt-repository ppa:ondrej/php && \
  apt-get update && \
  apt-get install php${PHP_VERSION} \
  php${PHP_VERSION}-zmq -y && \
  apt autoremove -y && \
  apt clean -y

CMD ["bash", "start.sh", "-D"]
