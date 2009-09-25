SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `tnmsauth` ;
USE `tnmsauth`;

-- -----------------------------------------------------
-- Table `tnmsauth`.`auth_types`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`auth_types` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`auth_types` (
  `authtype_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `method_name` VARCHAR(255) NOT NULL ,
  `authbin_name` VARCHAR(255) NOT NULL ,
  PRIMARY KEY (`authtype_id`) ,
  INDEX `authmethod_idx` (`authtype_id` ASC, `method_name` ASC) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`groups`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`groups` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`groups` (
  `gid` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `name` VARCHAR(255) NOT NULL ,
  `description` VARCHAR(255) NULL DEFAULT NULL ,
  `internal` TINYINT NOT NULL DEFAULT '0' ,
  PRIMARY KEY (`gid`) ,
  INDEX `groupname_idx` (`gid` ASC, `name` ASC) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`authorizations`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`authorizations` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`authorizations` (
  `subtree_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `subtree_name` VARCHAR(255) NOT NULL ,
  `isInclude` TINYINT NOT NULL ,
  `description` VARCHAR(255) NULL DEFAULT NULL ,
  PRIMARY KEY (`subtree_id`) ,
  INDEX `subtree_idx` (`subtree_id` ASC, `subtree_name` ASC) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`users` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`users` (
  `uid` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `gid` INT UNSIGNED NOT NULL ,
  `authtype_id` INT UNSIGNED NOT NULL ,
  `username` VARCHAR(255) NOT NULL ,
  `password` VARCHAR(255) NOT NULL DEFAULT '' ,
  `is_agent` TINYINT NOT NULL DEFAULT 0 ,
  `internal` TINYINT NOT NULL DEFAULT '0' ,
  `description` VARCHAR(255) NULL DEFAULT ' ' ,
  PRIMARY KEY (`uid`) ,
  INDEX `username_idx` (`uid` ASC, `username` ASC) ,
  INDEX `users_gid_FKidx` (`gid` ASC) ,
  INDEX `users_authtype_FKidx` (`authtype_id` ASC) ,
  CONSTRAINT `fk_users_gid`
    FOREIGN KEY (`gid` )
    REFERENCES `tnmsauth`.`groups` (`gid` )
    ON DELETE RESTRICT
    ON UPDATE RESTRICT,
  CONSTRAINT `fk_users_auth`
    FOREIGN KEY (`authtype_id` )
    REFERENCES `tnmsauth`.`auth_types` (`authtype_id` )
    ON DELETE RESTRICT
    ON UPDATE RESTRICT)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`user_configs`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`user_configs` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`user_configs` (
  `uid` INT UNSIGNED NOT NULL ,
  `config` LONGBLOB NOT NULL ,
  PRIMARY KEY (`uid`) ,
  INDEX `fk_user_configs_users1` (`uid` ASC) ,
  CONSTRAINT `fk_user_configs_users1`
    FOREIGN KEY (`uid` )
    REFERENCES `tnmsauth`.`users` (`uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`tickets`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`tickets` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`tickets` (
  `username` VARCHAR(255) NOT NULL ,
  `ticket` VARCHAR(255) NOT NULL ,
  `ipaddress` VARCHAR(255) NOT NULL ,
  PRIMARY KEY (`ticket`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`properties`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`properties` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`properties` (
  `name` VARCHAR(32) NOT NULL DEFAULT '' ,
  `value` INT UNSIGNED NOT NULL DEFAULT '0' ,
  PRIMARY KEY (`name`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `tnmsauth`.`mgr_usergroups`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`mgr_usergroups` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`mgr_usergroups` (
  `mgr_uid` INT UNSIGNED NOT NULL ,
  `mgr_gid` INT UNSIGNED NOT NULL ,
  `is_superuser` TINYINT UNSIGNED NOT NULL DEFAULT 0 ,
  `grant_rights` TINYINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`mgr_uid`, `mgr_gid`) ,
  INDEX `fk_mgr_uid` (`mgr_uid` ASC) ,
  INDEX `fk_mgr_gid` (`mgr_gid` ASC) ,
  CONSTRAINT `fk_mgr_uid`
    FOREIGN KEY (`mgr_uid` )
    REFERENCES `tnmsauth`.`users` (`uid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_mgr_gid`
    FOREIGN KEY (`mgr_gid` )
    REFERENCES `tnmsauth`.`groups` (`gid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `tnmsauth`.`access_filters`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`access_filters` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`access_filters` (
  `filter_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `ipaddress` VARCHAR(45) NOT NULL ,
  `description` VARCHAR(255) NULL DEFAULT NULL ,
  PRIMARY KEY (`filter_id`) ,
  INDEX `ipaddr_idx` (`filter_id` ASC, `ipaddress` ASC) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `tnmsauth`.`group_servers`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`group_servers` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`group_servers` (
  `server_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `gid` INT UNSIGNED NOT NULL ,
  `host` VARCHAR(45) NOT NULL ,
  `port` SMALLINT UNSIGNED NOT NULL ,
  PRIMARY KEY (`server_id`, `gid`, `host`, `port`) ,
  INDEX `fk_server_gid` (`gid` ASC) ,
  CONSTRAINT `fk_server_gid`
    FOREIGN KEY (`gid` )
    REFERENCES `tnmsauth`.`groups` (`gid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `tnmsauth`.`group_access_filters`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`group_access_filters` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`group_access_filters` (
  `gid` INT UNSIGNED NOT NULL ,
  `filter_id` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`gid`, `filter_id`) ,
  INDEX `fk_access_gid` (`gid` ASC) ,
  INDEX `fk_access_fid` (`filter_id` ASC) ,
  CONSTRAINT `fk_access_gid`
    FOREIGN KEY (`gid` )
    REFERENCES `tnmsauth`.`groups` (`gid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_access_fid`
    FOREIGN KEY (`filter_id` )
    REFERENCES `tnmsauth`.`access_filters` (`filter_id` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `tnmsauth`.`group_authorizations`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `tnmsauth`.`group_authorizations` ;

CREATE  TABLE IF NOT EXISTS `tnmsauth`.`group_authorizations` (
  `gid` INT UNSIGNED NOT NULL ,
  `subtree_id` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`gid`, `subtree_id`) ,
  INDEX `fk_auth_gid` (`gid` ASC) ,
  INDEX `fk_auth_sid` (`subtree_id` ASC) ,
  CONSTRAINT `fk_auth_gid`
    FOREIGN KEY (`gid` )
    REFERENCES `tnmsauth`.`groups` (`gid` )
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_auth_sid`
    FOREIGN KEY (`subtree_id` )
    REFERENCES `tnmsauth`.`authorizations` (`subtree_id` )
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
