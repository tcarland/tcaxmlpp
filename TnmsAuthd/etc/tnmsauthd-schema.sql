

CREATE DATABASE tnmsauth CHARACTER SET utf8;

DROP TABLE tnms.auth_types;
DROP TABLE tnms.groups;
DROP TABLE tnms.group_packages;


CREATE TABLE tnms.auth_types (
    authtype_id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    method_name VARCHAR(255) NOT NULL,
    authbin_name VARCHAR(255) NOT NULL,
    PRIMARY KEY(authtype_id),
    INDEX authmethod_idx(authtype_id, method_name)
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.groups (
    gid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    name VARCHAR(255) NOT NULL,
    description TEXT NULL,
    internal TINYINT NOT NULL DEFAULT '0',
    PRIMARY KEY(gid),
    INDEX groupname_idx(gid, name)
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.authorizations (
    subtree_id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    subtree_name VARCHAR(255) NOT NULL,
    isInclude TINYINT NOT NULL,
    description VARCHAR(255) NULL,
    PRIMARY KEY(subtree_id),
    INDEX subtree_idx(subtree_id, subtree_name)
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.agents (
    agent_id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    gid INTEGER UNSIGNED NOT NULL,
    name VARCHAR(255) NOT NULL,
    ipaddress VARCHAR(45) NULL,
    required TINYINT NOT NULL DEFAULT '0',
    PRIMARY KEY(agent_id),
    INDEX agentname_idx(agent_id, name),
    INDEX agents_gid_FKidx(gid),
    FOREIGN KEY(gid) REFERENCES groups(gid)
        ON DELETE RESTRICT
        ON UPDATE RESTRICT
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.agent_configs (
    agent_id INTEGER UNSIGNED NOT NULL,
    agent_config LONGBLOB NOT NULL,
    PRIMARY KEY(agent_id),
    INDEX agent_configs_FKidx(agent_id),
    FOREIGN KEY(agent_id) REFERENCES agents(agent_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.users (
    uid INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
    gid INTEGER UNSIGNED NOT NULL,
    authtype_id INTEGER UNSIGNED NOT NULL,
    username VARCHAR(255) NOT NULL,
    password VARCHAR(255) NULL,
    internal TINYINT NOT NULL DEFAULT '0',
    PRIMARY KEY(uid),
    INDEX username_idx(uid, username),
    INDEX users_gid_FKidx(gid),
    INDEX users_authtype_FKidx(authtype_id),
    FOREIGN KEY(gid) REFERENCES groups(gid)
        ON DELETE RESTRICT
        ON UPDATE RESTRICT,
    FOREIGN KEY(authtype_id) REFERENCES auth_types(authtype_id)
        ON DELETE RESTRICT
        ON UPDATE RESTRICT
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.tickets (
    username VARCHAR(255) NOT NULL,
    ticket VARCHAR(255) NOT NULL,
    ipaddress VARCHAR(255) NOT NULL,
    PRIMARY KEY(ticket)
) TYPE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE tnms.properties (
  name VARCHAR(32) NOT NULL DEFAULT '',
  value INTEGER UNSIGNED NOT NULL DEFAULT '0',
  PRIMARY KEY(name)
) TYPE=InnoDB DEFAULT CHARSET=utf8;





