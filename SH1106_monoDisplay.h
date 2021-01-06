#include "Arduino.h"
#include"lcdResources.h"
#include<Wire.h>

class SH1106_monoDisplay{
  public:
    // constructors
    SH1106_monoDisplay(byte DEV_ADDR);
    SH1106_monoDisplay(byte DEV_ADDR,uint8_t scl, uint8_t sda);
    
    // methods
    void setPageAddress(uint8_t page);
    void setColumnAddress(uint8_t col);
    void writeRamData(byte data);
    void sendCommand(byte command);
    byte readRamData();
    void startReadModify();
    void endReadModify();
    void writePixel(uint8_t x, uint8_t y);
    void setDC_ON_OFF();
    void setReverseDisplay(bool reverse);
    void setDisplayOff(bool off);
    void flipHorizontal(bool remap);
    void flipVertical(bool flip);
    void blinkDisplay(uint8_t del1ms, uint8_t del2ms);
    void clearScreen();
    void clearPage(uint8_t col, uint8_t page);
    void clearRow(int8_t col, uint8_t page, uint8_t len);
    void drawBar(uint8_t page, uint8_t col, uint8_t offset, uint8_t len, int del );
    void drawBar16(uint8_t page, uint8_t col, uint8_t len, int del );
    void drawText(uint8_t col, uint8_t page, String text);
    void drawText16(uint8_t col, uint8_t page, String text);
    void drawImage(uint8_t col, uint8_t page, uint8_t width, uint8_t height,const byte* img);
    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    
  private:
    byte _ADDR;
};
