#include <stdio.h>
#include <string.h>

#include "pi.h"
#include "cJSON.h"

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
#define TRUE 1
#define FALSE 0

SerialConf* Init_Pi(void *virtual_base) {
	SerialConf *sc = create_serial_conf(virtual_base,
			Bluetooth_ReceiverFifo, Bluetooth_TransmitterFifo,
			Bluetooth_InterruptEnableReg, Bluetooth_InterruptIdentificationReg,
			Bluetooth_FifoControlReg, Bluetooth_LineControlReg, Bluetooth_ModemControlReg,
			Bluetooth_LineStatusReg, Bluetooth_ModemStatusReg, Bluetooth_ScratchReg,
			Bluetooth_DivisorLatchLSB, Bluetooth_DivisorLatchMSB);
	// set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
	unsigned char line_control_register;
	line_control_register= *(sc->LineControlReg);
	line_control_register = line_control_register |  0x80;
	*(sc->LineControlReg) = line_control_register;
	 // set Divisor latch (LSB and MSB) with correct value for required baud rate
	// example 0x0145
	 *(sc->DivisorLatchLSB) = 0x1B;
	 *(sc->DivisorLatchMSB) = 0x00;
	 // set bit 7 of Line control register back to 0 and
	 // program other bits in that reg for 8 bit data,
	 // 1 stop bit, no parity etc
	 //0000 0011
	 *(sc->LineControlReg) = 0x03;
	 // Reset the Fifo�s in the FiFo Control Reg by setting bits 1 & 2
	*(sc->FifoControlReg) = *(sc->FifoControlReg) | 0x06;
	 // Now Clear all bits in the FiFo control registers
	*(sc->FifoControlReg) = *(sc->FifoControlReg) ^ 0x06;

	return sc;
}

int read_response(SerialConf *sc, char *buf) {
	int len = 0;
	char c = getchar_uart(sc);
	while (c != '{' && c != '[') {
		c = getchar_uart(sc);
	}
	char startChar = c;
	char stopChar = c == '{' ? '}' : ']';
	int stack = 1;
	buf[len++] = c;
	while (stack) {
		c = getchar_uart(sc);
		buf[len++] = c;
		if (c == stopChar) {
			stack--;
		} else if (c == startChar) {
			stack++;
		}
	}
	buf[len] = '\0';
	return len;
}

int scan_id(SerialConf *sc, Customer *c){
	putline(sc, "{\"cmd\":\"scanId\"}");
	char response[4096];
	int len = read_response(sc, response);
	printf("Got %d bytes\n", len);
	printf("%s\n", response);
	cJSON *json = cJSON_Parse(response);

	//{"CREDITS": 10000, "PHONENUMBER": "778-866-2529", "NAME": "Aidan Rosswood", "ID": 12}
	// DOCS FOR JSON -> https://github.com/DaveGamble/cJSON

	cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "NAME");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        c->name = strdup(name->valuestring);
    } else {
		printf("FAILED TO GET NAME");
		return 1;
	}

    cJSON *phone_number = cJSON_GetObjectItemCaseSensitive(json, "PHONENUMBER");
	if (cJSON_IsString(phone_number) && (phone_number->valuestring != NULL))
	{
		c->phone_number = strdup(phone_number->valuestring);
	} else {
		printf("FAILED TO GET PHONENUMBER");
		return 1;
	}

	cJSON *ID = cJSON_GetObjectItemCaseSensitive(json, "ID");
	if (cJSON_IsNumber(ID)) {
		c->ID = ID->valueint;
	} else {
		printf("FAILED TO GET ID");
		return 1;
	}

	cJSON *credits = cJSON_GetObjectItemCaseSensitive(json, "CREDITS");
	if (cJSON_IsNumber(credits)) {
		c->credits = credits->valueint;
	} else {
		printf("FAILED TO GET CREDITS");
		return 1;
	}

	cJSON_Delete(json);
	return 0;
}

int get_inventory(SerialConf *sc, int machine_id, Inventory ** i) {
	// Send command
	char exec_buf[256];
	int execlen = sprintf(exec_buf, "{\"cmd\":\"getInventory\",\"machineId\":%d}", machine_id);
	putline(sc, exec_buf);

	// Receive response
	char response[4096];
	int len = read_response(sc, response);
	printf("Got %d bytes\n", len);
	printf("%s\n", response);

	// Parse data


	return 0;
}