
CREATE SCHEMA IF NOT EXISTS `tnmsauthd`;


CREATE USER 'tnmsauth'@'localhost' IDENTIFIED BY 'tnmsauth11b';
GRANT ALL PRIVILEGES ON tnmsauthd.* TO 'tnmsauth'@'localhost';

CREATE USER 'tnmsauth'@'%' IDENTIFIED BY 'tnmsauth11b';
GRANT ALL PRIVILEGES ON tnmsauthd.* TO 'tnmsauth'@'%';


