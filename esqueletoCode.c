#include <esqueletoCode.h>
#use rs232(baud=9600,xmit=pin_c6,rcv=pin_c7, bits=8, parity=N)
#include<LCD.C>
#define LCD_ENABLE_PIN PIN_D0
#define LCD_RS_PIN PIN_D1
#define LCD_RW_PIN PIN_D2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
char ch='0',estado='0';

#int_rda
void serial_isr(){
   ch=getchar();
   printf("%c Recibido \r",ch);
   //puts("Recibido");
}

float tempLM35();

void main()
{
   //ADC
   setup_adc_ports(sAN0);
   setup_adc_ports(sAN1);
   setup_adc(ADC_CLOCK_INTERNAL);
   //Interrupciones
   enable_interrupts(global);
   enable_interrupts(int_rda);
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
         lcd_gotoxy(1,1);
         printf(lcd_putc," Sensor DS1621  ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c       ",ch);
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
         lcd_gotoxy(1,1);
         printf(lcd_putc," Frecuencia     ");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Valor %c       ",ch);
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
