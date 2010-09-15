#!/bin/bash
#
#   Build script for individual projects, or any sub-project that might wish 
#   to be extracted from the overall workspace.
#
#   Essentially provides a method to link the make environment within a 
#   standalone project for exporting or distributing individually.


PNAME=${0##*\/}
VERSION="1.12"
AUTHOR="tcarland@gmail.com"

PARENT=".."
TOPDIR="."
LINKLIST="tcamake"
BUILDDEF="build_defs"
DODIST=0
RSYNC="rsync"
OPTIONS="-avL --delete"
DRYRUN=" --dry-run"
retval=0


findTopDirectory()
{
    local srcdir="$PWD"
    local curdir=""
    local result=""
    
    retval=0

    while [ $retval -eq 0 ]
    do
        curdir="${PWD}"
        result=`find . -name "$BUILDDEF"`
        if [ -n "$result" ]; then
            retval=1
        fi
        
        if [ $retval -eq 0 ]; then
            if [ $TOPDIR == "." ]; then
                TOPDIR=""
            else
                TOPDIR="${TOPDIR}/"
            fi

            cd $PARENT
            TOPDIR="${TOPDIR}${PARENT}"
        fi
    done
  
    cd $srcdir

    if [ -z "$TOPDIR" ]; then
        return 0
    fi

    echo "  Project root set to '$TOPDIR'"

    return 1
}


clearLinks()
{
    for lf in $LINKLIST; do
        if [ -L $lf ]; then
            unlink $lf
        fi
    done
    return 1
}


makeLinks()
{
    local dryrun=$1

    echo "  Making links: $LINKLIST "
    
    for lf in $LINKLIST; do
        if [ -n "$dryrun" ]; then
            echo "  ln -s $TOPDIR/$lf ."
        else
            ln -s "$TOPDIR/$lf"
        fi
    done
    return 1
}


doBuild()
{
    local target=$1

    echo ""
    if [ -n "$target" ]; then
        make $target
    else
        make all
    fi
    echo ""
    retval=$?

    return $retval
}


doDist()
{
    local target=$1
    local dryrun=$2
    local curdir=$PWD
    local pname=`basename $curdir`
    local options="$OPTIONS"
    local dstpath=

    if [ -z "$target" ]; then
        echo " 'dist' requires a target path"
        return 0
    fi

    dstpath="${target}/${pname}"

    if [ -e $dstpath ]; then
        echo "WARNING! target path of '$dstpath' already exists."
        echo "If you continue, the contents will be overwritten"
        echo "Are you sure you wish to continue? (Y/N)"
        read reply

        case "$reply" in
            "Y" | "y")
                ;;
            *)
                return 0
                ;;
        esac
    fi

    if [ -n "$dryrun" ]; then
        options="${options}${dryrun}"
    fi

    echo "$RSYNC $options ./ $dstpath"
    $RSYNC $options ./ $dstpath

    return 0
}


usage()
{
    echo ""
    echo "Usage: $PNAME [command] {option} "
    echo ""
    echo "   [command] :  a standard 'make' target (eg. all, clean, etc) "
    echo "                or one of the following commands."
    echo ""
    echo "       'dist' [path] <dryrun> "
    echo "                  : requires a valid path as {option}"
    echo "                     Syncs the project to 'path/projectname'"
    echo "       'link'     : Creates project build links only"
    echo "       'unlink'   : Removes build links only"
    echo "       'clean'    : Removes build links and runs 'make clean'"
    echo "       'show'     : shows the determined project root and "
    echo "                     what links would be created. (dry run) "
    echo ""
    echo ""
    echo "   Summary: creates a complete distribution directory "
    echo "      that includes required project paths by creating "
    echo "      temporary soft links. Any unrecognized commands are "
    echo "      passed through to 'make'"
    echo ""
    echo "  $PNAME: Version: $VERSION by $AUTHOR"
    echo ""
}


if [ -z "$1" ]; then
    usage
    exit 0
fi


case "$1" in
    'help')
        usage
        exit 0
        ;;
    '-h')
        usage
        exit 0
        ;;
    '--help')
        usage
        exit 0
        ;;
     'dist')
        DODIST=1
        ;;
     'link')
        findTopDirectory
        makeLinks
        exit 0
        ;;
     'unlink')
        findTopDirectory
        clearLinks
        exit 0
        ;;
     'show')
        findTopDirectory
        makeLinks 1
        exit 0
        ;;
    *)
        ;;
esac

echo ""
echo "  Building in $PWD..."

findTopDirectory
retval=$?

if [ $retval -eq 0 ]; then
    echo "Failed to find project root directory"
    exit 1
fi


clearLinks
makeLinks

if [ $DODIST -eq 0 ]; then
    doBuild $1
else
    doDist $2
fi

clearLinks

exit 0
