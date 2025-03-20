FROM ghcr.io/tcarland/tcanetpp:v1.6.2

LABEL description="tcaxmlpp build container"
LABEL author="Timothy C. Arland <tcarland at gmail dot com>"

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

USER root

COPY . /opt/tcaxmlpp

RUN apt-get -y --no-install-recommends install \
    libxml2-dev && \
    chown -R tdh:tdh /opt/tcaxmlpp

WORKDIR /opt

RUN cd tcaxmlpp && make arlib && make install && make distclean

USER tdh
