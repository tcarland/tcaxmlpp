#!/bin/bash
#
#  tnms_install.sh
#
#    The installation script prepares our TNMS installation
#  creating any necessary directories and synchronizing the 
#  environment configurations.
#
VERSION="1.33"
AUTHOR="tcarland@gmail.com"

PNAME=${0##*\/}

RSYNC="rsync"
CONFIGDIR=
PREFIX=
SUBDIRS="bin sbin etc run tmp logs"

CURDIR=`dirname $0`
RETVAL=0

# ------------------------------------------
#  Setup
if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi
CONFIGDIR=$CURDIR

echo ""
if [ -z "$RC_TNMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tnmsrc ]; then
        echo "$PNAME: using rc: $CONFIGDIR/tnmsrc"
        source $CONFIGDIR/tnmsrc
    elif [ -e $HOME/tnmsrc ]; then
        echo "$PNAME: using rc from: $HOME/tnmsrc"
        source $HOME/tnmsrc
    elif [ -e $HOME/etc/tnmsrc ]; then
        echo "$PNAME: using rc from: $HOME/etc/tnmsrc"
        source $HOME/etc/tnmsrc
    else
        echo "$PNAME: Error: Failed to locate rc file: tnmsrc"
        exit 1
    fi
fi

# we remap to our destination
PREFIX=$TNMS_PREFIX
export TNMS_HOME="${PREFIX}"
export TNMS_BIN=${TNMS_HOME}/bin
export TNMS_SBIN=${TNMS_HOME}/sbin
export TNMS_ETC=${TNMS_HOME}/etc
export TNMS_TMP=${TNMS_HOME}/tmp
export TNMS_LOGS=${TNMS_HOME}/logs

# paths to create
for path in $SUBDIRS; do
    PATHLIST+="${PREFIX}/$path "
done

# globals
ENVIR=$TNMS_ENV
HOST=$TNMS_HOST
# flags
FORCE=

# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $PNAME [-fhv] [-e environment] [-t targethost]"
    echo ""
    echo "   -f | --force       : force overwrite of install target '${TNMS_PREFIX}'"
    echo "   -h | --help        : display this help and exit"
    echo "   -v | --version     : display verion info and exit"
    echo "   -e | --environment : environment name from which to sync configs "
    echo "                         ( or the TNMS_ENV var (optional) )"
    echo "   -t | --target      : name of the target host (or TNMS_HOST)"
    echo "                         (requires -e or TNMS_ENV to be set)"
    echo ""
    echo "   If the 'environment' and 'target' flags will sync the configs for the provided "
    echo "   environment and host within 'config/environment/envname/target'"
    echo "   If not provided, vars TNMS_ENV and TNMS_HOST will be used if set." 
    echo "   The script requires the environment variable TNMS_PREFIX as the "
    echo "   target path to install. The current settings are as follows: "
    echo ""
    echo "   TNMS_PREFIX = '$TNMS_PREFIX'"
    echo "   TNMS_ENV    = '$TNMS_ENV'"
    echo "   TNMS_HOST   = '$TNMS_HOST'"
    echo ""
    version
}

version()
{
    echo "$PNAME : Version $VERSION, by $AUTHOR"
    echo ""
}


# ------------------------------------------

create_subdirs()
{
    local subdir=

    for subdir in $PATHLIST; do
        if [ ! -d $subdir ]; then
            mkdir -p $subdir
            RETVAL=$?
        fi
        if [ $RETVAL -eq 1 ]; then
            break
        fi
        #chown $TNMS_USER $subdir
        #chgrp $TNMS_GROUP $subdir
        #chmod g+w $subdir
    done

    return $RETVAL
}

init_env_configs()
{
    local cfgenv=$1
    local cfghost=$2

    if [ -z "$cfgenv" ] || [ -z "$cfghost" ]; then
        echo "  Invalid environment settings"
        return 1
    fi

    local cfgpath="$CONFIGDIR/../environment/$cfgenv/$cfghost/"
    local target="$PREFIX/"
    local rcfile="$CONFIGDIR/../environment/$cfgenv/bashrc"

    echo "  Syncing configs from '$cfgenv/$cfghost'"

    if [ -e $rcfile ]; then
        echo "  'cp $rcfile ~/'"
        cp $rcfile ~/
    fi

    if [ -d $cfgpath ]; then
        echo "  '$RSYNC -r $cfgpath $target'"
        $RSYNC -r $cfgpath $target
    else
        echo "  Error, path not valid: $cfgpath"
        return 1
    fi

    return 0
}


copy_scripts()
{
    echo "  Copying scripts to $TNMS_HOME"
    ( cp $CONFIGDIR/tnms.sh $TNMS_BIN )
    ( cp $CONFIGDIR/tnms_functions.sh $TNMS_BIN )
    ( cp $CONFIGDIR/tnms_dbinit.sh $TNMS_BIN )
    ( cp $CONFIGDIR/mysql* $TNMS_BIN )
    ( cp $CONFIGDIR/tnmsrc $TNMS_ETC )
}


# --------------------------
#  MAIN


while [ $# -gt 0 ]; do
    case "$1" in
        -e|--environment)
            ENVIR="$2"
            shift
            ;;
        -f|--force)
            FORCE="true"
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
    echo "Error: TNMS_PREFIX not set."
    usage
    exit 1
fi


if [ -d $PREFIX ]; then
    if [ -n "$FORCE" ]; then
        echo "  Install directory '$PREFIX' already exists."
        echo "     '--force' is enabled, continuing..."
    else
        echo "Error: Install directory '$PREFIX' exists."
        echo "Use the '--force' option to overwrite"
        exit 1
    fi
fi

echo "  Creating directory structure in '$PREFIX'"
create_subdirs


if [ -n "$ENVIR" ] && [ -n "$HOST" ]; then
    init_env_configs $ENVIR $HOST
else
    echo ""
    echo "  !! Target environment and host not provided, configs NOT sync'd."
    echo "     Set the env vars TNMS_ENV and TNMS_HOST variables to sync configs"
    echo ""
fi

copy_scripts

if [ $RETVAL -eq 1 ]; then
    echo "$PNAME: finished with errors."
else
    echo "$PNAME: finished successfully."
fi
echo ""


exit $RETVAL

