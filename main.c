/////////////////////////////////////////////////////////////////////////
////                            EX_CAN.C                             ////
////                                                                 ////
//// Example of CCS's CAN library, using the PIC18Fxx8.  This        ////
//// example was tested using MCP250xxx CAN Developer's Kit.         ////
////                                                                 ////
//// Connect pin B2 (CANTX) to the CANTX pin on the open NODE A of   ////
//// the developer's kit, and connect pin B3 (CANRX) to the CANRX    ////
//// pin on the open NODE A.                                         ////
////                                                                 ////
//// NODE B has an MCP250xxx which sends and responds certan canned  ////
//// messages.  For example, hitting one of the GPX buttons on       ////
//// the development kit causes the MCP250xxx to send a 2 byte       ////
//// message with an ID of 0x290.  After pressing one of those       ////
//// buttons with this firmware you should see this message          ////
//// displayed over RS232.                                           ////
////                                                                 ////
//// NODE B also responds to certain CAN messages.  If you send      ////
//// a request (RTR bit set) with an ID of 0x18 then NODE B will     ////
//// respond with an 8-byte message containing certain readings.     ////
//// This firmware sends this request every 2 seconds, which NODE B  ////
//// responds.                                                       ////
////                                                                 ////
//// If you install Microchip's CANKing software and use the         ////
//// MCP250xxx , you can see all the CAN traffic and validate all    ////
//// experiments.                                                    ////
////                                                                 ////
//// For more documentation on the CCS CAN library, see can-18xxx8.c ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.         ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

//#include <18F248.h>
#include <18F2480.h>
#fuses HS,NOPROTECT,NOLVP,NOWDT
#device 18F2480*=16, ADC=8
#use delay(clock=20000000)
#use rs232(baud=38400, xmit=PIN_C6, rcv=PIN_C7)
#use fast_io(c)
#include <can-18xxx8.c>

#include <MAX31855.c>
#include<stdlib.h>
#include <LCD_TM.c>
int16 ms;

#int_timer1
void isr_timer2(void) {
   ms++; //keep a running timer that increments every milli-second
}

