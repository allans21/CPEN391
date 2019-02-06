
#include "screenfunctions.h"
#include <stdio.h>
#include <math.h>
// graphics register addresses
/*******************************************************************************************
* This function writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
    WAIT_FOR_GRAPHICS;              // is graphics ready for new command
    GraphicsX1Reg = x;              // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsColourReg = Colour;         // set pixel colour
    GraphicsCommandReg = PutAPixel;         // give graphics "write pixel" command
}
/*********************************************************************************************
* This function read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y)
{
    WAIT_FOR_GRAPHICS;          // is graphics ready for new command
    GraphicsX1Reg = x;          // write coords to x1, y1
    GraphicsY1Reg = y;
    GraphicsCommandReg = GetAPixel;     // give graphics a "get pixel" command
    WAIT_FOR_GRAPHICS;          // is graphics done reading pixel
    return (int)(GraphicsColourReg) ;   // return the palette number (colour)
}


/**********************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
**
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;        // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

/********************************************************************************************* This function draw a horizontal line, 1 pixel at a time starting at the x,y coords specified
*********************************************************************************************/


/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/******************************************************************* SHAPES AND LINES FOR EXERCISES **************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/


// Draws a horizantal line from x1 to x2 at a height of y in colour colour
void HLine(int x1,int x2, int y, int colour){
    WAIT_FOR_GRAPHICS;
    GraphicsX1Reg = x1;
    GraphicsX2Reg = x2;
    GraphicsY1Reg = y;
    GraphicsY2Reg = y;
    GraphicsColourReg = colour;
    GraphicsCommandReg = DrawHLine;
}

// Draws a vertical line from y1 to y2 at a horizantal distance of x in colour colour
void VLine(int x, int y1, int y2, int colour){

    WAIT_FOR_GRAPHICS;
    GraphicsX1Reg = x;
    GraphicsX2Reg = x;
    GraphicsY1Reg = y1;
    GraphicsY2Reg = y2;
    GraphicsColourReg = colour;
    GraphicsCommandReg = DrawVLine;
}

//Draws a rectangle with corners (x1,y1) and (x2,y2) in colour colour
void Rectangle(int x1, int x2,int y1,int y2,int colour){
    VLine(x1, y1, y2, colour);
    VLine(x2, y1, y2, colour);
    HLine(x1, x2 ,y1, colour);
    HLine(x1, x2, y2, colour);
}


//Draws a line from (x1,y1) to (x2,y2) in colour colour
void Line(int x1, int y1, int x2, int y2, int Colour)
{

    WAIT_FOR_GRAPHICS;
    GraphicsX1Reg = x1;
    GraphicsX2Reg = x2;
    GraphicsY1Reg = y1;
    GraphicsY2Reg = y2;
    GraphicsColourReg = Colour;
    GraphicsCommandReg = DrawLine;
}


//Draws a filled rectangle with corners (x1,y1) and (x2,y2) in colour colour
void FilledRectangle(int x1, int x2,int y1,int y2,int colour){
    int temp;

    if(y1 > y2){
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    for(int i = y1; i <= y2; i++){
        WAIT_FOR_GRAPHICS;
        GraphicsX1Reg = x1;
        GraphicsX2Reg = x2;
        GraphicsY1Reg = i;
        GraphicsY2Reg = i;
        GraphicsColourReg = colour;
        GraphicsCommandReg = DrawHLine;
    }
}

//Draws a filled rectangle with corners (x1,y1) and (x2,y2) in colour colour with a border of colour bordColour
void FilledRecWBorder(int x1, int x2,int y1,int y2,int colour, int bordColour){
    FilledRectangle(x1, x2, y1, y2, colour);
    Rectangle(x1, x2, y1, y2, bordColour);
}

//Draws a Circle with center (x0,y0) of radius radius and colour colour
void Circle(int x0, int y0, int radius, int colour)
{
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        WriteAPixel(x0 + x, y0 + y, colour);
        WriteAPixel(x0 + y, y0 + x, colour);
        WriteAPixel(x0 - y, y0 + x, colour);
        WriteAPixel(x0 - x, y0 + y, colour);
        WriteAPixel(x0 - x, y0 - y, colour);
        WriteAPixel(x0 - y, y0 - x, colour);
        WriteAPixel(x0 + y, y0 - x, colour);
        WriteAPixel(x0 + x, y0 - y, colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

//Draws an arc of 45 degrees in the quadrant specified. The arc will have center (x0,y0) and radius radius
//radius and colour colour. The quadrants are specified from 1-8 in a clockwise fashion with 1 being uppermost right
void QuadArc(int x0, int y0, int radius, int quadrant, int colour){
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        switch(quadrant){
            case 3 :
            WriteAPixel(x0 + x, y0 + y, colour);
            break;

            case 4 :
            WriteAPixel(x0 + y, y0 + x, colour);
            break;

            case 5 :
            WriteAPixel(x0 - y, y0 + x, colour);
            break;

            case 6 :
            WriteAPixel(x0 - x, y0 + y, colour);
            break;

            case 7 :
            WriteAPixel(x0 - x, y0 - y, colour);
            break;

            case 8 :
            WriteAPixel(x0 - y, y0 - x, colour);
            break;

            case 1 :
            WriteAPixel(x0 + y, y0 - x, colour);
            break;

            case 2 :
            WriteAPixel(x0 + x, y0 - y, colour);
            break;
        }
        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void OutGraphicsCharFont2(int x, int y, int fontcolour, int backgroundcolour, int c, int Erase)
{
// using register variables (as opposed to stack based ones) may make execution faster
// depends on compiler and CPU

    register int row, column, theX = x, theY = y ;
    register int pixels1, pixels2, pixels3;
    register char theColour = fontcolour  ;
    register int BitMask, theC = c ;

// if x,y coord off edge of screen don't bother

    if(((short)(x) > (short)(XRES-1)) || ((short)(y) > (short)(YRES-1)))
        return ;


// if printable character subtract hex 20
    if(((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~'))) {
        theC = theC - 0x20 ;
        for(row = 0; (char)(row) < (char)(120); row = row + 3)   {

// get the bit pattern for row 0 of the character from the software font
            pixels1 = Font22x40[theC][row];
            pixels2 = Font22x40[theC][row+1];
            pixels3 = Font22x40[theC][row+2];
            BitMask = 0x300000;

            for(column = 0; (char)(column) < (char)(22); column++)  {

// if a pixel in the character display it
                if((pixels1 & BitMask)){
                    WriteAPixel(theX+column, theY+(row/3), theColour) ;
                }
                if((pixels2 & BitMask)){
                    WriteAPixel(theX+column+8, theY+(row/3), theColour) ;
                }
                if((pixels3 & BitMask)){
                    WriteAPixel(theX+column+16, theY+(row/3), theColour) ;
                }
                else {
                    if(Erase == 1)

// if pixel is part of background (not part of character)
// erase the background to value of variable BackGroundColour

                        WriteAPixel(theX+column, theY+row, backgroundcolour) ;
                }
                BitMask = BitMask >> 1;
            }
        }
    }
}


/*  Prints the character string stored in str of length length. Bottom
*   left hand corner of the text string is at (x,y). colourback is
*   the background colour behind the text and the colourfont is the
*   colour of the text
*/
void PrintString(char str[], int length, int x, int y, int colourback, int colourfont){

    for(int i = 0; i < length; i++){
        OutGraphicsCharFont2(x+(i*22), y, colourfont, colourback, (int) str[i], 0);
    }
}


