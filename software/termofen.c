//#define DEBUG 1

#include <iom8.h>
#include <intrinsics.h>

//#ifdef DEBUG
//  #include <stdio.h>
//#endif

#define GIST 2
#define LEVEL 20

#define KP 100
#define KI 5  //3
#define KD 10 //10

//functions declare;
void initialize(void);					//функция инициализации

//variables declare
#ifdef DEBUG
char str[40],buff=0;
#endif
static unsigned char symgen[12]={0x0A,0xFA,0x2C,0xA8,0xD8,0x89,0x09,0xBA,0x08,0x88,0xFF,0x1D};
unsigned char screen[9]={10,10,10,10,10,10,10,10,10};
unsigned char dot=0,i=0,h=0,point=0,display=0,adcp=0,timer=0,pow=0;
short k,ust=0,temp=0,vent=0,err=0,err1=0,upr=0,i1=0;
short adc[3]={0,0,0},adcold1,adcold2;

//main function
int main(void)
{
  initialize();
	
  while(1)
  {
    __delay_cycles(1000000);
    
    if((ust==0)&&(temp<50))
    {
      pow=0;
      PORTB|=0x02;
      DDRB=0x03;
    }
    if(ust)
    {
      pow=1;
      DDRB=0x01;
      PORTB&=0xFD;
    }
    
    if(((adc[1]>(adcold1+GIST))||(adc[1]<(adcold1-GIST)))&&pow)
    {
      timer=255;
      display=3;
      adcold1=adc[1];
    }
    if((adc[2]>(adcold2+GIST))||(adc[2]<(adcold2-GIST)))
    {
      timer=255;
      display=6;
      adcold2=adc[2];
    }
    
    temp=(adc[0]>>1)+30;    //30 градусов - 25 градусов среда +5 градусов Uсм
    k=temp;
    
    if(!pow)
    {
      screen[0]=10;
      screen[1]=10;
      screen[2]=10;      
    }
    else
    {
      i=0;
      while(k>=100)
      {
        k-=100;
        i++;
      }
      screen[0]=i;
      i=0;
      while(k>=10)
      {
        k-=10;
        i++;
      }
      screen[1]=i;
      screen[2]=k;
    }
    
    if(adc[1]>0xD0)
      vent=(adc[1]>>3)-27;
    else
      vent=0;
    k=vent;
    i=0;
    while(k>=100)
    {
      k-=100;
      i++;
    }
    if(i)
      screen[3]=i;
    else
      screen[3]=10;
    i=0;

    while(k>=10)
    {
      k-=10;
      i++;
    }
    screen[4]=i;
    screen[5]=k;    
    
    if(adc[2]>322)
    {
      ust=(adc[2]>>1)-61;
      k=ust;
      i=0;
      while(k>=100)
      {
        k-=100;
        i++;
      }
      screen[6]=i;
      i=0;
      while(k>=10)
      {
        k-=10;
        i++;
      }
      screen[7]=i;
      screen[8]=k;
    }
    else
    {
      screen[6]=0;
      screen[7]=11;
      screen[8]=11;
      ust=0;
    }
    
    err=ust-temp;
    if(ust)
      if(err>LEVEL)
        upr=0x7FFF;
      else
        if(err>-LEVEL)
        {
          upr=err*KP + err*KI +i1 + (err - err1)*KD;
          if(upr<8000)
            i1+=err*KI;
        }
        else
        {
          upr=0;
          i1=0;
        }
    else
    {
      upr=0;
      i1=0;
      err=0;
    }
    
    err1=err;    
    
    if(upr>0)
      OCR1A=upr<<3;
    if(upr>0x7D00)
      OCR1A=0xFFFF;
    if(upr<=0)
      OCR1A=0;
 
#ifdef DEBUG
    if(!buff)
    {
      str[38]='\r';
      str[37]='U';
      str[36]='S';
      str[35]='T';
      str[34]='=';
      str[33]=screen[6]+0x30;
      str[32]=screen[7]+0x30;
      str[31]=screen[8]+0x30;
      str[30]=' ';
      str[29]='T';
      str[28]='E';
      str[27]='M';
      str[26]='P';
      str[25]='=';
      str[24]=screen[0]+0x30;
      str[23]=screen[1]+0x30;
      str[22]=screen[2]+0x30;
      str[21]=' ';
      str[20]='E';
      str[19]='R';
      str[18]='R';
      str[17]='=';
      
      k=err;
      if(k<0)
      {
        k=~k+1;
        str[16]='-';
      }
      else
        str[16]=' ';
      i=0;
      while(k>=100)
      {
        k-=100;
        i++;
      }
      str[15]=i+0x30;
      i=0;
      while(k>=10)
      {
        k-=10;
        i++;
      }
      str[14]=i+0x30;
      str[13]=k+0x30;

      str[12]=' ';
      str[11]='U';
      str[10]='P';
      str[9]='R';
      str[8]='=';
      
      k=upr;
      if(k<0)
      {
        k=~k+1;
        str[7]='-';
      }
      else
        str[7]=' ';
      
      i=0;
      while(k>=10000)
      {
        k-=10000;
        i++;
      }
      str[6]=i+0x30;
      i=0;
      while(k>=1000)
      {
        k-=1000;
        i++;
      }
      str[5]=i+0x30;
      i=0;
      while(k>=100)
      {
        k-=100;
        i++;
      }
      str[4]=i+0x30;
      i=0;
      while(k>=10)
      {
        k-=10;
        i++;
      }
      str[3]=i+0x30;
      str[2]=k+0x30;
      str[1]='\r';
      str[0]=0;
      
      buff=38;
    }
#endif    
  }
}

