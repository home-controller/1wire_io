#include <Arduino.h>

#include <OneWire.h>

#ifndef io_print
#define io_println(S) Serial.println(F(S)) // print a string stored in program flash instead of ram.
#define io_ln Serial.println()
#define io_print_nh(N) \
  Serial.print("0x");  \
  if (N < 16)          \
  {                    \
    Serial.print("0"); \
  }                    \
  Serial.print(N, HEX) // output a number
#define io_print_nh2(N) \
  if (N < 16)           \
  {                     \
    Serial.print("0");  \
  }                     \
  Serial.print(N, HEX) // output a number
#define io_print(S) Serial.print(F(S));
#define io_println_n(N) Serial.println(N)       // output a number
#define io_print_n(N) Serial.print(N)           // output a number
#define io_print_nb(N) Serial.print(N, BIN)     // output a number
#define io_println_nb(N) Serial.println(N, BIN) // output a number
#endif
;
#ifndef QuiteMode
#define QuiteMode 0
#endif

#define DS2405_FAMILY_ID 0x5       // 1 channel IO :1
#define DS18S20_FAMILY_ID 0x10     //: 2
#define DS2406_FAMILY_ID 0x12      // DS2406 or DS2407. Both have same id. // 2 channel IO :3
#define DS2407_FAMILY_ID 0x12      //: 4
#define DS18B20_FAMILY_ID 0x28     //: 5
#define DS2408_FAMILY_ID 0x29      // 8 channel IO :6
#define DS2413_FAMILY_ID 0x3A      // 2 channel IO :7
#define DS2413clone_FAMILY_ID 0x85 // 2 channel IO  :8
#define _3A_2100H_FAMILY_ID 0x85   // 3A 2100H markings on chip. = DS2413 clone with dif id :9
#define _3A1100H_FAMILY_ID 0xBA    // 3A+ 1100H markings on chip. = DS2413 clone with dif id
#define DS2413clone2_FAMILY_ID _3A1100H_FAMILY_ID
//{   0,    1,    2,    3,    4,    5,    6 };
const static PROGMEM byte type1wireChip[] = {0x5, 0x10, 0x12, 0x28, 0x29, 0x3A, 0x85, 0xBA}; // if adding more, array values must be in ascending order.
const static PROGMEM byte onewireSwitchIO[] = {1, 0, 2, 0, 8, 2, 2, 2};
#define num1wireIDs sizeof(type1wireChip) // some have the same ID so 7 different ids not 9

void io_print1wireAddr(byte addr[]);

/**
 * @brief Get the number of IO pins the chip has, i.e. 8 for the DS2408.
 *
 * @param[in] i the index into the array. use get1wireChipTypeIOPins() chip type id.
 * @return byte number of IO pins.
 * .
 */
byte get1wireIOno(byte i)
{ // from index. This is the number of IO pins the chip has i.e. 8 for the DS2408
  if ((i >= 0) and (i < num1wireIDs))
  {
    return pgm_read_byte_near(onewireSwitchIO + i);
  }
  return 0;
}

/**
 * @brief Get the chip type id at type1wireChip[i] in the array of 1wire chip type IDs.
 *
 * @param i Index into the program memory array.
 * @return byte The chip id at i.
 */
byte get1wireIdFIA(byte i)
{ // from index. This is the number of IO pins the chip has i.e. 8 for the DS2408
  if ((i >= 0) and (i < num1wireIDs))
  {
    return pgm_read_byte_near(type1wireChip + i);
  }
  return 0;
}

/**
 * This will return the index of the chip in the 2 arrays type1wireChip[x] & onewireSwitchIO[x]
 * eg use get1wireIOno(get1wireIDsIndex( addr[0] ) ); to get the number of IO pins/channels for chip in the working vars etc.
 *
 * @brief Gets the array index index from the chip type id
 *
 * @param[in] element chip type code referenced in fist byte of addrs from 1-wire chips
 * @return int16_t return the index or -1 if chip type not found.
 */
int16_t get1wireIDsIndex(byte element)
{
  uint8_t low, high;
  uint8_t middle, id;
  low = 0;
  high = num1wireIDs - 1;
  // Serial.print(F(" high = ") ); Serial.print( high);

  while (low <= high)
  {

    middle = (low + high) / 2;
    id = byte(pgm_read_byte_near(type1wireChip + middle));
    // Serial.println(); Serial.print(F("low= ") ); Serial.print( low); Serial.print(F(", middle = ") ); Serial.print( middle); Serial.print(F(", high= ") ); Serial.print( high); Serial.print(F(", id = ") ); Serial.print( id, HEX);
    if (element > byte(id))
      low = middle + 1;
    else if (element < id)
    {
      if (middle < 1)
      {
        break;
      }
      high = middle - 1;
    }
    else
    {
      return middle;
    }
  }
  return -1;
}

