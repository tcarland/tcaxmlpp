#!/bin/bash
#
#
VERSION="0.2"
AUTHOR="tcarland@gmail.com"
MYNAME=${0/#.\//}

PARENT=".."
TOPDIR="."
LINKLIST="tcamake common"
BUILDDEF="build_defs"
RSYNC="rsync"
OPTIONS="-av"
PATHLIST="include src docs test"
BUILDSH="build.sh"

PROJECT=
DRYRUN=0
RETVAL=0
reply=



usage()
{
    echo ""
    echo "Usage: $MYNAME {options} [projectname] "
    echo "    Options:"
    echo "       -h|--help      =  display usage information"
    echo "       -n|--dryrun    =  enables test mode"
    echo "       -v|--version   =  display version information"
    echo ""
    echo "    Creates a new project in a directory of the same name"
    echo ""
}

findTopDirectory()
{
    local srcdir="$PWD"
    local curdir=""
    local result=""
    
    RETVAL=1

    if [ -z "$BUILDDEF" ] || [ -z "$PARENT" ]; then
        echo "Error in settings"
        return $RETVAL
    fi

    while [ $RETVAL -eq 1 ]
    do
        curdir="${PWD}"
        result=`find . -name "$BUILDDEF"`
        if [ -n "$result" ]; then
            RETVAL=0
        fi
        
        if [ $RETVAL -eq 1 ]; then
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
        return 1
    fi

    return 0
}

isLib()
{
    echo ""
    echo " There are two types of projects; library or binary."
    echo "   Will this project be a library?  (Y/N)"
    read reply

    case "$reply" in
        "y" | "Y" )
            return 0
            ;;
    esac

    return 1
}

createProject()
{
    local srcpath=
    local dstpath="$1/"
    local options="$OPTIONS"
    
    if [ $DRYRUN -eq 1 ]; then
        options="${options} --dry-run"
    fi

    echo "createProject()"

    findTopDirectory
    RETVAL=$?
    if [ $RETVAL -eq 1 ]; then
        echo "Failed to locate tcamake"
        return 1
    fi
    
    srcpath="$TOPDIR/tcamake/template"

    isLib
    RETVAL=$?

    if [ $RETVAL -eq 0 ]; then
        srcpath="$srcpath/common/libproject/"
    else
        srcpath="$srcpath/project/"
    fi

    echo "$RSYNC $options $srcpath $dstpath"
    $RSYNC $options $srcpath $dstpath
    echo ""

    cd $dstpath
    findTopDirectory
    
    RETVAL=0
    for subdir in $PATHLIST; do
        if [ ! -d $subdir ]; then
            if [ $DRYRUN -eq 1 ]; then
                echo "mkdir -p $subdir"
                RETVAL=0
            else
                echo "mkdir -p $subdir"
                mkdir -p $subdir
                RETVAL=$?
            fi
        fi
        if [ $RETVAL -eq 1 ]; then
            return 1
        fi
    done

    if [ $DRYRUN -eq 1 ]; then
        echo "ln -s ${TOPDIR}/tcamake/scripts/build.sh"
    else
        if [ ! -e $BUILDSH ]; then
            ln -s "${TOPDIR}/tcamake/scripts/build.sh"
        fi
    fi

    return 0
}


# --------------------------
#  MAIN


while [ $# -gt 0 ]; do
    case "$1" in
        -n|--dry-run)
            DRYRUN=1
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -v|--version)
            version
            exit 0
            ;;
        *)
            PROJECT=$1
            ;;
    esac
    shift
done



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
        RETVAL=$?
        if [ $RETVAL -eq 1 ]; then
            echo "Error creating target directory '$PROJECT'"
            exit 1
        fi
    fi
fi

createProject $PROJECT
RETVAL=$?

if [ $RETVAL -eq 0 ]; then
    echo "tcamake_new.sh finished successfully"
else
    echo "tcamake_new.sh finished with errors"
fi

exit $RETVAL

