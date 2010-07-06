#!/bin/bash
#
# tnms_dbinit.sh
#
VERSION="0.1"
AUTHOR="tcarland@gmail.com"

PNAME=${0/#.\//}
NAME="tnms_dbinit.sh"
CURDIR=`dirname $0`
CONFIGDIR=
PREFIX=
RETVAL=0


dbname=
dbuser=
dbhost=
dbpass=
do_local=
dbsql=
auser=root
aprompt=
ahost=


# ------------------------------------------
#  Setup


if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

CONFIGDIR=$CURDIR

echo ""
echo "$PNAME: "
if [ -z "$RC_TNMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tnmsrc ]; then
        echo "  Using rc: $CONFIGDIR/tnmsrc"
        source $CONFIGDIR/tnmsrc
    elif [ -e $HOME/tnms/etc/tnmsrc ]; then
        echo "  Using rc from: $HOME/tnms/etc/tnmsrc"
        source $HOME/tnms/etc/tnmsrc
    elif [ -e $HOME/etc/tnmsrc ]; then
        echo "  Using rc from: $HOME/etc/tnmsrc"
        source $HOME/etc/tnmsrc
    elif [ -e $TNMS_PREFIX/tnms/etc/tnmsrc ]; then
        echo "  Using rc from $TNMS_PREFIX/etc/tnmsrc"
        source $TNMS_PREFIX/tnms/etc/tnmsrc
    else
        echo "Error: Failed to locate rc file: tnmsrc"
        exit 1
    fi
fi
echo ""

if [ -n "$TNMS_PREFIX" ]; then
    TNMS_HOME="$TNMS_PREFIX/tnms"
    TNMS_TMP="$TNMS_HOME/tmp"
    TNMS_ETC="$TNMS_HOME/etc"
fi


# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $NAME [-hLV] [-pxr] -D dbname -f sql -U user -H host -P passwd"
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
    echo "    Lastly if no --sqlfile(-f) is specified, the script will check for three "
    echo "    sql scripts in $TNMS_ETC/dbname/dbname-{init|schema|setup}.sql. "
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
    local SQL="${TNMS_TMP}/init_tnms_db.sql"

    if [ -z "$dbname" ] || [ -z "$dbuser" ]; then
        echo "  DB Name macros are not set, aborting init_db()"
        return 1
    fi
    if [ -z "$dbhost" ]; then
        dbhost="localhost"
        do_local=1
    fi
    if [ -z "$dbpass" ]; then
        echo "  No password set for $dbuser@$dbhost"
        return 1
    fi

    echo "init_db() creating sql script '$SQL'"

    if [ "${TNMS_USEDB}" == "mysql" ]; then
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

    return 0
}

exec_sql()
{
    local sql=$1
    local retval=

    if [ "${TNMS_USEDB}" == "mysql" ]; then
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
    local path="$TNMS_ETC/$dbname"    

    if [ ! -d $path ]; then
        echo "Error locating script path $path"
        return 1
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

    return 0
}
    

# --------------------------
#  MAIN

mysql_exec="mysql"

while [ $# -gt 0 ]; do
    case "$1" in
        -D|--database)
            dbname="$2"
            shift
            ;;
        -f|--sqlfile)
            dbsql="$2"
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
    if [ $retval -eq 0 ] && [ -z "$dbsql" ]; then
        run_scripts
    fi
fi

if [ -n "$dbsql" ]; then
    exec_sql $dbsql
    retval=$?
fi

exit $retval

