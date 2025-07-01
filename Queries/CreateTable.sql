CREATE TABLE devices
(
	id SERIAL PRIMARY KEY,
	dev_num varchar(255) NOT NULL UNIQUE,
	dev_name varchar(255) NOT NULL,
	dev_description varchar(255),
	dev_status int NOT NULL
)
