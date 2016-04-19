#ifndef __NNLIB_H__       // an identifier unlikely to be define elsewhere
#define __NNLIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#ifndef DEBUG
#define DEBUG(x)    fprintf(stderr, "%s = %g\t", #x,  ((double) x))
#endif

#ifndef DEBUGN
#define DEBUGN(x) fprintf(stderr, "%s=%g\n", #x, ((double) x))
#endif


#ifndef __linux__
#include <windows.h>

#define SERIAL_ERROR  '#'
#define SERIAL_OK 0
int serial_in_use(int portnum); // Defined elsewhere
int register_serial(int portnum, int flag);

//------------------------------------------------------------------------------
HANDLE SerialInit(int PortNum, int BaudRate, int dataBit, int parity,
                  int cts, int rts, int dsr, int dtr);
void SerialClose(HANDLE hCom);
void SerialReInit(HANDLE * hCom, int PortNum, int BaudRate, int dataBit,
                  int parity, int cts, int rts, int dsr, int dtr);
int SerialGetc(HANDLE hCom);    // WARNING: THIS IS A BLOCKING CALL!!
int SerialGets(HANDLE hCom, char buffer[], int numdesired);
BOOL SerialPutc(HANDLE hCom, char txchar);
BOOL SerialPuts(HANDLE hCom, char *s);

#define ClearDTR(hCom) EscapeCommFunction(hCom, CLRDTR)
#define   SetDTR(hCom) EscapeCommFunction(hCom, SETDTR)

#define ClearRTS(hCom) EscapeCommFunction(hCom, CLRRTS)
#define   SetRTS(hCom) EscapeCommFunction(hCom, SETRTS)

#define FlushRx(hCom)      	PurgeComm(hCom,PURGE_RXABORT|PURGE_RXCLEAR)
#define FlushTx(hCom)      	PurgeComm(hCom,PURGE_TXABORT|PURGE_TXCLEAR)


#else                           // LINUX

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ncurses.h>
#include "kbhit.h"

#define SerialClose close


int open_serial(char *port, int baud, int nbits, int parity);
void toggle_bit(int fd, int mask);
void set_bit(int fd, int mask);
void clr_bit(int fd, int mask);
void flush_serial(int fd);

#define toggle_DTR(fd) 	toggle_bit(fd, TIOCM_DTR)
#define set_DTR(fd) 	set_bit(fd, TIOCM_DTR)
#define clr_DTR(fd) 	clr_bit(fd, TIOCM_DTR)

#define toggle_RTS(fd) 	toggle_bit(fd, TIOCM_RTS)
#define set_RTS(fd) 	set_bit(fd, TIOCM_RTS)
#define clr_RTS(fd) 	clr_bit(fd, TIOCM_RTS)

// Clean up this mess


// This is just a dummy file. Eventually we will add mapping windows->linux

#define HANDLE int
#define  INVALID_HANDLE_VALUE   (-1)

#define Sleep(n) usleep((n)*1000)

#endif


#endif //__NNLIB_H__
