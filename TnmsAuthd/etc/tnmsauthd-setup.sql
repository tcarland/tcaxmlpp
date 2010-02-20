/**
  * Database setup to initialize the system with some defaults and 
  * testing accounts 
  *
**/


INSERT INTO tnmsauth.auth_types (method_name, authbin_name) 
VALUES ("dbstatic", "dbstatic");


INSERT INTO tnmsauth.groups (name, description, internal)
    VALUES ("tnms", "Core services group", 1);

INSERT INTO tnmsauth.groups (name, description, internal)
    VALUES ("tnmsagents", "Core services agent group", 1);

INSERT INTO tnmsauth.groups (name, description)
    VALUES ("users", "Default user group");


INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms", "tnms11b", 0, 1);

INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms/tnmsauth-master", "tnmsauth11b", 1, 1);

INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (2, 1, "tnms/tnms-console", "tnmsconsole11b", 1, 1);

INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms/tnmsd/devserver_a", "tnmsd11b", 1, 1);
INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms/tnmsd/devserver_b", "tnmsd11b", 1, 1);
INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms/tnmsd/devserver_c", "tnmsd11b", 1, 1);

INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms/tnmsarchiver/tnms", "tnmsarchive11b", 1, 1);

INSERT INTO tnmsauth.users ( gid, authtype_id, username, password, is_agent, internal )
    VALUES (1, 1, "tnms-console", "tnmsconsole11b", 1, 1);


INSERT INTO tnmsauth.mgr_usergroups ( mgr_uid, mgr_gid, is_superuser, grant_rights )
    VALUES (1, 1, 1, 1);
INSERT INTO tnmsauth.mgr_usergroups ( mgr_uid, mgr_gid, is_superuser, grant_rights )
    VALUES (5, 1, 1, 1);


INSERT INTO tnmsauth.access_filters ( ipaddress )
    VALUES ("127.0.0.1");
INSERT INTO tnmsauth.group_access_filters ( gid, filter_id )
    VALUES (1, 1);