/**
 * @brief Returns the number of IO pins the 1-wire chip has
 *
 * @param chipType the 1-wire chip type id e.g. 0x85(DS2413clone_FAMILY_ID).
 * @return byte
 */
byte get1wireChipTypeIOPins(byte chipType)
{
  int16_t x;
  x = get1wireIDsIndex(chipType);
  // io_print("get1wireIDsIndex(chipType) = ");
  // io_println_n(x);
  if (x >= 0)
    return get1wireIOno(x);
  else
    return 0;
}

/**
 * more or less the same thing as get1wireIDsIndex although you would think from the name it was the opposite.
 *
 * @brief Returns the index into the arrays type1wireChip[i] and  onewireSwitchIO[x].
 *
 * @param id the chip type id eg. 0x85 for DS2413clone_FAMILY_ID
 * @return byte
 */
byte get1wireIDfromIndex(byte id)
{
  if (id > 0x28)
  {
    if (id == 0x3A)
    {
      return 5;
    }
    else if (id == 0x85)
    {
      return 6;
    }
    else
    {
      return 4;
    } // only 0x29 left
  }
  else
  {
    if (id > 0x10)
    {
      if (id == 0x12)
      {
        return 2;
      }
      else
      {
        return 3;
      }
    }
    else
    {
      if (id == 0x5)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }
  }
  return 0xFF; // Unknown chip
}

/**
 * @brief Get the Chips number of IO pins.
 *
 * @param id The oneWire chips type Id number. ie: 0x3A = DS2413_FAMILY_ID
 * @return byte The number of IO pins the chip has.
 */
byte getChipsNoIO(byte id)
{
  return get1wireIOno(get1wireIDsIndex(id));
}

#define DS2413_ACCESS_READ 0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS 0xAA
#define DS2413_ACK_ERROR 0xFF

/*DS2413_ACCESS_READ returns [b7    |b6   ||b5    |b4   |b3          |b2       |b1         |b0       ]
                             [Complement of b3 to b0    |PIOB Output |PIOB Pin |PIOA Output|PIOA Pin ]
                             [                          |Latch State |State    |Latch State|State    ]
 so to get relays set to on in 2 least significant bit do x = ((r bitand b1) bitor ( (r >> 1) bitand b10)) xor B11
 */

byte readDS2413(OneWire &o, byte addr[])
{
  byte r;
  o.reset();
  delay(10); // todo remove delay;
  o.select(addr);
  o.write(DS2413_ACCESS_READ);
  r = o.read(); // for fast poll of pins you could keep reading without reset, maybe if lib supports it. not needed here though.
  o.reset();    // From data-sheet looks like 1-wire chip may send byte to bus forever after read without reset.
  // digitalWrite(13);
  return r;
}

bool testDS2413(OneWire &o, byte addr[])
{
  bool ok = false;
  uint8_t results;
  results = readDS2413(o, addr);              // Get the register results
  ok = ((!results) & 0x0F) == (results >> 4); // Compare nibbles
  // if(ok==0){io_print( " ok = " );io_print_n(ok);io_print( "results = " );io_print_n(results);}
  return ok;
}
bool test1wireIO(OneWire &o, byte addr[])
{
  if ((addr[0] == DS2413_FAMILY_ID) or (addr[0] == DS2413clone_FAMILY_ID))
  {
    return testDS2413(o, addr);
  }
  return false;
}

byte read1wireIO(OneWire &o, byte addr[])
{
  bool ok = false;
  uint8_t results = 0xff; // 1-wire uses ff for error so might as well keep the same.
  if ((addr[0] == DS2413_FAMILY_ID) or (addr[0] == DS2413clone_FAMILY_ID))
  {
    results = readDS2413(o, addr);              // Get the register results
    ok = ((!results) & 0x0F) == (results >> 4); // Compare nibbles
    results &= 0x0F;                            /* Clear inverted values*/
  }
  // return ok ? results : -1;
  if (ok)
  {
    return results;
  }
  else
  {
    return 0xff;
  }
}

/**
 * Each instance of the OneWire class stores it's pin as a reg + bitmap for direct access.
 *
 * DS2413 can only pull the IO pins to ground.
 * Open drain Mosfet keeps settings for quite awhile even with no power.
 *
 * @brief write output to a 1-wire DS2413 chip.
 *
 * @param o OneWire object of class OneWire. From the standard lib, look at the Arduino website manual for more info.
 * @param addr 8 byte array. Chip type code, 6 bytes of address and CRC
 * @param state bitmap of IO pin state sent to the 1-wire chip IO register.
 * @return true  for success.
 */
