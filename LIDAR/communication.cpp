#ifndef _COMM_CPP_
#define _COMM_CPP_
#include "communication.h"

comm::TCPConnection::TCPConnection(const char *address, int port) {
	log.open("tcp-log.txt", std::ios::out | std::ios::app);
	log.is_open() > 0 ? logging = true : logging = false;
	
	if(logging) { log << "----------------------------------------"; }
	
	if(logging) { log << timestamp() << "Begin logging"; }
	
	_address.assign(address);
	_port = port;
	
	if(logging) { log << timestamp() << "Attempting connection to " << _address << " on port " << _port; }
	
	sockaddr_in TCPout;
	
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
    	}
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}

}

comm::TCPConnection::~TCPConnection() {
	if(logging) {
		log << timestamp() << "Session closing\n";
		log.close();
	}
	shutdown(_socket, 2);
	close(_socket);
}

int comm::TCPConnection::getMessage(char *buffer, int len) {
	int bytesReceived;
	if(connected) {
		if ((bytesReceived = recv(_socket, buffer, len, 0)) < 0) {
			if(logging) { log << timestamp() << "Receive error. Reason: " << strerror(errno); }
			return -1;
		}
		return bytesReceived;
	}
	return 0;
}

int comm::TCPConnection::sendMessage(char *message) {
	int sendLength = strlen(message);
	if(connected) {
		if (send(_socket, message, sendLength, 0) != sendLength){
			if(logging) { log << timestamp() << "Send error. Reason: " << strerror(errno); }
			return -1;
		}
		return sendLength;
	}
	return 0;
}

void comm::TCPConnection::retry() {
	if(logging) { log << timestamp() << "Retrying connection to " << _address << " on port " << _port; }
	
	sockaddr_in TCPout;
	
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
    	}
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}
}

void comm::TCPConnection::retry(const char *address, int port) {
	_address.assign(address);
	_port = port;
	
	if(logging) { log << timestamp() << "Attempting connection to " << _address << " on port " << _port; }
	
	if(logging) { log << timestamp() << "Retrying connection to " << _address << " on port " << _port; }
	
	sockaddr_in TCPout;
	
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
    	}
    	if(connected) {
    		if(logging) { log << timestamp() << "Connected to " << _address << " on port " << _port; }
    	}
	}
}

std::string comm::TCPConnection::timestamp() {
	char timeBuffer[23];
	time(&rawTime);
	Time = localtime(&rawTime);
	strftime(timeBuffer, 23, "\n[%D %T] ", Time);
	std::string timeString(timeBuffer);
	return timeString;
}
#endif
