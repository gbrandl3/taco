-- MySQL dump 8.22
--
-- Host: localhost    Database: tango
---------------------------------------------------------
-- Server version	3.23.55-log

--
-- Table structure for table 'attribute_class'
--

CREATE TABLE attribute_class (
  class varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text,
  KEY index_attribute_class (class(64),name(64))
) TYPE=MyISAM;

--
-- Dumping data for table 'attribute_class'
--



--
-- Table structure for table 'device'
--

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
) TYPE=MyISAM;


--
-- Table structure for table 'property'
--

CREATE TABLE property (
  object varchar(255) default NULL,
  name varchar(255) default NULL,
  count int(11) default NULL,
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text
) TYPE=MyISAM;

--
-- Dumping data for table 'property'
--



--
-- Table structure for table 'property_attribute_class'
--

CREATE TABLE property_attribute_class (
  class varchar(255) NOT NULL default '',
  attribute varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text,
  KEY index_property_attribute_class (attribute(64),name(64),count)
) TYPE=MyISAM;

--
-- Dumping data for table 'property_attribute_class'
--



--
-- Table structure for table 'property_attribute_device'
--

CREATE TABLE property_attribute_device (
  device varchar(255) NOT NULL default '',
  attribute varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text,
  KEY index_property_attribute_device (attribute(64),name(64),count)
) TYPE=MyISAM;

--
-- Dumping data for table 'property_attribute_device'
--



--
-- Table structure for table 'property_class'
--

CREATE TABLE property_class (
  class varchar(255) NOT NULL default '',
  name varchar(255) NOT NULL default '',
  count int(11) NOT NULL default '0',
  value varchar(255) default NULL,
  updated timestamp(14) NOT NULL,
  accessed timestamp(14) NOT NULL,
  comment text,
  KEY index_property (class(64),name(64),count)
) TYPE=MyISAM;

--
-- Dumping data for table 'property_class'
--



--
-- Table structure for table 'property_device'
--

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
  comment text,
  KEY index_resource (device(64),name(64),count)
) TYPE=MyISAM;

--
-- Dumping data for table 'property_device'
--


--
-- Table structure for table 'server'
--

CREATE TABLE server (
  name varchar(255) NOT NULL default '',
  host varchar(255) NOT NULL default '',
  mode int(11) default '0',
  level int(11) default '0'
) TYPE=MyISAM;

--
-- Dumping data for table 'server'
--



