/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "defines.h"
#include "vid.c"
#include "uart.c"

extern char _binary_map_bmp_start;
extern char _binary_porkcar_bmp_start;
extern char _binary_pacman_bmp_start;

int color;


*******************************************/
extern int replacePix;
int main()
{
   char c, *p;
   int mode;
   uart_init();
   up = upp[0];
   int canWalkSpace = ffffff //Pacman can walk on white pixels
   int x = 60;
   int y = 60;
   mode = 0;
   fbuf_init(mode);
   p = &_binary_map_bmp_start;
   show_bmp1(p, 0, 0); // To align map 

   p = &_binary_pacman_bmp_start;
   show_bmp(p, 60, 60); //Pacman Location 
   int nomove = 0;
   int key;
   while (1)
   {
      uprintf("enter a key from this UART : ");
      nomove = 0;
      replacePix = 1;
      key = ugetc(up);
      //TODO:
      // add more keys for up,left,right
      switch (key)
      {
      case 'w':     //Up
         if (y -10 = 0) //Keeps pacman on the map
            y -= 10;
         break;
      case 's':       //Down
         if (y < 100) //Keeps pacman on the map
            y += 10;
         break;
      case 'a':      //Left
         if (x > 80) //Keeps pacman on the map
            x -= 10;
         break;
      case 'd':       //Right
         if (x < 550) //Keeps pacman on the map
            x += 10;
         break;
      default:
         nomove = 1;
      }
      if (!nomove)
         show_bmp(p, y, x);
   }
   while (1)
      ; // loop here
}
