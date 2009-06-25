#!/bin/bash
#
#  tcanms_install.sh
#


SYSHOME=""
CURDIR=`dirname $0`

if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

echo "init.tcanms: current dir: $CURDIR"


if [ -z "$TCANMS_PREFIX" ]; then
    echo "Variable TCANMS_PREFIX not set. aborting install"
    exit 1
fi

CONFIGDIR=$CURDIR

if [ -z "$RC_TCANMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tcanmsrc ]; then
        echo "Using rc file from $CONFIGDIR"
        source $CONFIGDIR/tcanmsrc
    elif [ -e $HOME/tcanmsrc ]; then
        echo "Using rc file from $HOME"
        source $HOME/tcanmsrc
    else
        echo "Failed to locate rc file"
        exit 1
    fi
fi


TCANMS_BIN=${TCANMS_PREFIX}/bin
TCANMS_SBIN=${TCANMS_PREFIX}/sbin
TCANMS_ETC=${TCANMS_PREFIX}/etc
TCANMS_TMP=${TCANMS_PREFIX}/tmp
TCANMS_LOG=${TCANMS_PREFIX}/logs
TCANMS_RUN=${TCANMS_PREFIX}/run

if [ ! -d $TCANMS_BIN ]; then
    mkdir -p $TCANMS_BIN
fi
if [ ! -d $TCANMS_SBIN ]; then
    mkdir -p $TCANMS_SBIN
fi
if [ ! -d $TCANMS_ETC ]; then
    mkdir -p $TCANMS_ETC
fi
if [ ! -d $TCANMS_TMP ]; then
    mkdir -p $TCANMS_TMP
fi
if [ ! -d $TCANMS_LOG ]; then
    mkdir -p $TCANMS_LOG
fi
if [ ! -d $TCANMS_RUN ]; then
    mkdir -p $TCANMS_RUN
fi

exit 0

