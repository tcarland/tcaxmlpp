SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

-- -----------------------------------------------------
-- Table ${SCHEMA}.element_name
-- -----------------------------------------------------
DROP TABLE IF EXISTS ${SCHEMA}.element_name ;

CREATE TABLE IF NOT EXISTS ${SCHEMA}.element_name (
  id INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  name TEXT NOT NULL ,
  PRIMARY KEY (id) ,
  INDEX element_name_idx USING BTREE (id ASC, name(255) ASC) )
ENGINE = MyISAM;


-- -----------------------------------------------------
-- Table ${SCHEMA}.immediate
-- -----------------------------------------------------
DROP TABLE IF EXISTS ${SCHEMA}.immediate ;

CREATE TABLE IF NOT EXISTS ${SCHEMA}.immediate (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  samples INT UNSIGNED NOT NULL DEFAULT 1 ,
  last INT UNSIGNED NOT NULL ,
  value_str VARCHAR(255) NULL ,
  data VARCHAR(255) NULL ,
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

CREATE TABLE IF NOT EXISTS ${SCHEMA}.short (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  samples INT UNSIGNED NOT NULL DEFAULT 1 ,
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

CREATE TABLE IF NOT EXISTS $SCHEMA.long (
  idx INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  element_id INT UNSIGNED NOT NULL ,
  value_type SMALLINT UNSIGNED NOT NULL ,
  value BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  value_avg BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  samples INT UNSIGNED NOT NULL DEFAULT 1 ,
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

