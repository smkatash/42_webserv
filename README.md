				                        W E B S E R V

                    (c) 2023- by smkatash, jgmaalouf, frame-src / JFK
                         https://https://github.com/smkatash/42_webserv
        
          		    Licensed under WTFPL (see LICENSE file)
                   *NOTE: this is a 42 Project made by three students at 42 Heilbronn*

### Implementation of a server in C++

> The focus of this project is to implement an HTTP/1.1 compliant server capable of handling multiple connections through kernel event notification mechanism according to [RFC](https://datatracker.ietf.org/doc/html/rfc2616).


#### Supported methods:
- GET
- POST
- DELETE

#### Supported CGI scripts:
- python
- perl
- php

### To compile server run: 

```
cd 42_webserv && make
```

If you use Linux, this will not work because we are using kqueue();

### To run server:
Run the executable and press enter or: 
```
./webserv [optional config.conf]
```
If no config file is specified, a default one is provided in the config directory.