bool writeDS2413(OneWire &o, byte addr[], byte state)
{
  uint8_t ack = 0;
  /* Top six bits must '1' */
  state |= 0xFC;
  //  state = state xor B11;// Invert IO bits as 0 = 0 voltage and as IO output is an n mosfet 0v is = on;
  o.reset();
  o.select(addr);
  o.write(DS2413_ACCESS_WRITE);
  o.write(state xor B11);       // Invert IO bits as 0 = 0 voltage and as IO output is an n mosfet 0v is = on;
  o.write(state xor B11111100); /* Invert data and resend     */
  ack = o.read();               /* 0xAA=success, 0xFF=failure */
  o.reset();                    // Stop chip sending. At least in some places the chip read continuously sends on the bus until reset.
  Serial.print(F("ack:"));
  Serial.print(ack, HEX);
  if (ack == DS2413_ACK_SUCCESS)
  { // 0xAA
    if ((QuiteMode) != 1)
    {
      Serial.print(F("Set success: B"));
      Serial.print(state, BIN);
    }
  }
  else
  {
    Serial.print(F(" Error writing to 1wire, 0xAA=success, 0xFF=failure: 0x"));
  }
  return (ack == DS2413_ACK_SUCCESS ? true : false);
}

/**
 * Takes a pointer to a 1-wire class objects from the standard Arduino OneWire libary initialized
 * with the pin number etc. and a chip address on that pin/bus and the sets the IO Register on the 1-wire ship.
 * Chip IO pins are a Mosfet so High impedance(off) and open drain(short to 0v).
 * @brief Writes bitmap to 1-wire pin IO Register(Sets all the IO pins on the 1-wire chip).
 *
 * @param o Pointer to a 1-wire bus/pin class object.
 * @param addr 8 byte long array 1-wire chip address. [0] is the chip type and [7] the crc.
 * @param state bitmask of IO output wanted. 0b11 should pull OIA and IOB low and 0 set them to off, high impedance?
 * @return true for successes or false for fail.
 */
bool write1wireIO(OneWire &o, byte addr[], byte state)
{
#ifndef v1wire
  //  io_print("Setting 1-wire on pin: ");
  //  io_print_n(lights_1wire_pins[owIndex]);
  if (!QuiteMode)
  {
    io_print("ChipType: ");
    io_print1wireAddr(addr);
  }
#endif
  if ((addr[0] == DS2413_FAMILY_ID) 
  or (addr[0] == DS2413clone_FAMILY_ID)
   or (addr[0] == DS2413clone2_FAMILY_ID))
  {
    // io_println( "calling writeDS2413(addr,owIndex,state)" );
    if (!QuiteMode)
    {
      io_print(", ");
    }
    return writeDS2413(o, addr, state);
  }
  else
  {
    io_println("write1wireIO() chip type not suported");
  }
  return 0;
}

void printChipInfo(byte id)
{
  Serial.print("\e[0;94m");
  switch (id)
  {
  case DS2413_FAMILY_ID:
    Serial.print(F("DS2413 IO/Open drain switch.\t"));
    break;
  case DS2413clone_FAMILY_ID:
    Serial.print(F("3A 2100H (DS2413 clone).\t"));
    break;
  case DS2413clone2_FAMILY_ID:
    Serial.print(F("3A + 1100H (DS2413 clone).\t"));
    break;
  case DS2408_FAMILY_ID:
    Serial.print(F("DS2408 IO/Open drain switch.\t"));
    break;
  case DS2406_FAMILY_ID:
    Serial.print(F("DS2406/DS2407 IO+ 1kBit OTPROM\t"));
    break;
  case DS2405_FAMILY_ID:
    Serial.print(F("DS2405 Switch. DEPRECATED.\t"));
    break;
  case DS18S20_FAMILY_ID:
    Serial.print(F("DS18S20 temperature. Obsolete\t"));
    break;
  case DS18B20_FAMILY_ID:
    Serial.print(F("DS18B20 temperature sensor.\t"));
    break;
    // case :     break;

  default:
    io_print("Unsupported chip type: ");
    io_print_nh(id);
    io_print("\t");
    break;
  }
  io_print_n(get1wireChipTypeIOPins(id));
  io_print(" IO pins. ");
}

void test_printChipInfo()
{
  byte x, id;
  io_println("Testing printChipInfo()");
  for (x = 0; x <= num1wireIDs; x++)
  {
    id = get1wireIdFIA(x);
    printChipInfo(id);
    io_ln;
  }
}

