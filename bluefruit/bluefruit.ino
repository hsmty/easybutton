/*********************************************************************
 This code is based on an example for the nRF51822 based Bluefruit LE modules
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "STATUS"
#define DEBOUNCE                     10

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

typedef struct {
  int pin;
  String name;
} button;

button buttons[4] = {
  {A4, String("DOWNLOA")},
  {A3, String("Migrate")},
  {A2, String("TEST")},
  {A1, String("DEPLOY")},
};

button start_button = {A0, "BEGIN"};
volatile byte pressed, justpressed, justreleased;
bool buttonState;

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("EasyButton Starting..."));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit"));
  }
  Serial.println( F("OK!") );

  /* Disable command echo from Bluefruit */
  ble.echo(false);
  while (! ble.isConnected())
  {
      delay(500);
  }

  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }
  ble.setMode(BLUEFRUIT_MODE_DATA);

  for(int i=0;i<4;i++)
  {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  pinMode(start_button.pin, INPUT_PULLUP);
  buttonState = digitalRead(start_button.pin);
}

void loop(void)
{
  char n, inputs[BUFSIZE+1];
  bool debounce1;
  bool debounce2;
  bool started;
  String stages;
  char s[42];
  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.print(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // Echo received data
  while ( ble.available() )
  {
    int c = ble.read();
    Serial.print((char)c);
  }

  debounce1 = !digitalRead(start_button.pin);
  delay(DEBOUNCE);
  debounce2 = !digitalRead(start_button.pin);
  
  if(debounce1 == debounce2 && debounce1 != buttonState && debounce1 == true) {
    stages = "";
    for(int i=0;i<4;i++)
    {
      if (!digitalRead(buttons[i].pin)) {
        stages = stages + buttons[i].name + ",";
      }
    }
    if (stages != "") {
      stages = "BEGIN: " + stages + "\n";
      stages.toCharArray(s,42);
      Serial.print(stages);
      ble.print(s);
    }
  }
  buttonState = debounce1;
}
