/*
  Arduino based RC Transmitter with nRF24L01 module
  by Aymen Alouache, www.aymenalouache.xyz
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

#define stick1_x_pin A1
#define stick1_y_pin A2
#define stick2_x_pin A4
#define stick2_y_pin A3
#define pot1_pin A5
#define pot2_pin A6
#define toggle1_pin 5
#define toggle2_pin 4
#define toggle3_pin 8
#define button1_pin 6
#define buzzer_pin 3
#define led_pin 2

RF24 radio(9, 10);               // nRF24L01 (CE, CSN)
const byte address[6] = "node1"; // Address
const uint8_t pipe1 = 0xF0F0F0F0AA;

unsigned long currentTime = 0;
unsigned long pastTime = 0;
int freq = 800;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package
{
  byte stick1_x;
  byte stick1_y;
  byte stick2_x;
  byte stick2_y;
  byte pot1;
  byte pot2;
  byte toggle1;
  byte toggle2;
  byte toggle3;
  byte button1;
};
Data_Package data; // Create a variable with the above structure



void ledBlink()
{
  if (data.toggle1 == 1)
  {
    if (currentTime - pastTime > 1000)
    {
      digitalWrite(led_pin, HIGH);
      pastTime = currentTime;
    }
    if (currentTime - pastTime > 50)
    {
      digitalWrite(led_pin, LOW);
    }
  }
}

void buzzWelcome()
{
  digitalWrite(led_pin, HIGH);
  tone(buzzer_pin, 500, 700);
  delay(200);
  tone(buzzer_pin, 700, 300);
  delay(200);
  tone(buzzer_pin, 900, 100);
  delay(500);
  digitalWrite(led_pin, LOW);
}

void buzz()
{
  if (data.toggle1 == 1)
  {
    if (currentTime - pastTime > 1500)
    {
      freq = map(data.pot2, 0, 255, 440, 900);
      tone(buzzer_pin, freq, 100);
      pastTime = currentTime;
    }
  }
}



void setup()
{
  // Activate the Arduino internal pull-up resistors
  pinMode(toggle1_pin, INPUT_PULLUP);
  pinMode(toggle2_pin, INPUT_PULLUP);
  pinMode(toggle3_pin, INPUT_PULLUP);
  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  Serial.begin(115200);
  printf_begin();

  if (!radio.begin())
  { // true if radio successfully initialized, false if faild to commnicate with the radio hardware
    Serial.println(F("radio hardware not responding!"));
    while (1)
    {
    } // hold program in infinite loop to prevent subsequent errors
  }
  else
  {
    Serial.println("Radio hardware working!");
  }

  buzzWelcome();

  // Define the radio communication
  radio.openWritingPipe(pipe1);
  radio.setChannel(124);
  radio.setAutoAck(true);
  radio.setCRCLength(RF24_CRC_8);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.printDetails();
  radio.stopListening();
}



void loop()
{
  // Read all analog inputs and map them to one Byte value
  data.stick1_x = map(analogRead(stick1_x_pin), 0, 1023, 0, 255);
  data.stick1_y = map(analogRead(stick1_y_pin), 0, 1023, 0, 255);
  data.stick2_x = map(analogRead(stick2_x_pin), 0, 1023, 0, 255);
  data.stick2_y = map(analogRead(stick2_y_pin), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(pot1_pin), 0, 1023, 0, 255);
  data.pot2 = map(analogRead(pot2_pin), 0, 1023, 0, 255);
  data.toggle1 = !digitalRead(toggle1_pin);
  data.toggle2 = !digitalRead(toggle2_pin);
  data.toggle3 = !digitalRead(toggle3_pin);
  data.button1 = !digitalRead(button1_pin);

  currentTime = millis();
  //   Send the whole data from the structure to the receiver
  if (radio.write(&data, sizeof(Data_Package)))
  {
    digitalWrite(led_pin, HIGH);
  }
  else
  {
    buzz();
    digitalWrite(led_pin, LOW);
  }

  // Serial.println(data.button1);
}
