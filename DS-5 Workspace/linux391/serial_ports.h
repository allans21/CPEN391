#ifndef SERIAL_PORTS_H_   /* Include guard */
#define SERIAL_PORTS_H_
//#include "virtualfuck.h"
#include <stdio.h>
#include "serial.h"

#define RS232_ReceiverFifo        ((volatile unsigned char *)(0xFF210200))
#define RS232_TransmitterFifo     ((volatile unsigned char *)(0xFF210200))
#define RS232_InterruptEnableReg  ((volatile unsigned char *)(0xFF210202))
#define RS232_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210204))
#define RS232_FifoControlReg ((volatile unsigned char *)(0xFF210204))
#define RS232_LineControlReg ((volatile unsigned char *)(0xFF210206))
#define RS232_ModemControlReg ((volatile unsigned char *)(0xFF210208))
#define RS232_LineStatusReg ((volatile unsigned char *)(0xFF21020A))
#define RS232_ModemStatusReg ((volatile unsigned char *)(0xFF21020C))
#define RS232_ScratchReg ((volatile unsigned char *)(0xFF21020E))
#define RS232_DivisorLatchLSB ((volatile unsigned char *)(0xFF210200))
#define RS232_DivisorLatchMSB ((volatile unsigned char *)(0xFF210202))

#define GPS_ReceiverFifo        ((volatile unsigned char *)(0xFF210210))
#define GPS_TransmitterFifo     ((volatile unsigned char *)(0xFF210210))
#define GPS_InterruptEnableReg  ((volatile unsigned char *)(0xFF210212))
#define GPS_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210214))
#define GPS_FifoControlReg ((volatile unsigned char *)(0xFF210214))
#define GPS_LineControlReg ((volatile unsigned char *)(0xFF210216))
#define GPS_ModemControlReg ((volatile unsigned char *)(0xFF210218))
#define GPS_LineStatusReg ((volatile unsigned char *)(0xFF21021A))
#define GPS_ModemStatusReg ((volatile unsigned char *)(0xFF21021C))
#define GPS_ScratchReg ((volatile unsigned char *)(0xFF21021E))
#define GPS_DivisorLatchLSB ((volatile unsigned char *)(0xFF210210))
#define GPS_DivisorLatchMSB ((volatile unsigned char *)(0xFF210212))

#define Bluetooth_ReceiverFifo        ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_TransmitterFifo     ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_InterruptEnableReg  ((volatile unsigned char *)(0xFF210222))
#define Bluetooth_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210224))
#define Bluetooth_FifoControlReg ((volatile unsigned char *)(0xFF210224))
#define Bluetooth_LineControlReg ((volatile unsigned char *)(0xFF210226))
#define Bluetooth_ModemControlReg ((volatile unsigned char *)(0xFF210228))
#define Bluetooth_LineStatusReg ((volatile unsigned char *)(0xFF21022A))
#define Bluetooth_ModemStatusReg ((volatile unsigned char *)(0xFF21022C))
#define Bluetooth_ScratchReg ((volatile unsigned char *)(0xFF21022E))
#define Bluetooth_DivisorLatchLSB ((volatile unsigned char *)(0xFF210220))
#define Bluetooth_DivisorLatchMSB ((volatile unsigned char *)(0xFF210222))

#define TouchScreen_ReceiverFifo        ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_TransmitterFifo     ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_InterruptEnableReg  ((volatile unsigned char *)(0xFF210232))
#define TouchScreen_InterruptIdentificationReg ((volatile unsigned char *)(0xFF210234))
#define TouchScreen_FifoControlReg ((volatile unsigned char *)(0xFF210234))
#define TouchScreen_LineControlReg ((volatile unsigned char *)(0xFF210236))
#define TouchScreen_ModemControlReg ((volatile unsigned char *)(0xFF210238))
#define TouchScreen_LineStatusReg ((volatile unsigned char *)(0xFF21023A))
#define TouchScreen_ModemStatusReg ((volatile unsigned char *)(0xFF21023C))
#define TouchScreen_ScratchReg ((volatile unsigned char *)(0xFF21023E))
#define TouchScreen_DivisorLatchLSB ((volatile unsigned char *)(0xFF210230))
#define TouchScreen_DivisorLatchMSB ((volatile unsigned char *)(0xFF210232))


#define TRUE 1
#define FALSE 0
/**************************************************************************
/* Subroutine to initialise the RS232 Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to UART data sheet for details of registers and programming
***************************************************************************/


//int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
//int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
//int TestForReceivedData(volatile unsigned char *  );
//void Flush( volatile unsigned char *  LineStatusReg , volatile unsigned char *  ReceiverFifo);

// Graphics + touchscreen
typedef struct { int x, y; } Point ;
int ScreenTouched( SerialConf * );
SerialConf * Init_Touch(void *);
void WaitForReleased(SerialConf *);
int ScreenTouched( SerialConf * );
int ScreenReleased( SerialConf * );
Point GetRelease(SerialConf *);
Point GetPress(SerialConf *);
void WaitForTouch(SerialConf *);
void WaitForReleased(SerialConf *);

// Motors
SerialConf* Init_Motors(void*);
void Run_Motors(int, SerialConf *);

#endif // FOO_H_
