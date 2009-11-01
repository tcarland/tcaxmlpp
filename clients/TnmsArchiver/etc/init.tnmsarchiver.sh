#!/bin/bash
#
#

VERSION="0.1"
AUTHOR="tcarland@gmail.com"

PNAME=${0/#.\//}
CURDIR=`dirname $0`
CONFIGDIR=


SCHEMA=$1;
SCHEMAFILE=$2

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
    TCANMS_TMP="$TCANMS_HOME/tmp"
    TCANMS_ETC="$TCANMS_HOME/etc"
fi

# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $PNAME {schemaname|dbname} {output sql filename}"
    echo ""
}


if [ -z "$SCHEMA" ] || [ -z "$SCHEMAFILE" ]; then
    usage
    exit 1
fi

echo "  Creating sql for schema '$SCHEMA' in '$SCHEMAFILE'"
echo ""

echo "
CREATE TABLE ${SCHEMA}.element_name (
    id          INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    name        TEXT,
  PRIMARY KEY ( id ),
  INDEX name_index ( name(255) )
) ENGINE = MYISAM;


CREATE TABLE ${SCHEMA}.immediate (
    idx         INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    id          INTEGER UNSIGNED NOT NULL,
    value       INTEGER UNSIGNED NOT NULL,
    strval      TEXT NULL,
    valavg      INTEGER UNSIGNED NOT NULL,
    time        INTEGER UNSIGNED NOT NULL,
  PRIMARY KEY ( idx ),
  UNIQUE KEY timekey ( id, time )
) ENGINE = HEAP;


CREATE TABLE ${SCHEMA}.short (
    idx         INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    id          INTEGER UNSIGNED NOT NULL,
    value       INTEGER UNSIGNED NOT NULL,
    strval      TEXT NULL,
    valavg      INTEGER UNSIGNED NOT NULL,
    time        INTEGER UNSIGNED NOT NULL,
  PRIMARY KEY ( idx ),
  UNIQUE KEY timekey ( id, time )
) ENGINE = MYISAM;


CREATE TABLE ${SCHEMA}.long (
    idx         INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    id          INTEGER UNSIGNED NOT NULL,
    value       INTEGER UNSIGNED NOT NULL,
    strval      TEXT NULL,
    valavg      INTEGER UNSIGNED NOT NULL,
    time        INTEGER UNSIGNED NOT NULL,
  PRIMARY KEY ( idx ),
  UNIQUE KEY timekey ( id, time )
) ENGINE = MYISAM;
" > $SCHEMAFILE

echo "Finished."
exit 1