//initialize function
void initialize(void)
{
	//init ports
	DDRB=0x01;		//PB1-PB0=out 
	PORTB=0x02;     //PB1=HiZ,PB0=0
	DDRC=0x38;		//PC5-PC3=out
	PORTC=0x38;		//PC5-PC3=1
	DDRD=0xFF;		//PD7-PD0=1 
	PORTD=0xFF;     //PD7-PD0=out

#ifdef DEBUG
	//init USART
	UBRRH=0x00;		//9600 B/S(0x33)
	UBRRL=0x33;

	UCSRA|=2;		//DOUBLE SPEED
	UCSRB=0x28;		//TXC INTERRUPT, TRANSMITTER ENABLE
	UCSRC=0xBE;		//8 BIT,ASYNC,ODD PARITY,2 STOP BITS

    UDR='\n';
#endif
	
	//init timer 0
	TCCR0=0x03;
	
    //init timer 1
	TCCR1A=0x00;
    TCCR1B=0x03;
    OCR1A=0x8000;
	
	//init interrupt
	TIMSK|=0x15;
	SREG|=128;
    
    //init ADC
    ADMUX=0x40;
    ADCSR=0xCF;

	return;
}

//interrupts functions

#ifdef DEBUG
#pragma vector=USART_UDRE_vect
__interrupt void transmit(void)
{
  __enable_interrupt();
  if(buff)
  {
    UDR=str[buff];
    buff--;
  }

  return;
}
#endif

#pragma vector=ADC_vect
__interrupt void adc_read(void)
{
short adct;

  __enable_interrupt();
  ADCSR=0x8F;
  adct=ADC;
  adc[adcp]+=(adct-adc[adcp])>>3;
 
  adcp++;
  if(adcp==3)
    adcp=0;
  
  ADMUX&=0xF0;
  ADMUX|=adcp;
  ADCSR=0xCF;
  return;
}

#pragma vector=TIMER1_OVF_vect
__interrupt void timerlovf(void)
{
  if((PINB&0x04)&&(OCR1A>0xFF))
  {
    dot=1;
    PORTB|=0x01;
  }
  return;
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void timer1compa(void)
{
  if(OCR1A==0xFFFF)
    return;
  dot=0;
  PORTB&=0xFE;
  return;
}

#pragma vector=TIMER0_OVF_vect
__interrupt void timer0ovf(void)
{
  __enable_interrupt();  
  PORTD=0xFF;
  
  if((++point)==3)
    point=0;
  
  PORTC|=0x38;
  PORTC&=(0xDF>>point);
  PORTD=symgen[screen[point+display]];
  
  if(dot&&(point==2))
    PORTD&=0xF7;
  
  if(timer)
    timer--;
  else
    display=0;
  return;
}

