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


#include "virtualfuck.h"
#include "serial_ports.h"
#include "screenfunctions.h"

enum state{main_page, scanid_page, error_page,picktype_page,payment_page, dispense_page,make_selection_page, complete_page};

enum item{beer,weed,cigars};

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
	next_state=main_page;
	enum item picked;
	picked = beer;
	int colour_welcome=0;
	int counter=0;
	SerialConf * touch_conf = Init_Touch(virtual_base); // TODO get real virtual base
	SerialConf * motor_conf = Init_Motors(virtual_base);
    int amount_to_pay=0;
    int valid;
	Point touch;
	int amount_beers;
	int amount_weed;
	int amount_cigar;
	int beers_stock=10000;
	int weed_stock=10000;
	int cigar_stock=1000;
	int current_credits=10000;
	int new_amountPay;
	int price_beer=200;
	int price_weed=420;
	int price_cigars=100;
	int flag = TRUE;
	while(1){



/// LOGIC IN EACH PAGE
		switch(next_state) {

		   case main_page  :
		   	  colour_welcome=0;
		   	   next_state= main_page;
		   	   First_StartingScreen();
		   	   flag=TRUE;
		   	   while(flag){
		   	   	StartingScreen(colour_welcome);
		   		   First_StartingScreen();
		   	   	colour_welcome++;
			   //loops here until the screen has been touched;

			  	for(int i=0;i<700000;i++){
			  	 	if(ScreenTouched(touch_conf) ){
			  				  		WaitForReleased(touch_conf);
			  						touch=  GetRelease(touch_conf);
			  						printf("x axis: %d", touch.x);
			  						printf("  y axis: %d\n", touch.y);
			  				  		if( touch.y>=80 && touch.y<= 380 && touch.x>=610 && touch.x<=680 ){
			  				  		/// read the status of the vending machine
			  						next_state= scanid_page;
			  						flag=FALSE;
			  						break;}
			  				  	}
			  	}

		   	   }
		   	break;



		   case  scanid_page  :
		   	   counter=0;
			   CheckingID(counter);
			   // THE FIRST SHOULD HAVE A CLEAN THEN WHILE UNTIL WE GET SOMETHING FROM THE API
			   flag=TRUE;
			   next_state= scanid_page;
			   while(flag){
			   	counter++;
			   	CheckingID(counter);
			   	for(int i=0;i<1200000;i++){
			   		;
			   		// check for the api while we do busy waiting
			   		// obtain the user id, credits
			   		// can loop
			   	}
			   	WaitForTouch(touch_conf);
		        WaitForReleased(touch_conf);
		        // just for demo purposes
			   	if(counter==8){
			   	next_state= picktype_page;
			   	flag=FALSE;
			   	counter=0;}

				}

			   break;
		   case  error_page  :
		 		ErrorID();
		 	    next_state= picktype_page;
		 	    break;
		   case picktype_page:
		   	   amount_to_pay=0;
			   PickType(amount_to_pay);
			   flag=TRUE;
			   amount_cigar=0;
			   amount_beers=0;
			   amount_weed=0;
			   while(flag){
			   		  amount_to_pay++;
	                  WaitForTouch(touch_conf);
				      WaitForReleased(touch_conf);
			       	  touch=  GetPress(touch_conf);
			          printf("x axis: %d", touch.x);
				      printf(" y axis: %d\n", touch.y);
				      // check if the touch was the pay button
				      if ( IsInBox(touch.x,touch.y,  670,160, 755, 258)){
					    next_state= payment_page;
					    flag=FALSE;
				    	break;
			        	}
			        	// check for plus beer
			        	// upper left , bottom right
			        else if (IsInBox(touch.x,touch.y,  240,327, 305, 357)){
			        	new_amountPay=amount_cigar*price_cigars + amount_weed*price_weed+ (amount_beers+1)*price_beer;
			        	if(amount_beers!=9 && new_amountPay <= current_credits && (amount_beers+1)<=beers_stock)
			        		amount_beers++;
			        }
			        	// check for minus beer

			        else if(IsInBox(touch.x,touch.y,  270,409, 310, 447)){
			        	if(amount_beers!=0)
			        		amount_beers--;
			        }
			        	// check for plus weed

			        else if (IsInBox(touch.x,touch.y,  370,328, 440, 360)){
			        	new_amountPay=amount_cigar*price_cigars + (amount_weed+1)*price_weed+ (amount_beers)*price_beer;
			        	if(amount_weed!=9 && new_amountPay <= current_credits && (amount_weed+1)<=weed_stock)
			        		amount_weed++;
			        }
			        	// check for minus weed

			        else if(IsInBox(touch.x,touch.y,  380,410, 445, 440)){
			        	if(amount_weed!=0)
			        		amount_weed--;
			        }
			        	// check for plus cigars

			        else if (IsInBox(touch.x,touch.y,  518,330, 583, 360)){
			        	new_amountPay=(amount_cigar+1)*price_cigars + amount_weed*price_weed+ (amount_beers)*price_beer;
			        	if(amount_cigar!=9 && new_amountPay <= current_credits && (amount_cigar+1)<=cigar_stock)
			        		amount_cigar++;
			        }
			        	// check for minus cigars
			        else if(IsInBox(touch.x,touch.y,  515,409, 560, 440)){
			        	if(amount_cigar!=0)
			        		amount_cigar--;
			        }
			        // check for the log out button
			        else if(IsInBox(touch.x,touch.y,  690,20, 750, 130)){
			        	next_state=main_page;
			        	flag=FALSE;
				    	break;
			        }
			        amount_to_pay= amount_cigar*price_cigars + amount_weed*price_weed+ amount_beers*price_beer;
			        PickTypeUpdate(amount_to_pay, amount_beers, amount_weed, amount_cigar);
			    }
			   break;

		   case payment_page:
			   Payment(amount_to_pay);
			   next_state= dispense_page;
			   flag=TRUE;
			   while(flag){
	                  WaitForTouch(touch_conf);
				      WaitForReleased(touch_conf);
			       	  touch=  GetPress(touch_conf);
			          printf("x axis: %d", touch.x);
				      printf(" y axis: %d\n", touch.y);
				      // check if the touch was the pay button
				      if ( IsInBox(touch.x,touch.y,  610,80, 680, 380)){
					    next_state= dispense_page;
					    flag=FALSE;
				    	break;
			        	}
			   }
			   break;

		   case dispense_page:
			   Dispense();
			   if(picked == beer)
				   Run_Motors(1, motor_conf);
			   else if (picked== weed)
				   Run_Motors(2, motor_conf);
			   else {
				   Run_Motors(1, motor_conf);
				   Run_Motors(2, motor_conf);
			   }
			   next_state= complete_page;
			   break;
		   case complete_page:
			   Complete();
			   next_state= main_page;
			   WaitForTouch(touch_conf);
		       WaitForReleased(touch_conf);
		       touch=  GetRelease(touch_conf);
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
