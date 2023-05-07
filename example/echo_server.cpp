#include <studio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "smtng.h"

class info
{
    
}

void die(char *message)
{
    if(message == NULL) 
        return ;
    printf("%s \n", message);
    return ;
}
  
int main (int argc, char *argv[])
{
    char *serverIp;
    char *message;
    //port number 7 is the classical used for echo protocol;
    int serverPort = (argc == 4) ? atoi(argv[3]) : 7 ;

    if (argc < 3 || argc > 4) //test for correct number of arguments
        std::cout << "ERROR, wrong args" << std::endl 
                    ;
        std::cout << "
                        USAGE: <server address> 
                        <word to echo> 
                        [?]<server port>" << std::endl 
                    ;
        
    serverIp = argv[1];
    message = argv[2];

    //TCP straming socket;
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
    if (sock < 0)
        die("socket failed");

}