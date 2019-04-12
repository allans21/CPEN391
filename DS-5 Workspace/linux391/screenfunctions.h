
#ifndef SCREENFUNCTIONS_H_   /* Include guard */
#define SCREENFUNCTIONS_H_

#include "virtualfuck.h"
#include "appObjects.h"
#include "serial.h"

#define HW_REGS_BASE ( 0xff200000 )
#define HW_REGS_SPAN ( 0x00200000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

#define GraphicsCommandReg              (*(volatile unsigned short int *)(((0xFF210000 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsStatusReg               (*(volatile unsigned short int *)(((0xFF210000 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsX1Reg                   (*(volatile unsigned short int *)(((0xFF210002 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsY1Reg                   (*(volatile unsigned short int *)(((0xFF210004 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsX2Reg                   (*(volatile unsigned short int *)(((0xFF210006 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsY2Reg                   (*(volatile unsigned short int *)(((0xFF210008 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsColourReg               (*(volatile unsigned short int *)(((0xFF21000E - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))
#define GraphicsBackGroundColourReg     (*(volatile unsigned short int *)(((0xFF210010 - HW_REGS_BASE) & HW_REGS_MASK) + virtual_base))

/************************************************************************************************
** This macro pauses until the graphics chip status register indicates that it is idle
***********************************************************************************************/

#define WAIT_FOR_GRAPHICS       while((GraphicsStatusReg & 0x0001) != 0x0001);

// #defined constants representing values we write to the graphics 'command' register to get
// it to draw something. You will add more values as you add hardware to the graphics chip
// Note DrawHLine, DrawVLine and DrawLine at the moment do nothing - you will modify these

#define DrawHLine       1
#define DrawVLine       2
#define DrawLine        3
#define PutAPixel       0xA
#define GetAPixel       0xB
#define ProgramPaletteColour    0x10

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "Colours.h" contains constants for all 256 colours
// while the course file "ColourPaletteData.c" contains the 24 bit RGB data
// that is pre-programmed into the palette

#define BLACK           0
#define WHITE           1
#define RED             2
#define LIME            3
#define BLUE            4
#define YELLOW          5
#define CYAN            6
#define MAGENTA         7

#define TRUE 1
#define FALSE 0

#define XRES 800
#define YRES 480

#define BACKGROUND      CYAN
#define FONT            BLACK
#define BUTTONCOL       WHITE
#define BUTTONFONT      BLACK

void WriteAPixel(int x, int y, int Colour);
/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y);

/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB);

/********************************************************************************************* This function draw a horizontal line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/
// Draws a horizantal line from x1 to x2 at a height of y in colour colour
void HLine(int x1,int x2, int y, int colour);

// Draws a vertical line from y1 to y2 at a horizantal distance of x in colour colour
void VLine(int x, int y1, int y2, int colour);
//Draws a rectangle with corners (x1,y1) and (x2,y2) in colour colour
void Rectangle(int x1, int x2,int y1,int y2,int colour);

//Draws a line from (x1,y1) to (x2,y2) in colour colour
void Line(int x1, int y1, int x2, int y2, int Colour);
//Draws a filled rectangle with corners (x1,y1) and (x2,y2) in colour colour
void FilledRectangle(int x1, int x2,int y1,int y2,int colour);
//Draws a filled rectangle with corners (x1,y1) and (x2,y2) in colour colour with a border of colour bordColour
void FilledRecWBorder(int x1, int x2,int y1,int y2,int colour, int bordColour);
//Draws a Circle with center (x0,y0) of radius radius and colour colour
void Circle(int x0, int y0, int radius, int colour);
//Draws an arc of 45 degrees in the quadrant specified. The arc will have center (x0,y0) and radius radius
//radius and colour colour. The quadrants are specified from 1-8 in a clockwise fashion with 1 being uppermost right
void QuadArc(int x0, int y0, int radius, int quadrant, int colour);

void OutGraphicsCharFont2(int x, int y, int fontcolour, int backgroundcolour, int c, int Erase);



/*  Prints the character string stored in str of length length. Bottom
*   left hand corner of the text string is at (x,y). colourback is
*   the background colour behind the text and the colourfont is the
*   colour of the text
*/
void PrintString(char str[], int length, int x, int y, int colourback, int colourfont);


/*  Creates a button centered at the arguments centerx and centery. The heights and width of the button are
*   Specified by their respective arguments. the arguments colour is for the background colour of the button,
*   bordColour is for the colour of the border of the button and colourfont is the colour of the text. Make a
*   char array and input the length of the array
*/
void Button(int width, int height, int centerx, int centery, int colour, int bordColour, int colourfont, char str[], int length);

//Clears screen replacing it with a blank screen of colour colour
void ClearScreen(int colour);
//
void First_StartingScreen();
void StartingScreen(int colour);
void Loading(char* txt, int length);
void ErrorID(char ** error_lines, int n_lines);
void PickType(Customer *c, Inventory * inventoryList, int inventoryLen);
void PickTypeUpdate(int amount, int *quantities, int numItems);
void Dispense();


//x is the amount the customer needs to pay 1234 = $12.34
void Payment(int x);
void Complete(int newBalance);
int IsInBox(int x,int y,int x_upper_L,int y_upperL, int x_lowerR, int y_lowerR);

void keypad_screen (SerialConf * touch_conf, char array [], int size);
	
void keypad_update(int pos, char val);
void keypad_screen_init ();
	  
char key_pressed (int x, int y);
int check_enter(int x, int y);

#endif // SCREENFUNCTIONS_H_
