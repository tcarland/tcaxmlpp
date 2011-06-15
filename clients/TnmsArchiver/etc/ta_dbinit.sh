#!/bin/bash
#
#  ta_initdb.sh  {schema}
#     Generates the SQL create statements for an archiver database
#     using the db credentials defined in ta_credentials.conf
#     located in $TNMS_HOME/etc and initializes the db.
#
VERSION="0.221"
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
if [ -z "$RC_TNMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/etc/tnmsrc ]; then
        echo "  Using rc: '$CONFIGDIR/etc/tnmsrc'"
        source $CONFIGDIR/etc/tnmsrc
    elif [ -e $HOME/tnms/etc/tnmsrc ]; then
        echo "  Using rc from: '$HOME/tnms/etc/tnmsrc'"
        source $HOME/tnms/etc/tnmsrc
    elif [ -e $HOME/etc/tnmsrc ]; then
        echo "  Using rc from: '$HOME/etc/tnmsrc'"
        source $HOME/etc/tnmsrc
    elif [ -e $TNMS_PREFIX/etc/tnmsrc ]; then
        echo "  Using rc from '$TNMS_PREFIX/etc/tnmsrc'"
        source $TNMS_PREFIX/etc/tnmsrc
    else
        echo "Error: Failed to locate rc file: tnmsrc'"
        exit 1
    fi
fi
echo ""

if [ -n "$TNMS_PREFIX" ]; then
    TNMS_HOME="$TNMS_PREFIX"
    TNMS_BIN="$TNMS_HOME/bin"
    TNMS_TMP="$TNMS_HOME/tmp"
    TNMS_ETC="$TNMS_HOME/etc"
fi

# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $PNAME {schema_name|db_name}"
    echo ""
    echo "   This script looks for the file ta_credentials.conf for "
    echo "   the required database credentials for the archiver db "
    echo "   being initialized."
    echo ""
}

# ------------------------------------------

DBINIT="${TNMS_BIN}/tnms_dbinit.sh"
DBSQLC="${TNMS_BIN}/ta_dbcreatesql.sh"
DBCRED="${TNMS_ETC}/ta_credentials.conf"

SQLDIR="${TNMS_ETC}/tnmsarchived"
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


if [ -z "$TNMS_DBUSER" ] || [ -z "$TNMS_DBPASS" ]; then
    echo "Error obtaining db user/password credentials from '$DBCRED'"
    echo ""
    usage
    exit 1
fi

if [ -z "$TNMS_DBHOST" ]; then
    TNMS_DBHOST="localhost"
fi

if [ ! -d "$SQLDIR" ]; then
    if [ ! -d "$TNMS_ETC" ]; then
        echo "Error: config path does not exist: '$TNMS_ETC'"
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

if [ ! -e $DBSQL ]; then
    echo "Error: Failed to locate sql script: '$DBSQL'"
    exit 1
fi

# exec db init
echo "$DBINIT -D $SCHEMA -U $TNMS_DBUSER -P $TNMS_DBPASS -H $TNMS_DBHOST -f $DBSQL -p"
$DBINIT -D $SCHEMA -U $TNMS_DBUSER -P $TNMS_DBPASS -H $TNMS_DBHOST -f $DBSQL -p
retval=$?

exit $retval

