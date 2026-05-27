CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    user_tag VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    public_key TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE users_info (
	user_id INTEGER PRIMARY KEY NOT NULL,
 	name VARCHAR(63),
	description TEXT,
	birth_date DATE,
	mood VARCHAR(31),

	FOREIGN KEY(user_id) REFERENCES users(id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
);

CREATE TABLE chats (
    id SERIAL PRIMARY KEY,
    is_group BOOLEAN DEFAULT FALSE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE groups_info (
	group_id INTEGER PRIMARY KEY REFERENCES chats(id) 
	ON DELETE CASCADE
	ON UPDATE CASCADE,
	is_public BOOLEAN DEFAULT FALSE NOT NULL,
	name VARCHAR(63),
	description TEXT
);

CREATE TABLE chat_members (
    chat_id INTEGER REFERENCES chats(id)
	ON DELETE CASCADE
	ON UPDATE CASCADE,
    user_id INTEGER REFERENCES users(id)
	ON DELETE CASCADE
	ON UPDATE CASCADE,
    joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (chat_id, user_id)
);

CREATE TABLE messages (
    id SERIAL PRIMARY KEY,
    chat_id INTEGER REFERENCES chats(id) 
	ON DELETE CASCADE
	ON UPDATE CASCADE,
    sender_id INTEGER REFERENCES users(id)
	ON DELETE SET NULL
	ON UPDATE CASCADE,
    message_text TEXT NOT NULL,
    replies_to INTEGER REFERENCES messages(id) 
	ON DELETE SET NULL 
	DEFAULT NULL,
    sent_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE message_statuses (
	message_id INTEGER REFERENCES messages(id)
	ON DELETE CASCADE
	ON UPDATE CASCADE,
	user_id INTEGER REFERENCES users(id)
	ON DELETE CASCADE
	ON UPDATE CASCADE,
	is_read BOOLEAN NOT NULL DEFAULT FALSE,
	read_at TIMESTAMP DEFAULT NULL,

	PRIMARY KEY(message_id, user_id)
);

CREATE TABLE message_attachments (
    id SERIAL PRIMARY KEY,
    message_id INTEGER NOT NULL REFERENCES messages(id)
	ON DELETE CASCADE
	ON UPDATE CASCADE,
    file_path TEXT NOT NULL,          
    file_type VARCHAR(31) NOT NULL,   
    file_size INTEGER NOT NULL,       
    file_name VARCHAR(255),           
    metadata JSONB DEFAULT NULL       
);

CREATE TABLE unverified_users (
    id SERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    user_tag VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    public_key TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    сonfirmation_code INTEGER,
    registered_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE session_codes (
    user_id INTEGER REFERENCES users(id)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
    code TEXT NOT NULL,
    device_id TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY(user_id, device_id)
);

CREATE TABLE message_reactions (
    message_id INTEGER REFERENCES messages(id)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
    user_id INTEGER REFERENCES users(id)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
    reaction VARCHAR(31) NOT NULL, 
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    PRIMARY KEY (message_id, user_id, reaction) 
);

