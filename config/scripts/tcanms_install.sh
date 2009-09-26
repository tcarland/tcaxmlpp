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

init_db()
{
    local SQL="${TCANMS_PREFIX}/tmp/init_tcanms_db.sql"

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


usage()
{
    echo ""
    echo "Usage: $MYNAME [target_path]"
    echo "    The target path is optional and the script will by default "
    echo "  check for the environment variable TCANMS_PREFIX to be used"
    echo "  as the target path"
    echo ""
}

version()
{
    echo "$MYNAME, Version $VERSION, $AUTHOR"
    echo ""
}


# --------------------------
#  MAIN

INITDB=

while [ $# -gt 0 ]; do
    case "$1" in
        -D|--database)
            INITDB="true"
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


PREFIX=$TCANMS_PREFIX
if [ -n "$1" ]; then
    PREFIX=$1
fi

if [ -z "$PREFIX" ]; then
    usage
    exit 1
fi


if [ -d $PREFIX ]; then
    echo "Install directory '$PREFIX' already exists. Continuing.."
fi


PATHLIST="${PREFIX}/bin ${PREFIX}/sbin ${PREFIX}/etc ${PREFIX}/tmp \
${PREFIX}/logs ${PREFIX}/run"

echo "Creating directory structure in '$TCANMS_PREFIX'"

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
    

exit $RETVAL

