#include <Arduino.h>

#include <OneWire.h>
//#include <1wire.h>
//#include <../../../src/1wire_chip_io.h>
#include <1wire_chip_io.h>

//++++++++++++++++++++++ Storage blocks class setup start++++++++++++++++
/**
 * @brief Storage blocks
 * leave 15 bytes for misc vars.
 * EEPROM[0] should be id/ip address.
 *
 * BLOCKS
 * 0: Rooms
 * 1: 1-wire
 * 2: Switch units
 * 3: groups
 *
 * global vars are here, blocks are setup in setup()
 */
#define one_wire_pin 5
#define serial_speed 38400
#define termIO

#define storageBlocksMax 5
#define eepromBlockStart 15
// storageBlock_T storageBlocksA[storageBlocksMax];  //Mem for sState. Do this here to save the need for acquiring memory in the class at run time.
// storageBlock_C sState(storageBlocksMax, storageBlocksA, eepromBlockStart);
//----------------------- Storage blocks declarations end.

// lights_C lights;// lights class.

// OneWire_mem v1wire;
OneWire ow(one_wire_pin);

void setup()
{
  // byte id1wire;
#ifdef termIO
  // Serial.begin( 9600 );
  Serial.begin(serial_speed);
  delay(250);
  // io_print( "Serial.begin(" );  io_print_n( serial_speed );  io_println( ")" );
  Serial.print(F("Serial.begin("));
  Serial.print(serial_speed);
  Serial.print(F(")"));
#endif
  Serial.print(F("\nStart search on 1-wire bus. "));
  Serial.print(F("On hardware pin "));
  Serial.println(one_wire_pin);
  print1wireDevicesOnPin(ow);

  // put your setup code here, to run once:
}

void loop()
{
  static byte ioState;
  byte addr[8];
  ioState = ioState bitand 0b11;
  // put your main code here, to run repeatedly:
  Serial.print(F("\nSetting state to: "));Serial.print(ioState,BIN);Serial.println();
  setAll1wirePin(ow, ioState);
  delay(5000);
  ioState++;
}