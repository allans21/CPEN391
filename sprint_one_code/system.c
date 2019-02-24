/*
 * system.c

 *
 *  Created on: Feb 6, 2019
 *      Author: criss
 */
#include <stdio.h>
#include <stdlib.h>
#include "serial_ports.h"
#include "screenfunctions.h"

enum state{main_page, scanid_page, error_page,picktype_page,payment_page, dispense_page,make_selection_page};

enum item{beer,weed,cigars};

void main () {
	enum state next_state;
	next_state=main_page;
	enum item picked;
	picked = beer;
	int colour_welcome=0;
	int counter=0;
	Init_Touch();
    Init_Motors();
    int amount_to_pay;
    int valid;
	Point touch;

	int flag = TRUE;
	while(1){



/// LOGIC IN EACH PAGE
		switch(next_state) {

		   case main_page  :
		   	   next_state= main_page;
		   	   First_StartingScreen();
		   	   while(flag){
		   	   	StartingScreen(colour_welcome);
		   	   	colour_welcome++;
			   //loops here until the screen has been touched;

			  	for(int i=0;i<70000;i++){
			  	 	if(ScreenTouched( ) ){
			  				  		WaitForReleased();
			  						touch=  GetRelease();
			  						printf("x axis: %d", touch.x);
			  						printf("  y axis: %d\n", touch.y);
			  				  		if( touch.y>=80 && touch.y<= 380 && touch.x>=610 && touch.x<=680 ){
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
			   	}
			   	WaitForTouch();
		        WaitForReleased();
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
			   while(flag){
			   		  amount_to_pay++;

			   		
	                  WaitForTouch();
				      WaitForReleased();
			       	  touch=  GetPress();
			          printf("x axis: %d", touch.x);
				      printf(" y axis: %d\n", touch.y);
				      valid= FALSE;
				      if ( IsInBox(touch.x,touch.y,  670,160, 755, 258)){
					    next_state= payment_page; 
					    flag=FALSE;
			
				    	break;
			        	}
			        	PickTypeUpdate(amount_to_pay,amount_to_pay,amount_to_pay,amount_to_pay);


			   		
			   }
			   break;
		   case payment_page:
		   	   if(picked==beer)
			   Payment(100);
		   	   else if(picked== weed)
		   	   Payment (200);
		   	   else
		   	   Payment (300);

			   next_state= dispense_page;
			   break;

		   case dispense_page:
			   Dispense();
			   if(picked == beer)
				   Run_Motors(1);
			   else if (picked= weed)
				   Run_Motors(2);
			   else {

				   Run_Motors(1);
				   Run_Motors(2);
			   }

			   next_state= make_selection_page;
			   break;
		   case make_selection_page:
			   MakeSelection();
			   next_state= main_page;
			   break;
		   // you can have any number of case statements
		   default : // Optional
		   ;
		}
		/// LOOP UNTIL WE FIND THE 
		//IF()

		WaitForTouch();
		WaitForReleased();
		touch=  GetRelease();
		printf("x axis: %d", touch.y);
		printf("  y axis: %d\n", touch.x);
		

		// logic for options
		//
		
		/*if(touch.y>=160 && touch.y<= 300 && next_state == payment_page){
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
				next_state= picktype_page;

			}
		}
		else{

			if(next_state == payment_page)
				next_state= picktype_page;

		}



	}*/


}

}
