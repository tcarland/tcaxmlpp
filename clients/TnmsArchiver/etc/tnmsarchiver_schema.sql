SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `tnmsarchive` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci ;
USE `tnmsarchive`;

-- -----------------------------------------------------
-- Table `tnmsarchive`.`element_name`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsarchive`.`element_name` ;

CREATE  TABLE IF NOT EXISTS `tnmsarchive`.`element_name` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `name` TEXT NOT NULL ,
  PRIMARY KEY (`id`) ,
  INDEX `element_name_idx` USING BTREE (`id` ASC, `name`(255) ASC) )
ENGINE = MyISAM;


-- -----------------------------------------------------
-- Table `tnmsarchive`.`immediate`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsarchive`.`immediate` ;

CREATE  TABLE IF NOT EXISTS `tnmsarchive`.`immediate` (
  `idx` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `element_id` INT UNSIGNED NOT NULL ,
  `value_type` SMALLINT UNSIGNED NOT NULL ,
  `value` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `value_avg` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `last` INT UNSIGNED NOT NULL ,
  `value_str` TEXT NULL ,
  `data` TEXT NULL ,
  PRIMARY KEY (`idx`, `element_id`) ,
  INDEX `fk_immediate_idx` (`element_id` ASC) ,
  INDEX `immediate_time_idx` (`element_id` ASC, `last` ASC, `idx` ASC) ,
  CONSTRAINT `fk_immediate_idx`
    FOREIGN KEY (`element_id` )
    REFERENCES `tnmsarchive`.`element_name` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MEMORY;


-- -----------------------------------------------------
-- Table `tnmsarchive`.`short`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsarchive`.`short` ;

CREATE  TABLE IF NOT EXISTS `tnmsarchive`.`short` (
  `idx` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `element_id` INT UNSIGNED NOT NULL ,
  `value_type` SMALLINT UNSIGNED NOT NULL ,
  `value` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `value_avg` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `last` INT UNSIGNED NOT NULL ,
  `value_str` TEXT NULL ,
  `data` TEXT NULL ,
  PRIMARY KEY (`idx`, `element_id`) ,
  INDEX `fk_short_idx` (`element_id` ASC) ,
  INDEX `short_time_idx` (`idx` ASC, `element_id` ASC, `last` ASC) ,
  CONSTRAINT `fk_short_idx`
    FOREIGN KEY (`element_id` )
    REFERENCES `tnmsarchive`.`element_name` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MyISAM;


-- -----------------------------------------------------
-- Table `tnmsarchive`.`long`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsarchive`.`long` ;

CREATE  TABLE IF NOT EXISTS `tnmsarchive`.`long` (
  `idx` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `element_id` INT UNSIGNED NOT NULL ,
  `value_type` SMALLINT UNSIGNED NOT NULL ,
  `value` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `value_avg` BIGINT UNSIGNED NOT NULL DEFAULT 0 ,
  `last` INT UNSIGNED NOT NULL ,
  `value_str` TEXT NULL ,
  `data` TEXT NULL ,
  PRIMARY KEY (`idx`, `element_id`) ,
  INDEX `fk_long_idx` (`element_id` ASC) ,
  INDEX `long_time_idx` (`idx` ASC, `element_id` ASC, `last` ASC) ,
  CONSTRAINT `fk_long_idx`
    FOREIGN KEY (`element_id` )
    REFERENCES `tnmsarchive`.`element_name` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MyISAM;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