/*  Creates a button centered at the arguments centerx and centery. The heights and width of the button are
*   Specified by their respective arguments. the arguments colour is for the background colour of the button,
*   bordColour is for the colour of the border of the button and colourfont is the colour of the text. Make a
*   char array and input the length of the array
*/
void Button(int width, int height, int centerx, int centery, int colour, int bordColour, int colourfont, char str[], int length){

    int x1 = (centerx - width/2);
    int x2 = (centerx + width/2);
    int y1 = (centery - height/2);
    int y2 = (centery + height/2);

    FilledRecWBorder(x1, x2, y1, y2, colour, bordColour);

    int texty = centery - 20;
    int textx = centerx - ((25 * length)/2);

    PrintString(str, length, textx, texty, colour, colourfont);
}

//Clears screen replacing it with a blank screen of colour colour
void ClearScreen(int colour){
    for(int i = 0; i < 480; i++){
        HLine(0, 799, i, colour);
    }
}



/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/******************************************************************* DIFFERENT SCREENS FOR UI ********************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/
/*****************************************************************************************************************************************************************/

void StartingScreen(){

    ClearScreen(BACKGROUND);
    // char str[] = {'W', 'e', 'l', 'c', 'o', 'm', 'e', '!'};
    char str[] = "Welcome!";

    PrintString(str, 8, 312, 100, BACKGROUND, FONT);

    char str1[] = "This is an age restricted";
    PrintString(str1, 25, 125, 150, BACKGROUND, FONT);

    char str2[] = "vending machine.";
    PrintString(str2, 16, 224, 200, BACKGROUND, FONT);

    char str3[] = "Please insert your I.D.";
    PrintString(str3, 23, 147, 250, BACKGROUND, FONT);

}

