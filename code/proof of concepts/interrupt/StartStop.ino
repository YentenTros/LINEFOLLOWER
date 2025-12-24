const int Start =2;
const int LedMotor = 12;
bool StatusLed= false;






void setup() 
{
  Serial.begin(9600);
  pinMode(Start,INPUT);
  pinMode(LedMotor,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(Start),Interrupt,RISING);

}

void loop() 
{
  digitalWrite(LedMotor, StatusLed);

}

void Interrupt()
{
  Serial.println("Interrupt");
  StatusLed = !StatusLed;
}