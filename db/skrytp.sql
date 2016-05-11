DROP DATABASE IF EXISTS recrutify;
CREATE DATABASE IF NOT EXISTS recrutify;

use recrutify;
CREATE TABLE Users(
	user_id INT(11) AUTO_INCREMENT,
	username varchar(20) NOT NULL,
	password varchar(128) NOT NULL,
	salt varchar(128) NOT NULL,
	password_plain varchar(64) NOT NULL, #holy shit! ''Plain''??!!
	email varchar(30),
	PRIMARY KEY(user_id)
);

CREATE TABLE Answers(
	answerr_id INT AUTO_INCREMENT PRIMARY KEY,
	answer_content TEXT
);

CREATE TABLE Questions_architect(
	question_id INT AUTO_INCREMENT ,
	question_content TEXT,
	answer_id INT,
	PRIMARY KEY (question_id),
    FOREIGN KEY (answer_id) REFERENCES Answers(answerr_id)
);

CREATE TABLE Questions_pm(
	question_id INT AUTO_INCREMENT PRIMARY KEY
);

CREATE TABLE Questions_programmer(
	question_id INT AUTO_INCREMENT PRIMARY KEY
);

CREATE TABLE Questions_networker(
	question_id INT AUTO_INCREMENT PRIMARY KEY
);

CREATE TABLE Questions_tester(
	question_id INT AUTO_INCREMENT PRIMARY KEY
);



INSERT INTO Answers(answer_content) VALUES ('Gryczka');
INSERT INTO Questions_architect(question_content, answer_id) VALUES ('Adam', 1);

-- archiktekt
-- pm
-- programista
-- sieciowiec
-- tester

SELECT Questions_architect.question_content, Answers.question_content
FROM Questions_architect
INNER JOIN Answers
ON Answers.answer_id = Questions_architect.answer_id