/*
 * system.c

 *
 *  Created on: Feb 6, 2019
 *      Author: criss
 */
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <time.h>

#define HW_REGS_BASE ( 0xff200000 )
#define HW_REGS_SPAN ( 0x00200000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define MACHINE_ID 1

#define N_ERR_LINES 3

#include "virtualfuck.h"
#include "serial_ports.h"
#include "screenfunctions.h"
#include "wifi.h"
#include "pi.h"
#include "serial.h"
#include "appObjects.h"

enum state{
	main_page,
	scanid_page,
	error_page,
	picktype_page,
	payment_page,
	dispense_page,
	make_selection_page,
	complete_page,
	payment_confirmation_page
};

volatile int virtual_base;
int main_tes () {

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


	enum state next_state;
	next_state = main_page;
	int colour_welcome = 0;
	int counter = 0;
	SerialConf * touch_conf = Init_Touch(virtual_base);
	SerialConf * motor_conf = Init_Motors(virtual_base);
	SerialConf * pi_conf    = Init_Pi(virtual_base);

    int amount_to_pay = 0;
    int valid;
	Point touch;
	int flag = TRUE;
	char key;
	int err = FALSE;
	Customer customer;

	int inventoryLen;
	int totalPrice = 500;
	int quantities[64] = {0};

	char *error_msgs[N_ERR_LINES]  = {"", "", ""};
					error_msgs[0] = "";
				   error_msgs[1] = "Failed to connect to the internet.";
				   error_msgs[2] = "";






	 customer.ID=9;


	 typedef struct {
	 	int    ID;
	 	int    credits;
	 	char * phone_number;
	 	char * name;
	 } Customer;



	 customer.credits=1000;
	 customer.phone_number="99";
	 customer.name="Cristian B";
		Inventory *inventory  = malloc(sizeof(Inventory) * 3);
	 int slotIdx=0;
	 inventory[0].name = "cigarettes";
	 		inventory[slotIdx].slot = 0;
	 		inventory[slotIdx].price = 100;
	 		inventory[slotIdx].stock_id = 0;
	 		inventory[slotIdx].quantity = 10;

	 		slotIdx++;
	 		inventory[slotIdx].name = "stellas";
	 				inventory[slotIdx].slot = 1;
	 				inventory[slotIdx].price = 4000;
	 				inventory[slotIdx].stock_id = 1;
	 				inventory[slotIdx].quantity = 10;







	printf("WiFi test... SUCCESS\n");
	while(1){
		char key [5]={'x','x','x','x','/0'};

		while(1){
		 Pal();
		 sleep(1000);
		 keypad_screen(touch_conf, key,4);
		}
		 WaitForTouch(touch_conf);
			    WaitForReleased(touch_conf);
		 First_StartingScreen();
		 WaitForTouch(touch_conf);
	    WaitForReleased(touch_conf);

	    Loading("   While Your I.D Is Verified    ", 34);
	    WaitForTouch(touch_conf);
	   	WaitForReleased(touch_conf);

	 	   ErrorID(error_msgs, 3);
	 	  WaitForTouch(touch_conf);
	 	 	    WaitForReleased(touch_conf);

	 	  PickType(&customer, inventory, 2);


	 	    WaitForTouch(touch_conf);
	 		   	WaitForReleased(touch_conf);
	 	 Payment(100);
	 	 Loading("While Your Payment Is Processed", 37);
	 	WaitForTouch(touch_conf);
	 		 		   	WaitForReleased(touch_conf);

	 		 		  Complete(customer.credits);
	 		 		WaitForTouch(touch_conf);
	 		 			 		 		   	WaitForReleased(touch_conf);

	 		 		  Dispense();



	}

	printf("Looping!\n");

return 0;
}
