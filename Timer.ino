#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>


int Buzzer   = 11;
int LOAD     = A0;
//------------ Key Pad ----------------------
const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6 , 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2};     //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//------------ LCD ----------------------------------
LiquidCrystal_I2C lcd(0x27,20,4);// PCF8574A =  0x3F

// ----------- DS3231 -------------------------------
RTC_DS3231 RTC;
DateTime Time;
char Year,Mon,Day,Hour,Min,Sec;
long TimerON1,TimerOFF1;

/*-------------------------------------------------------------*
*  Setup  Program                                              *
*-------------------------------------------------------------*/
void setup () 
{ 
  lcd.begin();
  lcd.setCursor(0,0);lcd.print("Date     =          "); 
  lcd.setCursor(0,1);lcd.print("Time     =          "); 
  lcd.setCursor(0,2);lcd.print("TimeON1  =          "); 
  lcd.setCursor(0,3);lcd.print("TimeOFF2 =          ");  
  RTC.begin();   
  pinMode(Buzzer,OUTPUT); digitalWrite(Buzzer,HIGH);
  pinMode(LOAD,OUTPUT); digitalWrite(LOAD,HIGH);
 
}
/*-------------------------------------------------------------*
*   Main  Loop                                                 *
*-------------------------------------------------------------*/
void loop () 
{char Key;

    while(Key == NO_KEY)
    { 
      Time = RTC.now();
      ShowDate(13,3); 
      ShowTime(13,4);
      CheckTime();
      Key = keypad.getKey();
    }
    Beep(); 
    if(Key == '1')
    { GetDate(13,1);
      GetTime(13,2);
      RTC.adjust(DateTime(Year,Mon,Day,Hour,Min,Sec));            
    }  
    if(Key == '2')
    {       
      GetTime(13,3);
      TimerON1 = Hour;
      TimerON1 = (TimerON1 << 8) + Min;
      TimerON1 = (TimerON1 << 8) + Sec;   
      
      GetTime(13,4); 
      TimerOFF1 = Hour;
      TimerOFF1 = (TimerOFF1 << 8) + Min;
      TimerOFF1 = (TimerOFF1 << 8) + Sec;       
    }   
      
}
/*-------------------------------------------------------------*
*  Show Time                                                   *
*-------------------------------------------------------------*/
void CheckTime()
{ long Timer;
  Timer = 0;
  Timer = (Timer << 8)+Time.hour();
  Timer = (Timer << 8)+Time.minute();
  Timer = (Timer << 8)+Time.second();

  if(Timer == TimerON1){digitalWrite(LOAD,LOW);}
  if(Timer == TimerOFF1){digitalWrite(LOAD,HIGH);}
}
/*-------------------------------------------------------------*
*  Show Time                                                   *
*-------------------------------------------------------------*/
void ShowTime(char X,char Y)
{   lcd.setCursor(X-1,Y-1);
    if(Time.hour() < 10){lcd.print(0);}
    lcd.print(Time.hour());
    lcd.print(':');
    if(Time.minute() < 10){lcd.print(0);}
    lcd.print(Time.minute());
    lcd.print(':');
    if(Time.second() < 10){lcd.print(0);}
    lcd.print(Time.second());
}
/*-------------------------------------------------------------*
*  Show Date                                                   *
*-------------------------------------------------------------*/
void ShowDate(char X,char Y)
{   lcd.setCursor(X-1,Y-1);
    if(Time.day() < 10){lcd.print(0);}
    lcd.print(Time.day());
    lcd.print('/');
    if(Time.month() < 10){lcd.print(0);}
    lcd.print(Time.month());
    lcd.print('/');    
    if(Time.year()%100 < 10){lcd.print(0);}
    lcd.print(Time.year()%100);
}
/*-------------------------------------------------------------*
*  Get Date From Key                                           *
*-------------------------------------------------------------*/
void  GetDate(char X,char Y)
{  
   ShowDate(X,Y);
   Day  = GetNum(2,X,Y);
   Mon  = GetNum(2,X+3,Y);
   Year = GetNum(2,X+6,Y);    
}
/*-------------------------------------------------------------*
* Get  Time From Key                                           *
*-------------------------------------------------------------*/
void  GetTime(char X,char Y)
{  
   ShowTime(X,Y);
   Hour  = GetNum(2,X,Y);
   Min   = GetNum(2,X+3,Y);
   Sec   = GetNum(2,X+6,Y);  
}
/*-------------------------------------------------------------*
*  Slide Number                                                *
*-------------------------------------------------------------*/
char BufNum[8];
void  SlideNum(void)
{
  BufNum[6]  = BufNum[5];
  BufNum[5]  = BufNum[4];
  BufNum[4]  = BufNum[3];
  BufNum[3]  = BufNum[2];
  BufNum[2]  = BufNum[1];
  BufNum[1]  = BufNum[0];
}
/*-------------------------------------------------------------*
* Get Number From Keypad                                      *
*-------------------------------------------------------------*/
unsigned long GetNum(int Count,int X,int Y)
{ char Key,i,N;
  int Sum; 
  lcd.blink();
  lcd.setCursor(X+(Count-2),Y-1); 
  N = 0;
  for(i=0;i<Count;i++){BufNum[i] = ' ';}
  i = 0;
  while(Key != '#')
  { Key = NO_KEY;
    while(Key == NO_KEY){Key = keypad.getKey();}    
    Beep(); delay(300);
   if(Key == '*'){for(i=0;i<Count;i++){BufNum[i] = ' ';}N = 0;}                  
   if((N < Count)&&(Key >= '0')&&(Key <= '9'))
   { SlideNum();
     BufNum[0] = Key;
     N++;
   }
   lcd.setCursor(X-1,Y-1);
   for(i=0;i<Count;i++){lcd.print(BufNum[Count-(i+1)]);}
   lcd.setCursor(X+(Count-2),Y-1);  
    
  }
Sum = 0;
for(i=0;i<Count;i++)
 {
   if(BufNum[Count-(i+1)]==0x20){BufNum[Count-(i+1)] = '0';}
   Sum = (Sum*10) + (BufNum[Count-(i+1)]-'0');
 }
lcd.noCursor();
lcd.noBlink();
return(Sum);  
}
/*-------------------------------------------------------------*
*  Beep  Sound                                                 *
*-------------------------------------------------------------*/
void Beep()
{
  digitalWrite(Buzzer,LOW);delay(50);
  digitalWrite(Buzzer,HIGH);
}
