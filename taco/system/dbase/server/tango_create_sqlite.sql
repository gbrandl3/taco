BEGIN TRANSACTION;
CREATE TABLE attribute_class (class varchar(255) NOT NULL default '', name varchar(255) NOT NULL default '', updated timestamp(14) NOT NULL, accessed timestamp(14) NOT NULL, comment text);
INSERT INTO "attribute_class" VALUES('DevClass', 'DEVCLASS', '2006-06-12 15:00:00', '2006-06-12 15:00:00', 'Test device class');
CREATE TABLE device (
  name varchar(255) default NULL,
  alias varchar(255) default NULL,
  domain varchar(85) default NULL,
  family varchar(85) default NULL,
  member varchar(85) default NULL,
  exported int(11) default NULL,
  ior text,
  host varchar(255) default NULL,
  server varchar(255) default NULL,
  pid int(11) default NULL,
  class varchar(255) default NULL,
  version varchar(8) default NULL,
  started datetime default NULL,
  stopped datetime default NULL,
  comment text
);
INSERT INTO "device" VALUES('dserver/DataBaseds/2', NULL, 'dserver', 'DataBaseds', '2', 0, 'nada', 'wow', 'DataBaseds/2', 0, 'DServer', '2', '2003-07-11 09:43:17', NULL, NULL);
INSERT INTO "device" VALUES('sys/database/2', NULL, 'sys', 'database', '2', 0, 'nada', 'wow', 'DataBaseds/2', 0, 'DataBase', '2', '2003-07-11 09:43:17', NULL, NULL);
CREATE TABLE property (
  object varchar(255) default NULL,
  name varchar(255) default NULL,
  count int(11) default NULL,
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text
);
CREATE TABLE property_attribute_class (
  class varchar(255) NOT NULL default '',
  attribute varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text);
CREATE TABLE property_class (
  class varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text);
CREATE TABLE property_device (
  device varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  domain varchar(255) NOT NULL default '',
  family varchar(255) NOT NULL default '',
  member varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text);
CREATE TABLE server (
  name varchar(255) NOT NULL default '',
  host varchar(255) NOT NULL default '',
  mode int(11) default '0',
  level int(11) default '0'
);
CREATE TABLE event (
  name varchar(255),
  exported int(11),
  ior text,
  host varchar(255),
  server varchar(255),
  pid int(11),
  version varchar(8),
  started datetime,
  stopped datetime
);
CREATE TABLE attribute_alias (
  alias varchar(255) DEFAULT '' NOT NULL,
  name varchar(255) DEFAULT '' NOT NULL,
  device varchar(255) DEFAULT '' NOT NULL,
  attribute varchar(255) DEFAULT '' NOT NULL,
  updated timestamp(14),
  accessed timestamp(14),
  comment text);
CREATE INDEX index_attribute_class on attribute_class(class, name);
CREATE INDEX index_property_attribute_class on property_attribute_class(attribute,name,count);
CREATE INDEX index_resource on property_device(device, name, count);
CREATE INDEX index_property on property_class(class, name, count);
CREATE INDEX index_attribute_alias on attribute_alias(alias, name);
COMMIT;
