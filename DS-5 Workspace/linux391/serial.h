#ifndef _SERIAL_H_
#define _SERIAL_H_

typedef struct {
	volatile unsigned char * ReceiverFifo;
	volatile unsigned char * TransmitterFifo;
	volatile unsigned char * InterruptEnableReg;
	volatile unsigned char * InterruptIdentificationReg;
	volatile unsigned char * FifoControlReg;
	volatile unsigned char * LineControlReg;
	volatile unsigned char * ModemControlReg;
	volatile unsigned char * LineStatusReg;
	volatile unsigned char * ModemStatusReg;
	volatile unsigned char * ScratchReg;
	volatile unsigned char * DivisorLatchLSB;
	volatile unsigned char * DivisorLatchMSB;
} SerialConf;

SerialConf *create_serial_conf(
		volatile void * virtual_base,
		volatile void * ReceiverFifo,
		volatile void * TransmitterFifo,
		volatile void * InterruptEnableReg,
		volatile void * InterruptIdentificationReg,
		volatile void * FifoControlReg,
		volatile void * LineControlReg,
		volatile void * ModemControlReg,
		volatile void * LineStatusReg,
		volatile void * ModemStatusReg,
		volatile void * ScratchReg,
		volatile void * DivisorLatchLSB,
		volatile void * DivisorLatchMSB);
int putchar_uart (int  , volatile unsigned char *  ,  volatile unsigned char * );
int getchar_uart( volatile unsigned char *  ,  volatile unsigned char *   );
int TestForReceivedData(volatile unsigned char *  );
void Flush( volatile unsigned char *   , volatile unsigned char *  );


#endif
