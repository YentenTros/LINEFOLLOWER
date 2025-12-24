#include "SerialCommand.h"
#include "EEPROMAnything.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(8, 7);

#define SerialPort BTSerial
#define Baudrate 9600
#define MotorLeftForward 6
#define MotorLeftBackward 5
#define MotorLeftPWM 3
#define MotorRightForward 10
#define MotorRightBackward 9
#define MotorRightPWM 11

SerialCommand sCmd(SerialPort);
bool debug = false;
bool run;
unsigned long previous, calculationTime;
const int sensor[] = {A7, A6, A5, A4, A3, A2, A1, A0};
long normalised[8];

struct param_t // Laatste parameters onthouden
  {
    unsigned long cycleTime;
    int zwart [8];
    int wit [8];
    int power;
    float diff;
    float kp;
  } params;

float debugPosition;
void onUnknownCommand(char * command);
void OnSet();
void onDebug();
void onCalibrate();
void onRun();

void setup()
{
  BTSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("HC-05 is klaar");

  sCmd.addCommand("set",OnSet);
  sCmd.addCommand("debug",onDebug);
  sCmd.addCommand("run",onRun);
  sCmd.addCommand("calibrate", onCalibrate);
  sCmd.setDefaultHandler(onUnknownCommand);

  EEPROM_readAnything(0,params);

  pinMode(MotorLeftForward, OUTPUT);
  pinMode(MotorLeftBackward, OUTPUT);
  pinMode(MotorLeftPWM,OUTPUT);
  pinMode(MotorRightForward,OUTPUT);
  pinMode(MotorRightBackward,OUTPUT);
  pinMode(MotorRightPWM,OUTPUT);

  previous = micros();

  run = false;
}

void loop() //Geen enkele blokkerende functie gebruiken
{
  sCmd.readSerial();

unsigned long current = micros();
if (current - previous > params.cycleTime)
{
    previous = current;
    for (int i = 0; i < 8; i++)
    {
        long value = analogRead(sensor[i]);
        normalised[i] = map(value, params.zwart[i], params.wit[i], 0, 1000);
    }
    
    int index = 0;
    for (int i = 1; i < 8; i++)
    {
        if (normalised[i] < normalised[index])
        {
            index = i;
        }
    }
    if (normalised[index] > 800)
      {
        run = false;
      }
    float position;
    if (index == 0)
    {
        position= 28;
    }
    else if (index == 7)
    {
        position = -28;
    }
    else
    {
        int sZero = normalised[index];
        int sMinusOne = normalised[index - 1];
        int sPlusOne = normalised[index + 1];
        float b = sPlusOne - sMinusOne;
        b = b / 2;
        float a = sPlusOne - b - sZero;
        position = -b / (2 * a);
        position += index;
        position -= 3.5;
        position *= 10;
    }
  debugPosition = position;
  float error = -position;  //error = setpoint - posisition
  float output = error * params.kp;

  output = constrain(output,-510,510);
  int powerLeft = 0;
  int powerRight = 0;
  if (run)
    {
      if(output>=0)
        {
          powerLeft = constrain(params.power + params.diff * output, -255, 255);
          powerRight = constrain(powerLeft - output, -255, 255);
          powerLeft = powerRight + output;
        }
      else
        {
          powerRight = constrain(params.power - params.diff * output, -255, 255);
          powerLeft = constrain(powerRight + output, -255 , 255);
          powerRight = powerLeft - output;
        }
      // Linker motor
  if (powerLeft > 0) 
  {
    digitalWrite(MotorLeftForward, HIGH);
    digitalWrite(MotorLeftBackward, LOW);
    analogWrite(MotorLeftPWM, powerLeft);
  } 
  else if (powerLeft < 0) 
  {
    digitalWrite(MotorLeftForward, LOW);
    digitalWrite(MotorLeftBackward, HIGH);
    analogWrite(MotorLeftPWM, -powerLeft);
  } 
  else
   {
    digitalWrite(MotorLeftForward, LOW);
    digitalWrite(MotorLeftBackward, LOW);
    analogWrite(MotorLeftPWM, 0);
   }
  
  // Rechter motor
  if (powerRight > 0) 
  {
    digitalWrite(MotorRightForward, HIGH);
    digitalWrite(MotorRightBackward, LOW);
    analogWrite(MotorRightPWM, powerRight);
  } 
  else if (powerRight < 0) 
  {
    digitalWrite(MotorRightForward, LOW);
    digitalWrite(MotorRightBackward, HIGH);
    analogWrite(MotorRightPWM, -powerRight);
  } 
  else 
  {
    digitalWrite(MotorRightForward, LOW);
    digitalWrite(MotorRightBackward, LOW);
    analogWrite(MotorRightPWM, 0);
  }
 }  
  else 
  {
  // Motoren stoppen wanneer run = false
  analogWrite(MotorLeftPWM, 0);
  analogWrite(MotorRightPWM, 0);
  digitalWrite(MotorLeftForward, LOW);
  digitalWrite(MotorLeftBackward, LOW);
  digitalWrite(MotorRightForward, LOW);
  digitalWrite(MotorRightBackward, LOW);
  }
}

  unsigned long difference = micros() - current;
  if(difference > calculationTime)
    {
      calculationTime = difference;
    }
}

