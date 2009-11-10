#!/bin/bash
#
#  ta_createdbsql.sh
#    Generates the archiver db create statements for the given 
#    database name.
#
VERSION="0.1"
AUTHOR="tcarland@gmail.com"

PNAME=${0/#.\//}
CURDIR=`dirname $0`
CONFIGDIR=


SCHEMA=$1;
SQLFILE=$2

# ------------------------------------------

if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi

CONFIGDIR=$CURDIR

echo ""
echo "$PNAME: "
if [ -z "$RC_TCANMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tcanmsrc ]; then
        echo "  Using rc: $CONFIGDIR/tcanmsrc"
        source $CONFIGDIR/tcanmsrc
    elif [ -e $HOME/tcanms/etc/tcanmsrc ]; then
        echo "  Using rc from: $HOME/tcanms/etc/tcanmsrc"
        source $HOME/tcanms/etc/tcanmsrc
    elif [ -e $HOME/etc/tcanmsrc ]; then
        echo "  Using rc from: $HOME/etc/tcanmsrc"
        source $HOME/etc/tcanmsrc
    elif [ -e $TCANMS_PREFIX/etc/tcanmsrc ]; then
        echo "  Using rc from $TCANMS_PREFIX/etc/tcanmsrc"
        source $TCANMS_PREFIX/etc/tcanmsrc
    else
        echo "Error: Failed to locate rc file: tcanmsrc"
        exit 1
    fi
fi
echo ""

if [ -n "$TCANMS_PREFIX" ]; then
    TCANMS_HOME="$TCANMS_PREFIX"
    TCANMS_TMP="$TCANMS_HOME/tmp"
    TCANMS_ETC="$TCANMS_HOME/etc"
fi

# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $PNAME {schemaname|dbname} {output sql filename}"
    echo ""
}

# ------------------------------------------

if [ -z "$SCHEMA" ] || [ -z "$SQLFILE" ]; then
    usage
    exit 1
fi

echo "  Creating sql for schema '$SCHEMA' in '$SQLFILE'"
echo ""


echo "
SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

-- -----------------------------------------------------
-- Table ${SCHEMA}.element_name
-- -----------------------------------------------------
DROP TABLE IF EXISTS ${SCHEMA}.element_name ;

CREATE  TABLE IF NOT EXISTS ${SCHEMA}.element_name (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  name TEXT NOT NULL ,
  PRIMARY KEY (id) ,
  INDEX element_name_idx USING BTREE (id ASC, name ASC) )
ENGINE = MyISAM;


-- -----------------------------------------------------
-- Table ${SCHEMA}.immediate
-- -----------------------------------------------------
DROP TABLE IF EXISTS ${SCHEMA}.immediate ;

CREATE  TABLE IF NOT EXISTS ${SCHEMA}.immediate (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  last INT UNSIGNED NOT NULL ,
  value_str TEXT NULL ,
  data TEXT NULL ,
  PRIMARY KEY (idx, element_id) ,
  INDEX fk_immediate_idx (element_id ASC) ,
  INDEX immediate_time_idx (element_id ASC, last ASC, idx ASC) ,
  CONSTRAINT fk_immediate_idx
    FOREIGN KEY (element_id )
    REFERENCES ${SCHEMA}.element_name (id )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MEMORY;


-- -----------------------------------------------------
-- Table ${SCHEMA}.short
-- -----------------------------------------------------
DROP TABLE IF EXISTS ${SCHEMA}.short ;

CREATE  TABLE IF NOT EXISTS ${SCHEMA}.short (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  last INT UNSIGNED NOT NULL ,
  value_str TEXT NULL ,
  data TEXT NULL ,
  PRIMARY KEY (idx, element_id) ,
  INDEX fk_short_idx (element_id ASC) ,
  INDEX short_time_idx (idx ASC, element_id ASC, last ASC) ,
  CONSTRAINT fk_short_idx
    FOREIGN KEY (element_id )
    REFERENCES ${SCHEMA}.element_name (id )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MyISAM;

-- -----------------------------------------------------
-- Table $SCHEMA.long
-- -----------------------------------------------------
DROP TABLE IF EXISTS $SCHEMA.long ;

CREATE  TABLE IF NOT EXISTS $SCHEMA.long (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  last INT UNSIGNED NOT NULL ,
  value_str TEXT NULL ,
  data TEXT NULL ,
  PRIMARY KEY (idx, element_id) ,
  INDEX fk_long_idx (element_id ASC) ,
  INDEX long_time_idx (idx ASC, element_id ASC, last ASC) ,
  CONSTRAINT fk_long_idx
    FOREIGN KEY (element_id )
    REFERENCES $SCHEMA.element_name (id )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MyISAM;
" > $SQLFILE

echo "Finished."
exit 0


