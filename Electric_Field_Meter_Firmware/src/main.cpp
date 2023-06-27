#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SPI.h"
#include "pins.h"
//#include "TinyGPS++.h"
#include "ADS125X.h"
#include <CircularBuffer.h>
//#define SPI_SPEED SD_SCK_MHZ(50)

// Data packet
struct DataPacket
{
  uint32_t adc_ready_time;
  int32_t adc_reading;
};

CircularBuffer<DataPacket, 150> DataPacketBuffer;

// Instance Creation
//TinyGPSPlus gps;
//SoftwareSerial gpsSer(5, 4);
ADS125X adc;

int32_t data = 0;
uint32_t data_clk_time = 0;
uint32_t pps_time = 0;

void GPSReadISR(void)
{
  pps_time = micros();
  //Serial.println(pps_time);
}

void ADCReadISR(void)
{
  DataPacket dp;
  dp.adc_ready_time = micros();
  dp.adc_reading = SPI.transfer16(0x00);
  dp.adc_reading = (dp.adc_reading << 8) | SPI.transfer(0x00);
  DataPacketBuffer.push(dp);
}


void serialWrite32(uint32_t data)
{
  byte buf[4];
  buf[0] = data & 255;
  buf[1] = (data >> 8) & 255;
  buf[2] = (data >> 16) & 255;
  buf[3] = (data >> 24) & 255;
  Serial.write(buf, sizeof(buf));
}


void setup()
{
  //delay(2000);
  // Pin Modes
  pinMode(PIN_GPS_PPS, INPUT);

  Serial.begin(115200);
 // gpsSer.begin(9600);
  adc.begin(PIN_ADC_CS, PIN_ADC_DRDY);
  delay(10);
 
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  adc.sendCommand(CMD_STOP_RDATAC);
  //SPI.transfer16(0x00);
  //SPI.transfer(0x00);
  
  adc.writeRegister(0x03, 0x23);
  delay(1);
  //adc.sendCommand(CMD_RDATAC);
  delay(1);
  digitalWrite(PIN_ADC_CS, LOW);
  delay(1);
  //adc.readRegister(0x00);
  
  
  attachInterrupt(digitalPinToInterrupt(PIN_ADC_DRDY), ADCReadISR, FALLING);
  //attachInterrupt(digitalPinToInterrupt(PIN_GPS_PPS), GPSReadISR, RISING);

}

void loop()
{
 // Serial.println(millis());
 //Serial.println(DataPacketBuffer.size());
 /*
 while(!DataPacketBuffer.isEmpty())
 {
   DataPacket dp = DataPacketBuffer.pop();
   //serialWrite32(dp.adc_ready_time);
   //serialWrite32(dp.adc_reading);
   Serial.print(dp.adc_ready_time);
   Serial.print(",");
   Serial.println(dp.adc_reading);
 }
 */
  //SPI.transfer16(0x00);
  //SPI.transfer(0x00);
  delay(100);
}