#include "SH1106_monoDisplay.h"

SH1106_monoDisplay::SH1106_monoDisplay(byte DEV_ADDR){
  i2c.begin();
#ifdef MSP430
  i2c.setModule(0);
#endif
  SH1106_ADDR = DEV_ADDR;
}


#ifdef WEMOS
SH1106_monoDisplay::SH1106_monoDisplay(byte DEV_ADDR,uint8_t sda, uint8_t scl){
  i2c.begin(sda,scl);
  SH1106_ADDR = DEV_ADDR;
}
#endif


void SH1106_monoDisplay::setPageAddress(uint8_t page){
  if (page < 8){
    i2c.beginTransmission(SH1106_ADDR);      // send page
    i2c.write(0x00);
    i2c.write(0xB0+page);
    i2c.endTransmission();
  }
}


void SH1106_monoDisplay::setColumnAddress(uint8_t col){
  uint8_t col_LOW = (uint8_t)(col&0x0F);
  uint8_t col_HIGH = (uint8_t)(((col>>4)&0x0F) + 0x10);
  i2c.beginTransmission(SH1106_ADDR);        // send column
  i2c.write(0x80);
  i2c.write(col_LOW);
  i2c.write(0x00);
  i2c.write(col_HIGH);
  i2c.endTransmission();
}


void SH1106_monoDisplay::writeRamData(byte data){
  i2c.beginTransmission(SH1106_ADDR);  
  i2c.write(0x40);
  i2c.write(data);
  i2c.endTransmission();
}


void SH1106_monoDisplay::sendCommand(byte command){
  /// sends a single command
  i2c.beginTransmission(SH1106_ADDR);      
  i2c.write(0x00);
  i2c.write(command);
  i2c.endTransmission();
}


byte SH1106_monoDisplay::readRamData(){
  byte data = 0;
  i2c.beginTransmission(SH1106_ADDR);  
  i2c.write(0x40);                   //01000000 - ram opperation command
  i2c.endTransmission();
  i2c.requestFrom(SH1106_ADDR, (uint8_t)1);
  if (i2c.available())
  { 
    // read one byte 
    data = i2c.read();
  }
  //i2c.endTransmission();
  return data;
}


void SH1106_monoDisplay::startReadModify(){
  /* - enter read modify mode - only write commands are incremental
     - must be followed by end read modify command
     - usual operation:
              - set address;
              - startReadModify();
         ->   - dummy read;
        |     - read;
         ---  - write;
              - endReadModify();
  */
  sendCommand(0xE0);
}


void SH1106_monoDisplay::endReadModify(){
  sendCommand(0xEE);
}


void SH1106_monoDisplay::writePixel(uint8_t x, uint8_t y){
  // write a pixel at the exact (x,y) position
  // (0,0) position is at the top left
  
  byte ram_data;
  uint8_t page = y/8;
  uint8_t pixel_page_pos = y%8;
  setPageAddress(page);
  setColumnAddress(x);
  startReadModify();
  readRamData();              // dummy read
  ram_data = readRamData();
  writeRamData((0x01<<pixel_page_pos)|ram_data);
  endReadModify();
}


void SH1106_monoDisplay::setDC_ON_OFF(){
  // set DC pump on/off
  i2c.beginTransmission(SH1106_ADDR);  
  i2c.write(0x80);
  i2c.write(0xAE);        // display off
  i2c.write(0x80);
  i2c.write(0xAD);        // DC-DC mode
  i2c.write(0x80);
  i2c.write(0x8B);        // DC off
  delay(20);
  i2c.write(0x80);
  i2c.write(0x8A);        // DC on
  i2c.write(0x00);
  i2c.write(0xAF);        // display on
  i2c.endTransmission();
}


void SH1106_monoDisplay::setReverseDisplay(bool reverse){
  if (reverse == true)
    sendCommand(0xA5);
  else
    sendCommand(0xA4);
}


void SH1106_monoDisplay::setDisplayOff(bool onOff){
  if (onOff == true)
    sendCommand(0xAE);     // display off
  else
    sendCommand(0xAF);     // display on
}


void SH1106_monoDisplay::flipHorizontal(bool remap){
  //revert screen left->right
       
  if (remap == true)
    sendCommand(0xA1);  //setSegmentRemap
  else
    sendCommand(0xA0);
}


void SH1106_monoDisplay::flipVertical(bool flip){ 
  //revert screen left->right
      
  if (flip == true)
    sendCommand(0xC8);   //setSegmentRemap reverse
  else
    sendCommand(0xC0);
}


