FROM ghcr.io/tcarland/tcanetpp:latest

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

RUN apt-get -y --no-install-recommends install \
    libxml2-dev

USER root
WORKDIR /opt

RUN mkdir -p /opt/tcaxmlpp
COPY . /opt/tcaxmlpp

RUN cd tcaxmlpp && make arlib && make install

USER tdh