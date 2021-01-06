#include "SH1106_monoDisplay.h"

SH1106_monoDisplay::SH1106_monoDisplay(byte DEV_ADDR){
  Wire.begin();
  _ADDR = DEV_ADDR;
}

SH1106_monoDisplay::SH1106_monoDisplay(byte DEV_ADDR,uint8_t sda, uint8_t scl){
  Wire.begin(sda,scl);
  _ADDR = DEV_ADDR;
}


void SH1106_monoDisplay::setPageAddress(uint8_t page){
  if (page < 8){
    Wire.beginTransmission(_ADDR);      // send page
    Wire.write(0x00);
    Wire.write(0xB0+page);
    Wire.endTransmission();
  }
}

void SH1106_monoDisplay::setColumnAddress(uint8_t col){
  uint8_t col_LOW = (uint8_t)(col&0x0F);
  uint8_t col_HIGH = (uint8_t)(((col>>4)&0x0F) + 0x10);
  Wire.beginTransmission(_ADDR);        // send column
  Wire.write(0x80);
  Wire.write(col_LOW);
  Wire.write(0x00);
  Wire.write(col_HIGH);
  Wire.endTransmission();
}

void SH1106_monoDisplay::writeRamData(byte data){
  Wire.beginTransmission(_ADDR);  
  Wire.write(0x40);
  Wire.write(data);
  Wire.endTransmission();
}

void SH1106_monoDisplay::sendCommand(byte command){                  //sends single command
  Wire.beginTransmission(_ADDR);      
  Wire.write(0x00);
  Wire.write(command);
  Wire.endTransmission();
}

byte SH1106_monoDisplay::readRamData(){
  Wire.beginTransmission(_ADDR);  
  Wire.write(0x40);                   //01000000 - ram opperation command
  Wire.endTransmission();
  Wire.requestFrom(_ADDR,1);
  //Wire.read();
  byte data = Wire.read();            // read one byte
  Wire.endTransmission();
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
  Wire.beginTransmission(_ADDR);  
  Wire.write(0x80);
  Wire.write(0xAE);        // display off
  Wire.write(0x80);
  Wire.write(0xAD);        // DC-DC mode
  Wire.write(0x80);
  Wire.write(0x8B);      // DC off
  delay(20);
  Wire.write(0x80);
  Wire.write(0x8A);      // DC on
  Wire.write(0x00);
  Wire.write(0xAF);        // display on
  Wire.endTransmission();
}

void SH1106_monoDisplay::setReverseDisplay(bool reverse){
  if (reverse == true)
    sendCommand(0xA5);
  else
    sendCommand(0xA4);
}

void SH1106_monoDisplay::setDisplayOff(bool off){
  if (off == true)
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

void SH1106_monoDisplay::blinkDisplay(uint8_t del1ms, uint8_t del2ms){
  setDisplayOff(true);
  delay(del1ms);
  setDisplayOff(false);
  delay(del2ms);
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

void SH1106_monoDisplay::drawBar(uint8_t page, uint8_t col, uint8_t offset, uint8_t len, int del ){
  // draw a progress bar with the specified delay
  
  setPageAddress(page);
  setColumnAddress(col+offset);
  for(uint8_t j=0;j<len;j++){
    writeRamData(0xFF);
    delay(del);
  }
}

void SH1106_monoDisplay::drawBar16(uint8_t page, uint8_t col, uint8_t len, int del ){
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
    delay(del);
  }
}

void SH1106_monoDisplay::drawText(uint8_t col, uint8_t page, String text){
  uint8_t maxLen = text.length();       //drop end line character - \0
  //clearPage(page,col);
  setPageAddress(page);
  setColumnAddress(col);
  for(uint8_t i=0;i<maxLen;i++ ){
    for(uint8_t j =0;j<5;j++)
      writeRamData(pgm_read_word_near(&(Terminal6x8[int(text[i])-32][j])));
  }
}

void SH1106_monoDisplay::drawText16(uint8_t col, uint8_t page, String text){
  uint8_t maxLen = text.length();   //drop end line character - \0
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
  
  for(uint8_t w=0; w<width ; w++){
    for(uint8_t h =0; h<height ; h++){
      
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
  // to be improved
  
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
