/* Connections:
 * USB---> Pi 
 *
 * SCL --> SCL (OLED)
 * SDL --> SDL (OLED)
 * 
 */

#include<SPI.h>
#include<Wire.h>
#include <avr/wdt.h>
#include<Adafruit_SSD1306.h>

#define RESET_OLED  12
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

/* Error values are for tuning the TARS's legs position. 
 * Do not change speed
 * Error values depends on SPEED.
 */
#define COAST_ERROR 10
#define PUSH_ERROR  140
#define SPEED 80

// delay without delay()
unsigned long motorStartMillis;

/* Structure for holding attributes for 
*  each DC motor.
*  Motor driver is LM298n 
*/ 
 typedef struct {
   int pwm;           // pwm values
   int INA;           // Motor driver pin INA
   int INB;           // Motor driver pin INB
   char id;           // Motor identifier
   char current_dir;  // Current direction of motor
   int  current_pos;  // Current position of motor 

 }Motor;

// This motor controls left leg 
Motor Motor1 = {.pwm=10,
                .INA=9,
                .INB=8,
                .id='1',
                .current_dir='F',
                .current_pos=0 
                };

// This motor controls right leg
Motor Motor2 = {.pwm=11,
                .INA=7,
                .INB=6,
                .id='2',
                .current_dir='F',   // Default direction
                .current_pos=0      // Default position    
                };



// setup motors 
void SetupMotors(){

   pinMode(Motor1.pwm,OUTPUT);
   pinMode(Motor1.INA,OUTPUT);
   pinMode(Motor1.INB,OUTPUT);

   pinMode(Motor2.pwm,OUTPUT);
   pinMode(Motor2.INA,OUTPUT);
   pinMode(Motor2.INB,OUTPUT);
   
}

void RunOLED(word time){
    Adafruit_SSD1306 display(RESET_OLED);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    display.clearDisplay();

    // 'ON' every pixel for given time
    for(int i=0;i<128;i++){
        for(int j=0;j<32;j++){
           display.drawPixel(i, j, WHITE);
        }
    }
    
    display.display();   
    delay(time);
    display.clearDisplay();
    display.ssd1306_command(SSD1306_DISPLAYOFF);   // put the OLED display in sleep mode
    display.ssd1306_command(0x8D);               // disable charge pump
    
}
// run motor in forward direction
void motor_FORWARD(char id,byte speed){
  if(id=='1'){
    digitalWrite(Motor1.INA,HIGH);
    digitalWrite(Motor1.INB,LOW);
    analogWrite(Motor1.pwm,speed);
  }
  else if(id=='2'){
    digitalWrite(Motor2.INA,HIGH);
    digitalWrite(Motor2.INB,LOW);
    analogWrite(Motor2.pwm,speed);
  }
  
}

// run motor in reverse direction
void motor_REVERSE(char id,byte speed){
  if(id=='1'){
    digitalWrite(Motor1.INA,LOW);
    digitalWrite(Motor1.INB,HIGH);
    analogWrite(Motor1.pwm,speed);
  }
  else if(id=='2'){
    digitalWrite(Motor2.INA,LOW);
    digitalWrite(Motor2.INB,HIGH);
    analogWrite(Motor2.pwm,speed);
  }
}

/*  Command format for controlling individual motors:
 *   
 *  |ControlByte|Identifier|Motor Id|Direction|Speed|Time
 *  
 *  controlbyte: 'A' or 'C' 
 *  Decides whether this is a Command or Action
 * 
 *  Identifier: 'D' or 'S'
 *  D for DC motors and S for servos (not implemented)
 *  
 *  Motor Id: 1 to 8
 *  1 and 2 for DC motors and 3 to 8 are for servos
 *  
 *  Direction: 'F' or 'B'
 *  sets direction for DC motors.
 *  
 *  Speed : 0 to FF 
 *  sets pwm speed value for DC motors. This will serve as an angle for servos (not implemented)
 *  
 *  Time : 0 to FFFF 
 *  run command for given time
 * ---------------------------------------------------------------- 
 *  Command format for executing predefined Actions:
 *  |ControlByte|Action Byte|Time
 *  
 *  Action:
 *    'R' reset
 *    'W' walk
 *    'D' Default
 *    'J' joke
 *    'B' standby
 *    'S' shutdown
 *  
 *  Time: 0 to FFFF
 *  run action for given time
 */

