#include <SoftwareSerial.h>
#define SOFT_RX 4
#define SOFT_TX 5


int StatusLed= 13;
char Doorgestuurd =0;
unsigned long vorigeMillis = 0;
const long interval = 500;
bool ledstatus = LOW;
bool knipperModus= false;

SoftwareSerial BTSerial(10,11);

void setup() 
{

  pinMode(StatusLed,OUTPUT);
  digitalWrite(StatusLed,LOW);
 BTSerial.println("Bluetooth gereed, wacht op commando");
}

void loop() 
{
  if (BTSerial.available() > 0)
    {
      Doorgestuurd = BTSerial.read();
      BTSerial.print("Ontvangen: ");
      BTSerial.println(Doorgestuurd);

      if (Doorgestuurd == '0')
        {
          BTSerial.println("Led knippert");
          knipperModus = true;
        }
      
     else if(Doorgestuurd =='1')
        {
          BTSerial.println("Led aan");
          knipperModus = false;
          digitalWrite(StatusLed, HIGH);

        }
      
      else if(Doorgestuurd == '2')
        {
          BTSerial.println("Led uit");
          knipperModus = false;
          digitalWrite(StatusLed,LOW);
        }
    }

  if (knipperModus)
    {
      unsigned long huidigeMillis = millis();
        if(huidigeMillis - vorigeMillis >= interval)
          {
            vorigeMillis = huidigeMillis,
            ledstatus = !ledstatus;
            digitalWrite(StatusLed,ledstatus);
          }
    }
  
}
