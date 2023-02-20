/**************************************************************************/
/*
    Driver for st7735 128x160 pixel TFT LCD displays.
    
    This driver uses a bit-banged SPI interface and a 16-bit RGB565
    colour palette.
*/
/**************************************************************************/
#include "st7735.h"
#include "timer.h"	
#include <stdio.h> 
#include <string.h>
#include "get_image.h"
#include "insert.h"
#include "tof.h"
#include "softI2C.h"
#include "LcdBmpPictures.h"	
#include "protocol.h"
#include "menu.h"
#include "my_adc.h"

const unsigned char * GlobalFont = Arial_22x23;  
unsigned int Paint_Color = WHITE;
unsigned int Back_Color = WHITE;
unsigned char Lcd_buf[ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT];
unsigned char buf2[20480];


void WriteCmd(uint8_t command) 
{  
	uint8_t i = 0;
  CLR_CS;
  CLR_RS;

  for (i=0; i<8; i++) 
  { 
    if (command & 0x80) {SET_SDA;} 
    else{CLR_SDA;} 
		
    CLR_SCL; 
    command <<= 1; 
    SET_SCL;
  } 
//	SPI1_Send (command);
  SET_CS; 
}

/*************************************************/
void WriteData(uint8_t data)
{  
	//uint8_t i = 0;
  CLR_CS;
  SET_RS; 

	if (data & 0x80) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x40) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x20) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x10) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x08) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x04) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x02) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	if (data & 0x01) {SET_SDA;}  else{CLR_SDA;} 
	CLR_SCL; SET_SCL;
	
//	SPI1_Send (data);
  SET_CS;
}


/*************************************************/
void st7735SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  WriteCmd(ST7735_CASET);   // column addr set
  WriteData(0x00);
  WriteData(x0);          // XSTART
  WriteData(0x00);
  WriteData(x1);          // XEND

  WriteCmd(ST7735_RASET);   // row addr set
  WriteData(0x00);
  WriteData(y0);          // YSTART
  WriteData(0x00);
  WriteData(y1);          // YEND
}


void Init_ILI9163V(void)
{
	delay_ms(1);
	
	WriteCmd(0x11);  //Exit Sleep
	delay_us(20);
	WriteCmd(0x26);  //Set Default Gamma// Gammut format
	WriteData(0x04);

	WriteCmd(0xB1);  //Set Frame Rate    
	WriteData(0x0b);
	WriteData(0x14);

	WriteCmd(0xC0);  //Power_Control 1     //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	WriteData(0x08);
	WriteData(0x05);

	WriteCmd(0xC1);  //Power_Control 2     //Set BT[2:0] for AVDD & VCL & VGH & VGL
	WriteData(0x02);

	WriteCmd(0xC5);  //Power_Control 3    //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	WriteData(0x44);
	WriteData(0x48);

	WriteCmd(0xC7);  // Set VMF        
	WriteData(0xc2);

	WriteCmd(0x3A);  // set color mode   Interface Pixel Format 
	WriteData(0x05); // 16-bit color

	WriteCmd(0x2A);  //Set Column Address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x7F);

	WriteCmd(0x2B);  //Set Page Address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x9F);
	
	WriteCmd(0x36);	 //Memory Access Control     
	WriteData(0xC8);  //!

	WriteCmd(0xB7);  // Source Driver Direction
	WriteData(0x00);

	WriteCmd(0xF2); //Enable Gamma bit
	WriteData(0x01);
	
	WriteCmd(0xE0); //Positive Gamma Correction Setting
	WriteData(0x3F);//p1
	WriteData(0x25);//p2
	WriteData(0x21);//p3
	WriteData(0x24);//p4
	WriteData(0x1d);//p5
	WriteData(0x0d);//p6
	WriteData(0x4c);//p7
	WriteData(0xB8);//p8
	WriteData(0x38);//p9
	WriteData(0x17);//p10
	WriteData(0x0f);//p11
	WriteData(0x08);//p12
	WriteData(0x04);//p13
	WriteData(0x02);//p14
	WriteData(0x00);//p15
	
	WriteCmd(0xE1); //Negative Gamma Correction Setting
	WriteData(0x00);//p1
	WriteData(0x1a);//p2
	WriteData(0x1e);//p3
	WriteData(0x0B);//p4
	WriteData(0x12);//p5
	WriteData(0x12);//p6
	WriteData(0x33);//p7
	WriteData(0x47);//p8
	WriteData(0x47);//p9
	WriteData(0x08);//p10
	WriteData(0x20);//p11
	WriteData(0x27);//p12
	WriteData(0x3c);//p13
	WriteData(0x3d);//p14
	WriteData(0x3F);//p15

	WriteCmd(0x29); // Display On
	//WriteCmd(0x21); // Display inversion on
}


