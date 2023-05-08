#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <stdbool.h>

// CORE
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>

// SERVER
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/event.h>
#include <stdint.h>
#include <machine/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Core"
#include "Server.hpp"
#include "Socket.hpp"

#define MAX_EVENT 10
#define NUM_SOCKET 10

int kqFD;

void serverClose();


#endif