void SH1106_monoDisplay::blinkDisplay(uint8_t timeOn_ms, uint8_t timeOff_ms){
  /// timeOn_ms time on in miliseconds
  setDisplayOff(true);
  delay(timeOn_ms);
  setDisplayOff(false);
  delay(timeOff_ms);
}


void SH1106_monoDisplay::clearScreen(){
  for(uint8_t i=0;i<8;i++){
    setPageAddress(i);
    setColumnAddress(0);
    for(uint8_t j=0;j<132;j++)
      writeRamData(0x00);
  }
}


void SH1106_monoDisplay::clearPage(uint8_t col, uint8_t page){
  setPageAddress(page);
  setColumnAddress(col);
  for(uint8_t j=col;j<132;j++)
    writeRamData(0x00);
}


void SH1106_monoDisplay::clearRow(int8_t col, uint8_t page, uint8_t len){
  //clears one page row from the coordinated for the specified length
  
  setPageAddress(page);
  setColumnAddress(col);
  for(uint8_t j=col;j<col+len;j++)
    writeRamData(0x00);
}


void SH1106_monoDisplay::drawBar(uint8_t page, uint8_t col, uint8_t offset, uint8_t len, int drawDelay ){
  // draw a progress bar with the specified delay
  setPageAddress(page);
  setColumnAddress(col+offset);
  for(uint8_t j=0;j<len;j++){
    writeRamData(0xFF);
    delay(drawDelay);
  }
}


void SH1106_monoDisplay::drawBar16(uint8_t page, uint8_t col, uint8_t len, int drawDelay ){
  // draw a progress bar on two pages (rows) with the specified delay
  setPageAddress(page);
  setColumnAddress(col);
  for(uint8_t j=0;j<len;j++){
    setPageAddress(page);
    setColumnAddress(col);
    writeRamData(0xFF);
    setPageAddress(page+1);
    setColumnAddress(col);
    writeRamData(0xFF);
    col++;
    delay(drawDelay);
  }
}


void SH1106_monoDisplay::drawText(uint8_t col, uint8_t page, String text){
  //drop end line character - "\0"
  uint8_t maxLen = text.length();       
  //clearPage(page,col);
  setPageAddress(page);
  setColumnAddress(col);
  for(uint8_t i=0;i<maxLen;i++ ){
    for(uint8_t j =0;j<5;j++)
      writeRamData(pgm_read_word_near(&(Terminal6x8[int(text[i])-32][j])));
  }
}


void SH1106_monoDisplay::drawText16(uint8_t col, uint8_t page, String text){
  //drop end line character - "\0"
  uint8_t maxLen = text.length();   
  uint8_t i=0;
  uint8_t j=0;
  uint8_t l=0;
  //clearPage(page,col);
  //clearPage(page+1,col);
  setPageAddress(page);
  setColumnAddress(col);
  for(l=0;l<maxLen;l++){
    i = uint8_t(text[l])-32;
    for(j =0;j<22;j=j+2){
      setPageAddress(page);
      setColumnAddress(col);
      writeRamData(pgm_read_word_near(&Terminal11x16[i][j]));
      setPageAddress(page+1);
      setColumnAddress(col);
      writeRamData(pgm_read_word_near(&Terminal11x16[i][j+1]));
      col++;
    } 
  }
}


void SH1106_monoDisplay::drawImage(uint8_t col, uint8_t page, uint8_t width, uint8_t height,const byte* img ){
  // height is in bytes 
  // width is in bits
  uint8_t i=0;
  
  for(uint8_t w=0; w<width ; w++)
  {
    for(uint8_t h =0; h<height ; h++)
    {
      setColumnAddress(col+w);
      setPageAddress(page-h);
      writeRamData(0x00); 
      
      setColumnAddress(col+w);
      setPageAddress(page-h);
      writeRamData(img[i]);
      i++;
     }
  }
}


void SH1106_monoDisplay::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
  // bresenham algorithm
  // works for vertical and horizontal and less than 45 -> m < 1
  // TO BE IMPROVED
  
  uint8_t dx, dy, sx, sy, e2, err;
  dx = abs(x1 - x0);
  dy = abs(y1 - y0);

  if (x0 < x1) sx = 1; else sx = -1;
  if (y0 < y1) sy = 1; else sy = -1;
  err = dx - dy;
  while(1){
    writePixel(x0,y0);
    if (x0==x1 && y0==y1) return;
    e2 = err<<1;
    if (e2 > -dy){ err = err - dy; x0 = x0 + sx;}
    if (e2 < dx) { err = err + dx; y0 = y0 + sy;}
  }
}
