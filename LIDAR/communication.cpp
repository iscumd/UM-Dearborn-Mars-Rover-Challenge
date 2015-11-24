#ifndef _COMM_CPP_
#define _COMM_CPP_
#include "communication.h"

/*
	Instead of defining macros to match up functions from the windows API to the Unix API
	(which could be done, and anyone who wants to is welcome to do so, provided it is in their own copy),
	I opted instead to write the sections as two separate ones, so anyone who wants to understand the differences
	can see it plainly.

	_WIN32 seems to be the common macro for windows machines, and Apple includes the standard headers with OSX

	So any time in either this code, or likely the rest of the lidar code that '#if !defined(_WIN32)' is written,
	know that is the section for Unix/OS X code, and in between the '#else' and '#end if' is the Windows code.

	Running this code on any machine will create 'tcp-log.txt' in the directory the program was run.
	This log file is appended to each time the program is run.

	Functions:

	constructor TCPConnection(const char *address, int port)
		Sets up a TCPConnection object using an IP address and a port to connect to.

	destructor TCPConnection()
		Shuts down socket and closes log file if logging was active

	int getMessage(char *buffer, int len)
		If there is an active connection, write any data received into the buffer up to len - 1.
		Make sure that the buffer is large enough to accomodate the size of messages you expect to get.
		If unsuccessful, returns -1, and logs the error. If successful, returns number of bytes read.
		Attempting to send with an inactive connection will cause function to return -2.

	int sendMessage(char *message)
		If there is an active connection, send data from provided message to the TCPConnection's address.
		If unsuccessful, returns -1, and logs the error. If successful, returns the number of bytes read.
		Attempting to send with an inactive connection will cause function to return -2.

	void retry()
		Attempts to connect to the currently stored address and port. Functions almost like the constructor, but uses
		previously provided connection information.

	void retry(char *address, int port)
		Attempts to connect to address at specified port. Functions like constructor, but using new connection information.


	Last Edit: 11/24/15 4:12 PM by Matthew Abraham

*/

comm::TCPConnection::TCPConnection(const char *address, int port) {
	log.open("tcp-log.txt", std::ios::out | std::ios::app);
	log.is_open() > 0 ? logging = true : logging = false;

	if(logging) { log << "----------------------------------------"; } // check if we're logging, and print out a session separator line. the logging check is done before every logged line

	if(logging) { log << timestamp() << "Begin logging"; }

	_address.assign(address); // store the address in the address string
	_port = port;

	sockaddr_in TCPout;

#	if !defined(_WIN32) /* BEGIN UNIX SOCKET CODE */

	if(logging) {
		log << timestamp() << "OS detected as Unix-like/Mac OS X";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); } // strerror prints out a short error description based on the int value it got as a parameter, in this case, the global errno variable.
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) { // retry allocating a socket 5 times before giving up. after that it's up to the user to call retry()
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0) {
				break;
			} else {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
			}
		}
		if(_socket < 0) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
		}
	}
	if(_socket >= 0) { // do we have a socket? good.
		memset(&TCPout, 0, sizeof(TCPout)); // sockaddr_in must be zero on every bit, hence the memset call
    	TCPout.sin_family = AF_INET; // refers to the protocol family
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str()); // human-readable address must be converted to a machine readable one
    	TCPout.sin_port = htons(_port); // uh, honestly dont know why this is there. magic nattu code?
    	if(connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) < 0) { // if we didn't connect, retry like we did for sockets
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if((connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) >= 0)) {
    				connected = true;
    				break;
    			} else {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    				connected = false;
    			}
    		}
    	} else { connected = true; } // we did connect, yay
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

#	else /* BEGIN WINDOWS SOCKET CODE */

	if(logging) {
		log << timestamp() << "OS detected as Windows";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	int errorCode;

	errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData); // honestly dont know what this is for.
	if(errorCode != 0) {
		log << timestamp() << "WSAStartup failed with error " << errorCode;
		connected = false;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); } // WSAGetLastError returns an integer value . In order to get the associated string, you have to jump through 20 hoops.
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) { // like in the unix code, if at first you don't succeed, try try again
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
			} else {
				break;
			}
		}
		if(_socket == INVALID_SOCKET) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
		}
	}

	if(_socket >= 0) {
		memset(&TCPout, 0, sizeof(TCPout));
    	TCPout.sin_family = AF_INET;
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str());
    	TCPout.sin_port = htons(_port);
    	if((errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout))) == SOCKET_ERROR) {
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if(errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) == SOCKET_ERROR) {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    				connected = false;
    			} else {
    				connected = true;
    				break;
    			}
    		}
    	} else { connected = true; }
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}
# 	endif
// maybe you noticed they're almost the same? winsock2 is basically a wrapper around unix sockets.
}

comm::TCPConnection::~TCPConnection() {
	if(logging) {
		log << timestamp() << "Session closing\n";
		log.close();
	}

#	if !defined(_WIN32)

	shutdown(_socket, 2);
	close(_socket);

#	else

	shutdown(_socket, SD_BOTH);
	closesocket(_socket);
	WSACleanup();

#	endif
}

int comm::TCPConnection::getMessage(char *buffer, int len) {
	int bytesReceived;
	if(connected) {

#	if !defined(_WIN32)

		if ((bytesReceived = recv(_socket, buffer, len, 0)) < 0) {
			if(logging) { log << timestamp() << "Receive error. Reason: " << strerror(errno); }
			return -1;
		}
		return bytesReceived;

#	else

	if ((bytesReceived = recv(_socket, buffer, len, 0)) < 0) {
			if(logging) { log << timestamp() << "Receive error. Reason: " << WSAGetLastError(); }
			return -1;
		}
		return bytesReceived;

#	endif

	}

	if(logging) { log << timestamp() << "No active connection. Use retry() or retry(char *address, int port) before calling getMessage or sendMessage."; }
	return -2;
}

