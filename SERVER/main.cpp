#include "./inc/webserv.hpp"

bool kqCreate()
{
	kqFD = kqueue();
	if (kqFD < 0 )
		return (false);
	return (true);
}

void serverClose()
{
	return ;
}

int main ()
{
	// CREATE THE KQUEUE
	if( kqCreate() == false )
		return (0);
		// serverClose();
	Core core;
	core.run();
}
