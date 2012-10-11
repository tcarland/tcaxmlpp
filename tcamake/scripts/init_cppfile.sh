#!/bin/bash
#
#       init_cppfile.sh
#
#   Creates a template source file with ifndef/define declarations
#   and namespace declarations, if applicable.
#
VERSION="1.31"
AUTHOR="tcarland@gmail.com"
PNAME=${0##*\/}

FILENAMES=
SOURCENAME=
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
    echo "    --noclass   | -C        : Do not autogenerate cpp class header"
    echo "    --namespace | -n <name> : Creates namespace declaration"
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
    local namespace=$1
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
    local srcname=$2
    local define=$3
    local namespace=$4

    echo "/**" > $filename
    echo "  * @file ${filename} " >> $filename
    echo "  * @author " >> $filename
    echo "  * " >> $filename
    echo " **/" >> $filename

    echo "#ifndef $define" >> $filename
    echo "#define $define" >> $filename
    echo "" >> $filename

    if [ -n "$namespace" ]; then
        echo "" >> $filename
        echo "namespace ${namespace} {" >> $filename
        echo "" >> $filename
    fi

    echo "" >> $filename

    if [ -n "$CLASS" ]; then
        echo "" >> $filename
        echo "class ${srcname} {" >> $filename
        echo "" >> $filename
        echo "  public:" >> $filename
        echo "" >> $filename
        echo "    ${srcname}();" >> $filename
        echo "" >> $filename
        echo "    ~${srcname}();" >> $filename
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
    echo "#endif  // $define" >> $filename
}

createSource()
{
    local filename=$1
    local srcname=$2
    local define=$3
    local namespace=$4

    echo "/**" > $filename
    echo "  * @file ${filename} " >> $filename
    echo "  * @author " >> $filename
    echo "  * " >> $filename
    echo " **/" >> $filename

    echo "#define $define" > $filename
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
        echo "${srcname}::${srcname}()" >> $filename
        echo "{" >> $filename
        echo "}" >> $filename
        echo "" >> $filename
        echo "${srcname}::~${srcname}()" >> $filename
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

    echo "// $define" >> $filename
}



# --------------------------
#  MAIN

fname=
namespace=

while [ $# -gt 0 ]; do
    case "$1" in
        -a|--author)
            shift
            AUTHOR=$1
            ;;
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
            FILENAMES="$FILENAMES $1"
            ;;
    esac
    shift
done


if [ -z "$FILENAMES" ]; then
    usage
    exit 0
fi

for fname in $FILENAMES; do

    if [ -e $fname ]; then
        echo "File '$fname' already exists. Overwrite? (y/n)"
        read reply

        case "$reply" in
            "y" | "Y" | "yes" | "YES")
                echo "Overwriting $fname"
                ;;
            *)
                echo "Aborting."
                exit 0
                ;;
        esac
    else
        echo "Generating source file $fname"
    fi

    setSourceName $fname
    setDefineName $namespace

    testext=${EXT%c*}

    if [ -n "$testext" ]; then
        createHeader $fname $SOURCENAME $DEFNAME $namespace
    else 
        createSource $fname $SOURCENAME $DEFNAME $namespace
    fi

done


exit 0