void io_print1wireAddr(byte addr[])
{
  byte i;
  io_print("Addr:");
  // not needed now with union type. io_print_nh2( working1wire.chipType );
  // read each byte in the address array
  io_print_nh2(addr[0]);
  for (i = 1; i < 7; i++)
  {
    io_print(",");
    // print each byte in the address array in hex format
    io_print_nh2(addr[i]);
  }
  io_print(".");
}

/**
 * @brief Find and print details of chips on the 1-wire bus. OneWire class stores the pin, and by default the address of the last search.
 *
 * @return int  No. chips found or -3 for CRC read fail.
 */
int print1wireDevicesOnPin(OneWire &o)
{
  byte addr[8];
  byte deviceCount = 0;

  // Serial.println(F("\nStart search on 1-wire bus No. "));  //Serial.print( pi );  Serial.print( F( " on hardware pin " ) ); p = pinsA[ pi ]; Serial.println( p );
  /* initiate a search for the OneWire object we created and read its value into
    addr array we declared above*/

  while (o.search(addr))
  {
    deviceCount++;
    // a check to make sure that what we read is correct.
    if (OneWire::crc8(addr, 7) != addr[7])
    {
      Serial.print(F("CRC failed!\n"));
      return -3;
    }
    else
    {
      printChipInfo(addr[0]);
      io_print1wireAddr(addr);
      Serial.print(F(" CRC No = "));
      Serial.print(addr[7], HEX);
      Serial.print(F(" Passed"));
      //   if (i < 0) {  //not stored in EEprom
      //     //oneWireChipsOnPin[pi]++;
      //     //oneWireChipsOnPinNotEE[pi]++;
      //     //Serial.print(F(", \e[91mNot found in EEPROM\e[37m"));
      //     // 30-37: foreground colour, 40-47:background. 90-97: bright foreground, 100-107: bright background
      //     // 0:black, 1:red, 2:green, 3:yellow, 4:blue, 5:magenta, 6:cyan, 7:white
      //     // effects: 0=reset, 1=bold, 2=faint, 3=Italic, 4=underlined, 5=flash, 7=Reverse(swap foreground with background), 8=Hidden(passwords), 9=Strikethrough
      //   } else {  // In eeprom
      //     //oneWireChipsOnPin[pi]++;
      //     //oneWireState[i] &= B11110000;  //and out the 3 bits for 1-wire bus plus 1 bit for saved chip found
      //     //oneWireState[i] |= (pi << 1) bitor B00000001;
      //     Serial.print(F(", \e[92;1mFound in EEPROM at: \e[92;3m"));
      //     Serial.print(i);
      //     Serial.print("\e[37;0m");
      //   }
      //    if (addr[0] == DS2413_FAMILY_ID) { Serial.println(F(" is DS2413 family chip. 2 Channel I/O Can use as switch or input\n") );    }
    }
    Serial.println();
  }
  o.reset_search();
  Serial.print(F("Finished searching bus, found "));
  Serial.print(deviceCount);
  Serial.println(F(" chips on this bus(pin)"));
  // Serial.println(pi);  //Serial.println();
  // EEPROM.update( 4 , no_storedChips );
  return deviceCount;
}

/**
 * @brief Find and print details of chips on the 1-wire bus. OneWire class stores the pin, and by default the address of the last search.
 *
 * @return int  No. chips found or -3 for CRC read fail.
 */
int setAll1wirePin(OneWire &o, byte state)
{
  byte addr[8];
  byte deviceCount = 0;

  // Serial.println(F("\nStart search on 1-wire bus No. "));  //Serial.print( pi );  Serial.print( F( " on hardware pin " ) ); p = pinsA[ pi ]; Serial.println( p );
  /* initiate a search for the OneWire object we created and read its value into
    addr array we declared above*/

  while (o.search(addr))
  {
    deviceCount++;
    // a check to make sure that what we read is correct.
    if (OneWire::crc8(addr, 7) != addr[7])
    {
      Serial.print(F("CRC failed!\n"));
      return -3;
    }
    else
    {
      printChipInfo(addr[0]);
      io_print1wireAddr(addr);
      Serial.print(F(" CRC No = "));
      Serial.print(addr[7], HEX);
      Serial.print(F(" Passed"));
      if (write1wireIO(o, addr, state))
      {
        Serial.print(F("Set state success"));
      }
      else
      {
        Serial.print(F("Failed to set state"));
      }
    }
    Serial.println();
  }
  o.reset_search();
  Serial.print(F("Finished searching bus, found "));
  Serial.print(deviceCount);
  Serial.println(F(" chips on this bus(pin)"));
  // Serial.println(pi);  //Serial.println();
  // EEPROM.update( 4 , no_storedChips );
  return deviceCount;
}