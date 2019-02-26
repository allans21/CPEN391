#include <stdio.h>
#include <string.h>

#include "pi.h"
#include "cJSON.h"

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

#define TRUE 1
#define FALSE 0

SerialConf* Init_Pi(void *virtual_base) {
	SerialConf *sc = create_serial_conf(virtual_base,
			GPS_ReceiverFifo, GPS_TransmitterFifo,
			GPS_InterruptEnableReg, GPS_InterruptIdentificationReg,
			GPS_FifoControlReg, GPS_LineControlReg, GPS_ModemControlReg,
			GPS_LineStatusReg, GPS_ModemStatusReg, GPS_ScratchReg,
			GPS_DivisorLatchLSB, GPS_DivisorLatchMSB);
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
	 // Reset the Fifo’s in the FiFo Control Reg by setting bits 1 & 2
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
	printf("read start char %c\n", c);
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
		printf("FAILED TO GET NAME\n");
		return 1;
	}

    cJSON *phone_number = cJSON_GetObjectItemCaseSensitive(json, "PHONENUMBER");
	if (cJSON_IsString(phone_number) && (phone_number->valuestring != NULL))
	{
		c->phone_number = strdup(phone_number->valuestring);
	} else {
		printf("FAILED TO GET PHONENUMBER\n");
		return 1;
	}

	cJSON *ID = cJSON_GetObjectItemCaseSensitive(json, "ID");
	if (cJSON_IsNumber(ID)) {
		c->ID = ID->valueint;
	} else {
		printf("FAILED TO GET ID\n");
		return 1;
	}

	cJSON *credits = cJSON_GetObjectItemCaseSensitive(json, "CREDITS");
	if (cJSON_IsNumber(credits)) {
		c->credits = credits->valueint;
	} else {
		printf("FAILED TO GET CREDITS\n");
		return 1;
	}

	cJSON_Delete(json);
	return 0;
}

// Returns error. Points Inventory ** i to a newly allocated array. Caller must free.
// length contains the length of the newly allocated array
int get_inventory(SerialConf *sc, int machine_id, Inventory ** inventoryRet, int *length) {
	// Send command
	char exec_buf[256];
	int execlen = sprintf(exec_buf, "{\"cmd\":\"getInventory\",\"machineId\":%d}", machine_id);
	putline(sc, exec_buf);

	// Receive response
	char response[4096];
	int len = read_response(sc, response);
	printf("Got %d bytes\n", len);
	printf("%s\n", response);

	cJSON *item = NULL;
	cJSON *inventoryArray = cJSON_Parse(response);

	*length = cJSON_GetArraySize(inventoryArray);

	if (*length <= 0) {
		printf("FAILED TO GET JSON INVENTORY ARRAY\n");
		return 1;
	}

	int i = 0;
	Inventory *inventory = malloc(sizeof(Inventory) * (int)(*length));

	// Parse data
	cJSON_ArrayForEach(item, inventoryArray)
	{
		cJSON *slot = cJSON_GetObjectItemCaseSensitive(item, "slot");
		cJSON *price = cJSON_GetObjectItemCaseSensitive(item, "price");
		cJSON *name = cJSON_GetObjectItemCaseSensitive(item, "name");
		cJSON *stock_id = cJSON_GetObjectItemCaseSensitive(item, "stock_id");
		cJSON *quantity = cJSON_GetObjectItemCaseSensitive(item, "quantity");

		char *string = cJSON_Print(item);
		printf("%d: %s\n", i, string);
		free(string);

		if (!cJSON_IsNumber(slot) || !cJSON_IsNumber(price)
				|| !cJSON_IsNumber(stock_id) || !cJSON_IsNumber(quantity)
				|| !cJSON_IsString(name))
		{
			printf("BAD TYPE FOR ITEM %d\n", i);
			free(inventory);
			cJSON_Delete(inventoryArray);
			return 1;
		}
		int slotIdx = slot->valueint;// TODO once DB is fixed slot->valueint;

		inventory[slotIdx].name = strdup(name->valuestring);
		inventory[slotIdx].slot = slot->valueint;
		inventory[slotIdx].price = price->valueint;
		inventory[slotIdx].stock_id = stock_id->valueint;
		inventory[slotIdx].quantity = quantity->valueint;
		i++;
	}


	cJSON_Delete(inventoryArray);
	*inventoryRet = inventory;
	return 0;
}

int make_purchase(SerialConf *sc, int machine_id, int customer_id, int *purchases, int purchases_len, int *newBalance) {
	cJSON *item_ids = cJSON_CreateIntArray(purchases, purchases_len);
	char *item_ids_str = cJSON_Print(item_ids);
	// Send command
	char exec_buf[1024];
	int execlen = sprintf(exec_buf, "{\"cmd\":\"confirmPurchase\",\"machineId\":%d,\"userId\":%d,\"itemIds\":%s}",
			machine_id, customer_id, item_ids_str);
	putline(sc, exec_buf);

	free(item_ids_str);

	// Receive response
	char response[4096];
	int len = read_response(sc, response);
	printf("Got %d bytes\n", len);
	printf("%s\n", response);

	cJSON *json = cJSON_Parse(response);
	cJSON *jsonNewBal = cJSON_GetObjectItemCaseSensitive(json, "newCredit");
	if (!jsonNewBal || !cJSON_IsNumber(jsonNewBal)) {
		cJSON_Delete(json);
		return 1;
	}
	*newBalance = jsonNewBal->valueint;
	cJSON_Delete(json);
	return 0;
}

