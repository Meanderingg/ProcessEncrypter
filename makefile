all: encryptor

encryptor: encryptor.c decr encr
	gcc -o encryptor encryptor.c

decr: decr.c
	gcc -o decr decr.c

encr: encr.c
	gcc -o encr encr.c