void main() {
   //setup_spi(SPI_MASTER | SPI_MODE_1 | SPI_CLK_DIV_64, ); 
   struct rx_stat rxstat;
   int32 rx_id;
   int in_data[8];
   int rx_len;

//send a request (tx_rtr=1) for 8 bytes of data (tx_len=8) from id 24 (tx_id=24)
   int out_data[8];
   int32 tx_id=29; // id = 0x29 for MAX31855 temperature
   
   int1 tx_rtr=0;// CU LA 1
   int1 tx_ext=1;// CU LA 0
   int tx_len=8;
   int tx_pri=3;

   int i;
  
   
   for (i=0;i<8;i++) {
      out_data[i]=0x33;
      in_data[i]=0;
   
     
   }

 ////////////////////
union conv { 
    float f; 
    int8 b[4]; 
  };
  union conv Max31855exthermo;  // p is alias pointer
//val1.f=123.45; 
//printf(" %4.4f : %x : %x : %x : %x \r\n",tempp.f,\ 
//    val1.b[0],val1.b[1],val1.b[2],val1.b[3]); 
    //The four bytes that make up the float 

//Put four bytes into the second 
/*
val2.b[0]= 0x86; 
val2.b[1]= 0x6A; 
val2.b[2]= 0x8F; 
val2.b[3]= 0x5C; 
*/
/*
out_data[0] = Max31855exthermo.b[0];
out_data[1] = Max31855exthermo.b[1];
out_data[2] = Max31855exthermo.b[2];
out_data[3] = Max31855exthermo.b[3];

*/
//printf(" %4.4f : %x : %x : %x : %x \r\n",val2.f,\ 
//    val2.b[0],val2.b[1],val2.b[2],val2.b[3]); 
//    //The new float value, and the bytes that make it. 

 //////////////////// 
 float thermo, exthermo, inthermo;
 int8 low, high;
   printf("\r\n\r\nCCS CAN TRANSFER BME280 DATA\r\n");

   //setup_timer_2(T2_DIV_BY_4,79,16);   //setup up timer2 to interrupt every 1ms if using 20Mhz clock
   
   // init lcd
   lcd_init();
  // lcd_gotoxy(1,1);
  // printf(lcd_putc,"BSD PROJECT");  
   lcd_init();
   
   can_init();
 
   
  // can_set_mode(CAN_OP_LOOPBACK);
   setup_timer_2(T2_DIV_BY_16, 24, 1);   //
   enable_interrupts(INT_TIMER1);   //enable timer2 interrupt
   set_tris_c(0x00);
   output_low(PIN_C2);
   setup_ccp1(CCP_PWM);
   enable_interrupts(GLOBAL);       //enable all interrupts (else timer2 wont happen)
   setup_adc(ADC_CLOCK_INTERNAL); //ADC_CLOCK_DIV_8
   setup_adc_ports(AN0_TO_AN1); //ALL_ANALOG
   //#define VSS_VDD
   printf("\r\nRunning...");
   
   // quat cu
   while(TRUE)
   {   
        
       set_adc_channel(0); // EasyPIC Board: Read ADC 2 for PWM fan
       delay_ms(20);
       low= read_adc()/7; // 36
       if(low <30)
       low =30;
       printf("\r\n\low=%u",low);
       set_adc_channel(1); // EasyPIC Board: Read ADC 2 for PWM fan
       delay_ms(20);
       high= read_adc()/5; //63
       if(high < 40)
       high = 40;
       if(high>50)
       high = 50;
       //high = high + 35;
       //if((high > 60)||(high<35))
       //high = 60;
       printf("\r\n\high=%u",high);
       
       thermo = readMAX();
       delay_ms(20);
       
       printf("\r\nSPIVAL3 = %X SPIVAL2 = %X SPIVAL1 = %X SPIVAL0 = %X ", spival[3], spival[2], spival[1], spival[0]);
        //thermo1 = readExtTemp();
       delay_ms(10);
       // printf("\r\nThermocouple 1 = %f", thermo1);
       exthermo = PreFilter() ;
       delay_ms(10);
       Max31855exthermo.f = exthermo;
       
       out_data[0] = Max31855exthermo.b[0];
       out_data[1] = Max31855exthermo.b[1];
       out_data[2] = Max31855exthermo.b[2];
       out_data[3] = Max31855exthermo.b[3];
       
       inthermo = Linearized_TC();
       delay_ms(10);
       printf("\r\Pre Filter = %f Linearized = %f ", exthermo, inthermo);
        
        lcd_gotoxy(1,1);
        printf(lcd_putc,"T=%3.1f",exthermo);
       lcd_gotoxy(1,2);
       printf(lcd_putc,"ID:0x29");
       set_pwm1_duty(high); 
       
      
       if(exthermo > high)
       {
       set_pwm1_duty(255); 
       }
       if((exthermo  < low))
       {
       set_pwm1_duty(8); 
       }
       if((exthermo > low)&&(exthermo < high))
       {
       set_pwm1_duty(18); 
       }
       // lcd_gotoxy(1,2);
       // printf(lcd_putc,"T=%3.1f",inthermo);
        
      if ( can_kbhit() )   //if data is waiting in buffer...
      {
         if(can_getd(rx_id, &in_data[0], rx_len, rxstat)) { //...then get data from buffer
            printf("\r\nGOT: BUFF=%U ID=%LU LEN=%U OVF=%U ", rxstat.buffer, rx_id, rx_len, rxstat.err_ovfl);
            printf("FILT=%U RTR=%U EXT=%U INV=%U", rxstat.filthit, rxstat.rtr, rxstat.ext, rxstat.inv);
            printf("\r\n    DATA = ");
            for (i=0;i<rx_len;i++) {
               printf("%X ",in_data[i]);
            }
            printf("\r\n");
         }
         else {
            printf("\r\nFAIL on GETD\r\n");
         }

      }
      //every two seconds, send new data if transmit buffer is empty
      if ( can_tbe() && (ms > 2000))
      {
         ms=0;
         i=can_putd(tx_id, out_data, tx_len,tx_pri,tx_ext,tx_rtr); //put data on transmit buffer
         if (i != 0xFF) { //success, a transmit buffer was open
            printf("\r\nPUT %U: ID=%LU LEN=%U ", i, tx_id, tx_len);  //i return 1 if transmit success
            printf("PRI=%U EXT=%U RTR=%U\r\n   DATA = ", tx_pri, tx_ext, tx_rtr);
            //  printf("\r\nID=%X", out_data[0]);
            for (i=0;i<tx_len;i++) {
               printf("\r\n%X ",out_data[i]);
            }
            printf("\r\n");
         }
         else { //fail, no transmit buffer was open
            printf("\r\nFAIL on PUTD\r\n");
        
         }
        
      }
   
      
     delay_ms(500); 
   }
}
