#!/bin/bash
#
#

VERSION="0.1"
AUTHOR="tcarland@gmail.com"

PNAME=${0/#.\//}
CURDIR=`dirname $0`
CONFIGDIR=



SCHEMA=$1;

# ------------------------------------------

if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

CONFIGDIR=$CURDIR


echo ""
echo "$PNAME: "
if [ -z "$RC_TCANMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tcanmsrc ]; then
        echo "  Using rc: $CONFIGDIR/tcanmsrc"
        source $CONFIGDIR/tcanmsrc
    elif [ -e $HOME/tcanms/etc/tcanmsrc ]; then
        echo "  Using rc from: $HOME/tcanms/etc/tcanmsrc"
        source $HOME/tcanms/etc/tcanmsrc
    elif [ -e $HOME/etc/tcanmsrc ]; then
        echo "  Using rc from: $HOME/etc/tcanmsrc"
        source $HOME/etc/tcanmsrc
    elif [ -e $TCANMS_PREFIX/etc/tcanmsrc ]; then
        echo "  Using rc from $TCANMS_PREFIX/etc/tcanmsrc"
        source $TCANMS_PREFIX/etc/tcanmsrc
    else
        echo "Error: Failed to locate rc file: tcanmsrc"
        exit 1
    fi
fi
echo ""

if [ -n "$TCANMS_PREFIX" ]; then
    TCANMS_HOME="$TCANMS_PREFIX"
    TCANMS_BIN="$TCANMS_HOME/bin"
    TCANMS_TMP="$TCANMS_HOME/tmp"
    TCANMS_ETC="$TCANMS_HOME/etc"
fi

# ------------------------------------------

DBINIT="${TCANMS_BIN}/tcanms_dbinit.sh"
DBSQLC="${TCANMS_BIN}/ta_dbcreatesql.sh"
DBCRED="${TCANMS_ETC}/ta_credentials.conf"

SQL="${TCANMS_TMP}/ta_${SCHEMA}_init.sql"


if [ ! -e $DBINIT ]; then
    echo "Failed to locate dbinit script '$DBINIT'"
    exit 1
fi

if [ ! -e $DBCRED ]; then
    echo "Failed to locate db config '$DBCRED'. Please create and rerun"
    echo ""
    usage
fi


source $DBCRED


if [ -z "$TCANMS_DBUSER" ] || [ -z "$TCANMS_DBPASS" ]; then
    echo "Error obtaining db user/password credentials from '$DBCRED'"
    echo ""
    usage
    exit 1
fi

if [ -z "$TCANMS_DBHOST" ]; then
    TCANMS_DBHOST="localhost"
fi

# generate sql
$DBSQLC $SCHEMA $SQL
retval=$?

if [ retval -eq 1 ]; then
    echo "Error in sql create script"
    exit 1
fi

if [ ! -e $SQL ]; then
    echo "Error: Failed to locate sql script: '$SQL'"
    exit 1
fi

# exec db init
$DBINIT -D $SCHEMA -f $SQL -U $TCANMS_DBUSER -P $TCANMS_DBPASS -H $TCANMS_DBHOST
retval=$?

exit $retval

