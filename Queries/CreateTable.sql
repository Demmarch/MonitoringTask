CREATE TABLE devices
(
	id SERIAL PRIMARY KEY,
	dev_num varchar(255) NOT NULL UNIQUE,
	dev_name varchar(255) NOT NULL,
	dev_description varchar(255),
	dev_status int NOT NULL
)

CREATE TABLE status_log
(
    log_id SERIAL PRIMARY KEY,
    device_id INT NOT NULL REFERENCES devices(id) ON DELETE CASCADE,
    dev_num VARCHAR(255) NOT NULL,
    change_time TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    old_status INT NOT NULL,
    new_status INT NOT NULL
);