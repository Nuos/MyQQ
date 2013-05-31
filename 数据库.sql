/*
SQLyog 企业版 - MySQL GUI v7.14 
MySQL - 5.0.51b-community-nt : Database - myqq
*********************************************************************
*/


/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`myqq` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `myqq`;

/*Table structure for table `friendlist` */

DROP TABLE IF EXISTS `friendlist`;

CREATE TABLE `friendlist` (
  `id` int(11) NOT NULL auto_increment,
  `uid` int(11) default NULL COMMENT '用户id',
  `fid` int(11) default NULL COMMENT '好友id',
  `flistnum` int(11) default NULL COMMENT '好友所在列表id',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Data for the table `friendlist` */

/*Table structure for table `userinfor` */

DROP TABLE IF EXISTS `userinfor`;

CREATE TABLE `userinfor` (
  `id` int(11) NOT NULL auto_increment,
  `uid` int(11) NOT NULL,
  `uname` varchar(20) NOT NULL,
  `upasswd` varchar(20) NOT NULL,
  `uiconum` int(11) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8;
