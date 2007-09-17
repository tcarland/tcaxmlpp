#!/bin/bash
#
#   Build script for one-time building of external libraries.
#

PWD=`pwd`
DOTFILE=".external"
TARGETLIST="all"
retval=


check_state()
{
    if [ -e $DOTFILE ]; then
        return 0
    fi

    return 1
}


build_targets()
{
    for target in $TARGETLIST; do
        make $target
        retval=$?
    
        if [ $retval -ne 0 ]; then
            echo "Error building external libraries."
            return $result
        fi
    done
    return 0
}


if [ -n "$1" ]; then

    TARGETLIST="$1"
    build_targets
    retval=$?

else

    check_state
    retval=$?

    if [ $retval -eq 0 ]; then
        echo "External dependencies already built. Skipping..."
        exit 0
    fi

    build_targets
 
    if [ $retval -ne 0 ]; then
        echo "Error building external libraries."
    else
        echo "External libraries built..."
        touch ${PWD}/${DOTFILE}
    fi

fi


exit $retval

