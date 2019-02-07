/*
 * system.c

 *
 *  Created on: Feb 6, 2019
 *      Author: criss
 */
#include <stdio.h>
#include "serial_ports.h"
#include "screenfunctions.h"
enum state{main_page, scanid_page, error_page,picktype_page,payment_page, dispense_page,make_selection_page};

enum item{beer,weed,cigars};

void main () {
	enum state current_state;
	current_state=main_page;
	enum item picked;
	picked = beer;
	Init_Touch();
   Init_Motors();


	Point touch;

	int flag = TRUE;
	while(1){

		flag=FALSE;


		switch(current_state) {

		   case main_page  :
			   StartingScreen();
			   //loops here until the button exits
			   current_state= main_page;
			   break;

		   case  scanid_page  :
			   CheckingID();
			   current_state= error_page;

			   break;
		   case  error_page  :
		 		ErrorID();
		 	    current_state= picktype_page;
		 	    break;
		   case picktype_page:
			   PickType();
			   current_state= payment_page;
			   break;
		   case payment_page:
		   	   if(picked==beer)
			   Payment(100);
		   	   else if(picked== weed)
		   	   Payment (200);
		   	   else
		   	   Payment (300);

			   current_state= dispense_page;
			   break;

		   case dispense_page:
			   Dispense();
			   if(picked == beer)
				   Run_Motors(1);
			   else if (picked= weed)
				   Run_Motors(2);

			   current_state= make_selection_page;
			   break;
		   case make_selection_page:
			   MakeSelection();
			   current_state= main_page;
			   break;
		   // you can have any number of case statements
		   default : // Optional
		   ;
		}

		WaitForTouch();
		WaitForReleased();
		touch=  GetRelease();
		printf("x axis: %d", touch.x);
		printf("  y axis: %d\n", touch.y);

		// logic for options
		//
		if(current_state== main_page &&  touch.y>=80 && touch.y<= 380 && touch.x>=610 && touch.x<=680 ){
			current_state= scanid_page;

		}
		if(touch.y>=160 && touch.y<= 300 && current_state == payment_page){
			if(touch.x>=270 && touch.x<=330){
				picked= beer;


			}
			else if (touch.x>=380 && touch.x<=445){
				picked= weed;


			}
			else if (touch.x>= 515 && touch.x<= 595){
				picked = cigars;

			}
			else{
				current_state= picktype_page;

			}
		}
		else{

			if(current_state == payment_page)
				current_state= picktype_page;

		}



	}




}