void CheckingID(){

    ClearScreen(BACKGROUND);

    char str1[] = "Please wait a moment";
    PrintString(str1, 20, 180, 150, BACKGROUND, FONT);

    char str[] = "while your I.D is verified";
    PrintString(str, 27, 103, 200, BACKGROUND, FONT);

}

void ErrorID(){

    ClearScreen(BACKGROUND);

    char str1[] = "There was an error";
    PrintString(str1, 18, 202, 150, BACKGROUND, FONT);

    char str[] = "processing your I.D.";
    PrintString(str, 20, 180, 200, BACKGROUND, FONT);
}

void PickType(){

    ClearScreen(BACKGROUND);

    char str1[] = "Beer";
    Button(250, 60, 400, 150, BUTTONCOL, BLACK, BUTTONFONT, str1, 4);

    char str2[] = "Weed";
    Button(250, 60, 400, 240, BUTTONCOL, BLACK, BUTTONFONT, str2, 4);

    char str3[] = "Cigarettes";
    Button(250, 60, 400, 330, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);

}

//x is the amount the customer needs to pay 1234 = $12.34
void Payment(int x){
    ClearScreen(BACKGROUND);

    int y = x % 10;
    char fir = (char) 48 + y;
    x -= y;
    y = (x % 100) / 10;
    char sec = (char) 48 + y;
    x -= y*10;
    y = (x % 1000) / 100;
    char thir = (char) 48 + y;
    x -= y*100;
    y = (x % 10000) / 1000;
    char forth = (char) 48 + y;
    x -= y*1000;

    char str1[] = {'Y', 'o', 'u', 'r', ' ', 't', 'o', 't', 'a', 'l', ' ', 'i', 's', ':'};
    char chr[] = {'$', fir, sec, '.', thir, forth};

    PrintString(str1, 14, 246, 100, BACKGROUND, FONT);
    PrintString(chr, 6, 334, 200, BACKGROUND, FONT);

}

void MakeSelection(){

    ClearScreen(BACKGROUND);
    char str3[] = "Cigarettes";

    Button(350, 60, 205, 47,  BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 205, 124, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 205, 201, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 205, 278, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 205, 355, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 205, 432, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);

    Button(350, 60, 595, 47,  BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 595, 124, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 595, 201, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 595, 278, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 595, 355, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);
    Button(350, 60, 595, 432, BUTTONCOL, BLACK, BUTTONFONT, str3, 10);

}
