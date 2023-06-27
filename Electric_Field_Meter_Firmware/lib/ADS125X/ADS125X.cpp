   
#include "ADS125X.h"
#include "Arduino.h"
#include "SPI.h"

ADS125X::ADS125X()
{
}


void ADS125X::writeRegister(uint8_t address, uint8_t value)
{
  digitalWrite(ADS125X_CS_PIN, LOW);
  delayMicroseconds(1);
  SPI.transfer(CMD_WREG|(address));
  SPI.transfer(0x00);
  SPI.transfer(value);
  delayMicroseconds(1);
  digitalWrite(ADS125X_CS_PIN, HIGH);
}

uint8_t ADS125X::readRegister(uint8_t address)
{
  digitalWrite(ADS125X_CS_PIN,LOW);
  delayMicroseconds(1);
  SPI.transfer(CMD_RREG|(address));
  SPI.transfer(0x00);
  delayMicroseconds(7);
  uint8_t data = SPI.transfer(SPI_MASTER_DUMMY);
  delayMicroseconds(2);
  digitalWrite(ADS125X_CS_PIN,HIGH);
  return data;
}

uint8_t ADS125X::readNext()
{
    uint8_t data = SPI.transfer(SPI_MASTER_DUMMY);
    return data;
}

void ADS125X::begin(uint8_t cs_pin, uint8_t drdy_pin) {
  // Set pins up
  ADS125X_CS_PIN = cs_pin;
  ADS125X_DRDY_PIN = drdy_pin;

  // Configure the SPI interface (CPOL=0, CPHA=1)
  SPI.begin();
  SPI.beginTransaction(
      //SPISettings(2000000, MSBFIRST, SPI_MODE1));
      //SPISettings(7.68 * 1000000 / 2, MSBFIRST, SPI_MODE1));
      SPISettings(1000000, MSBFIRST, SPI_MODE1));

  // Configure chip select as an output
  pinMode(ADS125X_CS_PIN, OUTPUT);
  digitalWrite(ADS125X_CS_PIN, HIGH); 
  
  // Configure DRDY as as input (mfg wants us to use interrupts)
  pinMode(ADS125X_DRDY_PIN, INPUT);

  /*

  digitalWrite(ADS125X_CS_PIN, LOW); // Set CS Low
  delayMicroseconds(1); // Wait a minimum of td(CSSC)
  reset(); // Send reset command
  delayMicroseconds(1);; // Delay a minimum of 50 us + 32 * tclk

  // Sanity check read back (optional)

  startSync(); // Send start/sync for continuous conversion mode
  delayMicroseconds(1); // Delay a minimum of td(SCCS)
  digitalWrite(ADS125X_CS_PIN, HIGH); // Clear CS to high
  */
}

void ADS125X::sendCommand(uint8_t command)
{
  digitalWrite(ADS125X_CS_PIN, LOW);
  SPI.transfer(command);
  delayMicroseconds(1);
  digitalWrite(ADS125X_CS_PIN, HIGH);
}

void ADS125X::reset()
{
  sendCommand(CMD_RESET);
}

void ADS125X::startSync()
{
  sendCommand(CMD_SYNC);
}