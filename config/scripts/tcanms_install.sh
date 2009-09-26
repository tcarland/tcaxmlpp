#!/bin/bash
#
#  tcanms_install.sh
#
VERSION="0.1"
AUTHOR="tcarland@gmail.com"

MYNAME=${0/#.\//}
SYSHOME=
CONFIGDIR=
PREFIX=

CURDIR=`dirname $0`

RETVAL=0

# ------------------------------------------
#  Setup


if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

if [ -z "$TCANMS_PREFIX" ]; then
    echo "Variable TCANMS_PREFIX not set."
    exit 1
fi

CONFIGDIR=$CURDIR
echo ""
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

# remap our destination
export TCANMS_HOME="${TCANMS_PREFIX}"

export TCANMS_BIN=${TCANMS_HOME}/bin
export TCANMS_SBIN=${TCANMS_HOME}/sbin
export TCANMS_TMP=${TCANMS_HOME}/tmp
export TCANMS_LOGS=${TCANMS_HOME}/logs


init_db()
{
    local SQL="${TCANMS_TMP}/init_tcanms_db.sql"

    if [ -z "$TCANMS_DBNAME" ] || [ -z "$TCANMS_DBUSER" ]; then
        echo "DB Name macros are not set, aborting init_db()"
        return 0
    fi
    if [ -z "$TCANMS_DBHOST" ]; then
        TCANMS_DBHOST="localhost"
    fi

    echo "CREATE DATABASE $TCANMS_DBNAME;" > $SQL
    echo "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON tnmsauth.* \
        TO $TCANMS_DBUSER@$TCANMS_DBHOST IDENTIFIED BY '$TCANMS_DBPASS';" >> $SQL
    echo "" >> $SQL

    if [ ${TCANMS_USEDB} == "mysql" ]; then
        echo " executing mysql init script '$SQL' "
        mysql -u root -p < $SQL
    fi

    return 1
}

init_env_configs()
{
    local cfgenv=$1
    local cfghost=$2

    if [ -z "$cfgenv"] || [ -z "$cfghost" ]; then
        echo "Invalid environment settings"
        return -1
    fi

    local cfgpath="$CONFIGDIR/../environment/$cfgenv/$cfghost/"
    local target="$TCANMS_PREFIX/"

    echo "rsync -av $cfgpath $target"

    if [ -d $cfgpath ]; then
        rsync -av $cfgpath $target
    fi

    return 1
}


usage()
{
    echo ""
    echo "Usage: $MYNAME [-DhvW] [-e environment] [-t targethost]"
    echo ""
    echo "   -D | --database    : generate and run database init script "
    echo "   -h | --help        : display this help and exit"
    echo "   -v | --version     : display verion info and exit"
    echo "   -e | --environment : argument matching environment name $(TOPDIR)/config/environment"
    echo "   -t | --target      : target host configs to sync (requires -e)"
    echo "   -f | --force       : force overwrite of install target '$(TCANMS_PREFIX)'"
    echo ""
    echo "   The 'environment' flag will sync the configs for the provided environment and host."
    echo "   The script will by default use the environment variable TCANMS_PREFIX as the "
    echo "   target path to install"
    echo ""
}

version()
{
    echo "$MYNAME, Version $VERSION, $AUTHOR"
    echo ""
}


# --------------------------
#  MAIN

PREFIX=$TCANMS_PREFIX
PATHLIST="${PREFIX}/bin ${PREFIX}/sbin ${PREFIX}/etc ${PREFIX}/tmp \
${PREFIX}/logs ${PREFIX}/run"

INITDB=
ENVIR=
HOST=localhost

while [ $# -gt 0 ]; do
    case "$1" in
        -D|--database)
            INITDB="true"
            ;;
        -e|--environment)
            ENVIR="$2"
            shift
            ;;
        -t|--target)
            HOST="$2"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -v|--version)
            version
            exit 0
            ;;

    esac
    shift
done


if [ -z "$PREFIX" ]; then
    usage
    exit 1
fi

if [ -d $PREFIX ]; then
    echo "Install directory '$PREFIX' already exists. Continuing.."
fi


echo "Creating directory structure in '$PREFIX'"

for SUBDIR in $PATHLIST; do
    if [ ! -d $SUBDIR ]; then
        mkdir -p $SUBDIR
        RETVAL=$?
    fi
    if [ $RETVAL -eq 1 ]; then
        break
    fi
done

if [ $RETVAL -eq 1 ]; then
    echo "$MYNAME: finished with errors."
else
    echo "$MYNAME: finished successfully."
fi
echo ""

if [ -n "$INITDB" ] && [ -n "$TCANMS_USEDB" ]; then
    init_db
fi

if [ -n "$ENVIR" ]; then
    init_env_configs $ENVIR $HOST
fi

exit $RETVAL

