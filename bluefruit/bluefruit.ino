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
#define MODE_LED_BEHAVIOUR          "DISABLED"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

int download = A4;
int migrate  = A3;
int test     = A2;
int deploy   = A1;
int start    = A0;
int led = 13;

void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup(void)
{

  Serial.begin(115200);
  Serial.println(F("EasyButton Starting"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit"));
  }
  Serial.println( F("OK!") );

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  ble.info();
  ble.verbose(false);

  /*
  while (! ble.isConnected()) {
      delay(500);
  }
  */

  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }
  ble.setMode(BLUEFRUIT_MODE_DATA);

  /* Set up the input pins */
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1 , INPUT_PULLUP);
  pinMode(A2    , INPUT_PULLUP);
  pinMode(A3  , INPUT_PULLUP);
  pinMode(A4   , INPUT_PULLUP);
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

  starting = !digitalRead(start);

  if(started != started) {
    started = 0;
  }
  
  if(starting && !started) {
    started = starting;
    Serial.print("{\n");
    if (!digitalRead(download)) {
      Serial.print("  download: 1,\n");
    }
    if (!digitalRead(migrate)) {
      Serial.print("  migrate: 1,\n");
    }
    if (!digitalRead(test)) {
      Serial.print("  test: 1,\n");
    }
    if (!digitalRead(deploy)) {
      Serial.print("  deploy: 1,\n");
    }
    Serial.print("}\n");
  }



  /*
  operations[1] = digitalRead(migrate);
  operations[2] = digitalRead(test);
  operations[3] = digitalRead(deploy);
  operations[4] = digitalRead(start);

  */
  //Serial.print(operations,HEX);
  //ble.print(operations,HEX);
  
}