/*************************************************/
/* Public Methods                                */
/*************************************************/

/*************************************************/
void lcdInit(void)
{
	CLR_RS;
	CLR_SDA;
	CLR_SCL;
	CLR_CS;
	
  CLR_RES;
  delay_us(100);
  SET_RES;
  delay_us(100);

	Init_ILI9163V();	// Run LCD init sequence
  lcdSetOrientation(0);
  lcdFillRGB(YELLOW);
	LCD_CleanTerminal();
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	x=y*ST7735_PANEL_WIDTH+x;
	Lcd_buf[x]= (unsigned char) color;	
	
}


void LCD_Refresh(void)
{  	
	int x, y, z;
	
  st7735SetAddrWindow(0, 0, lcdGetWidth() - 1, lcdGetHeight() - 1);
  WriteCmd(ST7735_RAMWR);  // write to RAM
	z = (ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT);
  for (x=z-1; x!=0; x--) {
			y=FindColor(Lcd_buf[x]);
      WriteData(y >> 8);    
      WriteData(y);     
  }
  WriteCmd(ST7735_NOP);
}


// pictures were converted by https://littlevgl.com/image-to-c-array
void LCD_DrawBMP(const char* buf, int x0, int y0, int w, int h)
{  	
	int x, y;
	
  st7735SetAddrWindow(x0+0, y0+0, x0+w-1, y0+h-1);
  WriteCmd(ST7735_RAMWR);  // write to RAM
  for (x=0; x < (w*h*2); x++) {
		y =  buf[x]; 
    WriteData(y);     
  }
  WriteCmd(ST7735_NOP);
}


/*************************************************/
void lcdFillRGB(uint16_t color)
{
  uint16_t x;
	uint16_t color2;
	
	color2=color >> 8;
  st7735SetAddrWindow(0, 0, lcdGetWidth() - 1, lcdGetHeight() - 1);
  WriteCmd(ST7735_RAMWR);  // write to RAM
  for (x=0; x < ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT; x++) {
		WriteData(color2);    
		WriteData(color);    
  }
  WriteCmd(ST7735_NOP);
}


