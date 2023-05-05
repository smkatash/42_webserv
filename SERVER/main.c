#include "./inc/webserv.hpp"

#define MAX_EVENT 10
#define NUM_SOCKET 10

bool kqCreate(int *kq)
{
	*kq = kqueue;
	if (*kq < 0 )
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
	int kq;

	if( kqCreate( &kq) == false )
		serverClose();
	Core Core(kq);
}