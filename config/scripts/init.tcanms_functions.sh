#
# init.tcanms_functions.sh

RC_TCANMS_FUNCTIONS="true"
PROCKEY=
PIDFILE=
PID=

check_binary_is_script()
{
    local BIN=${BINDIR}/$1
    local IS_SCRIPT=`file $BIN | sed -n '/script/p'`
    if [ "$IS_SCRIPT" != "" ]; then
        return 1
    else
        return 0
    fi
}

get_process_list()
{
    local PROC=$1
    local FLIST=

    if [  ! -d $CONFIGDIR ]; then
        return 0
    fi
        
    cd $CONFIGDIR

    FLIST=`ls -1`

    for f in $FLIST; do
        if [ -d $f ] && [ -e "${BINDIR}/${f}" ]; then
            if [ -n "$PROC" ]; then
                if [ "$PROC" == "$f" ]; then
                    PROCLIST="$PROCLIST $f"
                fi
            else
                PROCLIST="$PROCLIST $f"
            fi
        fi
    done

    if [ -z "$PROCLIST" ]; then
        return 0
    fi

    return 1
}


get_config_list()
{
    local PROC=$1
    local KEY=$2
    local LIST=
    
    if [  ! -d $CONFIGDIR ]; then
        return 0
    fi
        
    cd $CONFIGDIR

    CONFIGLIST=""
    LIST=`find . -name "*.xml" -print | perl -n -e 'm/.+\/(.+)\/(.+_.+xml)$/ && print "$1/$2\n"'`

    for cfg in $LIST; do

        if [ -n "$PROC" ]; then
            if echo "$cfg" | grep $PROC | grep -v grep 1> /dev/null 2> /dev/null; then
                if [ -n "$KEY" ]; then
                    if echo "$cfg" | grep "${PROC}_${KEY}.xml" | grep -v grep 1> /dev/null 2> /dev/null; then
                        CONFIGLIST="$CONFIGLIST $cfg"
                    fi
                else
                    CONFIGLIST="$CONFIGLIST $cfg"
                fi
            fi
        else
            CONFIGLIST="$CONFIGLIST $cfg"
        fi

    done

    if [ -z "$CONFIGLIST" ]; then
        return 0
    fi

    return 1
}


check_process_pid()
{
    local pid=$1

    if ps ax | grep $pid | grep -v grep 1> /dev/null 2> /dev/null ; then
        return 1;
    fi

    echo "DEAD" >> ${PROCESS_STATUS_FILE}

    return 0
}


check_process()
{
    local bin=$1
    local key=$2
    local retval=0

    check_binary_is_script $bin
    local is_script=$?

    # Is the binary a script?  If so, pass the call through
    if [ $is_script -ne 0 ]; then
	${BINDIR}/$bin status $key
	PID=$?  # Just set PID to 0 or 1, and return it
	return $PID
    fi

    PIDFILE="${RUNDIR}/${bin}_${key}.pid"
    PID=0

    if [ -r $PIDFILE ]; then
        PID=`cat $PIDFILE`

        check_process_pid $PID

        retval=$?

        if [ $retval -eq 0 ] ; then
            echo "DEAD" >> ${PROCESS_STATUS_FILE}
            echo "  Removing stale pid file: $PIDFILE"
            rm -f $PIDFILE
            PID=0
        fi
    fi

    return $retval
}


get_config_key()
{
    local cfg=$1

    PROCKEY=`echo $cfg | perl -n -e 'm/.+\/.+_(.+)\.xml$/ && print "$1"'`

    if [ -z "$PROCKEY" ]; then
        return 0
    fi

    return 1
}


start_all_processes()
{
    local bin=$1
    local result=0
    local cfg=
    local key=
    local retval=

    for cfg in $CONFIGLIST; do

        get_config_key $cfg

        retval=$?
        key=$PROCKEY

        if [ $retval -eq 0 ]; then
            echo "failed to determine process key for $cfg"
            ((result += 1))
        else
            
            start_process $bin $key $cfg
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "Failed to start process $bin $cfg"
                ((result += 1))
            fi
        fi
    done

    return $result
}


start_process()
{
    local bin=$1
    local key=$2
    local cfg=$3
    local pid=
    local retval=0
    local dbin=

    if [ -z "$bin" ] || [ -z "$key" ]; then
        echo "No valid process or key given"
        return -1
    fi

    check_process $bin $key

    check_binary_is_script $bin
    local is_script=$?

    # Is the binary a script?  If so, pass the call through
    if [ $is_script -gt 0 ]; then
        ${BINDIR}/$bin start ${CONFIGDIR}/${cfg}
        if [ $? != 1 ]; then
            return $?
        fi
        echo "Process $bin started"
    else
        if [ $PID -gt 0 ]; then
            echo "Process $PID is already running..stop first"
            return 0
        fi

        if [ -n "$DEBUG" ]; then
            dbin="$bin -d"
            echo "'${BINDIR}/${dbin} -c ${CONFIGDIR}/${cfg} &'"
        else
            dbin="$bin -D"
        fi
    
        $BINDIR/$dbin -c $CONFIGDIR/$cfg 1> /dev/null 2> /dev/null &
        echo ""
        sleep 1

        pid=`ps awwwx | grep "$bin" | grep "$cfg" | grep -v "grep" | grep -v "init" | awk '{ print $1 }'`
        echo ""
    
        echo $pid > $PIDFILE
        echo "Process $bin started: $pid"
    fi

    if [ $TCANMS_USE_CRON -eq 1 ]; then
        install_cron_task $bin $key
    fi
    return 1
}