/*************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  uint16_t x, pixels;

  if (x1 < x0) {
		x = x1;
		x1 = x0;
		x0 = x;
  }

  if (x1 >= 160) {	// Check limits
    x1 = 159;
  }
  if (x0 >= 160) {
    x0 = 159;
  }
	
	for (pixels = x0; pixels < x1+1; pixels++) {
		lcdDrawPixel(pixels,y,color);
	}
}

/*************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  uint16_t y, pixels;

	if (y1 < y0) {
		y = y1;
		y1 = y0;
		y0 = y;
	}

  if (y1 >= 128) {	// Check limits
    y1 = 127;
  }
  if (y0 >= 128) {
    y0 = 127;
  }
	for (pixels = y0; pixels < y1 + 1; pixels++) {
		lcdDrawPixel(x,pixels,color);
	}
}


void LcdDrawRectangle(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1,uint16_t color)
{
	uint16_t x,y;
	
	if(x0>x1) { x=x0;	x0=x1; x1=x; }
	if(y0>y1) { y=y0;	y0=y1; y1=y; }
	if(x0>=160) { x0 = 159;}
	if(x1>=160) { x1 = 159;}
	if(y0>=128) { y0 = 127;}
	if(y1>=128) { y1 = 127;}
	
	for(x=x0;x<=x1;x++) {
		for(y=y0;y<y1;y++) {
			lcdDrawPixel(x,y,color);
		}
	}
}


void LcdDrawGraphSimple(unsigned int *buf, unsigned int color)
{
	unsigned int i;
	signed int tmp;
	for(i=0;i<22;i++) {
		tmp=124-buf[i]/1;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(4+7*i,10+7*i,124,tmp,color);
	}
}


void LcdDrawGraph(unsigned int *bufLow,unsigned int *bufMiddle, unsigned int *bufHigh)
{
	unsigned int i;
	signed int tmp;
	
	for(i=0;i<22;i++) {
		tmp=124-bufMiddle[i]/8;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(4+7*i,10+7*i,124,tmp,blue);
		
		tmp=124-bufLow[i]/8;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(5+7*i,9+7*i,tmp,tmp+2,green);
		
		tmp=124-bufHigh[i]/8;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(5+7*i,9+7*i,tmp,tmp+2,red);
	}
}


void LcdDrawUvGraph(unsigned int Low,unsigned int Middle, unsigned int High)
{
	signed int tmp;
	
	tmp=124-Middle/8;
	if(tmp < 0) { tmp = 0;}
	LcdDrawRectangle(70,90,124,tmp,blue);
	
	tmp=124-Low/8;
	if(tmp < 0) { tmp = 0;}
	LcdDrawRectangle(70,90,tmp,tmp+2,green);
	
	tmp=124-High/8;
	if(tmp < 0) { tmp = 0;}
	LcdDrawRectangle(70,90,tmp,tmp+2,red);
}


void LcdDrawASGraph(unsigned int left,unsigned int right)
{
	signed int tmp;
	
	tmp=123-left/4;
	if(tmp < 24) { tmp = 24;}
	LcdDrawRectangle(40,60,124,tmp,blue);
	
	tmp=123-right/4;
	if(tmp < 24) { tmp = 24;}
	LcdDrawRectangle(100,120,124,tmp,blue);
}


void LcdDrawMgGraph(int *buf, int low, int high)
{
	signed int tmp;
	int i;
	
	for(i=0;i<6;i++) {
		tmp=123-buf[i]/3;
		if(tmp < 24) { tmp = 24;}
		LcdDrawRectangle(20+20*i,38+20*i,124,tmp,blue);
		
		tmp=124-high/3;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(10,150,tmp,tmp+2,red);
		
		tmp=124-low/3;
		if(tmp < 0) { tmp = 0;}
		LcdDrawRectangle(10,150,tmp,tmp+2,green);
	}
}


/*************************************************/
void lcdSetOrientation(unsigned char orientation)
{
  if(orientation == 1)
  {
	  WriteCmd(ST7735_MADCTL);  // Memory Data Access Control
	  WriteData(0x00);          // 000 - Normal
	                                  // 0 - Vertical refresh top to bottom
	                                  // 0 - RGB order
	                                  // 0 - Horizontal refresh left to right
  }
  else if (orientation == 0)
  {
	  WriteCmd(ST7735_MADCTL);  // Memory Data Access Control
	  WriteData(0xA0);          // 101 - X-Y Exchange, Y-Mirror
	                                  // 0 - Vertical refresh top to bottom
	                                  // 0 - RGB order
	                                  // 0 - Horizontal refresh left to right
  }
}


/*************************************************/
uint16_t lcdGetWidth(void)
{
  return ST7735_PANEL_WIDTH;
}

/*************************************************/
uint16_t lcdGetHeight(void)
{
  return ST7735_PANEL_HEIGHT;
}


//***********************************************//
void LCD_SetFont(const unsigned char * font, uint32_t color)
{
 	GlobalFont=font;
	Paint_Color = color;
}


