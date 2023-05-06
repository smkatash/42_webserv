#include "./inc/webserv.hpp"

bool kqCreate()
{
	kqFD = kqueue;
	if (kqFD < 0 )
		return (false);
	return (true);
}

void serverClose()
{
	exit ();
}

int main ()
{
	// CREATE THE KQUEUE
	if( kqCreate() == false )
		serverClose();
	Core Core();
	Core.run;
}