#include <SPI.h>
#include <CircularBuffer.h>

typedef struct
{
  byte sb;
  byte adc_b1;
  byte adc_b2;
  byte adc_b3;
  uint32_t adc_pps_time;
  byte eb;
} datapacket;

CircularBuffer<datapacket, 10000> datapackets;

const uint8_t PIN_ADC_INT = 10;
const uint8_t PIN_ADC_CS = 5;
const uint8_t PIN_BUFFER_FULL = A2;
const uint8_t PIN_SERIAL_FULL = A3;
byte b1;
byte b2;
byte b3;

void setAllRegisters()
{
  // Sets all registers to what we think we need
  digitalWrite(PIN_ADC_CS, LOW);
  delay(5);
  SPI.transfer(0x46); // incremental write starting at 0x01
  SPI.transfer(0b01000011); // CONFIG0
  //SPI.transfer(0b00000000); // CONFIG1 for 38.4kHZ
  //SPI.transfer(0b00000100); // CONFIG1 for 19.2kHz
  SPI.transfer(0b00001000); // CONFIG1 for 9.6kHz
  SPI.transfer(0b10001011); // CONFIG2
  SPI.transfer(0b11000000); // CONFIG3
  SPI.transfer(0b01110011); // IRQ
  SPI.transfer(0b00000001); // MUX
  SPI.transfer(0x000000); // SCAN
  SPI.transfer(0x000000); // TIMER
  SPI.transfer(0x000000); // OFFSETCAL
  SPI.transfer(0x800000); // GAINCAL
  SPI.transfer(0x900000); // RESERVED
  SPI.transfer(0x30); // RESERVED
  SPI.transfer(0xA5); // LOCK
  SPI.transfer(0x000F); // RESERVED
  delay(5);
  digitalWrite(PIN_ADC_CS, HIGH);
}

void setup()
{
  Serial.begin(2000000);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  pinMode(PIN_ADC_CS, OUTPUT);
  pinMode(PIN_BUFFER_FULL, OUTPUT);
  pinMode(PIN_SERIAL_FULL, OUTPUT);
  pinMode(PIN_ADC_INT, INPUT_PULLUP);
  digitalWrite(PIN_ADC_CS, HIGH);
  digitalWrite(PIN_BUFFER_FULL, LOW);
  digitalWrite(PIN_SERIAL_FULL, LOW);
  
  delay(100);
  setAllRegisters();
  delay(100);
  attachInterrupt(digitalPinToInterrupt(PIN_ADC_INT), adcisr, FALLING);
}

void adcisr()
{
  uint32_t adcus = micros();
  digitalWrite(PIN_ADC_CS, LOW);
  SPI.transfer(0x41); // Read ADC DATA
  b1 = SPI.transfer(0x00);
  b2 = SPI.transfer(0x00);
  b3 = SPI.transfer(0x00);
  datapackets.push(datapacket{0xBE, b1, b2, b3, adcus, 0xEF});
  digitalWrite(PIN_ADC_CS, HIGH);
}

void loop()
{
  if ((!datapackets.isEmpty()) && (Serial.availableForWrite()> 10))
  {
    // We have things to write and the place to write them
    datapacket dp = datapackets.pop();
    Serial.write((byte*)&dp.sb, 1);
    Serial.write((byte*)&dp.adc_b1, 1);
    Serial.write((byte*)&dp.adc_b2, 1);
    Serial.write((byte*)&dp.adc_b3, 1);
    Serial.write((byte*)&dp.adc_pps_time, 4);
    Serial.write((byte*)&dp.eb, 1);
  }

  if (datapackets.isFull())
  {
    digitalWrite(PIN_BUFFER_FULL, HIGH);
  }
  if (Serial.availableForWrite() < 10)
  {
    digitalWrite(PIN_SERIAL_FULL, HIGH);
  }
}
