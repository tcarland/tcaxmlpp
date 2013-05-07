#!/bin/bash
#
#   Build script for projects that may wish to be extracted or 
#   distributed individually from the overall workspace. 
#   Primarily created for making project distributions via the 
#   'build.sh dist' command.
#

PNAME=${0##*\/}
VERSION="1.17"
AUTHOR="tcarland@gmail.com"

PARENT=".."

LINKLIST="tcamake"
TMDEPFILE="tcamake_depends"
DODIST=0
RSYNC="rsync"
OPTIONS="-avL --delete --exclude=.cvs --exclude=.svn --exclude=.hg --exclude=.git "
DRYRUN="--dry-run"

dry=
retval=0

if [ -z "$TOPDIR" ]; then
    TOPDIR="."
fi

if [ -n "$TCAMAKE_BUILD_LINKS" ]; then
    LINKLIST="$TCAMAKE_BUILD_LINKS $LINKLIST"
fi

usage()
{
    echo ""
    echo "Usage: $PNAME [-hn] [command] {args} "
    echo ""
    echo "   [command] :  a standard 'make' target or one of the "
    echo "                following commands."
    echo ""
    echo "       -h|--help   : displays this help"
    echo "       -n|--dryrun : enables dry-run test mode" 
    echo ""
    echo "       'dist' [path/project] "
    echo "                   : takes a valid path as addt. argument"
    echo "                     Syncs the project to 'path/project'"
    echo "       'link'      : Creates project build links only"
    echo "       'unlink'    : Removes build links only"
    echo "       'clean'     : Removes build links and runs 'make clean'"
    echo "       'show'      : shows the determined project root and "
    echo "                     what links would be created. (dry run) "
    echo ""
    echo " Summary: Creates a complete distribution directory that"
    echo " includes required project paths by creating temporary "
    echo " soft links. Any unrecognized commands are passed through"
    echo " to 'make'."
    echo " Additional project links can be defined by setting the "
    echo " EnvVar TCAMAKE_BUILD_LINKS to the list of relative paths"
    echo " from TOPDIR.";
    echo ""
    echo "  $PNAME: Version: $VERSION by $AUTHOR"
    echo ""
}


findTopDirectory()
{
    local srcdir="$PWD"
    local curdir=""
    local result=""
    
    retval=0

    while [ $retval -eq 0 ]
    do
        curdir="${PWD}"
        result=`find . -name "$TMDEPFILE"`
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

    echo "  <tcamake> project root set to '$TOPDIR'"

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
    echo "  <tcamake> generating links: $LINKLIST "
    
    for lf in $LINKLIST; do
        echo "  ln -s $TOPDIR/$lf ."
        ln -s "$TOPDIR/$lf"
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
    local target="$1"
    local curdir=$PWD
    local options="$OPTIONS"
    local dstpath=

    if [ -z "$target" ]; then
        echo " 'dist' requires a target path"
        return 0
    fi

    if [ -n "$dry" ]; then
        echo "  Dry run enabled."
        options="${options}${DRYRUN}"
    fi

    dstpath="${target}/"

    if [ -e $dstpath ]; then
        echo "WARNING! target path of '$dstpath' already exists."
        echo "If you continue, the contents will be overwritten"
        echo "Are you sure you wish to continue? (Y/N)"
        read reply

        case "$reply" in
            "Y" | "y" | "yes" | "YES")
                ;;
            *)
                return 0
                ;;
        esac
    fi

    echo "$RSYNC $options ./ $dstpath"
    $RSYNC $options ./ $dstpath

    return 0
}




if [ -z "$1" ]; then
    usage
    exit 0
fi


while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -n|--dryrun)
            dry=1
            shift
            ;;
         *)
            break
            ;;
    esac
done
        
case "$1" in
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
echo "  $PNAME running in '$PWD'"

findTopDirectory
retval=$?

if [ $retval -eq 0 ]; then
    echo "$PNAME Failed to find the 'tcamake' workspace root directory"
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
