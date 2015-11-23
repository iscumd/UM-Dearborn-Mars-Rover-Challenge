#include "communication.h"
#define BUFFER_SIZE 20000

char startLaser[] = "BM\n";
char stopLaser[] = "QT\n";
char version[] = "VV\n";
char param[] = "PP\n";
char status[] = "II\n";
char singleScan[] = "GD0000108000\n";
char singleIScan[] = "GE0000108000\n";

void zeroBuffer(char *buffer, int len) {
	for(int i = 0; i < len; i++) {
		buffer[i] = 0;
	}
}

void checkHokuyo(comm::TCPConnection *Hokuyo) {
	char buffer[BUFFER_SIZE];
	
	Hokuyo->sendMessage(version);	
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
		
	Hokuyo->sendMessage(param);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
		
	Hokuyo->sendMessage(status);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
	
	Hokuyo->sendMessage(startLaser);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
	
	Hokuyo->sendMessage(singleScan);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
				
	Hokuyo->sendMessage(singleIScan);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
	
	Hokuyo->sendMessage(stopLaser);
	sleep(1);
	Hokuyo->getMessage(buffer, BUFFER_SIZE);
	std::cout << buffer;
	zeroBuffer(buffer, BUFFER_SIZE);
}

int main() {
	std::string address;
	int port;
	std::cout << "Enter IP address: ";
	std::cin >> address;
	std::cout << "\nPort: ";
	std::cin >> port;
	comm::TCPConnection Hokuyo(address.c_str(), port);
	if(Hokuyo.isConnected()) {
		checkHokuyo(&Hokuyo);
	} else {
		Hokuyo.retry();
		if(Hokuyo.isConnected()) {
			checkHokuyo(&Hokuyo);
		} else {
			std::cout << "Really could not connect to Hokuyo.\n";
		}
	}
}
	