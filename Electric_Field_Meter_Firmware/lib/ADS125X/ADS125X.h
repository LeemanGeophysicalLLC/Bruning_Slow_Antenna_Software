
#ifndef ADS125X_h
#define ADS125X_h

#include "Arduino.h"

#define SPI_MASTER_DUMMY   0x00

// Commands for the ADC
#define CMD_RESET 0xFE
#define CMD_SYNC 0xFC
#define CMD_STDBY 0xFD
#define CMD_RDATA 0x01
#define CMD_RREG 0x10
#define CMD_WREG 0x50
#define CMD_RDATAC 0x03
#define CMD_STOP_RDATAC 0x0F
#define CMD_WAKEUP 0x00
#define CMD_SELFCAL 0xF0
#define CMD_SELFOCAL 0xF1
#define CMD_SELFGCAL 0xF2
#define CMD_SYSOCAL 0xF3
#define CMD_SYSGCAL 0xF4

class ADS125X {
  public:
    ADS125X();
    uint8_t ADS125X_CS_PIN;
    uint8_t ADS125X_DRDY_PIN;
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t readRegister(uint8_t address);
    void begin(uint8_t cs_pin, uint8_t drdy_pin);
    void startSync(void);
    void reset(void);
    void sendCommand(uint8_t command);
    uint8_t readNext(void);
};

#endif