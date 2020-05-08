#include <esqueletoCode.h>
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#use i2c(Master,fast,sda=PIN_C4,scl=PIN_C3)

#include<LCD.C>
#include "EEPROM_24512.c"     //Ficheros driver de los periféricos
#include "TEMP_ds1621.c"
#include <floatee.c>
#include <float.h>

#define LCD_ENABLE_PIN PIN_D0
#define LCD_RS_PIN PIN_D1
#define LCD_RW_PIN PIN_D2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
char ch='0',estado='0';

#INT_CCP1
void CCP1_isr()
{
   if (flag==0)
   {
      set_timer1(55);//Empezar contador
      flag=1;
   }else
   {
      captura=CCP_1;
      flag=0;
   }
   
   clear_interrupt(INT_CCP1);
}

#int_rda
void serial_isr(){
   ch=getchar();
   printf("%c Recibido \r",ch);
   //puts("Recibido");
}

float tempLM35();
float pressureMPX4250();
void tempI2C();

void main()
{
   //ADC
   setup_adc_ports(sAN0);
   setup_adc_ports(sAN1);
   setup_adc(ADC_CLOCK_INTERNAL);
   //Interrupciones
   enable_interrupts(global);
   enable_interrupts(PERIPH);
   enable_interrupts(int_rda);
   enable_interrupts(INT_CCP1);//Activar interrupciones por ccp1
   //Timer1
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   setup_ccp1(CCP_CAPTURE_RE);
   //LCD instrucciones iniciales
   lcd_init();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"SistemaMultitask");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"EAE, JVLP y JALP");
   delay_ms(1000);
   //FSM
   estado='0';
   float temperatura,presion;
   int16 freq=0;
   while(TRUE)
   {
      switch(estado){
         case '0':
         lcd_gotoxy(1,1);
         printf(lcd_putc," MENU PRINCIPAL ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c        ",ch);
         if(ch!='9'&&ch!='#')
         estado=ch;
         break;
         case '1':
         temperatura=tempLM35();
         lcd_gotoxy(1,1);
         printf(lcd_putc," Sensor LM35    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Temp = %02.1f C\n",temperatura);
         if(ch=='9'){
         estado='0';
         }
         break;
         case '2':
         presion=pressureMPX4250();
         lcd_gotoxy(1,1);
         printf(lcd_putc," Sensor MPX4250 ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Presion=%03.0f Pa\n",presion);
         if(ch=='9'){
         estado='0';
         }
         break;
         case '3':
         tempI2C();
         lcd_gotoxy(1,1);
         printf(lcd_putc," Sensor DS1621  ");
         //lcd_gotoxy(1,2);
         //printf(lcd_putc," Valor %c       ",ch);
         if(ch=='9'){
         estado='0';
         }
         break;
         case '4':
         lcd_gotoxy(1,1);
         printf(lcd_putc," Interruptores  ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c       ",ch);
         if(ch=='9'){
         estado='0';
         }
         break;
         case '5':
         freq=(1000000/captura);
         lcd_gotoxy(1,1);
         printf(lcd_putc," Frecuencia     ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," freq=%04lu Hz",freq);
         delay_ms(500);
         if(ch=='9'){
         estado='0';
         }
         break;
         case '6':
         lcd_gotoxy(1,1);
         printf(lcd_putc," Control PWM    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c       ",ch);
         if(ch=='#'){
         estado='0';
         }
         break;
         case '7':
         lcd_gotoxy(1,1);
         printf(lcd_putc," Servomotor     ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c       ",ch);
         if(ch=='#'){
         estado='0';
         }
         break;
         case '8':
         lcd_gotoxy(1,1);
         printf(lcd_putc,"Generador   tonos");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c        ",ch);
         if(ch=='#'){
         estado='0';
         }
         break;
         default:
         estado='0';
         break;
      }
   }
}

float tempLM35(){
       float temp;
       int16 adc;
       set_adc_channel(0);
       delay_us(20);
       adc=read_adc();
       temp=(500.0*adc)/1024.0;
       delay_ms(500);
       return temp;
}

float pressureMPX4250(){
       float pressure;
       int16 adc;
       set_adc_channel(1);
       delay_us(20);
       adc=read_adc();
       pressure=adc/3.988;
       delay_ms(500);
       return pressure;
}

void tempI2C(){
      int16 address=0;
      float dato;
      init_temp(0x01);      //Inicializa el DS1621
      delay_ms(100);
      dato = read_full_temp(0x01);  //Lee temperatura del DS1621
      WRITE_FLOAT_EXT_EEPROM(address,dato);  //Guarda 4 bytes del FLOAT
      address=address+4;
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Temp=%4.1f C\n",dato);  //Visualiza en LCD la temperatura
      if (address==0xffff) address=0;
      delay_ms(200);
}
