#!/bin/bash
#
#  mysql-slave-backup.sh
#

PNAME=${0##*\/}
VERSION="1.11"
AUTHOR="tcarland@gmail.com"

BINLOGNAME="mysql-bin"
MYSQLDIR="/var/lib/mysql"
MYSQLDUMP_OPTIONS="--single-transaction --flush-logs --master-data=2 --all-databases"
FULL=0
DRYRUN=0

MYSQLUSER=
MYSQLPASS=
MYSQLSOCK=
TARGET=
MASTER_LASTLOG=
retval=


usage()
{
    echo ""
    echo "Usage: $PNAME [options]  full|incremental"
    echo "           --user|-u <username>  =  Database username"
    echo "           --pass|-p <password>  =  Database users password (optional) "
    echo "           --target|-t <target>  =  Backup target path (required)"
    echo "           --data|-d <datadir>   =  Path to Mysql data dir "
    echo "                                      (default: /var/lib/mysql)"
    echo "           --help|-h             =  Display usage information"
    echo "           --dry-run|-n          =  Perform a dry-run (don't do anything)"
    echo "           --sock|-S             =  Provide an alternate socket to connect"
    echo "                                      (eg. -S /var/run/mysqld/mysqld-inst1.sock)"
    echo "           --version|-v          =  Display version information"
    echo "          [action=full|inc]      =  Default action is 'incremental'"
    echo "                incremental      =  Syncs to binary logs only"
    echo "                full             =  Syncs logs, data dump of all databases,"
    echo "                                      purges old binary logs"
    echo ""
    echo " eg. $PNAME --user root --pass mypass --target /m1/Backup/mysql full"
    echo ""
    return 1
}


version()
{
    echo " $PNAME "
    echo "   Version = $VERSION"
    echo "   Author  = $AUTHOR"
    echo ""
}


startSlave()
{    
    local options="--user=$MYSQLUSER"

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ -n "$MYSQLSOCK" ]; then
        options="${options} -S ${MYSQLSOCK}"
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  startSlave()"
        echo "mysqladmin $options start-slave"
        echo ""
        retval=0
    else
        mysqladmin $options start-slave
        retval=$?
    fi

    return $retval
}


stopSlave()
{
    local options="--user=$MYSQLUSER"

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ -n "$MYSQLSOCK" ]; then
        options="${options} -S ${MYSQLSOCK}"
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  stopSlave()"
        echo "mysql $options -e 'STOP SLAVE SQL_THREAD;'"
        echo ""
        retval=0
    else
        mysql $options -e 'STOP SLAVE SQL_THREAD;'
        retval=$?
    fi

    return $retval
}


dumpSlave()
{
    local targetpath=$1
    local savedate=$2
    local targetdbfile="$targetpath/$HOSTNAME-full-$savedate.sql.gz"
    local options="--user=$MYSQLUSER"

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ -n "$MYSQLSOCK" ]; then
        options="${options} -S ${MYSQLSOCK}"
    fi

    options="${options} $MYSQLDUMP_OPTIONS"

    if [ -e $targetdbfile ]; then
        echo "Error: target file already exists: '$targetdbfile'"
        return 1
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  dumpSlave()"
        echo "mysqldump $options | gzip  > $targetdbfile "
        echo ""
        retval=0
    else
        mysqldump $options | gzip > $targetdbfile
        retval=$?
    fi

    return $retval
}


syncLogs()
{
    local targetpath=$1
    local options="-a"

    if [ ! -d $targetpath ]; then
        mkdir -p $targetpath
        retval=$?
        if [ $retval -eq 1 ]; then
            echo "Failed to create path '$targetpath'"
            return $retval
        fi
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  syncLogs()"
        options="${options}vn"
    fi
    
    for FILE in `cat $MYSQLDIR/$BINLOGNAME.index`
    do
        if [ -n "$DRYRUN" ]; then
            echo "rsync $options $MYSQLDIR/$FILE $targetpath/$FILE"
            rsync $options $MYSQLDIR/$FILE $targetpath/$FILE
        else
            rsync $options $MYSQLDIR/$FILE $targetpath/$FILE
        fi
        MASTER_LASTLOG=$FILE;
    done
    echo ""


    return 0
}