int comm::TCPConnection::sendMessage(char *message) {
	int sendLength = strlen(message);
	if(connected) {

#	if !defined(_WIN32)

		if (send(_socket, message, sendLength, 0) != sendLength){
			if(logging) { log << timestamp() << "Send error. Reason: " << strerror(errno); }
			return -1;
		}
		return sendLength;

#	else
		if (send(_socket, message, sendLength, 0) != sendLength){
			if(logging) { log << timestamp() << "Send error. Reason: " << WSAGetLastError(); }
			return -1;
		}
		return sendLength;

#	endif

	}

	if(logging) { log << timestamp() << "No active connection. Use retry() or retry(char *address, int port) before calling getMessage or sendMessage."; }
	return -2;
}

void comm::TCPConnection::retry() {
	if(logging) { log << timestamp() << "Retrying connection to " << _address << " on port " << _port; }

	sockaddr_in TCPout;

#	if !defined(_WIN32) /* BEGIN UNIX SOCKET CODE */

	if(_socket >= 0) {
		shutdown(_socket, 2);
		close(_socket);
	}

	if(logging) {
		log << timestamp() << "OS detected as Unix-like/Mac OS X";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) {
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0) {
				break;
			} else {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
			}
		}
		if(_socket < 0) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
		}
	}
	if(_socket >= 0) {
		memset(&TCPout, 0, sizeof(TCPout));
    	TCPout.sin_family = AF_INET;
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str());
    	TCPout.sin_port = htons(_port);
    	if(connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) < 0) {
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if((connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) >= 0)) {
    				connected = true;
    				break;
    			} else {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    				connected = false;
    			}
    		}
    	} else { connected = true; } // we did connect, yay
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

#	else /* BEGIN WINDOWS SOCKET CODE */

	if(_socket >= 0) {
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
	}

	if(logging) {
		log << timestamp() << "OS detected as Windows";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	int errorCode;

	errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(errorCode != 0) {
		log << timestamp() << "WSAStartup failed with error " << errorCode;
		connected = false;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) {
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
			} else {
				break;
			}
		}
		if(_socket == INVALID_SOCKET) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
		}
	}

	if(_socket >= 0) {
		memset(&TCPout, 0, sizeof(TCPout));
    	TCPout.sin_family = AF_INET;
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str());
    	TCPout.sin_port = htons(_port);
    	if((errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout))) == SOCKET_ERROR) {
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if(errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) == SOCKET_ERROR) {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    				connected = false;
    			} else {
    				connected = true;
    				break;
    			}
    		}
    	} else { connected = true; }
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

# 	endif

}

void comm::TCPConnection::retry(const char *address, int port) {
	_address.assign(address);
	_port = port;

	if(logging) { log << timestamp() << "Attempting connection to " << _address << " on port " << _port; }

	sockaddr_in TCPout;

#	if !defined(_WIN32) /* BEGIN UNIX SOCKET CODE */

	if(_socket >= 0) {
		shutdown(_socket, 2);
		close(_socket);
	}

	if(logging) {
		log << timestamp() << "OS detected as Unix-like/Mac OS X";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) {
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0) {
				break;
			} else {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
			}
		}
		if(_socket < 0) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << strerror(errno); }
		}
	}
	if(_socket >= 0) {
		memset(&TCPout, 0, sizeof(TCPout));
    	TCPout.sin_family = AF_INET;
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str());
    	TCPout.sin_port = htons(_port);
    	if(connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) < 0) {
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if((connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) >= 0)) {
    				connected = true;
    				break;
    			} else {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << strerror(errno); }
    				connected = false;
    			}
    		}
    	} else { connected = true; } // we did connect, yay
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

#	else /* BEGIN WINDOWS SOCKET CODE */

	if(_socket >= 0) {
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
	}

	if(logging) {
		log << timestamp() << "OS detected as Windows";
		log << timestamp() << "Attempting connection to " << _address << " on port " << _port;
	}

	int errorCode;

	errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(errorCode != 0) {
		log << timestamp() << "WSAStartup failed with error " << errorCode;
		connected = false;
	}

	if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
		if(logging) { log << timestamp() << "Retrying socket."; }
		for(int i = 0; i < 5; i++) {
			if((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
				if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
			} else {
				break;
			}
		}
		if(_socket == INVALID_SOCKET) {
			if(logging) { log << timestamp() << "Could not allocate socket. Reason: " << WSAGetLastError(); }
		}
	}

	if(_socket >= 0) {
		memset(&TCPout, 0, sizeof(TCPout));
    	TCPout.sin_family = AF_INET;
    	TCPout.sin_addr.s_addr = inet_addr(_address.c_str());
    	TCPout.sin_port = htons(_port);
    	if((errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout))) == SOCKET_ERROR) {
    		if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    		if(logging) { log << timestamp() << "Retrying connection."; }
    		for(int i = 0; i < 5; i++) {
    			if(errorCode = connect(_socket, (sockaddr *)&TCPout, sizeof(TCPout)) == SOCKET_ERROR) {
    				if(logging) { log << timestamp() << "Could not connect. Reason: " << WSAGetLastError(); }
    				connected = false;
    			} else {
    				connected = true;
    				break;
    			}
    		}
    	} else { connected = true; }
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

# 	endif
}

std::string comm::TCPConnection::timestamp() {
	char timeBuffer[23]; // how long the time string is
	time(&rawTime); // wibbly wobbly timey wimey stuff
	Time = localtime(&rawTime);
	strftime(timeBuffer, 23, "\n[%D %T] ", Time); // returns time formatted as MM/DD/YY HH:MM:SS
	std::string timeString(timeBuffer);
	return timeString;
}
#endif
