#include <Arduino.h>

#define PRESET_VALUE 0xFFFF
#define POLYNOMIAL 0x8408

//#define DEBUG
#ifdef DEBUG
  #define DEBUG_PRINT(msg,value) Serial.print(msg); Serial.println(value)
#else
  #define DEBUG_PRINT(msg,value) 
#endif

void TaskReadCommand();
unsigned int uiCrc16Cal(unsigned char const *, unsigned char);
void parseCommad(uint8_t *);

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  TaskReadCommand();
}

void TaskReadCommand()
{
  static uint8_t state = 0;
  static uint8_t buffer[32] = {0};
  static uint8_t dataCounter = 0;

  switch (state)
  {
  case 0: // wait for the first byte: len

    if (Serial.available())
    {
      buffer[dataCounter] = Serial.read();
      dataCounter++;
      state = 1;
      DEBUG_PRINT("Go to rx data","");
    }
    break;

  case 1: // read data
    while (Serial.available())
    {
      buffer[dataCounter] = Serial.read();
      dataCounter++;

      if (dataCounter == (buffer[0] + 1))
      { // if all bytes arrived
        // verify the checksum
        DEBUG_PRINT("Verify the checksum","");
        DEBUG_PRINT("dataCount: ",dataCounter);
        if (dataCounter >= 5)
        {
          unsigned int checksum = uiCrc16Cal(buffer, dataCounter - 2);
          uint8_t lsBChecksum = (uint8_t)(checksum & 0x000000FF);
          uint8_t msBChecksum = (uint8_t)((checksum & 0x0000FF00) >> 8);
          if ((lsBChecksum == buffer[dataCounter - 2]) && (msBChecksum == buffer[dataCounter - 1]))
          {
            DEBUG_PRINT("ChecksumOK","");
            parseCommad(buffer);
          }
        }
        dataCounter = 0;
        state = 0;
        DEBUG_PRINT("Go to rx len","");
      }
    }
    break;
  }
}

uint8_t command21[] = {0x0D, 0x00, 0x21, 0x00, 0x02, 0x44, 0x09, 0x03, 0x4E, 0x00, 0x1E, 0x0A, 0xF2, 0x16};
uint8_t command24[] = {0x05, 0x00, 0x24, 0x00, 0x25, 0x29};
uint8_t command2F[] = {0x05, 0x00, 0x2F, 0x00, 0x8D, 0xCD};
uint8_t command22[] = {0x05, 0x00, 0x22, 0x00, 0xF5, 0x7D};
uint8_t command28[] = {0x05, 0x00, 0x28, 0x00, 0x85, 0x80};
uint8_t command25[] = {0x05, 0x00, 0x25, 0x00, 0xFD, 0x30};

void parseCommad(uint8_t *pdata)
{
  uint8_t command = pdata[2];

  switch (command)
  {
  case 0x21:
    Serial.write(command21, sizeof(command21));
    break;
  case 0x24:
    Serial.write(command24, sizeof(command24));
    break;

  case 0x2F:
    Serial.write(command2F, sizeof(command2F));
    break;

  case 0x22:
    Serial.write(command22, sizeof(command22));
    break;

  case 0x28:
    Serial.write(command28, sizeof(command28));
    break;

  case 0x25:
    Serial.write(command25, sizeof(command25));
    break;
  }
}

unsigned int uiCrc16Cal(unsigned char const *pucY, unsigned char ucX)
{
  unsigned char ucI, ucJ;
  unsigned short int uiCrcValue = PRESET_VALUE;

  for (ucI = 0; ucI < ucX; ucI++)
  {
    uiCrcValue = uiCrcValue ^ *(pucY + ucI);
    for (ucJ = 0; ucJ < 8; ucJ++)
    {
      if (uiCrcValue & 0x0001)
      {
        uiCrcValue = (uiCrcValue >> 1) ^ POLYNOMIAL;
      }
      else
      {
        uiCrcValue = (uiCrcValue >> 1);
      }
    }
  }
  return uiCrcValue;
}