void onUnknownCommand(char* command)
{
  SerialPort.print("Unknown Command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void OnSet()
{
  char* parameter = sCmd.next();
  char* value = sCmd.next();
 
  if(strcmp(parameter,"cycle")==0)
    {
       params.cycleTime = atol(value); // Hiermee waardes uitlezen l voor long, f voor floating,...  ideaal voor kalibratie
    }
  else if (strcmp(parameter,"power") == 0)
    {
      params.power = atol(value);
    }
  else if (strcmp(parameter,"diff") == 0)
    {
      params.diff = atof(value);
    }
    else if (strcmp(parameter,"kp") == 0)
    {
      params.kp = atof(value);
    }
  EEPROM_writeAnything(0,params);
}

void onDebug()
{
  SerialPort.print("cycle time: ");

  SerialPort.println(params.cycleTime);
  SerialPort.print("calculationTime: ");
  SerialPort.println(calculationTime);

  SerialPort.print("zwart: ");
    for(int i = 0; i < 8; i++)
      {
        SerialPort.print(params.zwart[i]);
        SerialPort.print(" ");
      }
      SerialPort.println(" ");

  SerialPort.print("wit: ");
    for(int i = 0; i < 8; i++)
      {
        SerialPort.print(params.wit[i]);
        SerialPort.print(" ");
      }
      SerialPort.println(" ");

  SerialPort.print("normalised: ");
    for (int i =0; i<8; i++)
      {
        SerialPort.print(normalised[i]);
        SerialPort.print(" ");
      }
      SerialPort.println(" ");
  
  SerialPort.print("position: ");
  SerialPort.println(debugPosition);

  SerialPort.print("power: ");
  SerialPort.println(params.power);
  SerialPort.print("diff: ");
  SerialPort.println(params.diff);
  SerialPort.print("kp: ");
  SerialPort.println(params.kp);
 
}

void onCalibrate()
{
  char* parameter = sCmd.next();

  if (strcmp(parameter,"zwart") ==0)
    {
      SerialPort.print("start calibrating zwart... ");
      for (int i = 0; i < 8; i++)
        {
          params.zwart[i] = analogRead(sensor[i]);
          SerialPort.print("done");
        }
    }
  else if (strcmp(parameter, "wit") == 0)
    {
      SerialPort.print("start calibrating wit...");
      for (int i = 0; i < 8; i++)
        {
          params.wit[i] = analogRead(sensor[i]);
          SerialPort.print("done");
        }
    }
  EEPROM_writeAnything(0,params);
}

void onRun()
{
  run = !run;
  SerialPort.print("Run is bezig");
}
  