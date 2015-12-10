CREATE TABLE ADMINISTRATOR
(
ADMINID CHAR(6)  NOT NULL PRIMARY KEY,
ADMINNAME VARCHAR2(20) NOT NULL,
PASSWD VARCHAR2(20) NOT NULL
);
CREATE TABLE TEACHER
(
TEACHERNO CHAR(6) NOT NULL PRIMARY KEY,
PASSWD VARCHAR2(20) NOT NULL,
TEACHERNAME VARCHAR2(20) NOT NULL,
SEX CHAR(2),
EMAIL VARCHAR2(50),
TEACHERINFO CLOB,
DEPARTMENT VARCHAR2(20) NOT NULL
);
CREATE TABLE COURSEAPPLY
(
APPLYNO CHAR(6) NOT NULL PRIMARY KEY,
TEACHERNO CHAR(6) NOT NULL,
COUSEINFO CLOB,
COURSENAME VARCHAR2(50) NOT NULL,
COURSEPROPERTY CHAR(4) NOT NULL,
STATUS CHAR(6),
CONSTRAINT TEACHERFK FOREIGN KEY(TEACHERNO) REFERENCES TEACHER(TEACHERNO)
);
CREATE TABLE COURSE
(
COURSENO CHAR(6) NOT NULL PRIMARY KEY,
TIMEOFCLASS VARCHAR2(5) NOT NULL,
CREDIT NUMBER(2) NOT NULL,
CLASSROOM VARCHAR2(10) ,
CAPPACITY NUMBER(4),
CURRENTNUMBER NUMBER(4),
APPLYNO CHAR(6) NOT NULL,
CONSTRAINT COURSEFK FOREIGN KEY (APPLYNO) REFERENCES COURSEAPPLY(APPLYNO)
);
CREATE TABLE STUDENT
(
STUDENTNO CHAR(10) NOT NULL PRIMARY KEY,
STUDENTNAME VARCHAR2(20) NOT NULL,
PASSWD VARCHAR2(20) NOT NULL,
SEX CHAR(2),
EMAIL VARCHAR2(50),
CLASSNO VARCHAR2(10),
DEPARTMENT VARCHAR2(20) NOT NULL
);