//***********************************************//
uint32_t LCD_FastShowChar(uint32_t x,uint32_t y,uint8_t num)
{        
  uint8_t tmp;
	uint16_t tmp2,tmp3,tmp4;
  uint32_t dy,i;
	uint32_t tmpMaxWidth = 0, maxWidth = 0;
	uint32_t symbolHeight, symbolLeghth, symbolByteWidth;
	 
    if(x>ST7735_PANEL_WIDTH||y>ST7735_PANEL_HEIGHT)return 0;

	if (num == ' ') { return 10; }		  	// special case - " " symbol doesn't have any width
	if (num < ' ') return 0;	            					
	else num=num-' ';

	symbolByteWidth = GlobalFont[0];	 	
	symbolHeight =  GlobalFont[2];
	symbolLeghth = (GlobalFont[0])*(GlobalFont[2]);
	//symbolLeghth =  GlobalFont[3];

 	for(dy=0;dy<symbolHeight;dy++) 
	{
		tmp4=y+dy;
		for(i=0;i<symbolByteWidth;i++)
		{
			tmp2=i*8;
			tmp3=x+i*8;
			tmp = GlobalFont[num*symbolLeghth + dy*symbolByteWidth+i]; 
			
			if (tmp&0x80) {lcdDrawPixel(tmp3+0,tmp4,Paint_Color); tmpMaxWidth = tmp2+1; }
			if (tmp&0x40) {lcdDrawPixel(tmp3+1,tmp4,Paint_Color); tmpMaxWidth = tmp2+2; }
			if (tmp&0x20) {lcdDrawPixel(tmp3+2,tmp4,Paint_Color); tmpMaxWidth = tmp2+3; }
			if (tmp&0x10) {lcdDrawPixel(tmp3+3,tmp4,Paint_Color); tmpMaxWidth = tmp2+4; }
			if (tmp&0x08) {lcdDrawPixel(tmp3+4,tmp4,Paint_Color); tmpMaxWidth = tmp2+5; }
			if (tmp&0x04) {lcdDrawPixel(tmp3+5,tmp4,Paint_Color); tmpMaxWidth = tmp2+6; }
			if (tmp&0x02) {lcdDrawPixel(tmp3+6,tmp4,Paint_Color); tmpMaxWidth = tmp2+7; }
			if (tmp&0x01) {lcdDrawPixel(tmp3+7,tmp4,Paint_Color); tmpMaxWidth = tmp2+8; }

			if (tmpMaxWidth > maxWidth) { maxWidth = tmpMaxWidth; }
		}
 	}
	return (maxWidth+maxWidth/8+1);
}

uint32_t LCD_GetCharWidth(uint32_t y,uint8_t num)
{        
  uint8_t tmp;
	uint16_t tmp2;
  uint32_t dy,i;
	uint32_t tmpMaxWidth = 0, maxWidth = 0;
	uint32_t symbolHeight, symbolLeghth, symbolByteWidth;
	 
	if(y>ST7735_PANEL_HEIGHT)return 0;

	if (num == ' ') { return 10; }		  	// special case - " " symbol doesn't have any width
	if (num < ' ') return 0;	            					
	else num=num-' ';

	symbolByteWidth = GlobalFont[0];	 	
	symbolHeight =  GlobalFont[2];
	symbolLeghth = (GlobalFont[0])*(GlobalFont[2]);
	//symbolLeghth =  GlobalFont[3];

 	for(dy=0;dy<symbolHeight;dy++) 
	{
		for(i=0;i<symbolByteWidth;i++)
		{
			tmp2=i*8;
			tmp = GlobalFont[num*symbolLeghth + dy*symbolByteWidth+i]; 
			
			if (tmp&0x80) {tmpMaxWidth = tmp2+1; }
			if (tmp&0x40) {tmpMaxWidth = tmp2+2; }
			if (tmp&0x20) {tmpMaxWidth = tmp2+3; }
			if (tmp&0x10) {tmpMaxWidth = tmp2+4; }
			if (tmp&0x08) {tmpMaxWidth = tmp2+5; }
			if (tmp&0x04) {tmpMaxWidth = tmp2+6; }
			if (tmp&0x02) {tmpMaxWidth = tmp2+7; }
			if (tmp&0x01) {tmpMaxWidth = tmp2+8; }

			if (tmpMaxWidth > maxWidth) { maxWidth = tmpMaxWidth; }
		}
 	}
	return (maxWidth+maxWidth/8+1);
}

