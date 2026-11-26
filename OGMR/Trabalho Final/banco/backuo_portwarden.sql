-- MySQL dump 10.13  Distrib 8.0.46, for Linux (x86_64)
--
-- Host: localhost    Database: portwarden
-- ------------------------------------------------------
-- Server version	8.0.46-0ubuntu0.24.04.5

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `agendamentos`
--

DROP TABLE IF EXISTS `agendamentos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `agendamentos` (
  `id` int NOT NULL AUTO_INCREMENT,
  `porta_id` int NOT NULL,
  `usuario_id` int DEFAULT NULL,
  `lote` varchar(40) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `inicio` datetime NOT NULL,
  `fim` datetime NOT NULL,
  `status` enum('agendado','ativo','concluido','cancelado','erro') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'agendado',
  `criado_em` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `atualizado_em` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `fk_ag_porta` (`porta_id`),
  KEY `fk_ag_usuario` (`usuario_id`),
  KEY `idx_ag_status` (`status`),
  KEY `idx_ag_lote` (`lote`),
  CONSTRAINT `fk_ag_porta` FOREIGN KEY (`porta_id`) REFERENCES `portas` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk_ag_usuario` FOREIGN KEY (`usuario_id`) REFERENCES `usuarios` (`id`) ON DELETE SET NULL,
  CONSTRAINT `ck_ag_periodo` CHECK ((`fim` > `inicio`))
) ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `agendamentos`
--

