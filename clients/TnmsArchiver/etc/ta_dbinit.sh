#!/bin/bash
#
#  ta_initdb.sh  {schema}
#     Generates the SQL create statements for an archiver database
#     using the db credentials defined in ta_credentials.conf
#     located in $TCANMS_HOME/etc and initializes the db.
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

usage()
{
    echo ""
    echo "Usage: $PNAME {schemaname|dbname}"
    echo ""
    echo "   This script looks for the file ta_credentials.conf for "
    echo "   the required database credentials for the archiver db "
    echo "   being initialized."
    echo ""
}

# ------------------------------------------

DBINIT="${TCANMS_BIN}/tcanms_dbinit.sh"
DBSQLC="${TCANMS_BIN}/ta_dbcreatesql.sh"
DBCRED="${TCANMS_ETC}/ta_credentials.conf"

SQLDIR="${TCANMS_ETC}/tnmsarchived"
DBSQL="${SQLDIR}/ta_${SCHEMA}_schema.sql"

if [ -z "$SCHEMA" ]; then
    usage
    exit 1
fi

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

if [ ! -d "$SQLDIR" ]; then
    if [ ! -d "$TCANMS_ETC" ]; then
        echo "Error: config path does not exist: '$TCANMS_ETC'"
        exit 1
    fi
    mkdir -p $SQLDIR
fi

# generate sql
$DBSQLC $SCHEMA $DBSQL
retval=$?

if [ $retval -eq 1 ]; then
    echo "Error in sql create script"
    exit 1
fi

if [ ! -e $SQL ]; then
    echo "Error: Failed to locate sql script: '$SQL'"
    exit 1
fi

# exec db init
$DBINIT -D $SCHEMA -U $TCANMS_DBUSER -P $TCANMS_DBPASS -H $TCANMS_DBHOST
retval=$?

exit $retval

