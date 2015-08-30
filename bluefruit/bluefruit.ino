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
  {A4, "DOWNLOAD"},
  {A3, "MIGRATE"},
  {A2, "TEST"},
  {A1, "DEPLOY"},
};

button start_button = {A0, "BEGIN"};
volatile byte pressed, justpressed, justreleased;

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
}

void check_begin(void) {
  
}

void loop(void)
{
  char n, inputs[BUFSIZE+1];
  bool starting, started = 0;
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

  starting = !digitalRead(start_button.pin);
  
  if(starting && !started) {
    started = starting;

    Serial.print("BEGIN:");
    ble.print("BEGIN:");
    for(int i=0;i<4;i++)
    {
      if (!digitalRead(buttons[i].pin)) {
        Serial.print(buttons[i].name + ",");
        ble.print(buttons[i].name + ",");
      }
    }
    Serial.print("\n");
    ble.print("\n");
  }
}
