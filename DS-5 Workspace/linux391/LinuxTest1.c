/*
 * LinuxTest1.c
 *
 *  Created on: Dec 13, 2016
 *      Author: paul
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>

#include "wifi.h"
#include "pi.h"
#include "appObjects.h"

#define MACHINE_ID 1

static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64encode_len(int len)
{
    return ((len + 2) / 3 * 4) + 1;
}

int Base64encode(char *encoded, const char *string, int len)
{
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    *p++ = basis_64[((string[i] & 0x3) << 4) |
                    ((int) (string[i + 1] & 0xF0) >> 4)];
    *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                    ((int) (string[i + 2] & 0xC0) >> 6)];
    *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
        *p++ = basis_64[((string[i] & 0x3) << 4)];
        *p++ = '=';
    }
    else {
        *p++ = basis_64[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
    }
    *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}


#define HW_REGS_BASE ( 0xff200000 )
#define HW_REGS_SPAN ( 0x00200000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define RED_LEDS_OFFSET 0x20
#define HEX1_OFFSET 0x30
#define HEX2_OFFSET 0x40
#define HEX3_OFFSET 0x50

void *update(unsigned int * location){
	int k;
	for(k = 0; k <10; k++){
		*location = *location + 1;
		sleep(1);
	}
}


void upload_img(FILE *openFile, SerialConf *wifi_conf) {
	time_t img_id = time(0);
	const size_t BUFSIZE = 65536;
	char read[BUFSIZE];
	char write[BUFSIZE];

	fseek(openFile, 0, SEEK_END);
	long fsize = ftell(openFile);
	fseek(openFile, 0, SEEK_SET);
	fread(read, 1, fsize, openFile);

	char b64_encoded[BUFSIZE];
	int b64_len = Base64encode(b64_encoded, read, fsize);

	printf("%s\n", b64_encoded);

	printf("og len %d b64 len %d\n", fsize, b64_len);

	const size_t XBUFLEN = 128;
	char exec_buf[XBUFLEN*2];
	char b64_buf[XBUFLEN];
	unsigned int j = 0;
	unsigned int chunk_num = 0;
	while (j < b64_len) {
		memcpy(b64_buf, b64_encoded+j, b64_len-j < XBUFLEN-1 ? b64_len-j : XBUFLEN-1);
		b64_buf[XBUFLEN-1] = '\0';
		int len = sprintf(exec_buf, "s=\"%s\" ", b64_buf);
		exec_lua(wifi_conf, exec_buf, read);
		len = sprintf(exec_buf, "upload_chunk(\"aidanrosswood.ca\", 4000, %d, %d, s)", img_id, chunk_num);
		exec_lua(wifi_conf, exec_buf, read);
		wait_for(wifi_conf, "200 OK");
		j += XBUFLEN - 1;
		chunk_num += 1;
	}

	sprintf(exec_buf, "upload_img(\"aidanrosswood.ca\", 4000, %d) ", img_id);
	exec_lua(wifi_conf, exec_buf, read);
}

int main(void)
{
	unsigned int *Red_Leds = NULL;
	unsigned int *HEX1 = NULL;
	unsigned int *HEX2 = NULL;
	unsigned int *HEX3 = NULL;
	void *virtual_base;
	int fd;

	// Open memory as if it were a device for read and write access
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	// map 2Mbyte of memory starting at 0xFF200000 to user space
	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ),MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return(1);
	}

	// Get the address that maps to the LEDs
	Red_Leds =(unsigned int *)(virtual_base + (( RED_LEDS_OFFSET ) & (HW_REGS_MASK ) ));
	HEX1 =(unsigned int *)(virtual_base + (( HEX1_OFFSET ) & (HW_REGS_MASK ) ));
	HEX2 =(unsigned int *)(virtual_base + (( HEX2_OFFSET ) & (HW_REGS_MASK ) ));
	HEX3 =(unsigned int *)(virtual_base + (( HEX3_OFFSET ) & (HW_REGS_MASK ) ));
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_create(&thread1,NULL, update,Red_Leds);
	pthread_create(&thread2,NULL, update,HEX1);
	pthread_create(&thread3,NULL, update,HEX2);
	pthread_create(&thread4,NULL, update,HEX3);

//	SerialConf * wifi_conf = Init_WiFi(virtual_base);
//
//	char read[65536];
//	FILE* openFile = fopen("/media/barcode.jpg","r");
//	upload_img(openFile, wifi_conf);

	printf("=======================================\n");
	printf("Contacting Pi\n");
	char buf[4096];
	SerialConf * pi_conf = Init_Pi(virtual_base);
	Customer c;
	int err = scan_id(pi_conf, &c);
	printf("Customer ID=%d, credits=%d, name=\"%s\" phone_number=\"%s\"\n", c.ID, c.credits, c.name, c.phone_number);

	Inventory *inventory;
	int len;
	err = get_inventory(pi_conf, MACHINE_ID, &inventory, &len);
	for (int i = 0; i < len; i++) {
		printf("INVENTORY name->%s slot->%d price->%d quantity->%d stock_id->%d\n",
				inventory[i].name, inventory[i].slot, inventory[i].price,
				inventory[i].quantity, inventory[i].stock_id);
	}


	int purchases[] = {inventory[0].stock_id, inventory[0].stock_id, inventory[1].stock_id};
	int new_balance;
	err = make_purchase(pi_conf, MACHINE_ID, c.ID, purchases, 3, &new_balance);

	printf("new_balance after purchase=%d\n", new_balance);


	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);

	FILE *openFile = fopen("/home/root/word.txt","w+");
	fputs("Hello World",openFile);
	fclose(openFile);
	openFile = fopen("/home/root/word.txt","r");
	fgets(buf, 20, (FILE*)openFile);
	printf("%s\n", buf);
	fclose(openFile);

	// when finished, unmap the virtual space and close the memory "device"
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	close( fd );
	return 0;
}
