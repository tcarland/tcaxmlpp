#!/bin/bash
#
#   TcaNms init script 
#
#   The script is used for starting, stopping, and verifying TcaNms services.
#
#

SYSHOME=""
CURDIR=`dirname $0`

if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

if [ -n "$TCANMS_HOME" ] && [ -d $TCANMS_HOME ]; then
    SYSHOME="$TCANMS_HOME"
fi
if [ -z "$SYSHOME" ] && [ -d "$TCANMS_PREFIX" ]; then
    SYSHOME="$TCANMS_PREFIX"
fi
if [ -z "$SYSHOME" ]; then
    SYSHOME="$CURDIR"
fi

echo ""
echo "init.tcanms:"
echo ""
echo "  current dir: $CURDIR"
echo "  tcanms home: $SYSHOME"
echo ""


CONFIGDIR="$SYSHOME/etc"


if [ -z "$RC_TCANMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tcanmsrc ]; then
        echo "Using rc file from $CONFIGDIR"
        source $CONFIGPATH/tcanmsrc
    elif [ -e $HOME/tcanmsrc ]; then
        echo "Using rc file from $HOME"
        source $HOME/tcanmsrc
    elif [ -e $CURDIR/tcanmsrc ]; then
        echo "Using rc file from $CURDIR"
        source $CURDIR/tcanmsrc
    else
        echo "Failed to locate rc file"
        exit 1
    fi
fi



if [ -z "$RC_TCANMS_FUNCTIONS" ] && [ -e ${SYSHOME}/bin/init.tcanms_functions.sh ]; then
    source ${SYSHOME}/bin/init.tcanms_functions.sh
fi
if [ -z "$RC_TCANMS_FUNCTIONS" ] && [ -e ./bin/init.tcanms_functions.sh ]; then
    source ./bin/init.tcanms_functions.sh
fi
if [ -z "$RC_TCANMS_FUNCTIONS" ] && [ -e ./init.tcanms_functions.sh ]; then
    source ./init.tcanms_functions.sh
fi

if [ -z "$RC_TCANMS_FUNCTIONS" ]; then
    echo "Failed to locate init.tcanms_functions.sh"
    exit 1
fi



BINDIR="$SYSHOME/bin"
RUNDIR="$SYSHOME/run"
LOGDIR="$SYSHOME/logs"

export INITERRLOG="$SYSHOME/tmp/init_error.log"
export PROCESS_STATUS_FILE="$SYSHOME/tmp/init_status.log"
echo -n "PROCESS_STATUS_CODE=" >> $PROCESS_STATUS_FILE
INITERR=0



start_services()
{
    local proc=$1
    local key=$2
    local retval=0

    echo "Starting services..."

    get_process_list $proc $key

    retval=$?
    if [ $retval -eq 0 ]; then
        echo "No services found"
        return 0
    fi

    if [ -n "$proc" ] && [ -n "$key" ]; then
        echo "Starting service $proc for $key "

        get_config_list $proc $key

        retval=$?
        if [ $retval -eq 0 ]; then
            echo "  Config not found for $proc $key" >> $INITERRLOG
            return 0
        else
            
            start_process $proc $key $CONFIGLIST

        fi
    else
            
        for svc in $PROCLIST; do
            echo "Starting processes for service: $svc"

            get_config_list $svc
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "  Config not found for $proc $key" >> $INITERRLOG
                return 0
            else
                
                start_all_processes $svc

                retval=$?

                if [ $retval -gt 0 ]; then
                    echo "  Error starting $retval processes for $svc" >> $INITERRLOG
                fi
            fi
        done
    fi
    
    return 1
}


stop_services()
{
    local proc=$1
    local key=$2
    local retval=

    echo "Stopping services..."

    get_process_list $proc $key

    retval=$?

    if [ $retval -eq 0 ]; then
        echo "  No services found"
        return 0
    fi

    if [ -n "$proc" ] && [ -n "$key" ]; then
        echo "Stopping service $proc for $key "

        get_config_list $proc $key

        retval=$?

        if [ $retval -eq 0 ]; then
            echo "Config not found for $proc $key"
            return 0
        else
            
            stop_process $proc $key $CONFIGLIST

        fi
    else
            
        for svc in $PROCLIST; do
            echo "Stopping processes for service: $svc"

            get_config_list $svc
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "  Config not found for $svc"
                return 0
            else
                
                stop_all_processes $svc

                retval=$?

                if [ $retval -gt 0 ]; then
                    echo "  Error stopping $retval processes for $svc" >> $INITERRLOG
                fi

            fi
        done
    fi
    
    return 1
}


restart_services()
{
    local proc=$1
    local key=$2
    local retval=
    local result=0

    get_process_list $proc $key

    retval=$?

    if [ $retval -eq 0 ]; then
        echo "  No processes found" >> $INITERRLOG
        return 0
    fi

    if [ -n "$proc" ] && [ -n "$key" ]; then
        echo "Restarting service $proc for $key "

        get_config_list $proc $key

        retval=$?

        if [ $retval -eq 0 ]; then
            echo "  Config not found for $proc $key" >> $INITERRLOG
            return 0
        else
            
            restart_process $proc $key $CONFIGLIST

            retval=$?

            if [ $retval -eq 0 ]; then
                echo "Error restarting service $proc $key" >> $INITERRLOG
                return 0
            fi
        fi

    else
            
        for svc in $PROCLIST; do
            echo "Restarting any stopped processes for service: $svc"

            get_config_list $svc
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "  Config not found for $svc" >> $INITERRLOG
                return 0
            else
                
                restart_all_processes $svc

                retval=$?

                if [ $retval -gt 0 ]; then
                    echo "  Restarted $retval processes for $svc" 
                    ((result += 1))
                fi
            fi
        done
    fi

    if [ $result -gt 0 ]; then
        return 0
    fi

    return 1
}


info_services()
{
    local proc=$1
    local key=$2
    local retval=
    local result=

    result=0

    echo "Verifying services..."

    get_process_list $proc $key

    retval=$?

    if [ $retval -eq 0 ]; then
        echo "  No services found" >> $INITERRLOG
        return 0
    fi

    if [ -n "$proc" ] && [ -n "$key" ]; then
        echo "Checking service $proc for $key "

        check_process $proc $key

        retval=$?

        if [ $retval -eq 0 ]; then
	    source $PROCESS_STATUS_FILE
            echo "  Process $proc $key:  [ ${PROCESS_STATUS_CODE} ]" >> $INITERRLOG
	    echo "  Process $proc $key:  [ ${PROCESS_STATUS_CODE} ]"
            result=1
        else
            echo "  Process $proc $key:   [ OK ]"
        fi

    else
        for svc in $PROCLIST; do
            echo "Checking processes for service: $svc"

            get_config_list $svc
            
            retval=$?

            if [ $retval -eq 0 ]; then
                echo "  Configs not found for $svc" >> $INITERRLOG
                return 0
            else
                
                verify_processes $svc

                retval=$?

                result=`expr $result + $retval`
            fi
        done
    fi

    if [ $result -gt 0 ]; then
        echo "Services not running: $result" >> $INITERRLOG
        return 0
    else
        echo "All services running"
    fi

    return 1
}


usage()
{
    echo ""
    echo "Usage: $0 {start|stop|restart|info}  [service]  [key]"
    echo ""
    echo "  service = service name to start"
    echo "  key     = process identifier "
    echo ""
    echo "  Note: 'restart' will only restart services determined not currently "
    echo "  running. To force restart of a process stop|start must be used."
    echo ""
    return 1
}


retval=

case "$1" in
    'start')
        start_services $2 $3
        ;;
    'stop')
        stop_services $2 $3
        ;;
    'restart')
        restart_services $2 $3
        ;;
    'info')
        info_services $2 $3
        ;;
    *)
        usage
esac


retval=$?

if [ $retval -eq 0 ]; then
    if [ -n "${EMAIL_ON_ERROR}" ] && [ ${EMAIL_ON_ERROR} == 1 ]; then
        send_errlog $INITERRLOG
    else
	echo
	echo "Error Log:"
        cat $INITERRLOG 
    fi
fi

rm -f $INITERRLOG
rm -f $PROCESS_STATUS_FILE

exit

