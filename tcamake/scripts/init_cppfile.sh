#!/bin/bash
#
#       init_sourcefile.sh
#
#   creates a template source file with ifndef/define declarations
#   and namespace declarations, if provided.


VERSION="1"
AUTHOR="tcarland@gmail.com"
MYNAME="init_sourcefile.sh"

SOURCENAME=
NAMESPACE=
EXT=
DEFNAME=

usage()
{
    echo ""
    echo "Usage: $MYNAME -n <namespace>  sourcefile"
    echo ""
    return 0
}

version()
{
    echo "$MYNAME, Version $VERSION, $AUTHOR"
    echo ""
}


setSourceName()
{
    local filename=$1
    local result=${filename##*/}
    
    SOURCENAME=${result%.*}
    EXT=${result##*.}

    return 0 
}

setDefineName()
{
    local filename=$1
    local namespace=$2
    local def=
    local Sr=
    local Nr=
    local Er=

    Sr="`echo $SOURCENAME | tr '[:lower:]' '[:upper:]'`"
    Er="`echo $EXT | tr '[:lower:]' '[:upper:]'`"

    if [ -n "$namespace" ]; then
        Nr="`echo $namespace | tr '[:lower:]' '[:upper:]'`"
        def="_${Nr}"
    fi

    DEFNAME="${def}_${Sr}_${Er}_"

    return 0
}


createHeader()
{
    local filename=$1
    local namespace=$2
    local class=$3

    echo "#ifndef $DEFNAME" > $filename
    echo "#define $DEFNAME" >> $filename
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "namespace ${namespace} {" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$class" ]; then
        echo "" >> $filename
        echo "class ${SOURCENAME} {" >> $filename
        echo "" >> $filename
        echo "  public:" >> $filename
        echo "" >> $filename
        echo "    ${SOURCENAME}();" >> $filename
        echo "" >> $filename
        echo "    ~${SOURCENAME}();" >> $filename
        echo "" >> $filename
        echo "};" >> $filename
        echo "" >> $filename
    fi
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "}  // namespace" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename
    echo "#endif  //$DEFNAME" >> $filename
}

createSource()
{
    local filename=$1
    local namespace=$2
    local class=$3

    echo "#define $DEFNAME" > $filename
    echo "" >> $filename
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "namespace ${namespace} {" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$class" ]; then
        echo "" >> $filename
        echo "${SOURCENAME}::${SOURCENAME}()" >> $filename
        echo "{" >> $filename
        echo "}" >> $filename
        echo "" >> $filename
        echo "${SOURCENAME}::~${SOURCENAME}()" >> $filename
        echo "{" >> $filename
        echo "}" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "}  // namespace" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    echo "// $DEFNAME" >> $filename
}



# --------------------------
#  MAIN

CLASS=1
filename=
namespace=


while [ $# -gt 0 ]; do
    case "$1" in
        -C)
            CLASS=
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -n|--namespace)
            shift
            namespace=$1
            ;;
        *)
            filename=$1
            ;;
    esac
    shift
done



setSourceName $filename
setDefineName $filename $namespace

echo "File: $filename  Sourcename: $SOURCENAME"

testext=${EXT%c*}
if [ -n "$testext" ]; then
    createHeader $filename $namespace $CLASS
else 
    createSource $filename $namespace $CLASS
fi


exit 0
