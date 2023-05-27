#include "TestServer.hpp"
using namespace webserv;

int main() {
	// std::cout << "Starting ... " << std::endl;
	// std::cout << "Binding socket..." << std::endl;
	// BindingSocket bs = BindingSocket(AF_INET, SOCK_STREAM, 0, 4950, INADDR_ANY);

	// std::cout << "Listening socket..." << std::endl;
	// ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 4951, INADDR_ANY, 10);
	// std::cout << "Completed" << std::endl;
	struct sigaction sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction failed");
		exit(1);
	}
	TestServer	t;

}