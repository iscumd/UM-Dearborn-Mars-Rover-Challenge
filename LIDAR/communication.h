//TCP communication for LIDARs
#ifndef _COMM_H_
#define _COMM_H_
#include <ctime> // for timestamp() and required time functions
#include <cstring> // for strerror()
#include <iostream> // logging
#include <fstream>
#include <string>
#if !defined(_WIN32)
#	include <errno.h> // global error variable set by send, recv, and friends
#	include <arpa/inet.h> // inet.h and socket.h have the socket interfaces and functions
#	include <sys/socket.h>
#	include <unistd.h> // unix standard header
#else
#	include <winsock2.h>
#   include <ws2tcpip.h>
#   define sleep(n) Sleep(n)
#endif

namespace comm {

	class TCPConnection {
		public:
			TCPConnection(const char *address, int port);
			~TCPConnection();
			int getMessage(char *buffer, int len);
			int sendMessage(char *message);
			void retry();
			void retry(const char *address, int port);
			bool isLogging() { return logging; };
			bool isConnected() { return connected; };
		private:
			std::string _address;
			std::fstream log;
			bool logging = true, connected = true;
			time_t rawTime;
			tm *Time;
			std::string timestamp();
			//const int BUFFER_SIZE = 20000;
#			if !defined(_WIN32)
				int _port, _socket;
#			else
				WSADATA wsaData; // honestly, no clue what this is for, except for starting up windows sockets? used once.
				SOCKET _socket = INVALID_SOCKET;
				int _port;
#			endif
			//char internalBuffer[BUFFER_SIZE];
	};
}
#endif