stop_all_processes()
{
    local bin=$1
    local result=0
    local cfg=
    local key=
    local retval=

    for cfg in $CONFIGLIST; do

        get_config_key $cfg

        retval=$?
        key=$PROCKEY

        if [ $retval -eq 0 ]; then
            echo "  Failed to determine process key for $cfg"
            ((result += 1))
        else
            
            stop_process $bin $key $cfg
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "  Failed to properly stop process $bin $cfg"
                ((result += 1))
            fi
        fi
    done

    return $result
}


stop_process()
{
    local bin=$1
    local key=$2
    local retval=0

    if [ -z "$bin" ] || [ -z "$key" ]; then
        echo "No valid process or key given"
        return 0 
    fi

    check_binary_is_script $bin
    local is_script=$?

    check_process $bin $key
    retval=$?

    # Is the binary a script?  If so, pass the call through
    if [ $is_script -gt 0 ]; then
        echo "Killing process"
        ${BINDIR}/$bin stop $key
        if [ $? != 1 ]; then
            echo "Failed to terminate process" >> $INITERRLOG
            return 0
        fi
    else
        if [ $retval -eq 0 ]; then
            return 1
        fi

        echo "Killing process $PID"
        kill $PID
        sleep 3

        check_process_pid $PID

        retval=$?

        if [ $retval -gt 0 ] ; then
            echo "  Sending SIGKILL"
       	    kill -9 $PID
        else
            echo "  Process terminated"
        fi

        # check again to enforce removal of pidfile
        check_process $bin $key

        fi
    if [ $TCANMS_USE_CRON -eq 1 ]; then
        uninstall_cron_task $bin $key
    fi
    return 1
}


#  Will restart any processes detected as not running and will email alert
#  any restarts to ${TCANMS_EMAIL_ADDRESS}
#
restart_all_processes()
{
    local bin=$1
    local cfg=
    local key=
    local retval=
    local result=0

    for cfg in $CONFIGLIST; do

        get_config_key $cfg

        retval=$?
        key=$PROCKEY

        if [ $retval -eq 0 ]; then
            echo "Failed to determine key for $bin $cfg" >> $INITERRLOG
        else

            check_process $bin $key

            retval=$?

            if [ $retval -eq 0 ]; then
                echo "Process for $bin $key is dead, restarting..."

                start_process $bin $key $cfg

                retval=$?

                if [ $retval -eq 0 ]; then
                    echo "Error starting service $bin $key" >> $INITERRLOG
                else
                    echo "Restarted service $bin $key " >> $INITERRLOG
                fi
                ((result += 1))
            fi
        fi
    done
    
    return $result
}


verify_processes()
{
    local bin=$1
    local cfg=
    local key=
    local retval=
    local result=

    result=0

    for cfg in $CONFIGLIST; do

        get_config_key $cfg

        retval=$?
        key=$PROCKEY

        if [ $retval -eq 0 ]; then
            echo "  Failed to determine key for $bin $cfg" >> $INITERRLOG
        else
            check_process $bin $key

            retval=$?

            if [ $retval -eq 0 ]; then
                if [ -z "$NOEMAIL_ON_ERR" ]; then
                    echo "  Process $bin $key :  [ DEAD ]" >> $INITERRLOG
                fi
                echo "  Process $bin $key :  [ DEAD ]"
                ((result += 1))
            else
                echo "  Process $bin $key : $PID  [ OK ]"
            fi
        fi
    done
    
    return $result
}


restart_process()
{
    local retval=
    stop_process $1 $2
    sleep 1
    start_process $1 $2 $3
    retval=$?
    return $retval
}


send_errlog()
{
    local errlog=$1

    if [ -e $errlog ]; then
        mailx -s '[tcanms] init.tcanms report' ${TCANMS_EMAIL_ADDRESS} < $errlog
    fi

    return 1
}

uninstall_cron_task()
{
    local bin=$1
    local key=$2
    local bin_name=`basename $0`

    echo "Uninstalling cron task for ${cfg}..."
    local cron_file=${TCANMS_TMP}/crontab.tmp

    # Be sure cfg doesn't have slashes in it
    crontab -l | sed /"${bin_name} info ${bin} ${key}"/d > ${cron_file}
    crontab ${cron_file}
}

install_cron_task()
{
    local bin=$1
    local key=$2
    local bin_name=`basename $0`
    local bin_path=${TCANMS_SBIN}/${bin_name}

    # First remove any previously installed cron task for this key/cfg pair
    uninstall_cron_task ${bin} ${key}

    # Now add the cron task for this key/cfg
    # Be sure cfg doesn't have slashes in it
    local cron_file=${TCANMS_TMP}/crontab.tmp

    crontab -l > ${cron_file}

    echo "*/${TCANMS_CRON_INTERVAL} * * * * export EMAIL_ON_ERROR=1 && ${bin_path} info ${bin} ${key} >> /dev/null 2>&1" >> ${cron_file}
    echo "Installing cron task for ${bin}/${key}..."

    crontab ${cron_file}	
}

