#pragma once
#include <Wire.h>
#include "lcdResources.h"

#ifdef MSP430
#include <Energia.h>
#else
#include <Arduino.h>
#endif

#ifndef SH1106_monoDisplay_h
#define SH1106_monoDisplay_h


class SH1106_monoDisplay{
  public:
    // constructors
    SH1106_monoDisplay(byte DEV_ADDR                          );
#ifdef WEMOS
    SH1106_monoDisplay(byte DEV_ADDR,uint8_t scl, uint8_t sda );
#endif
    // methods
    void setPageAddress     (uint8_t page);
    void setColumnAddress   (uint8_t col);
    
    void writePixel         (uint8_t x, uint8_t y);
    void setDC_ON_OFF       ();
    void setDisplayOff      (bool onOff);
    void setReverseDisplay  (bool reverse);
    void flipHorizontal     (bool remap);
    void flipVertical       (bool flip);
    void blinkDisplay       (uint8_t timeOn_ms, uint8_t timeOff_ms);
    void clearScreen        ();
    void clearPage          (uint8_t col, uint8_t page);
    void clearRow           (int8_t col, uint8_t page, uint8_t len);
    void drawBar            (uint8_t page, uint8_t col, uint8_t offset, uint8_t len, int del );
    void drawBar16          (uint8_t page, uint8_t col, uint8_t len, int del );
    void drawText           (uint8_t col, uint8_t page, String text);
    void drawText16         (uint8_t col, uint8_t page, String text);
    void drawImage          (uint8_t col, uint8_t page, uint8_t width, uint8_t height,const byte* img);
    void drawLine           (uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

  private:
    void writeRamData       (byte data);
    void sendCommand        (byte command);
    void startReadModify    ();
    void endReadModify      ();
    byte readRamData        ();

  private:
    uint8_t                 SH1106_ADDR;
    TwoWire                 i2c;
};

#endif