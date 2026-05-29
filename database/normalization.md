# Proving normal forms

    Normal Forms (NF) are a progressive set of mathematical rules and criteria used to design a relational database schema. Each level of normalization (from 1NF up to 6NF) builds upon the previous one, ensuring that the database structure becomes cleaner, more logical, and more stable.

    The combination of 1NF, 2NF, and 3NF mitigates roughly 99% of all real-world data anomalies and redundancy issues. Higher normal forms require breaking tables down into tiny, hyper-isolated fragments. While mathematically pure, this forces the backend server to run heavy, nested JOIN queries, severely degrading read/write performance under load. 

## 1 Normal Form:

- Every table has its unique primary key:

    |	DB TABLE		| KEY |
    | :---: | :---: |
    |	users 			| id |
    |	users_info	 	| user_id |
    |	unverified_users 	| id |
    |	chats		 	| id |
    |	groups_info	 	| group_id |
    |	chat_members	 	| chat_id, user_id |
    |	messages	 	| id |
    |	message_statuses 	| message_id, user_id |
    |	message_reactions	| message_id, user_id, reaction |
    |	message_attachments	| id |
    |	session_codes 		| user_id, device_id |

- Every cell contains only one inseparable value.

## 2 Normal Form

- **1NF** rules are followed.

- Every non-key column must depend on all parts of primary key, not only a half:

### USERS
	email              ->  id
	user_tag           ->  id
	password_hash      ->  id
	public_key         ->  id
	created_at         ->  id

### USERS_INFO
	name               ->  user_id
	description        ->  user_id
	birth_date         ->  user_id
	mood               ->  user_id

### CHATS
	is_group           ->  id
	created_at         ->  id

### GROUPS_INFO
	is_public          ->  group_id
	name               ->  group_id
	description        ->  group_id

### CHAT_MEMBERS
	joined_at          ->  chat_id, user_id  (Full dependency on composite key)

### MESSAGES
	chat_id            ->  id
	sender_id          ->  id
	message_text       ->  id
	replies_to         ->  id
	sent_at            ->  id

### MESSAGE_STATUSES
	is_read            ->  message_id, user_id  (Full dependency on composite key)
	read_at            ->  message_id, user_id  (Full dependency on composite key)

### UNVERIFIED_USERS
	email              ->  id
	user_tag           ->  id
	password_hash      ->  id
	public_key         ->  id
	confirmation_code  ->  id
	created_at         ->  id

### MESSAGE_ATTACHMENTS
	message_id         ->  id
	file_path          ->  id
	file_type          ->  id
	file_size          ->  id
	file_name          ->  id
	metadata           ->  id

### SESSION_CODES
	code               ->  user_id, device_id  (Full dependency on composite key)
	created_at         ->  user_id, device_id  (Full dependency on composite key)

### MESSAGE_REACTIONS
	created_at         ->  message_id, user_id, reaction (Full dependency on composite key)


## 3 Normal Form

- **1NF** and **2NF** rules are followed.

- No *transitive* dependencies exist:

### USERS_INFO
    user_id -> name        (name does not determine description, birth\_date or mood)
    user_id -> description
    user_id -> birth_date
    user_id -> mood



### GROUPS_INFO
    group_id -> is_public  (is_public does not determine name or description)
    group_id -> name
    group_id -> description



### MESSAGES
    id -> chat_id          (chat_id does not determine sender_id or message_text)
    id -> sender_id
    id -> message_text
    id -> replies_to
    id -> sent_at



### SESSION_CODES
    (user_id, device_id) -> code        (code and created_at are independent)
    (user_id, device_id) -> created_at



### MESSAGE_STATUSES
    (message_id, user_id) -> is_read    (read_at is a timestamp helper for is_read,
    (message_id, user_id) -> read_at     both strictly depend on the composite PK)



### MESSAGE_ATTACHMENTS
    id -> message_id       (message_id does not determine file_path, file_type, etc.)
    id -> file_path
    id -> file_type
    id -> file_size
    id -> file_name
    id -> metadata


> [!NOTE]
>**Tables with 0 or 1 non-key attributes automatically satisfy 3NF.**  
>It is mathematically impossible to have a transitive dependency with less than two non-key columns:
>  
>- CHATS&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; - Only 1 non-key column (is_group)  
>- CHAT_MEMBERS&emsp;&emsp;&nbsp;&nbsp; - Only 1 non-key column (joined_at)  
>- MESSAGE_REACTIONS - Only 1 non-key column (created_at)


> [!IMPORTANT]
> Fields "email" and "user_tag" in tables USERS and UNVERIFIED_USERS are unique candidate keys. Every non-key attribute depends strictly on these keys. No transitive dependencies between non-prime attributes exist, which satisfies 3NF and BCNF.

