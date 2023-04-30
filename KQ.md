KQ is a fast signal-delivery mechanism that allows software to process events frin OS in a very effective way.
We don't access data of the KQ directly but we use some API that could be:
Kqueue, epool IOCP ecc...;

In our case it is usefull because: 
Our app "Webserv" get notified about a TCP connections such as " a network connection" 
So:

-1 TCP socket is registered with a KQ;
-2 


#First.
	- We need to create the KQ object. It's the easiest part, we just call a function which returns the descriptor for our new KQ.
	- We may create KQ objects as many as we want, but I don't see the point of creating more than 1 per process or thread.

#Second. 
	- We attach file/socket descriptor along with opaque user data to KQ. 
	- We have to tell the OS that we want it to notify us about any particular descriptor through a particular KQ object. How else the kernel should know what to notify us about? Here we also associate some data with the descriptor, which is usually a pointer to some kind of a structure object. How else are we going to handle the received signal? The attachment is needed only once for each descriptor, usually it's done right after the descriptor is configured and ready for I/O operations (though we can delay that until absolutely necessary to probably save a context switch). 
	- The detachment procedure usually is not needed (with the right design).
#Third.
	- The main app calls a KQ waiting function. We specify the output array of the events and the timeout value and it will fill the array with the value that we need for which event.
#Fourth.
	- If we dont need the KQ anymore we destroy it closing the KQ file.