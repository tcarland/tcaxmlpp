/**
  * Database setup to initialize the system with some defaults and testing accounts 
  *
** /



INSERT INTO tnms.auth_types (method_name, authbin_name) 
VALUES ("dbstatic", "dbstatic");


INSERT INTO tnms.groups (name, description, internal)
    VALUES ("tnms", "Core services group", 1);
INSERT INTO tnms.groups (name, description, internal)
    VALUES ("tnmsagents", "Core services agent group", 1);
INSERT INTO tnms.groups (name, description)
    VALUES ("users", "Default user group");

INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (1, 1, "tnms", "tnms11b", 1);
INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (1, 1, "tnmsd", "tnmsd11b", 1);
INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (1, 1, "tnmsauth", "tnmsauth11b", 1);
INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (1, 1, "tnmsdb", "tnmsdb11b", 1);
INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (2, 1, "tnmsconsole", "tnmsconsole11b", 1);
INSERT INTO tnms.users ( gid, authtype_id, username, password, internal )
    VALUES (2, 1, "tnmsclient", "tnmsclient11b", 1);

INSERT INTO tnms.mgr_usergroups ( mgr_uid, mgr_gid, is_superuser, grant_rights )
    VALUES (1, 1, 1, 1);
INSERT INTO tnms.mgr_usergroups ( mgr_uid, mgr_gid, is_superuser, grant_rights )
    VALUES (5, 1, 1, 1);

INSERT INTO tnms.agents ( gid, name, parent_name )
    VALUES ( 2, "tnmsd", "tcanms/Agents" );
INSERT INTO tnms.agents ( gid, name, parent_name )
    VALUES ( 2, "tnmsauth", "tcanms/Agents" );
INSERT INTO tnms.agents ( gid, name, parent_name )
    VALUES ( 2, "tnmsconsole", "tcanms/Agents" );


INSERT INTO tnms.access_filters ( ipaddress )
    VALUES ("127.0.0.1");
INSERT INTO tnms.group_access_filters ( gid, filter_id )
    VALUES (1, 1);

