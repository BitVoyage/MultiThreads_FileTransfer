#include"file-stream.h"
#include"file-transfer.h"



void main()
{
	int fd;
	
	fd = open_client();
	
	request(fd, "hello.txt");
	
	tcp_file_get(fd);
	
}