void ParseSerial(String data) {
  char hold[4];
  String temp;
  char  controlbyte;
  word time;
  char direction;
  byte speed;
  char id;
  char identifier;
  byte action;
  
  controlbyte = data.charAt(1);

  if(controlbyte=='C'){
    identifier  = data.charAt(3);
    id          = data.charAt(5);
    direction   = data.charAt(7);

    temp = data.substring(9, 11);
    temp.toCharArray(hold, 3);
    String bemp = temp;
    speed = strtol(hold, NULL, 16);
    temp  = data.substring(12, data.length());
    temp.toCharArray(hold, temp.length() + 1);
    time = strtol(hold, NULL, 16);
    


    if(id=='1'){
      if(direction=='F'){
        motor_FORWARD('1',speed);
        delay(time);
        motor_BRAKE('1');
      }
      else if(direction=='R'){
         motor_REVERSE('1',speed);
         delay(time);
         motor_BRAKE('1');
      }
    }
    else if(id=='2'){
       if(direction=='F'){
        motor_FORWARD('2',speed);
        delay(time);
        motor_BRAKE('2');
       }
       else if(direction=='R'){
         motor_REVERSE('2',speed);
         delay(time);
         motor_BRAKE('2');
      }
    }

  }

  if(controlbyte=='A'){
    action = data.charAt(3);
    temp   = data.substring(5,data.length());
    temp.toCharArray(hold,temp.length()+1);
    time = strtol(hold,NULL,16);

    HandleAction(action,time);
  }

 }


void HandleAction(char action,word time){

/*
 * Actions:
 *        'R' reset
 *        'W' walk
 *        'D' Default
 *        'J' joke
 *        'B' standby
 *        'S' shutdown
 */

     switch(action){
      case 'S':
         Shutdown();
         break;
      case 'B':
         CentrePos();
         break;
      case 'D':
        DefaultPos();
         break;  
      case 'W':
        Walk(time,SPEED);
         break;
      case 'J':
        RunOLED(time);
         break;
      case 'R':
        Reset(WDTO_30MS);
         break;
     }
}

// Reset arduino
void Reset(uint8_t prescaller){
  wdt_enable(prescaller);
  while(1){}
}


// BRAKE motors
void motor_BRAKE(char id){
  if(id=='1'){
    analogWrite(Motor1.pwm,0);
  }
  else if(id=='2'){
    analogWrite(Motor2.pwm,0);
  }
}

// Walk action
void Walk(word time,short speed){
     //set position
     Motor1.current_pos+=time;
     Motor2.current_pos+=time;

     // set motor directions
     Motor1.current_dir='F';
     Motor2.current_dir='R';

     //start motors
     motor_FORWARD('1',speed);
     motor_REVERSE('2',speed);
     
     motorStartMillis = millis();
     
     while(millis()-motorStartMillis < time+0){
      ;
     }

     //urgent BRAKE
     UrgentBrake('1');
     UrgentBrake('2');
     
     delay(500);
     Motor1.current_dir='R';
     Motor2.current_dir='F';
     motor_FORWARD('2',speed);
     motor_REVERSE('1',speed);
     motorStartMillis = millis();
     
      while(millis()-motorStartMillis < time+PUSH_ERROR){
      ;
     }
     UrgentBrake('1');
     UrgentBrake('2');
         
}
// Standing position of TARS
void DefaultPos(){

  Motor1.current_dir='R';
  Motor2.current_dir='R';
  Motor1.current_pos+=400;
  Motor2.current_pos+=400;
  motor_REVERSE('1',90);
  motor_REVERSE('2',90);
  delay(400);
  UrgentBrake('1');
  UrgentBrake('2');
  
}

/* This DC motors dosen't have PID control,
 * so to track the current position I'm using Time as 
 * a heuristic.
 * 
 * The assumption is, on start up all motors are at their 
 * Center position. i.e current_pos = 0
 * 
 * TODO: use map()
 */
void CentrePos(){
  if(Motor1.current_dir='F'){
    motor_FORWARD('1',SPEED);
    motorStartMillis = millis();
    while(millis()-motorStartMillis < Motor1.current_pos-COAST_ERROR){
      ;
     }
     UrgentBrake('1');
     Motor1.current_pos=0;
  }
  else if(Motor1.current_dir='R'){
    motor_FORWARD('1',SPEED);
    motorStartMillis = millis();
    while(millis()-motorStartMillis < Motor1.current_pos-COAST_ERROR){
      ;
     }
     UrgentBrake('1');
     Motor1.current_pos=0;

  }
  
  if(Motor2.current_dir='F'){
    motorStartMillis = millis();
    motor_FORWARD('2',SPEED);
    while(millis()-motorStartMillis < Motor2.current_pos-120){
      ;
     }
     UrgentBrake('2');
     Motor2.current_pos=0;
  }
  else if(Motor2.current_dir='R'){
    motorStartMillis = millis();
    motor_FORWARD('2',SPEED);
    while(millis()-motorStartMillis < Motor2.current_pos-120){
      ;
     }
     UrgentBrake('2');
     Motor2.current_pos=0;
  }

}

void Shutdown(){
  // TODO: Store current values in eeprom
    CentrePos();
    Reset(WDTO_30MS);
}

// Urgent brakes
void UrgentBrake(char id){
  if(id=='1'){
    analogWrite(Motor1.pwm,255);
    digitalWrite(Motor1.INA,LOW);
    digitalWrite(Motor1.INB,LOW);
  }
  if(id=='2'){
    analogWrite(Motor2.pwm,255);
    digitalWrite(Motor2.INA,LOW);
    digitalWrite(Motor2.INB,LOW);
  }
}

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

    String Commands;
    if(Serial.available()>0){
      Commands = Serial.readStringUntil(' ');
      SetupMotors();
      ParseSerial(Commands);
      
    }   
}









