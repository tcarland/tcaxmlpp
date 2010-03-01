#!/bin/bash
#
#       init_cppfile.sh
#
#   creates a template source file with ifndef/define declarations
#   and namespace declarations, if provided.


VERSION="1.2"
AUTHOR="tcarland@gmail.com"
PNAME=${0##*\/}

SOURCENAME=
NAMESPACE=
EXT=
DEFNAME=
CLASS=1



version()
{
    echo "$PNAME, Version $VERSION, $AUTHOR"
    echo ""
}

usage()
{
    echo ""
    echo "Usage: $PNAME [-hCV] -n <namespace>  sourcefile"
    echo ""
    echo "    --help      | -h        : Show usage info and exit"
    echo "    --noclass   | -C        : Do not autogenerate cpp class"
    echo "    --namespace | -n <name> : Creates namespace tags accordingly"
    echo "    --version   | -V        : Show version info and exit"
    echo ""
    version
    return 0
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

    echo "#ifndef $DEFNAME" > $filename
    echo "#define $DEFNAME" >> $filename
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "namespace ${namespace} {" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$CLASS" ]; then
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
    echo "#endif  // $DEFNAME" >> $filename
}

createSource()
{
    local filename=$1
    local namespace=$2

    echo "#define $DEFNAME" > $filename
    echo "" >> $filename
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "namespace ${namespace} {" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$CLASS" ]; then
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

filename=
namespace=


while [ $# -gt 0 ]; do
    case "$1" in
        -C|--noclass)
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
        -V|--version)
            version
            exit 0
            ;;
        *)
            filename=$1
            ;;
    esac
    shift
done


if [ -z "$filename" ]; then
    usage
    exit 0
fi

if [ -e $filename ]; then
    echo "File '$filename' already exists. Overwrite? (y/n)"
    read reply

    case "$reply" in
        "y" | "Y")
            echo "Overwriting $filename"
            ;;
        *)
            echo "Aborting."
            exit 0
            ;;
    esac
else
    echo "Generating source file $filename"
fi

setSourceName $filename
setDefineName $filename $namespace

#echo "File: $filename  Sourcename: $SOURCENAME"

testext=${EXT%c*}
if [ -n "$testext" ]; then
    createHeader $filename $namespace
else 
    createSource $filename $namespace
fi


exit 0
