
				  W E B S E R V

                (c) 20023- by smkatash jgmaalouf frame-src / JFK
                 https://https://github.com/smkatash/42_webserv

  		    Licensed under WTFPL (see LICENSE file)
           NOTE: this is a 42 Project made by three student in Heilbronn;

INTRODUCTION
------------
The focus of this project is to create an HTTP1.1 TCP Server 
capable of handling multiple connection trough kernel event notification mechanism.
The hardest part of the project was at the start:
No one of us had a previous experience with HTTP protocols 
so we had to read and understand the rfc first, and play with NGNX.

There are already several server available, and this will not be the best ones,
but it will be HTTP1.1 compliant enough for basic static pages.

The server was tested with siege, and it perform well with -b flag.

Currently this server supports the following methods:
GET POST DELETE;
We are handling 3 different CGI, for non static request, and cookie sessions;


HOW TO COMPILE
--------------
To configure, compile and install webserv, just type:

```
make
```
If you use Ubuntu/Debian, this will not work because we are using kqueue() (shade :( );

HOW TO USE
----------
Run the executable and press enter or: 
```
./webserv [optional]config.conf
```
If no config file is specified it will run the default one.

If something goes wrong you will recive a notification.

