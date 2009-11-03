#!/bin/bash
#
#  mysql-slave-backup.sh
#

VERSION="1.0.3"
AUTHOR="tcarland@gmail.com"

MYSQLBIN="mysql"
MYSQLADMIN="mysqladmin"
MYSQLDUMP="mysqldump"
RSYNC="rsync"

BINLOGNAME="mysql-bin"
MYSQLDIR="/var/lib/mysql"

MYSQLDUMP_OPTIONS="--single-transaction --opt --flush-logs --master-data=2 --all-databases"

FULL=0
DRYRUN=0

MYSQLUSER=
MYSQLPASS=
TARGET=
MASTER_LASTLOG=
RETVAL=


usage()
{
    echo ""
    echo "Usage: $0  [options]  {action} "
    echo "        options:"
    echo "           -u|--user   <username>  =  Database username"
    echo "           -p|--pass   <password>  =  Database users password (optional) "
    echo "           -t|--target <target>    =  Backup target path (required)"
    echo "           -d|--data   <datadir>   =  Path to Mysql data directory"
    echo "                                       (default: /var/lib/mysql)"
    echo "           -b|--binlog <logname>   =  An alternate binlog from: '$BINLOGNAME'"
    echo "           -h|--help               =  Display usage information"
    echo "           -n|--dry-run            =  Performs a dry-run"
    echo "           -v|--version            =  Display version information"
    echo ""
    echo "        actions:"
    echo "            incremental            =  Syncs to binary logs only"
    echo "                                       (default: if no action defined"
    echo "            full                   =  Syncs logs, dumps all databases,"
    echo "                                       and purges stale binary logs"
    echo ""
    echo "eg: $0 --user root --pass mypass \\"
    echo "     --data /m1/mysql --target /m2/Backup/mysql full"
    echo ""
    return 1
}


version()
{
    local myname=${0/#.\//}

    echo "$myname, Version $VERSION, $AUTHOR"
    echo ""
}


startSlave()
{    
    local options="--user=$MYSQLUSER"
    local retval=0

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  startSlave()"
        echo "$MYSQLADMIN $options start-slave"
        echo ""
    else
        $MYSQLADMIN $options start-slave
        retval=$?
    fi

    return $retval
}


stopSlave()
{
    local options="--user=$MYSQLUSER"
    local retval=0

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  stopSlave()"
        echo "$MYSQLBIN $options -e 'STOP SLAVE SQL_THREAD;'"
        echo ""
    else
        $MYSQLBIN $options -e 'STOP SLAVE SQL_THREAD;'
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
    local retval=0

    if [ -n "$MYSQLPASS" ]; then
        options="${options} --password=$MYSQLPASS"
    else
        options="${options} -p"
    fi

    options="${options} $MYSQLDUMP_OPTIONS"

    if [ -e $targetdbfile ]; then
        echo "Error: target file already exists: '$targetdbfile'"
        return 1
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  dumpSlave()"
        echo "$MYSQLDUMP $options | gzip  > $targetdbfile "
        echo ""
    else
        $MYSQLDUMP $options | gzip > $targetdbfile
        retval=$?
    fi

    return $retval
}


syncLogs()
{
    local targetpath=$1
    local options="-a"
    local retval=0

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
            echo "$RSYNC $options $MYSQLDIR/$FILE $targetpath/$FILE"
            $RSYNC $options $MYSQLDIR/$FILE $targetpath/$FILE
        else
            $RSYNC $options $MYSQLDIR/$FILE $targetpath/$FILE
        fi
        MASTER_LASTLOG=$FILE;
    done
    echo ""


    return $retval
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

    if [ -z "$lastlog" ]; then
        echo "error, no lastlog set"
        return 1
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "Current Master log is $lastlog"
        echo "mysql $options -e 'PURGE BINARY LOGS TO '$lastlog';'"
        echo ""
        RETVAL=0
    else
        mysql $options -e "PURGE BINARY LOGS TO '$lastlog';"
        RETVAL=$?
    fi

    return 0
}


checkTarget()
{
    local targetpath="$TARGET/$HOSTNAME/binlog"

    if [ -d $targetpath ]; then
        rm -rf $targetpath
        RETVAL=$?
        if [ $RETVAL -eq 1 ]; then
            echo "Failed to clear $targetpath"
            return $RETVAL
        fi
    fi

    if [ $DRYRUN -eq 1 ]; then
        echo "  checkTarget()"
        echo "mkdir -p $targetpath"
        echo ""
        RETVAL=0
    else
        mkdir -p $targetpath
        RETVAL=$?
    fi


    return $RETVAL;
}


# ---------------------------------
#  MAIN

# process command arguments
while [ $# -gt 0 ]; do
    case "$1" in
        -b|--binlog)
            BINLOGNAME="$2"
            shift
            if [ -z "$BINLOGNAME" ]; then
                usage
                exit 1
            fi
            ;;
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

# ensure we have required options
if [ -z "$MYSQLUSER" ] || [ -z "$TARGET" ]; then
    usage
    exit 1
fi

# setup our targets
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

#  ensure target is clear and path is writeable
checkTarget $targetbinlog
RETVAL=$?
if [ $RETVAL -eq 1 ]; then
    exit 1
fi

#  halt the slave replication thread
stopSlave
RETVAL=$?
if [ $RETVAL -eq 1 ]; then
    exit 1
else
    if [ $DRYRUN -eq 1 ]; then
        echo "Slave stopped successfully (not really!)"
    else
        echo "Slave stopped"
    fi
fi

#  synchronize the master bin logs
syncLogs $targetbinlog
RETVAL=$?
if [ $RETVAL -eq 1 ]; then
    exit 1
fi

#  perform full db dump if desired
if [ $FULL -eq 1 ]; then
    dumpSlave $targetpath $savedate
    RETVAL=$?
    if [ $RETVAL -eq 1 ]; then
        exit 1
    fi
fi

#  restart slave replication
startSlave
RETVAL=$?
if [ $RETVAL -eq 1 ]; then
    exit 1
fi

#  purge stale/old master bin logs if desired
if [ $FULL -eq 1 ]; then
    purgeLogs
    RETVAL=$?
    if [ $RETVAL -eq 1 ]; then
        exit 1
    else 
        if [ $DRYRUN -eq 1 ]; then
            echo "Logs purged (not really!)"
        else
            echo "Logs purged"
        fi
    fi
fi

exit 0


