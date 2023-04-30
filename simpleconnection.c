#include <sys/types.h>
#include <sys/socket.h>
main (){
	int socketDescriptor;
	int newSocketDescriptor;
	
	char buf[256];
	struct sockaddr socketAddress;
	int fromLen;

	socketDescriptor = socket(AF_UNIX,SOCK_STREAM, 0);

	bind(socketDescriptor,"socketName", sizeof("socketname") -1);
	listen(socketDescriptor,1);

	for(;;)
	{
		newSocketDescriptor = accept(socketDescriptor, &socketAddress, &fromLen);
		if(fork() == 0)
		{
			close(socketDescriptor);
			read(newSocketDescriptor,buf, sizeof(buf));
			printf("server read %s \n", buf);
			exit();
		}
		close(newSocketDescriptor);
	}
}