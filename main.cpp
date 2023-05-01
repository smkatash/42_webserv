#include "webserv.hpp"
#include <sys/event.h>


int main(int argc, char **argv) {
	std::ifstream	file;

	if (argc < 2)
		file.open("./config/default.conf");
	else if (argc == 2) {
		const std::string ext = ".conf";
		std::string input = argv[1];
		if (input.find_last_of(".") == std::string::npos || \
			input.substr(input.find_last_of(".")) != ext) {
			std::cerr << "error: file must have .conf extension" << std::endl;
			exit(EXIT_FAILURE);
		}
		file.open(argv[1]);
	}
	else {
		std::cerr << "usage: ./webserv config_file\n";
		exit(EXIT_FAILURE);
	}

	try {
		Parser	conf(file);
		conf.setConfigFile();
		conf.debugInput();
	} catch (std::invalid_argument& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	} catch (const std::out_of_range & e) {
		std::cerr << e.what() << "\n";
		exit(EXIT_FAILURE);
	}
	kq = kqueue();
	Core core(conf.conf_, kq);
	Socket socket (); 
	
}
/*
	1) -Create a socket: The first step in creating a server is to create a socket that listens for incoming connections. 
		This can be achieved using the socket() function.

	2) -Bind the socket to an address: Once the socket is created, it needs to be bound to a specific address and port on the server.
		This can be achieved using the bind() function.

	3) -Listen for incoming connections: After binding the socket, the server needs to start listening for incoming connections. 
		This can be achieved using the listen() function.

	4) -Initialize the kqueue: The next step is to initialize the kqueue data structure, which will be used to monitor events on the socket.
		This can be achieved using the kqueue() function.

	5) -Add the socket to the kqueue: Once the kqueue is initialized, the server needs to add the socket to the kqueue
		This, using the kevent() function, will allow the server to monitor for incoming connections and data.
	
	6) -Monitor events using kqueue: The server can now monitor events using the kqueue data structure.
		This can be achieved using the kevent() function to check for new events on the socket.

	7) -Accept incoming connections: When a new connection is detected, the server can accept it using the accept() function.
		This will create a new socket for communication with the client.

	8) -Handle incoming data: Once a connection is established, the server can handle incoming data from the client. 
		This require recv() function.
		
	9) -Send data back to the client: Finally, the server can send data back to the client.
		This use the send() function.
*/

/*
	1) -Create a kqueue object using the kqueue() function;

	2) -Create an array of kevent structures to describe the events that the server is interested in monitoring.
		In this case, you would create a kevent structure for each socket file descriptor that the server is listening on;

	3) -Call the kevent() function to monitor the events on the sockets;

	4) -When a socket event is detected, the kevent() function will return 
		the corresponding kevent structure in the eventlist array.
		You can use the file descriptor in the kevent structure to determine which socket has an event pending:
*/