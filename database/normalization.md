1 Normal Form:

&#x20;- Every table have its unique primary key:

&#x09;users 			- id

&#x09;users\_info	 	- user\_id

&#x09;unverified\_users 	- id

&#x09;chats		 	- id

&#x09;groups\_info	 	- group\_id

&#x09;chat\_members	 	- chat\_id, user\_id

&#x09;messages	 	- id

&#x09;message\_statuses 	- message\_id, user\_id

&#x09;message\_reactions	- message\_id, user\_id, reaction

&#x09;message\_attachments	- id

&#x09;session\_codes 		- user\_id, device\_id



\- Every cell contains only one inseparatble value.



2 Normal Form

&#x20;- 1NF rules are followed.

&#x20;- Every non-key column must depend on all parts of primary key, not only a half:

&#x09;		USERS

&#x09;email	     	 ->		id

&#x09;user\_tag     	 ->		id

&#x09;password\_hash	 ->		id

&#x09;public\_key     	 ->		id

&#x09;created\_at     	 ->		id



&#x09;		USERS\_INFO

&#x09;name		 ->		user\_id

&#x09;description	 ->		user\_id

&#x09;birth\_date	 ->		user\_id

&#x09;mood		 ->		user\_id



&#x09;		CHATS

&#x09;is\_group	 ->		id

&#x09;created\_at	 ->		id



&#x09;		GROUPS\_INFO

&#x09;is\_public	 ->		group\_id

&#x09;name		 ->		group\_id

&#x09;description	 ->		group\_id



&#x09;		CHAT\_MEMBERS

&#x09;joined\_at	 ->		chat\_id, user\_id  (Full dependency on composite key)



&#x09;		MESSAGES

&#x09;chat\_id		 ->		id

&#x09;sender\_id	 ->		id

&#x09;message\_text	 ->		id

&#x09;replies\_to	 ->		id

&#x09;sent\_at		 ->		id



&#x09;		MESSAGE\_STATUSES

&#x09;is\_read		 ->		message\_id, user\_id  (Full dependency on composite key)

&#x09;read\_at		 ->		message\_id, user\_id  (Full dependency on composite key)



&#x09;		UNVERIFIED\_USERS

&#x09;email		 ->		id

&#x09;user\_tag	 ->		id

&#x09;password\_hash	 ->		id

&#x09;public\_key	 ->		id

&#x09;code		 ->		id

&#x09;created\_at	 ->		id



&#x09;		MESSAGE\_ATTACHMENTS

&#x09;message\_id 	 ->		id

&#x09;file\_path	 ->		id

&#x09;file\_type	 ->		id

&#x09;file\_size	 ->		id

&#x09;file\_name	 ->		id

&#x09;metadata	 ->		id



&#x09;		SESSION\_CODES

&#x09;code		 ->		user\_id, device\_id  (Full dependency on composite key)

&#x09;created\_at	 ->		user\_id, device\_id  (Full dependency on composite key)



&#x09;		MESSAGE\_REACTIONS

&#x09;created\_at	 ->		message\_id, user\_id, reaction (Full dependency on composite key)



3 Normal Form

&#x20;- 1NF and 2NF rules are followed.

&#x20;- No transitive dependencies exists:



&#x09;		USERS\_INFO

&#x09;user\_id -> name        (name does not determine description, birth\_date or mood)

&#x09;user\_id -> description

&#x09;user\_id -> birth\_date

&#x09;user\_id -> mood



&#x09;		GROUPS\_INFO

&#x09;group\_id -> is\_public  (is\_public does not determine name or description)

&#x09;group\_id -> name

&#x09;group\_id -> description



&#x09;		MESSAGES

&#x09;id -> chat\_id          (chat\_id does not determine sender\_id or message\_text)

&#x09;id -> sender\_id

&#x09;id -> message\_text

&#x09;id -> replies\_to

&#x09;id -> sent\_at



&#x09;		SESSION\_CODES

&#x09;(user\_id, device\_id) -> code        (code and created\_at are independent)

&#x09;(user\_id, device\_id) -> created\_at



&#x09;		MESSAGE\_STATUSES

&#x09;(message\_id, user\_id) -> is\_read    (read\_at is a timestamp helper for is\_read, 

&#x09;(message\_id, user\_id) -> read\_at     both strictly depend on the composite PK)



&#x09;		MESSAGE\_ATTACHMENTS

&#x09;id -> message\_id       (message\_id does not determine file\_path, file\_type, etc.)

&#x09;id -> file\_path

&#x09;id -> file\_type

&#x09;id -> file\_size

&#x09;id -> file\_name

&#x09;id -> metadata



&#x20;-- Tables with 0 or 1 non-key attributes automatically satisfy 3NF because it is mathematically 

&#x20;  impossible to have a transitive dependency with less than two non-key columns:

&#x09;CHATS             - Only 1 non-key column (is\_group)

&#x09;CHAT\_MEMBERS      - Only 1 non-key column (joined\_at)

&#x09;MESSAGE\_REACTIONS - Only 1 non-key column (created\_at)



&#x20;-- Fields "email" and "user\_tag" in tables USERS and UNVERIFIED\_USERS are unique candidate keys. Every non-key attribute depends strictly on these keys. No transitive dependencies between non-prime attributes exist, which satisfies 3NF and BCNF.



