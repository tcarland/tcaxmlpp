#!/bin/bash
#

PARENT=".."
TOPDIR="."
LINKLIST="tcamake common"
BUILDDEF="builddefs"
DODIST=0
retval=0


findTopDirectory()
{
    local srcdir="$PWD"
    local curdir=""
    local result=""
    
    retval=0

    #echo ""
    #echo "Searching for Project root starting from:"
    #echo "  $srcdir"
    
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
  
    # ensure we return to source directory
    cd $srcdir

    if [ -z "$TOPDIR" ]; then
        return 0
    fi

    echo "Project root set to '$TOPDIR'"

    return 1
}

clearLinks()
{
    for lf in $LINKLIST; do
        if [ -L $lf ]; then
            #echo "Removing link '$lf'"
            unlink $lf
        fi
    done
    return 1
}

makeLinks()
{
    #echo "Making links: $LINKLIST"
    for lf in $LINKLIST; do
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
    return 0
}


usage()
{
    echo ""
    echo "Usage: $0 [command] {option} "
    echo ""
    echo "   [command] :  a standard 'make' target (eg. all, clean, etc) "
    echo "                or one of the following commands"
    echo "             = 'dist'  : requires a valid path as {option}"
    echo "                Builds the complete distribution in the 'path'"
    echo "             = 'link'  : Creates project links only"
    echo "             = 'clean' : Removes project links and runs 'make clean'"
    echo ""
    echo ""
    echo "   Summary: $0  creates a complete distribution directory "
    echo "      that includes required project paths by creating "
    echo "      temporary soft links. Any unrecognized commands are "
    echo "      passed through to 'make'"
    echo ""
}

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
    *)
        usage
        exit 0
        ;;
esac

echo ""
echo "Building in $PWD"

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
    doDist
fi

clearLinks

exit 0
