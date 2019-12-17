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
#include "string.c"
struct sprite {
   int x,y;
   int buff[16][16];
   int replacePix;
   char *p;
   int oldstartRow;
   int oldstartCol;
};
struct obstacle {
int topx,topy;
int botx,boty;
};
struct sprite sprites[3];
int spriteMove = 0;
char *tab = "0123456789ABCDEF";
int color;

#include "timer.c"

#include "interrupts.c"
#include "kbd.c"
#include "uart.c"
#include "vid.c"
extern char _binary_panda_bmp_start;
extern char _binary_porkcar_bmp_start;
extern char _binary_pacman_bmp_start;
extern char _binary_speedy_bmp_start;
extern char _binary_pokey_bmp_start;
extern char _binary_power_bmp_start;


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

void uart0_handler()
{
  uart_handler(&uart[0]);
}
void uart1_handler()
{
  uart_handler(&uart[1]);
}

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER *tp[4];

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    // VIC status BITs: timer0,1=4, uart0=13, uart1=14, SIC=31: KBD at 3
    /**************
    if (vicstatus & 0x0010){   // timer0,1=bit4
      if (*(tp[0]->base+TVALUE)==0) // timer 0
         timer_handler(0);
      if (*(tp[1]->base+TVALUE)==0)
         timer_handler(1);
    }
    if (vicstatus & 0x0020){   // timer2,3=bit5
       if(*(tp[2]->base+TVALUE)==0)
         timer_handler(2);
       if (*(tp[3]->base+TVALUE)==0)
         timer_handler(3);
    }
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
    *********************/
    /******************
    if (vicstatus & (1<<4)){   // timer0,1=bit4
      if (*(tp[0]->base+TVALUE)==0) // timer 0
         timer_handler(0);
      if (*(tp[1]->base+TVALUE)==0)
         timer_handler(1);
    }
    if (vicstatus & (1<<5)){   // timer2,3=bit5
       if(*(tp[2]->base+TVALUE)==0)
         timer_handler(2);
       if (*(tp[3]->base+TVALUE)==0)
         timer_handler(3);
    }
    *********************/
    if (vicstatus & (1<<4)){   // timer0,1=bit4
       timer_handler(0);
    }   
    if (vicstatus & (1<<12)){   // bit 12: uart0 
         uart0_handler();
    }
    if (vicstatus & (1<<13)){   // bit 13: uart1
         uart1_handler();
    }

    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
       //   kbd_handler();
       }
    }
}
extern int oldstartR;
extern int oldstartC;
extern int replacePix;
extern int buff[16][16];
struct sprite sprites[3];
int noObstacles = 2;
struct obstacle obstacles[] ={{60,60,100,440},{460,60,500,440}};
int main()
{
   int i; 
   char line[128], string[32]; 
   UART *up;
  
  int x=80;
   int y=0;
   sprites[0].x = 400;
   sprites[0].y = 400;
   sprites[0].replacePix =0;
   sprites[0].p = &_binary_speedy_bmp_start;
   sprites[1].x = 200;
   sprites[1].y = 200;
   sprites[1].replacePix =0;
   sprites[1].p = &_binary_pokey_bmp_start;
   sprites[2].x = 600;
   sprites[2].y = 500;
   sprites[2].replacePix =0;
   sprites[2].p = &_binary_power_bmp_start;

   char * p;   
   color = YELLOW;
   row = col = 0; 
   fbuf_init();

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5 

   VIC_INTENABLE |= (1<<12); // UART0 at 12
   VIC_INTENABLE |= (1<<13); // UART1 at 13
 
   UART0_IMSC = 1<<4;  // enable UART0 RXIM interrupt
   UART1_IMSC = 1<<4;  // enable UART1 RXIM interrupt

   VIC_INTENABLE |= 1<<31;   // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   //SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   //SIC_PICENSET = 1<<3;  // KBD int=3 on SIC

  // kprintf("C3.3 start: Interrupt-driven drivers for Timer KBD UART\n");
   timer_init();
   /***********
   for (i=0; i<4; i++){
      tp[i] = &timer[i];
      timer_start(i);
   }
   ************/
   timer_start(0);
   kbd_init();

   uart_init();
   up = &uart[0]; 
p = &_binary_panda_bmp_start;
   //show_bmp1(p, 0, 80); 

    p = &_binary_pacman_bmp_start;
   show_bmp(p, 0, 80,buff,replacePix,&oldstartR,&oldstartC);
 
  // while(1);
   
   /*
   while(1){
      color = CYAN;
      kprintf("Enter a line from KBD\n");
      kgets(line);
   }
   */
   unlock();
 //kprintf("\nenter KBD here\n");
//uuprints("from UART0 here\n\r");
int move=0;
int key;
for (int g=0;g<noObstacles;g++)
    showObstacle(obstacles+g);
   while(1){
     //uprintf("enter a key from this UART : ");
     move=0;
     replacePix =1;
     if (upeek(up)){
     key=ugetc(up);
     switch(key){
      
     
      case 'e':
         if ( y< 600 )
            y+=10;
         move=1;
         break;
     
      default:
        move=0;
      }
}
      if (move)
         show_bmp(p, y, x,buff,replacePix,&oldstartR,&oldstartC);
      if (spriteMove){
          // move the sprites 
          show_bmp(sprites[0].p, sprites[0].y, sprites[0].x,sprites[0].buff,sprites[0].replacePix,
         &(sprites[0].oldstartRow),&(sprites[0].oldstartCol));
         sprites[0].x+=10;
         spriteMove =0;
         sprites[0].replacePix=1;


      }
   
   }
}