/*
void LCD_ShowStringCentered(uint16_t y, char *p)
{
	int x,x1;
	char *p1;
	
	p1 = p;	
	while(*p!='\0')	{       
		if(x>=ST7735_PANEL_WIDTH){ x=0; y=y+GlobalFont[2]-1; }
		if(y>=ST7735_PANEL_HEIGHT){y=x=0;}
		x+=LCD_GetCharWidth(y,*p);
		p++;
	} 
	x1 = 80-x/2;
	if(x1<0) {x1 = 0;}
	LCD_ShowString(x1,y,p1);
}
*/

void LCD_ShowString(uint16_t x,uint16_t y, char *p)
{         
	while(*p!='\0')	{       
		if(x>=ST7735_PANEL_WIDTH){ x=0; y=y+GlobalFont[2]-1; }
		if(y>=ST7735_PANEL_HEIGHT){y=x=0;}
		x+=LCD_FastShowChar(x,y,*p);
		p++;
	}  
}


void LCD_ShowStringSize(uint16_t x,uint16_t y, char *p,unsigned int size)
{         
	while(size--)	{       
		if(x>=ST7735_PANEL_WIDTH){ x=0; y=y+GlobalFont[2]-1; }
		if(y>=ST7735_PANEL_HEIGHT){y=x=0;}
		x+=LCD_FastShowChar(x,y,*p);
		p++;
	}  
}


uint16_t FindColor (unsigned char color)
{
	if(color==black){return BLACK;}
	if(color==white){return WHITE;}	
	if(color==green){return GREEN;}	
	if(color==red){return RED;}	
	if(color==blue){return BLUE;}
	if(color==yellow){return YELLOW;}
	if(color==grey){return GRAY;}
	return 0;
}


void ClearLcdMemory(void)
{
	int tmp=0;
	for(tmp=0;tmp<20480;tmp++)
	{
		Lcd_buf[tmp]=0xFF;
	}
}
	

void ShowAdcChannel(int channel, char* buf)
{
	int tmp;
	char string[50];
	
	ADC1_ChannelSelect(channel);
	tmp=ADC1_getValue();
	sprintf(string,"%s=%d", buf, tmp);
	LCD_ShowString(0,channel*10,string);
}


void LCD_ShowVoltages(void)
{
	char string[50];
	unsigned int y=0;
	
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);

	sprintf(string,"IN_3_3V=%d", avg.in_3_3v);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_DRV_MAIN=%d", avg.in_drv_main);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_DRV_BAT=%d", avg.in_drv_bat);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_INVERTOR=%d", avg.in_invertor);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_VBAT=%d", avg.in_vbat);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_PRINTER=%d", avg.in_printer);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_12V=%d", avg.in_12v);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_5V=%d", avg.in_5v);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"OUT_24V=%d", avg.out_24v);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_24V=%d", avg.in_24v);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_LED=%d", avg.in_led);
	LCD_ShowString(0,y,string);
	y+=10;
	
	sprintf(string,"IN_BUTTON=%d", avg.in_button);
	LCD_ShowString(0,y,string);

	LCD_Refresh();
}
	

#define ROW_NUMBER 9
char strArray[ROW_NUMBER][30];


void LCD_CleanTerminal(void)
{
	int i;
	
	for(i=0;i<ROW_NUMBER;i++) {
		strArray[i][0]=0;
	}
}


void LCD_ShowTerminal(void)
{
	int i,y=0;
	
	ClearLcdMemory();
	for(i=0;i<ROW_NUMBER;i++) {
		LCD_ShowString(0,y,strArray[i]);
		y+=13;
	}
	LCD_Refresh();
}

	
void LCD_TerminalAddString(char *newStr)
{
	int i,n=0;

	newStr[24]=0;			// stings must be no longer than 24 characters to fit the LCD width
	
	LCD_SetFont(Arial_15x17,black);
	for(i=0;i<ROW_NUMBER;i++) {
		if(strArray[i][0]==0) {
			strcpy(strArray[i],newStr);
			break;
		}
		n++;
	}
	if(n==ROW_NUMBER) {
		for(i=0;i<(ROW_NUMBER-1);i++) {
			strcpy(strArray[i],strArray[i+1]);
		}
		strcpy(strArray[ROW_NUMBER-1],newStr);
	}
	LCD_ShowTerminal();
}