LOCK TABLES `agendamentos` WRITE;
/*!40000 ALTER TABLE `agendamentos` DISABLE KEYS */;
INSERT INTO `agendamentos` VALUES (1,3,1,'exemplo-1','2026-11-20 13:23:35','2026-11-20 14:13:35','concluido','2026-11-22 13:23:35','2026-11-22 13:23:35'),(2,5,1,'exemplo-2','2026-11-23 13:23:35','2026-11-23 14:23:35','cancelado','2026-11-22 13:23:35','2026-11-22 14:49:18'),(3,8,1,'b4b66ff7e2d6','2026-11-22 14:49:10','2026-11-22 15:40:00','cancelado','2026-11-22 14:49:10','2026-11-22 14:49:18'),(4,3,1,'4f22ee92301c','2026-11-23 10:46:19','2026-11-23 11:37:00','cancelado','2026-11-23 10:46:19','2026-11-23 10:47:18'),(5,3,1,'feac0845d237','2026-11-23 10:47:34','2026-11-23 10:50:00','cancelado','2026-11-23 10:47:34','2026-11-23 10:48:39'),(6,4,1,'e2b72d12123c','2026-07-23 11:57:09','2026-07-23 12:48:00','concluido','2026-07-23 11:57:09','2026-11-24 09:31:04'),(7,5,1,'5897934c39af','2026-11-24 09:45:32','2026-11-24 09:47:00','erro','2026-11-24 09:45:32','2026-11-24 09:45:40'),(8,3,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:52:57','2026-11-24 09:55:02'),(9,4,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:53:12','2026-11-24 09:55:16'),(10,5,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:53:16','2026-11-24 09:55:18'),(11,6,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','erro','2026-11-24 09:53:24','2026-11-24 09:53:32'),(12,7,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:53:33','2026-11-24 09:55:20'),(13,8,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','erro','2026-11-24 09:53:34','2026-11-24 09:53:46'),(14,9,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:53:46','2026-11-24 09:55:23'),(15,10,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','erro','2026-11-24 09:53:56','2026-11-24 09:54:05'),(16,11,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','cancelado','2026-11-24 09:54:05','2026-11-24 09:55:34'),(17,12,1,'36214977c968','2026-11-24 09:52:57','2026-11-24 10:43:00','erro','2026-11-24 09:54:07','2026-11-24 09:54:16'),(18,12,1,'fba8cb8ed5f8','2026-11-24 11:40:17','2026-11-24 11:43:00','erro','2026-11-24 11:40:17','2026-11-24 11:40:22'),(19,12,1,'8cc647f3eeac','2026-11-24 11:42:04','2026-11-24 11:44:00','concluido','2026-11-24 11:42:04','2026-11-24 11:44:13'),(20,2,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','concluido','2026-11-24 11:44:58','2026-11-24 11:46:10'),(21,3,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','erro','2026-11-24 11:45:03','2026-11-24 11:46:09'),(22,4,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','concluido','2026-11-24 11:45:04','2026-11-24 11:46:09'),(23,5,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','erro','2026-11-24 11:45:04','2026-11-24 11:46:08'),(24,7,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','concluido','2026-11-24 11:45:05','2026-11-24 11:46:09'),(25,8,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','erro','2026-11-24 11:45:05','2026-11-24 11:46:09'),(26,9,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','erro','2026-11-24 11:45:06','2026-11-24 11:46:09'),(27,10,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','erro','2026-11-24 11:45:06','2026-11-24 11:46:08'),(28,11,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','concluido','2026-11-24 11:45:06','2026-11-24 11:46:09'),(29,12,1,'cb3b92e936e6','2026-11-24 11:44:58','2026-11-24 11:46:00','concluido','2026-11-24 11:45:07','2026-11-24 11:46:10');
/*!40000 ALTER TABLE `agendamentos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `hosts`
--

DROP TABLE IF EXISTS `hosts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `hosts` (
  `id` int NOT NULL AUTO_INCREMENT,
  `nome` varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL,
  `ip` varchar(45) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `mac` char(17) COLLATE utf8mb4_unicode_ci NOT NULL,
  `porta_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `mac` (`mac`),
  UNIQUE KEY `porta_id` (`porta_id`),
  CONSTRAINT `fk_hosts_porta` FOREIGN KEY (`porta_id`) REFERENCES `portas` (`id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `hosts`
--

LOCK TABLES `hosts` WRITE;
/*!40000 ALTER TABLE `hosts` DISABLE KEYS */;
INSERT INTO `hosts` VALUES (1,'SERVIDOR','10.90.90.92','aa:bb:cc:00:00:01',1),(2,'F203-PC00','10.90.90.101','D8:43:A3:DF:8A:D1',2),(3,'NOTEBOOK-ANA','10.90.90.91','00:0e:c8:a1:b2:81',4),(4,'F203-PC02','10.90.90.102','aa:bb:cc:00:00:04',NULL),(5,'F203-PC03','10.90.90.103','aa:bb:cc:00:00:05',5),(6,'PROF-NOVO','10.90.90.101','d8:43:ae:df:8a:d1',6),(7,'F203-PC05','10.90.90.105','aa:bb:cc:00:00:07',7),(8,'F203-PC06','10.90.90.106','aa:bb:cc:00:00:08',8),(9,'PROF-F204','10.90.90.102','04:7C:16:FE:4E:37',12),(10,'F204-PC01','10.90.90.114','aa:bb:cc:00:00:14',14),(11,'F204-PC02','10.90.90.115','aa:bb:cc:00:00:15',15),(16,'NOTEBOOK-RESERVA',NULL,'aa:bb:cc:00:00:99',NULL);
/*!40000 ALTER TABLE `hosts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `log_acoes`
--

DROP TABLE IF EXISTS `log_acoes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `log_acoes` (
  `id` int NOT NULL AUTO_INCREMENT,
  `data_hora` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `usuario_id` int DEFAULT NULL,
  `porta_id` int DEFAULT NULL,
  `agendamento_id` int DEFAULT NULL,
  `acao` enum('bloquear','desbloquear','status','login','acesso_negado') COLLATE utf8mb4_unicode_ci NOT NULL,
  `origem` enum('web','cron') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'web',
  `sucesso` tinyint(1) NOT NULL,
  `mensagem` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ip_cliente` varchar(45) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_log_usuario` (`usuario_id`),
  KEY `fk_log_porta` (`porta_id`),
  KEY `fk_log_ag` (`agendamento_id`),
  CONSTRAINT `fk_log_ag` FOREIGN KEY (`agendamento_id`) REFERENCES `agendamentos` (`id`) ON DELETE SET NULL,
  CONSTRAINT `fk_log_porta` FOREIGN KEY (`porta_id`) REFERENCES `portas` (`id`) ON DELETE SET NULL,
  CONSTRAINT `fk_log_usuario` FOREIGN KEY (`usuario_id`) REFERENCES `usuarios` (`id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=87 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `log_acoes`
--

LOCK TABLES `log_acoes` WRITE;
/*!40000 ALTER TABLE `log_acoes` DISABLE KEYS */;
INSERT INTO `log_acoes` VALUES (1,'2026-11-22 14:43:38',NULL,NULL,NULL,'login','web',0,'falha de login: udesc','10.0.2.15'),(2,'2026-11-22 14:44:11',1,NULL,NULL,'login','web',1,'login em F203','10.0.2.15'),(3,'2026-11-22 14:49:10',1,8,3,'bloquear','web',1,'imediato até 2026-11-22 15:40:00','10.0.2.15'),(4,'2026-11-22 14:49:17',1,3,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(5,'2026-11-22 14:49:17',1,4,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(6,'2026-11-22 14:49:18',1,5,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(7,'2026-11-22 14:49:18',1,6,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(8,'2026-11-22 14:49:18',1,7,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(9,'2026-11-22 14:49:19',1,8,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(10,'2026-11-22 14:49:19',1,9,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(11,'2026-11-22 14:49:19',1,10,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(12,'2026-11-22 14:49:19',1,11,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(13,'2026-11-22 14:49:19',1,12,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(14,'2026-11-23 09:40:54',1,NULL,NULL,'login','web',1,'login em F203','10.0.2.15'),(15,'2026-11-23 10:02:01',1,NULL,NULL,'login','web',1,'login em F203','10.0.2.15'),(16,'2026-11-23 10:46:23',1,3,4,'bloquear','web',1,'imediato até 2026-11-23 11:37:00','10.0.2.15'),(17,'2026-11-23 10:47:18',1,3,NULL,'desbloquear','web',1,'manual','10.0.2.15'),(18,'2026-11-23 10:47:35',1,3,5,'bloquear','web',1,'imediato até 2026-11-23 10:50:00','10.0.2.15'),(19,'2026-11-23 10:48:39',1,3,5,'desbloquear','web',1,'cancelado agendamento 5','10.0.2.15'),(20,'2026-07-23 11:00:05',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(21,'2026-07-23 11:00:45',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(22,'2026-07-23 11:09:04',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(23,'2026-07-23 11:15:31',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(24,'2026-07-23 11:41:42',1,NULL,NULL,'login','web',1,'login em F203','10.90.90.93'),(25,'2026-07-23 11:46:03',1,NULL,NULL,'acesso_negado','web',0,'IP 10.0.2.15 MAC 08:00:27:eb:3f:25','10.0.2.15'),(26,'2026-07-23 11:46:04',1,NULL,NULL,'acesso_negado','web',0,'IP 10.0.2.15 MAC 08:00:27:eb:3f:25','10.0.2.15'),(27,'2026-07-23 11:49:09',1,NULL,NULL,'acesso_negado','web',0,'IP 10.0.2.15 MAC 08:00:27:eb:3f:25','10.0.2.15'),(28,'2026-07-23 11:57:11',1,4,6,'bloquear','web',1,'imediato até 2026-07-23 12:48:00','10.90.90.93'),(29,'2026-11-24 09:31:01',1,NULL,NULL,'login','web',1,'login em F203','10.0.2.15'),(30,'2026-11-24 09:31:04',1,4,6,'desbloquear','cron',1,'fim do prazo','10.0.2.15'),(31,'2026-11-24 09:31:45',1,NULL,NULL,'login','web',1,'login em F203','10.90.90.93'),(32,'2026-11-24 09:44:54',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(33,'2026-11-24 09:45:40',1,5,7,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(34,'2026-11-24 09:47:05',1,5,NULL,'desbloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.0.2.15'),(35,'2026-11-24 09:53:12',1,3,8,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(36,'2026-11-24 09:53:15',1,4,9,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(37,'2026-11-24 09:53:24',1,5,10,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(38,'2026-11-24 09:53:32',1,6,11,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(39,'2026-11-24 09:53:34',1,7,12,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(40,'2026-11-24 09:53:46',1,8,13,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(41,'2026-11-24 09:53:56',1,9,14,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(42,'2026-11-24 09:54:05',1,10,15,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(43,'2026-11-24 09:54:07',1,11,16,'bloquear','web',1,'imediato até 2026-11-24 10:43:00','10.90.90.93'),(44,'2026-11-24 09:54:16',1,12,17,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(45,'2026-11-24 09:55:16',1,3,NULL,'desbloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(46,'2026-11-24 09:55:18',1,4,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(47,'2026-11-24 09:55:19',1,5,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(48,'2026-11-24 09:55:20',1,6,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(49,'2026-11-24 09:55:21',1,7,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(50,'2026-11-24 09:55:23',1,8,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(51,'2026-11-24 09:55:26',1,9,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(52,'2026-11-24 09:55:34',1,10,NULL,'desbloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.93'),(53,'2026-11-24 09:55:36',1,11,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(54,'2026-11-24 09:55:37',1,12,NULL,'desbloquear','web',1,'manual','10.90.90.93'),(55,'2026-11-24 10:59:33',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.102 MAC 04:7c:16:fe:4e:37','10.90.90.102'),(56,'2026-11-24 11:03:48',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.93 MAC 70:69:79:a7:40:8e','10.90.90.93'),(57,'2026-11-24 11:03:51',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.93 MAC 70:69:79:a7:40:8e','10.90.90.93'),(58,'2026-11-24 11:10:22',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(59,'2026-11-24 11:15:16',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.101 MAC 00:0e:c8:a1:b2:81','10.90.90.101'),(60,'2026-11-24 11:16:29',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.93 MAC 70:69:79:a7:40:8e','10.90.90.93'),(61,'2026-11-24 11:38:40',NULL,NULL,NULL,'acesso_negado','web',0,'IP 10.90.90.102 MAC 04:7c:16:fe:4e:37','10.90.90.102'),(62,'2026-11-24 11:39:38',1,NULL,NULL,'login','web',1,'login em F203','10.90.90.101'),(63,'2026-11-24 11:40:22',1,12,18,'bloquear','web',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.','10.90.90.101'),(64,'2026-11-24 11:41:26',1,12,NULL,'desbloquear','web',1,'manual','10.90.90.101'),(65,'2026-11-24 11:42:09',1,12,19,'bloquear','web',1,'imediato até 2026-11-24 11:44:00','10.90.90.101'),(66,'2026-11-24 11:44:14',1,12,19,'desbloquear','cron',1,'fim do prazo',NULL),(67,'2026-11-24 11:45:03',1,2,20,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(68,'2026-11-24 11:45:04',1,3,21,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(69,'2026-11-24 11:45:04',1,4,22,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(70,'2026-11-24 11:45:05',1,5,23,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(71,'2026-11-24 11:45:05',1,7,24,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(72,'2026-11-24 11:45:06',1,8,25,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(73,'2026-11-24 11:45:06',1,9,26,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(74,'2026-11-24 11:45:06',1,10,27,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(75,'2026-11-24 11:45:07',1,11,28,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(76,'2026-11-24 11:45:07',1,12,29,'bloquear','web',1,'imediato até 2026-11-24 11:46:00','10.90.90.101'),(77,'2026-11-24 11:46:08',1,10,27,'desbloquear','cron',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.',NULL),(78,'2026-11-24 11:46:08',1,5,23,'desbloquear','cron',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.',NULL),(79,'2026-11-24 11:46:09',1,8,25,'desbloquear','cron',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.',NULL),(80,'2026-11-24 11:46:09',1,9,26,'desbloquear','cron',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.',NULL),(81,'2026-11-24 11:46:09',1,3,21,'desbloquear','cron',0,'sem resposta (timeout). Confira IP, cabo, se o SNMP está habilitado no switch e a community.',NULL),(82,'2026-11-24 11:46:09',1,4,22,'desbloquear','cron',1,'fim do prazo',NULL),(83,'2026-11-24 11:46:10',1,7,24,'desbloquear','cron',1,'fim do prazo',NULL),(84,'2026-11-24 11:46:10',1,11,28,'desbloquear','cron',1,'fim do prazo',NULL),(85,'2026-11-24 11:46:10',1,12,29,'desbloquear','cron',1,'fim do prazo',NULL),(86,'2026-11-24 11:46:10',1,2,20,'desbloquear','cron',1,'fim do prazo',NULL);
/*!40000 ALTER TABLE `log_acoes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `maquinas_autorizadas`
--

DROP TABLE IF EXISTS `maquinas_autorizadas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `maquinas_autorizadas` (
  `id` int NOT NULL AUTO_INCREMENT,
  `sala_id` int NOT NULL,
  `mac` char(17) COLLATE utf8mb4_unicode_ci NOT NULL,
  `descricao` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ativa` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `mac` (`mac`),
  KEY `fk_aut_sala` (`sala_id`),
  CONSTRAINT `fk_aut_sala` FOREIGN KEY (`sala_id`) REFERENCES `salas` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `maquinas_autorizadas`
--

LOCK TABLES `maquinas_autorizadas` WRITE;
/*!40000 ALTER TABLE `maquinas_autorizadas` DISABLE KEYS */;
INSERT INTO `maquinas_autorizadas` VALUES (1,1,'D8:43:AE:DF:8A:D1','Notebook do Prof. Adriano',1),(2,2,'aa:bb:cc:00:00:13','PC do professor - F204',1),(3,1,'08:00:27:eb:3f:25','VM Linux Mint',1);
/*!40000 ALTER TABLE `maquinas_autorizadas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `portas`
--

DROP TABLE IF EXISTS `portas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `portas` (
  `id` int NOT NULL AUTO_INCREMENT,
  `switch_id` int NOT NULL,
  `numero` int NOT NULL,
  `if_index` int NOT NULL,
  `sala_id` int DEFAULT NULL,
  `tipo` enum('host','autorizada','servidor','uplink','livre') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'livre',
  `protegida` tinyint(1) NOT NULL DEFAULT '0',
  `com_defeito` tinyint(1) NOT NULL DEFAULT '0',
  `observacao` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_porta_numero` (`switch_id`,`numero`),
  UNIQUE KEY `uq_porta_ifindex` (`switch_id`,`if_index`),
  KEY `fk_portas_sala` (`sala_id`),
  CONSTRAINT `fk_portas_sala` FOREIGN KEY (`sala_id`) REFERENCES `salas` (`id`) ON DELETE SET NULL,
  CONSTRAINT `fk_portas_switch` FOREIGN KEY (`switch_id`) REFERENCES `switches` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `portas`
--

LOCK TABLES `portas` WRITE;
/*!40000 ALTER TABLE `portas` DISABLE KEYS */;
INSERT INTO `portas` VALUES (1,1,1,1,NULL,'servidor',1,0,'Servidor web + gerente SNMP. NUNCA bloquear.'),(2,1,2,2,1,'host',0,0,'Máquina do professor (F203).'),(3,1,3,3,1,'host',0,0,NULL),(4,1,4,4,1,'host',0,0,NULL),(5,1,5,5,1,'host',0,0,NULL),(6,1,6,6,1,'autorizada',1,0,NULL),(7,1,7,7,1,'host',0,0,'Fast Ethernet (100 Mbps).'),(8,1,8,8,1,'host',0,0,'Fast Ethernet (100 Mbps).'),(9,1,9,9,1,'livre',0,0,NULL),(10,1,10,10,1,'livre',0,0,NULL),(11,1,11,11,1,'livre',0,0,NULL),(12,1,12,12,1,'host',0,0,NULL),(13,1,13,13,2,'host',0,0,'Máquina do professor (F204).'),(14,1,14,14,2,'host',0,0,NULL),(15,1,15,15,2,'host',0,0,NULL),(16,1,16,16,2,'livre',0,0,NULL),(17,1,17,17,2,'livre',0,0,NULL),(18,1,18,18,2,'livre',0,0,NULL),(19,1,19,19,2,'livre',0,0,NULL),(20,1,20,20,2,'livre',0,0,NULL),(21,1,21,21,NULL,'livre',0,1,'Com defeito (anotação no switch). Porta combo.'),(22,1,22,22,NULL,'livre',0,1,'Com defeito (anotação no switch). Porta combo.'),(23,1,23,23,NULL,'uplink',1,0,'Uplink. NUNCA bloquear.'),(24,1,24,24,NULL,'livre',0,1,'Com defeito (anotação no switch). Porta combo.');
/*!40000 ALTER TABLE `portas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `salas`
--

DROP TABLE IF EXISTS `salas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `salas` (
  `id` int NOT NULL AUTO_INCREMENT,
  `nome` varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL,
  `descricao` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `salas`
--

LOCK TABLES `salas` WRITE;
/*!40000 ALTER TABLE `salas` DISABLE KEYS */;
INSERT INTO `salas` VALUES (1,'F203','Laboratório de ensino - bancada do trabalho'),(2,'F204','Laboratório vizinho (exemplo de switch compartilhado)');
/*!40000 ALTER TABLE `salas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `switches`
--

DROP TABLE IF EXISTS `switches`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `switches` (
  `id` int NOT NULL AUTO_INCREMENT,
  `nome` varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL,
  `modelo` varchar(50) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `patrimonio` varchar(30) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ip` varchar(45) COLLATE utf8mb4_unicode_ci NOT NULL,
  `porta_snmp` int NOT NULL DEFAULT '161',
  `community_leitura` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'public',
  `community_escrita` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'private',
  `qtd_portas` int NOT NULL DEFAULT '24',
  `localizacao` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `nome` (`nome`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `switches`
--

LOCK TABLES `switches` WRITE;
/*!40000 ALTER TABLE `switches` DISABLE KEYS */;
INSERT INTO `switches` VALUES (1,'SW-BANCADA','D-Link DGS-3100-24','UDESC 104818','10.90.90.90',161,'public','private',24,'Bancada OGMR');
/*!40000 ALTER TABLE `switches` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `usuarios`
--

DROP TABLE IF EXISTS `usuarios`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `usuarios` (
  `id` int NOT NULL AUTO_INCREMENT,
  `login` varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL,
  `nome` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL,
  `senha_hash` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `ativo` tinyint(1) NOT NULL DEFAULT '1',
  `criado_em` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `login` (`login`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `usuarios`
--

LOCK TABLES `usuarios` WRITE;
/*!40000 ALTER TABLE `usuarios` DISABLE KEYS */;
INSERT INTO `usuarios` VALUES (1,'professor','Professor da Disciplina','$2y$10$QONNrOc4rp6AoisTqBXu1ObuWT1b7U3k0vI.oST.OCGVqBd9h.mJy',1,'2026-11-22 13:23:35');
/*!40000 ALTER TABLE `usuarios` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary view structure for view `vw_portas`
--

DROP TABLE IF EXISTS `vw_portas`;
/*!50001 DROP VIEW IF EXISTS `vw_portas`*/;
SET @saved_cs_client     = @@character_set_client;
/*!50503 SET character_set_client = utf8mb4 */;
/*!50001 CREATE VIEW `vw_portas` AS SELECT 
 1 AS `porta_id`,
 1 AS `porta_numero`,
 1 AS `if_index`,
 1 AS `tipo`,
 1 AS `protegida`,
 1 AS `com_defeito`,
 1 AS `observacao`,
 1 AS `switch_id`,
 1 AS `switch_nome`,
 1 AS `switch_ip`,
 1 AS `community_leitura`,
 1 AS `community_escrita`,
 1 AS `sala_id`,
 1 AS `sala_nome`,
 1 AS `host_id`,
 1 AS `host_nome`,
 1 AS `host_ip`,
 1 AS `host_mac`,
 1 AS `bloqueavel`*/;
SET character_set_client = @saved_cs_client;

--
-- Final view structure for view `vw_portas`
--

/*!50001 DROP VIEW IF EXISTS `vw_portas`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `vw_portas` AS select `p`.`id` AS `porta_id`,`p`.`numero` AS `porta_numero`,`p`.`if_index` AS `if_index`,`p`.`tipo` AS `tipo`,`p`.`protegida` AS `protegida`,`p`.`com_defeito` AS `com_defeito`,`p`.`observacao` AS `observacao`,`s`.`id` AS `switch_id`,`s`.`nome` AS `switch_nome`,`s`.`ip` AS `switch_ip`,`s`.`community_leitura` AS `community_leitura`,`s`.`community_escrita` AS `community_escrita`,`sa`.`id` AS `sala_id`,`sa`.`nome` AS `sala_nome`,`h`.`id` AS `host_id`,`h`.`nome` AS `host_nome`,`h`.`ip` AS `host_ip`,`h`.`mac` AS `host_mac`,((`p`.`protegida` = 0) and (`p`.`com_defeito` = 0) and (`p`.`tipo` in ('host','livre')) and (`p`.`sala_id` is not null)) AS `bloqueavel` from (((`portas` `p` join `switches` `s` on((`s`.`id` = `p`.`switch_id`))) left join `salas` `sa` on((`sa`.`id` = `p`.`sala_id`))) left join `hosts` `h` on((`h`.`porta_id` = `p`.`id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-11-24 11:52:35
