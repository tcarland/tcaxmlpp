#!/bin/bash
#
# tcanms_dbinit.sh
#
VERSION="0.1"
AUTHOR="tcarland@gmail.com"

PNAME=${0/#.\//}
CURDIR=`dirname $0`
CONFIGDIR=
PREFIX=
RETVAL=0


dbname=
dbuser=
dbhost=
dbpass=
do_local=


# ------------------------------------------
#  Setup


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
    echo "Usage: $PNAME [-hLV] [-pxr] -D dbname -f sql -U user -H host -P passwd"
    echo ""
    echo "    -h | --help      : display help and exit"
    echo "    -V | --version   : display version info and exit"
    echo "   -------------------------------------------------------"
    echo "   Database create options:"
    echo "    -D | --database  : name of database to create (required)"
    echo "    -f | --sqlfile   : name of file containing SQL to exec"
    echo "                       this file is exec'd after creating the DB (if requested)"
    echo "    -U | --user      : name of user to have access to new db (required)"
    echo "    -H | --host      : host from which user will access db (default: localhost)"
    echo "    -L | --localhost : include localhost access for user (if host is other)"
    echo "    -P | --password  : the user's password (required) "
    echo "   -------------------------------------------------------"
    echo "   Privileged account info:"
    echo "    -x | --adminuser : the admin db user to exec the script if not 'root'"
    echo "    -p | --prompt    : prompt for password when exec'ing script"
    echo "                       (eg. 'mysql -u root -p < SQL')"
    echo "    -r | --remote    : remote database host if not exec'ing sql on localhost"
    echo ""
    echo "    This script will run the sql client as root (eg. 'mysql -u root < SQL')"
    echo "    If there is a password necessary to exec the script, then use -p to prompt."
    echo "    Note that this is separate from the user(-U)/password(-P) being used to"
    echo "    identify access to the database(-D).  The exec'ing user (default: root) "
    echo "    can be overridden with (-x). Additionally, the script is assumed to run "
    echo "    against a local database. Use the -r option to set a remote database host "
    echo "    on which to exec the sql."
    echo ""
    version
}

version()
{
    echo "$PNAME, Version $VERSION, by $AUTHOR"
    echo ""
}


init_db()
{
    local SQL="${TCANMS_TMP}/init_tcanms_db.sql"

    if [ -z "$dbname" ] || [ -z "$dbuser" ]; then
        echo "  DB Name macros are not set, aborting init_db()"
        return 0
    fi
    if [ -z "$dbhost" ]; then
        dbhost="localhost"
    fi
    if [ -z "$dbpass" ]; then
        echo "  No password set for $dbuser@$dbhost"
        return 0
    fi

    echo "init_db() creating sql script '$SQL'"

    if [ "${TCANMS_USEDB}" == "mysql" ]; then
        # generate sql
        echo "CREATE SCHEMA IF NOT EXISTS \`$dbname\`;" > $SQL

        if [ -n "$do_local" ]; then
            echo "CREATE USER '$dbuser'@'localhost' IDENTIFIED BY '$dbpass';" >> $SQL
            echo "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON $dbname.* TO '$dbuser'@'localhost';" >> $SQL
        fi
        if [ "$dbhost" != "localhost" ]; then
            echo "CREATE USER '$dbuser'@'$dbhost' IDENTIFIED BY '$dbpass';" >> $SQL
            echo "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON $dbname.* TO '$dbuser'@'$dbhost';" >> $SQL
        fi
        echo "" >> $SQL
        
        exec_sql $SQL
    fi

    return 1
}

exec_sql()
{
    local sql=$1
    local retval=

    if [ "${TCANMS_USEDB}" == "mysql" ]; then
        local cmd="-u $auser"

        if [ -n "$ahost" ]; then
            cmd="$cmd -h $ahost"
        fi
        if [ -n "$aprompt" ]; then
            cmd="$cmd $aprompt"
        fi

        echo ""
        echo "Exec: mysql $cmd < $sql"
        mysql $cmd < $sql
    fi

    retval=$?

    return $retval 
}



run_scripts()
{
    local path="$TCANMS_ETC/$dbname"    

    if [ ! -d $path ]; then
        echo "Error locating script path $path"
        return 0
    fi

    initf="${path}/${dbname}-init.sql"
    schemaf="${path}/${dbname}-schema.sql"
    setupf="${path}/${dbname}-setup.sql"

    echo "$initf"
    if [ -e $initf ]; then
        exec_sql $initf
    fi
    if [ -e $schemaf ]; then
        exec_sql $schemaf
    fi
    if [ -e $setupf ]; then
        exec_sql $setupf
    fi

    return 1
}
    

# --------------------------
#  MAIN

auser=root
aprompt=
ahost=

mysql_exec="mysql"

while [ $# -gt 0 ]; do
    case "$1" in
        -D|--database)
            dbname="$2"
            shift
            ;;
        -H|--host)
            dbhost="$2"
            shift
            ;;
        -U|--user)
            dbuser="$2"
            shift
            ;;
        -p|--prompt)
            aprompt="-p"
            ;;
        -P|--password)
            dbpass="$2"
            shift
            ;;
        -L|--localhost)
            do_local=1
            ;;
        -x|--adminuser)
            auser="$2"
            shift;
            ;;
        -r|--remote)
            ahost="$2"
            shift;
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -V|--version)
            version
            exit 0
            ;;

    esac
    shift
done

if [ -z "$dbname" ] && [ -z "$dbsql" ]; then
    usage
    exit 1
fi

if [ -n "$dbname" ]; then
    init_db
    retval=$?
    if [ $retval -eq 1 ]; then
        run_scripts
    fi
fi

if [ -n "$dbsql" ]; then
    exec_sql $dbsql
    retval=$?
fi

exit $retval

