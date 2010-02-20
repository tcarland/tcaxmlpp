
CREATE SCHEMA IF NOT EXISTS `tnmsauth`;


CREATE USER 'tnmsauth'@'localhost' IDENTIFIED BY 'tnmsauth11b';
GRANT ALL PRIVILEGES ON *.tnmsauth TO 'tnmsauth'@'localhost';

CREATE USER 'tnmsauth'@'%' IDENTIFIED BY 'tnmsauth11b';
GRANT ALL PRIVILEGES ON *.tnmsauth TO 'tnmsauth'@'%';


