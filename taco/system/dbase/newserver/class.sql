CREATE TABLE NAMES(
	DEVICE_SERVER_CLASS	CHAR(23)	NOT NULL,
	DEVICE_SERVER_NAME	CHAR(11)	NOT NULL,
	INDEX_NUMBER		INTEGER(10)	NOT NULL,
	DEVICENAME		CHAR(48)	NOT NULL,
	HOSTNAME		CHAR(19)	DEFAULT "not_exp",
	PROGRAM_NUMBER		INTEGER(10)	DEFAULT 0,
	VERSION_NUMBER		SMALLINT(4)	DEFAULT 0,
	DEVICE_TYPE		CHAR(23)	DEFAULT "unknown",
	DEVICE_CLASS		CHAR(23)	DEFAULT "unknown",
	PROCESS_ID		SMALLINT(6)	DEFAULT 0,
	PROCESS_NAME		CHAR(39)	DEFAULT "unknown"
);
CREATE UNIQUE INDEX prim ON NAMES(DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, INDEX_NUMBER);

