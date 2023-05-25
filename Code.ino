#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(6, 8, 9, 10, 11, 12);    // rs en, d4, d5, d6, d7

#define den  3

long    duty, dutyt,dt,chay; //duty do rong xung pwm
int     tt=0;       // bao ngat

int   bt=A0;
long  adc,gt,i;
long    bh;

int   nnss=4;
int   nnc=5;
char  data;

int   ttc,ttss;

BH1750 sensor;
long  as,chn,ng,tr,ch;

void setup() 
{
  Serial.begin(9600);
  pinMode(7, OUTPUT); // khai bao bien chan xuan hien ra man hinh LCD
  digitalWrite(7,LOW); 
  lcd.begin(16, 2);

  Wire.begin();
  sensor.begin();

  attachInterrupt(0, ngat_ngoai, RISING); // 0:stt ngat RISING: muc 0 len 1   //FALLING  //trang thai ngat 
  Timer1.initialize(100); //Khởi động ngắt, thời gian đặt cho nó là 100us
  Timer1.attachInterrupt(kich_thiet_bi); 
  
  pinMode(den,OUTPUT);
  pinMode(nnc,INPUT);
  pinMode(nnss,INPUT);
  Serial.println(duty);
  ttc=0; //trang thai nut nhan chuyen
  ttss=0; // tràng thai nut nhan start stop
  duty=9; // giá trị pwm
}
void loop()
{ 
  dutyt=duty*10;
  kiem_tra_nnc();
  kiem_tra_nnss();
  doc_bt();
  hienthi();
  if(ttss==1)
  {
    if(ttc==0)
    {
      lcd.setCursor(0,1);
      lcd.print(" CHE DO TU DONG ");
      if(as<100)                
      {
        duty=2;
      }
      else if(as<200)     
      {
        duty=5;
      } 
      else if(as<300)     
      {
        duty=7; 
      }
      else
      {
        duty=9; 
      }  
    }
    else
    {
      lcd.setCursor(0,1);
      lcd.print("CHE DO CHINH TAY");
      doc_bt();
    }    
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("    TAT DEN     ");     
  }
}

void doc_bt()
{
  adc=0;
  for(i=0;i<100;i++)
  {
    adc = adc+analogRead(bt);      
    delay(1);
  }
  adc=adc/100; 
  adc=adc*5000/1023;                                          // vref=5v,   adc = 10bit = 1023
  gt = adc; 
  duty= 2 + (gt*9)/5000;                                       // 2 - 9      // 3 - 8
  Serial.print("BT:");
  Serial.println(duty);
}
  
void hienthi()
{
  as = sensor.readLightLevel();
  chn=as/10000%10+0x30;
  ng=as/1000%10+0x30;
  tr=as/100%10+0x30;
  ch=as/10%10+0x30;
  if(chn==0x30)
  {
    chn=0x20;
    if(ng==0x30)
    {
      ng=0x20;
      if(tr==0x30)
      {
        tr=0x20;
        if(ch==0x30)      
        ch=0x20;  
      }  
    }  
  }
  lcd.setCursor(0,0);
  lcd.print("CD AS:");
  lcd.write(chn);
  lcd.write(ng);
  lcd.write(tr);
  lcd.write(ch);
  lcd.write(as%10+0x30);
  lcd.print(" lux  "); 
  delay(200);
}

void ngat_ngoai()     
{
  if(ttss==1)
  {
    tt=1;
    chay=0;   
  }
  else
  {
    tt=0;
    chay=0;  
  }
}

void kiem_tra_nnc()
{
  if(digitalRead(nnc)==0)
  {
    delay(20);
    if(digitalRead(nnc)==0)  
    {
      ttc++;
      if(ttc>1)     
      ttc=0;
      duty=9;       
      while(digitalRead(nnc)==0);    
    }
  }  
}

void kiem_tra_nnss()
{
  if(digitalRead(nnss)==0)
  {
    delay(20);
    if(digitalRead(nnss)==0)  
    {
      ttss++;
      if(ttss>1)     
      ttss=0;   
      while(digitalRead(nnss)==0);    
    }
  }  
}

void kich_thiet_bi()                          // cu 100us xay ra 1 lan
{
  if(ttss==1)     
  {
    if(tt==1)
    {
      chay++;
      if(chay<dutyt)                                   // 2
      {
        digitalWrite(den,LOW);    
      } 
      else if(chay==dutyt)
      {
        digitalWrite(den,HIGH);    
      }
      else
      {
        tt=0;
        chay=0;
        digitalWrite(den,LOW);    
      } 
    }   
  }
  else
  {
    digitalWrite(den,LOW);       
  } 
}
