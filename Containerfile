FROM ghcr.io/tcarland/tcanetpp:latest

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

USER root

RUN apt-get -y --no-install-recommends install \
    libxml2-dev && \
    mkdir -p /opt/tcaxmlpp && \
    chown -R tdh:tdh /opt/tcaxmlpp

WORKDIR /opt

COPY . /opt/tcaxmlpp
RUN cd tcaxmlpp && make arlib && make install

USER tdh