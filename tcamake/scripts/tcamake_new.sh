#!/bin/bash

PARENT=".."
TOPDIR="."
LINKLIST="tcamake common"
BUILDDEF="builddefs"
PROJECT=
RSYNC="rsync"
OPTIONS="-av"
DRYRUN="-n"
retval=0


findTopDirectory()
{
    local srcdir="$PWD"
    local curdir=""
    local result=""
    
    retval=0

    if [ -z "$BUILDDEF" ] || [ -z "$PARENT" ]; then
        echo "Error in settings"
        return $retval
    fi

    while [ $retval -eq 0 ]
    do
        curdir="${PWD}"
        result=`find . -name "$BUILDDEF"`
        if [ -n "$result" ]; then
            retval=1
        fi
        
        if [ $retval -eq 0 ]; then
            if [ "$TOPDIR" == "." ]; then
                TOPDIR=""
            else
                TOPDIR="${TOPDIR}/"
            fi

            cd $PARENT
            TOPDIR="${TOPDIR}${PARENT}"
        fi
    done
  
    # ensure we return to source directory
    cd $srcdir

    if [ -z "$TOPDIR" ]; then
        return 0
    fi

    return 1
}

isLib()
{
    echo ""
    echo " There are two types of projects; library or binary."
    echo "   Will this project be a library?  (Y/N)"
    read reply

    case "$reply" in
        "y" | "Y" )
            return 1
            ;;
    esac

    return 0
}

createProject()
{
    local srcpath=
    local dstpath="$1/"
    local dryrun=$2
    local options="$OPTIONS"
    
    if [ -n "$dryrun" ]; then
        options="${options}${DRYRUN}"
    fi

    findTopDirectory
    retval=$?
    if [ $retval -eq 0 ]; then
        echo "Failed to locate tcamake"
        return 0
    fi

    srcpath="$TOPDIR/tcamake/template"

    isLib
    retval=$?

    if [ $retval -eq 1 ]; then
        srcpath="$srcpath/common/libproject/"
    else
        srcpath="$srcpath/project/"
    fi

    $RSYNC $options $srcpath $dstpath

    cd $dstpath

    findTopDirectory

    if [ -n "$dryrun" ]; then
        echo "ln -s ${TOPDIR}/tcamake/scripts/build.sh"
        echo "mkdir -p include"
        echo "mkdir -p src"
    else
        ln -s "${TOPDIR}/tcamake/scripts/build.sh"
        mkdir -p include
        mkdir -p src
    fi

    return 0
}


usage()
{
    echo ""
    echo "Usage: $0 [projectname] {dryrun}"
    echo ""
    echo "    Creates a new project in a directory of the same name"
    echo "    An additional parameter of 1 is a test mode where no "
    echo "    changes occur."
    echo ""
}


PROJECT=$1

if [ -z "$PROJECT" ]; then
    usage
    exit 1
fi


if [ -d "$PROJECT" ]; then
    echo "A directory of that name already exists. Proceed anyway? (Y/N)"
    read reply

    case "$reply" in

        "y" | "Y")
            ;;
        *)
            echo "aborting"
            exit 0
            ;;
    esac
else
    if [ -e "$PROJECT" ]; then
        echo "Error! A file of that name already exists. Aborting..."
        exit 1
    else
        mkdir -p $PROJECT
    fi
fi

createProject $PROJECT $2

exit 0

