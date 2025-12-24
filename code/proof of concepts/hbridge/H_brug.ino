//Motor Links
const int AIN1= 6;
const int AIN2 =5;
const int PWMA= 3;

//Motor Rechts
const int BIN1 =10;
const int BIN2= 9;
const int PWMB =11;



void setup()
{   
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
 //Voorwaarts
    Serial.print("start");
    for (int i = 0; i <= 255; i++)
        {
            Rotate_Motor(i,i);
            delay(20);
        }
    delay(1000);

//Achterwaarts
 for(int i = 0; i <= 255; i++)
        {
            Rotate_Motor(-i,-i);
            delay(20);
        }
    delay(1000);

//Links draaien
    for(int i = 0; i <= 255; i++)
        {
            Rotate_Motor(-i,i);
            delay(20);
        }
    delay(1000);

//Rechts draaien
    for(int i = 0; i <= 255; i++)
        {
                Rotate_Motor(i,-i);
                delay(20);
        }
    delay(1000);
}

void Rotate_Motor(int left, int right)
{
    //Links
    if( left >= 0)
        {
        digitalWrite(AIN1,1);
        digitalWrite(AIN2,0);
        } 
    else
        {
        left = -(left);
        digitalWrite(AIN1,0);
        digitalWrite(AIN2,1);
        }
    
    //Rechts
    if( right> 0)
        {
        digitalWrite(BIN1,1);
        digitalWrite(BIN2,0);
        } 
    else
        {
        right = -(right);
        digitalWrite(BIN1,0);
        digitalWrite(BIN2,1);
        }
    
    //PWM Begrenzen
    if(left >255)  left =255;
    if(right >255) right = 255;
        
    analogWrite(PWMA, left);
    analogWrite(PWMB, right);

}