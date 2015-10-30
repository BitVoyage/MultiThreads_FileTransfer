all:	dbtime.c file-stream.c file-transfer.c fileserver.c fileclient.c
	@gcc -z execstack -fno-stack-protector  -o server dbtime.c file-stream.c file-transfer.c fileserver.c -lpthread
	@gcc -z execstack -fno-stack-protector  -o client dbtime.c file-stream.c file-transfer.c fileclient.c -lpthread