purgeLogs()
{
    local options="--user=$MYSQLUSER"
    local lastlog=${MASTER_LASTLOG/#.\//}

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ -n "$MYSQLSOCK" ]; then
        options="${options} -S ${MYSQLSOCK}"
    fi

    if [ -z "$lastlog" ]; then
        echo "error, no lastlog set"
        return 1
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "Current Master log is $lastlog"
        echo "mysql $options -e 'PURGE BINARY LOGS TO '$lastlog';'"
        echo ""
        retval=0
    else
        mysql $options -e "PURGE BINARY LOGS TO '$lastlog';"
        retval=$?
    fi

    return 0
}


checkTarget()
{
    local targetpath="$TARGET/$HOSTNAME/binlog"

    if [ -d $targetpath ]; then
        rm -rf $targetpath
        retval=$?
        if [ $retval -eq 1 ]; then
            echo "Failed to clear $targetpath"
            return $retval
        fi
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  checkTarget()"
        echo "mkdir -p $targetpath"
        echo ""
        retval=0
    else
        mkdir -p $targetpath
        retval=$?
    fi


    return $retval;
}


while [ $# -gt 0 ]; do
    case "$1" in
        -d|--debug)
            echo " --debug enabled"
            ;;
        -u|--user)
            MYSQLUSER="$2"
            shift
            ;;
        -p|--pass)
            MYSQLPASS="$2"
            shift
            if [ -z "$MYSQLPASS" ]; then
                echo "error: no password given"
                exit 1
            fi
            ;;
        -t|--target)
            TARGET="$2"
            shift
            ;;
        -d|--data)
            MYSQLDIR="$2"
            shift
            if [ -z "$MYSQLDIR" ]; then
                echo "No data dir provided"
                exit 1
CREATE USER 'yukai'@'localhost' IDENTIFIED BY '12kiaG';
GRANT ALL PRIVILEGES ON *.* TO 'yukai'@'localhost';


            fi
            if [ ! -d $MYSQLDIR ]; then
                echo "Invalid directory $MYSQLDIR"
                exit 1
            fi
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -n|--dry-run)
            DRYRUN=1
            ;;
        -S|--sock)
            MYSQLSOCK="$2"
            shift
            ;;
        -v|--version)
            version
            exit 0
            ;;
        'full')
            FULL=1
            ;;
    esac
    shift
done

echo ""

if [ -z "$MYSQLUSER" ] || [ -z "$TARGET" ]; then
    usage
    exit 1
fi

if [ -n "$MYSQLSOCK" ] && [ ! -e $MYSQLSOCK ]; then
    echo "Error: local socket file not found: '$MYSQLSOCK'"
    exit 1
fi

targetpath="$TARGET/$HOSTNAME"
targetbinlog="$targetpath/binlog"
savedate=`date +%Y%m%d`


if [ $FULL -eq 1 ]; then
    echo "Performing full backup..."
    targetbinlog="${targetbinlog}-${savedate}"
else
    echo "Performing incremental backup..."
fi
if [ $DRYRUN -eq 1 ]; then
    echo "  DRYRUN enabled "
fi
echo ""


checkTarget $targetbinlog
retval=$?
if [ $retval -eq 1 ]; then
    exit 1
fi


stopSlave
retval=$?
if [ $retval -eq 1 ]; then
    exit 1
else
    echo "Slave stopped"
fi


syncLogs $targetbinlog
retval=$?
if [ $retval -eq 1 ]; then
    exit 1
fi


if [ $FULL -eq 1 ]; then
    dumpSlave $targetpath $savedate
    retval=$?
    if [ $retval -eq 1 ]; then
        exit 1
    fi
fi


startSlave
retval=$?
if [ $retval -eq 1 ]; then
    exit 1
fi


if [ $FULL -eq 1 ]; then
    purgeLogs
    retval=$?
    if [ $retval -eq 1 ]; then
        exit 1
    else 
        echo "Logs purged"
    fi
fi

exit 0


