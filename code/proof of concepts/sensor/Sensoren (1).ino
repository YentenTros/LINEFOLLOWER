int array[7] = {A6,A5,A4,A3,A2,A1,A0};
int Waarde;

void setup() 
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Waarde Sensoren: ");
  for(int i = 0; i < 7; i++)
    {
      Waarde = analogRead(array[i]);
      Serial.print(Waarde);
      Serial.print(" ");
    }

Serial.println();
delay(1000);
}
