/*
  DIY Arduino based RC Receiver code for the Arduino Radio control with PWM output
              == Receiver Code ==

  by Aymen Alouache, www.aymenalouache.xyz
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h>

#define ch1 2
#define ch2 3
#define ch3 4
#define ch4 5
#define ch5 6
#define ch6 17
#define ch7 16
#define ch8 15
#define ch9 14
#define ch10 9
#define ch11 10

RF24 radio(7, 8); // nRF24L01 (CE, CSN)
const byte address[6] = "node1";
const uint8_t pipe1 = 0xF0F0F0F0AA;

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;
unsigned long pastTime = 0;
int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int ch7_value = 0;
int ch8_value = 0;
int ch9_value = 0;
int ch10_value = 0;
int ch11_value = 0;
int freq = 0;

Servo channel_1;
Servo channel_2;
Servo channel_3;
Servo channel_4;
Servo channel_5;
Servo channel_6;
Servo channel_7;
Servo channel_8;
Servo channel_9;
Servo channel_10;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package
{
  byte stick1_x;
  byte stick1_y;
  byte stick2_x;
  byte stick2_y;
  byte pot1;
  byte pot2;
  byte switch1;
  byte switch2;
  byte switch3;
  byte button1;
};
Data_Package data; // Create a variable with the above structure



void buzz()
{
  if (data.switch1 == 1)
  {
    freq = map(data.pot2, 0, 255, 440, 900);
    if (currentTime - pastTime > 1500)
    {
      tone(ch11, freq, 100);
      pastTime = currentTime;
    }
    // currentTime = millis();
    // if (currentTime - pastTime > 2000) {
    //   tone(ch11, freq, 100);
    // }
  }
}
void resetData()
{
  data.stick1_x = 127;
  data.stick1_y = 0;
  data.stick2_x = 127;
  data.stick2_y = 127;
  data.pot1 = 0;
  data.pot2 = 0;
  data.switch1 = 0;
  data.switch2 = 0;
  data.switch3 = 0;
  data.button1 = 0;
}



void setup()
{
  // Attach the servo signal on defined up there
  channel_1.attach(ch1);
  channel_2.attach(ch2);
  channel_3.attach(ch3);
  channel_4.attach(ch4);
  channel_5.attach(ch5);
  channel_6.attach(ch6);
  channel_7.attach(ch7);
  channel_8.attach(ch8);
  channel_9.attach(ch9);
  channel_10.attach(ch10);

  resetData();

  Serial.begin(115200);
  printf_begin();

  // Start the radio and make sure the hardware connected
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

  // Define the radio communication
  radio.openReadingPipe(1, pipe1);
  radio.setChannel(124);
  radio.setAutoAck(true);
  radio.setCRCLength(RF24_CRC_8);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.printDetails();
  radio.startListening();
}



void loop()
{
  // Check whether there is data to be received
  currentTime = millis();
  if (radio.available())
  {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis();              // At this moment we have received the data
    Serial.print(data.stick1_y);
    Serial.print(", ");
    Serial.println(data.stick2_y);
    buzz();
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  if (currentTime - lastReceiveTime > 1000)
  {              // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }

  // Map values received to channel values
  ch1_value = map(data.stick1_x, 0, 255, 1000, 2000);
  ch2_value = map(data.stick1_y, 0, 255, 1000, 2000);
  ch3_value = map(data.stick2_x, 0, 255, 1000, 2000);
  ch4_value = map(data.stick2_y, 0, 255, 1000, 2000);
  ch5_value = map(data.pot1, 0, 255, 1000, 2000);
  ch6_value = map(data.pot2, 0, 255, 1000, 2000);
  ch7_value = map(data.switch1, 0, 1, 1000, 2000);
  ch8_value = map(data.switch2, 0, 1, 1000, 2000);
  ch9_value = map(data.switch3, 0, 1, 1000, 2000);
  ch10_value = map(data.button1, 0, 1, 1000, 2000);

  // Map data to output pins
  channel_1.writeMicroseconds(ch1_value);
  channel_2.writeMicroseconds(ch2_value);
  channel_3.writeMicroseconds(ch3_value);
  channel_4.writeMicroseconds(ch4_value);
  channel_5.writeMicroseconds(ch5_value);
  channel_6.writeMicroseconds(ch6_value);
  channel_7.writeMicroseconds(ch7_value);
  channel_8.writeMicroseconds(ch8_value);
  channel_9.writeMicroseconds(ch9_value);
  channel_10.writeMicroseconds(ch10_value);

  // Print the data in the Serial Monitor
  // Serial.print("_ Sticks  ");
  // Serial.print(data.stick2_x);
  // Serial.print(", ");
  // Serial.print(data.stick2_y);
  // Serial.print(",      ");
  // Serial.print(data.pot1);
  // Serial.print(", ");
  // Serial.print(data.pot2);
  // Serial.print(",      ");
  // Serial.print(data.switch1);
  // Serial.print(", ");
  // Serial.print(data.switch2);
  // Serial.print(", ");
  // Serial.println(data.button1);
}
