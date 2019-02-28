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
int main () {

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
	int err = FALSE;
	Customer customer;
	Inventory *inventory = NULL;
	int inventoryLen;
	int totalPrice = 500;
	int quantities[64] = {0};

	char *error_msgs[N_ERR_LINES]  = {"", "", ""};

	printf("Pi serial test... ");
	err = test_pi_serial(pi_conf);
	if (err) {
		printf("FAILURE\n");
	} else {
		printf("SUCCESS\n");
	}

	printf("Pi serial to server test... ");
	err = test_pi_server(pi_conf);
	if (err) {
		printf("FAILURE\n");
	} else {
		printf("SUCCESS\n");
	}

	printf("Pi to server to database test... ");
	err = get_inventory(pi_conf, MACHINE_ID, &inventory, &inventoryLen);
	if (err) {
		printf("FAILURE\n");
	} else {
		printf("SUCCESS\n");
		for (int i = 0; i < inventoryLen; i++) {
			free(inventory[i].name);
		}
		free(inventory);
		inventory = NULL;
	}

	printf("Arduino serial test... ");
	test_arduino_serial(motor_conf);
	printf("SUCCESS\n");

	SerialConf * wifi_conf  = Init_WiFi(virtual_base);
	printf("WiFi test... SUCCESS\n");

	printf("Looping!\n");
	while(1){

        // Per page logic
		switch(next_state) {

		   case main_page:
			   if (inventory) {
				   for (int i = 0; i < inventoryLen; i++) {
				       free(inventory[i].name);
				   }
				   free(inventory);
				   inventory = NULL;
			   }
		   	   colour_welcome = 0;
		   	   next_state = main_page;
		   	   First_StartingScreen();
		   	   flag = TRUE;
		   	   while(flag) {
		   	   	   StartingScreen(colour_welcome);
		   	   	   colour_welcome++;
			       //loops here until the screen has been touched;

			  	   for(int i=0;i<700000;i++) {
			  	 	   if( ScreenTouched(touch_conf) ) {
						   WaitForReleased(touch_conf);
						   touch = GetRelease(touch_conf);
						   printf("x axis: %d", touch.x);
						   printf("  y axis: %d\n", touch.y);
						   if( touch.y>=80 && touch.y<= 380 && touch.x>=610 && touch.x<=680 ){
							   /// read the status of the vending machine
							   next_state = scanid_page;
							   flag = FALSE;
							   break;
						   }
                       }
			  	   }
		   	   }
		   	break;



		   case  scanid_page:
			   Loading("   While Your I.D Is Verified    ", 34);
			   err = scan_id(pi_conf, &customer);
			   // We should consider adding a background thread to display a moving loading screen and
			   // avoid blocking the main thread
			   if (err) {
				   error_msgs[0] = "Failed to scan your ID";
				   error_msgs[1] = "Try to scan again or visit our";
				   error_msgs[2] = "website to register your ID";
				   next_state = error_page;
				   printf("SCAN ID ERROR\n");
				   break;
			   }

			   // Successfully scanned ID and got customer, get inventory now
			   err = get_inventory(pi_conf, MACHINE_ID, &inventory, &inventoryLen);
			   if (err) {
				   // Internet connection failed
				   // Could be DB, Server, serial or Internet connection
				   printf("DB/SERVER/SERIAL/INTERNET CONN ERROR\n");

				   error_msgs[0] = "";
				   error_msgs[1] = "Failed to connect to the internet.";
				   error_msgs[2] = "";
				   next_state = error_page;
				   break;
			   }

			   // Successfully scanned a valid customer, AND got the vending machine inventory from the DB
			   next_state = picktype_page;
			   break;
		   case  error_page:
		 	   ErrorID(error_msgs, N_ERR_LINES);

			   //loops here until the screen has been touched;
		 	   flag = TRUE;
			   while (flag) {
			  	   if( ScreenTouched(touch_conf) ){
					   WaitForReleased(touch_conf);
					   touch = GetRelease(touch_conf);
					   printf("x axis: %d", touch.x);
					   printf("  y axis: %d\n", touch.y);
					   if( touch.y>=80 && touch.y<= 380 && touch.x>=610 && touch.x<=680 ){
					       /// read the status of the vending machine
					       next_state = scanid_page;
					       flag = FALSE;
					       break;
					   }
				   }
			   }
		 	   next_state = main_page;
		 	   break;
		   case picktype_page:
		   	   amount_to_pay=0;
			   PickType(&customer, inventory, inventoryLen);
			   flag = TRUE;
			   for (int i = 0; i < inventoryLen; i++) {
				   quantities[i] = 0;
			   }
			   while(flag){
	               WaitForTouch(touch_conf);
				   WaitForReleased(touch_conf);
			       touch = GetPress(touch_conf);
			       printf("x axis: %d", touch.x);
				   printf(" y axis: %d\n", touch.y);

				   // Check for +/- item presses
				   for (int i = 0; i < inventoryLen; i++){
					   int dy = 140*i;
					   if (IsInBox(touch.x,touch.y, 215 + dy, 335, 320 + dy, 365)) {
						   printf("isIn +[%d]\n", i);
						   if (quantities[i] < 9 && quantities[i] < inventory[i].quantity) {
							   quantities[i]++;
						   }
					   } else if (IsInBox(touch.x,touch.y, 215 + dy, 410, 320 + dy, 440)) {
						   printf("isIn -[%d]\n", i);
						   if (quantities[i] > 0) {
							   quantities[i]--;
						   }
					   }
				   }

				   // check if the touch was the pay button
				   if ( IsInBox(touch.x,touch.y,  670,160, 755, 258)){
					   amount_to_pay = 0;
					   for (int i = 0; i < inventoryLen; i++) {
						   amount_to_pay += inventory[i].price * quantities[i];
					   }
					   if (amount_to_pay > 0 && amount_to_pay < customer.credits) {
						   next_state = payment_page;
						   flag = FALSE;
						   break;
					   }
			       }
			       // check for the log out button
			       else if(IsInBox(touch.x,touch.y,  690,20, 750, 130)){
			           next_state = main_page;
			           flag = FALSE;
				       break;
			       }
			       amount_to_pay = 0;
			       for (int i = 0; i < inventoryLen; i++) {
			           amount_to_pay += inventory[i].price * quantities[i];
			       }
			       PickTypeUpdate(amount_to_pay, quantities, inventoryLen);
			   }
			   break;

		   case payment_page:
			   Payment(amount_to_pay);
			   next_state = payment_confirmation_page;
			   flag = TRUE;
			   while(flag){
	               WaitForTouch(touch_conf);
				   WaitForReleased(touch_conf);
			       touch = GetPress(touch_conf);
			       printf("x axis: %d", touch.x);
				   printf(" y axis: %d\n", touch.y);
				   // check if the touch was the pay button
				   if ( IsInBox(touch.x,touch.y,  610,80, 680, 380)){
				       next_state = payment_confirmation_page;
					   flag = FALSE;
				       break;
			       }
			   }
               break;

		   case payment_confirmation_page:
			   Loading("While Your Payment Is Processed", 37);
			   int purchases [64] = {-1};
			   int purchasesLen = 0;
			   int newBalance;
			   for (int i = 0; i < inventoryLen; i++) {
				   for (int j = 0; j < quantities[i]; j++) {
					   purchases[purchasesLen] = inventory[i].stock_id;
					   purchasesLen += 1;
				   }
			   }
			   err = make_purchase(pi_conf, MACHINE_ID, customer.ID, purchases, purchasesLen, &newBalance);
			   if (err) {
				   printf("MAKE PURCHASE ERROR\n");
				   error_msgs[0] = "";
				   error_msgs[1] = "There was an error processing your payment";
				   error_msgs[2] = "";
				   next_state = error_page;
				   break;
			   }
			   customer.credits = newBalance;

			   char sms_body[4096] = "";
			   char * res = strcat(sms_body, "Thank you for your purchase!\n");
			   printf("sms_body=%h res=%h\n", sms_body, res);

			   char exec_buf[128];

			   for (int i = 0; i < inventoryLen; i++) {
				   if (quantities[i] > 0){
					   sprintf(exec_buf, "%d %s\n", quantities[i], inventory[i].name);
					   res = strcat(sms_body, exec_buf);
				   }
			   }

			   sprintf(exec_buf, "Your new balance is %d credits", newBalance);
			   res = strcat(sms_body, exec_buf);
			   send_sms(wifi_conf, "(778) 866-2529", sms_body);

			   next_state = dispense_page;
			   break;
		   case dispense_page:
			   Dispense();
			   for (int i = 0; i < inventoryLen; i++) {
				   for (int j = 0; j < quantities[i]; j++) {
					   Run_Motors(i, motor_conf);
				   }
			   }
			   next_state = complete_page;
			   break;
		   case complete_page:
			   Complete(customer.credits);
			   next_state= main_page;
			   WaitForTouch(touch_conf);
		       WaitForReleased(touch_conf);
		       touch = GetRelease(touch_conf);
		       printf("x axis: %d", touch.y);
		       printf("  y axis: %d\n", touch.x);
			   break;
		   // you can have any number of case statements
		   default : // Optional
		   ;
		}
}
return 0;
}
