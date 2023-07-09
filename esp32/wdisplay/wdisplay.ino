/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
//#include "imagedata.h"
#include <stdlib.h>


//new includes
#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
//#include "SPIFFS.h"
#include "LITTLEFS.h" // LittleFS is declared
#define SPIFFS LITTLEFS

/***** Settings ******/
const char* ssid = "wdisplay";
const char* password = "wdisplay";

uint8_t MAC_array[6];
char MAC_char[18];

String serverName = "http://10.0.13.23:80/getpicture";

//#define FORMAT_SPIFFS_IF_FAILED true
#define FORMAT_LITTLEFS_IF_FAILED true


/* Entry point ----------------------------------------------------------------*/
void setup()
{
    
  //printf(“Free PSRAM : %d\n”, ESP.getFreePsram());
  initWiFi();
  printf("\r\n");
  
	printf("EPD_2in13_V3_test Demo\r\n");
	DEV_Module_Init();

	printf("e-Paper Init and Clear...\r\n");
	EPD_2in13_V3_Init();
	EPD_2in13_V3_Clear();
  

	//Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) 
	{
		printf("Failed to apply for black memory...\r\n");
		while (1);
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);

  //show image for array    
	Paint_SelectImage(BlackImage);
	Paint_Clear(WHITE);
	//Paint_DrawBitMap(gImage_2in13);


    if(!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

  printf("display 2nd picutre from filesystem...");
  String filename = "/picture";
  File x = SPIFFS.open(filename , "r");
      if (!x)
    {
      printf("file open failed\r\n");
      //return false;
    }else{
          printf("file open OK\r\n");
    }

  size_t filesize = x.size(); //the size of the file in bytes  
  printf("Size of the file itself: %i\n", filesize);
    x.close();


//unsigned char wbuffer[512] = {0x65, 0x66, 0x67, 0x68, 0x00, 0x65, 0x66, 0x00, 0x00, 0xff, 0x05, 0x11, 0x55};
unsigned char wbuffer[2048] =  { /* 0X00,0X01,0X80,0X00,0XFA,0X00, */
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X38,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X00,0X00,0X78,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0X80,0X00,0XF8,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XC0,0X03,0XF8,0X00,0X00,0X00,
0X00,0X00,0X00,0X01,0X00,0X00,0X00,0X00,0X01,0XFF,0XE0,0X1F,0XF8,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0XC0,0X00,0X00,0X00,0X03,0XFF,0X70,0X1F,0XF0,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,0X03,0XFF,0X78,0X38,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X3F,0X00,0X00,0X00,0X03,0XFF,0XF8,0XF0,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X0F,0XC0,0X00,0X00,0X03,0XF0,0XF8,0XC1,0XF8,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X03,0XF0,0X00,0X00,0X03,0XF3,0XF3,0X8F,0XFE,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X01,0XF8,0X00,0X00,0X03,0XFF,0XF3,0XBF,0XFF,0XC0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X3E,0X00,0X00,0X03,0XFF,0XFA,0X7F,0XFF,0XF0,0X00,0X00,
0X00,0X0F,0X00,0X00,0X00,0X3F,0XE0,0X00,0X00,0XE7,0XFB,0XFF,0XFF,0XF0,0X00,0X00,
0X00,0X07,0X00,0X00,0X00,0X3F,0XF0,0X00,0X00,0XE7,0XFB,0XFF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X1F,0XFC,0X00,0X00,0XFF,0XFB,0XFF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X0F,0XFE,0X00,0X00,0X7F,0XFB,0XCF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X03,0XFE,0X00,0X00,0X3F,0XF9,0XCF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X0F,0XF1,0XFF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0XFF,0XC0,0X00,0X00,0X3D,0XEF,0XFF,0XE0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X7F,0XE0,0X00,0X03,0XFF,0XF7,0XFF,0XC0,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X3F,0XE0,0X00,0X3F,0XFF,0XFF,0XFF,0X80,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X1F,0XE0,0X00,0X7E,0XFF,0XFF,0XFF,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X0F,0XE0,0X00,0X3E,0XFF,0XF1,0XFF,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X0F,0XE0,0X00,0X3F,0XFF,0XBF,0XFC,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X3E,0X7F,0X3F,0XF8,0X38,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X7E,0X7F,0XC0,0X00,0X7C,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X7E,0X3F,0XC0,0X01,0XFE,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X7F,0X1F,0XDF,0XFF,0XFF,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X00,0X7F,0X87,0XEF,0XFF,0XFF,0X80,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X00,0X7F,0XE3,0XF1,0XFF,0XFF,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X01,0X87,0XFC,0X78,0XFE,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X03,0X01,0XF8,0XFF,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X0E,0X00,0X01,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X0C,0X00,0X03,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X1C,0X00,0X0F,0X98,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X38,0X00,0X7F,0X9C,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X00,0X00,0X03,0XF0,0X30,0X00,0X3F,0XDC,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X07,0XFE,0X07,0XE0,0X30,0X00,0X1F,0XFE,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X0F,0XFF,0X87,0XF0,0X70,0X00,0X1F,0XFC,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X1F,0XFF,0XC7,0XF0,0X00,0X00,0X0F,0XFE,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X3F,0XFF,0XE7,0XE0,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X7F,0XFF,0XF3,0XE0,0X7F,0XFF,0XEF,0XE0,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0XFF,0X9F,0XFB,0XCF,0XFF,0XFF,0XC6,0X18,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X03,0XFF,0XCF,0XF9,0XDF,0XFF,0XFF,0XC0,0X08,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X0E,0XFF,0XDF,0XFD,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X01,0XF0,0XFF,0XFF,0XFD,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X03,0XF8,0XFF,0XFF,0XFD,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X07,0XFC,0XFF,0XDF,0XFD,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X0F,0XFE,0XC7,0XDF,0XFD,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X3E,0X7F,0XF7,0XDF,0XFF,0XFE,0X30,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X3E,0X7F,0XF7,0XDF,0XCF,0XF0,0X38,0X00,0X06,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X7E,0X7F,0XFF,0XFF,0X9F,0XE0,0X38,0X00,0X1F,0XC0,0X00,0X00,0X00,0X00,
0X00,0X03,0X7F,0X1F,0XFF,0XFE,0X3E,0X00,0X3C,0X00,0X3B,0XE0,0X00,0X00,0X00,0X00,
0X00,0X03,0X7F,0X8F,0XFF,0XFE,0X7C,0X00,0X3C,0X00,0X3B,0XE0,0X00,0X00,0X00,0X00,
0X00,0X03,0X7F,0X87,0XFE,0X3F,0XF8,0X00,0X3C,0X00,0X7F,0XF0,0X00,0X00,0X00,0X00,
0X00,0X03,0X7F,0XA1,0XF8,0X1F,0XFC,0X00,0X1C,0X00,0X7C,0XF0,0X00,0X00,0X00,0X00,
0X00,0X03,0X3F,0XFF,0XF0,0X0F,0XFC,0X00,0X0C,0X00,0X7E,0X7C,0X00,0X00,0X00,0X00,
0X00,0X03,0X3F,0XFF,0XF0,0X0F,0XFE,0X00,0X0C,0X00,0X7F,0X3C,0X00,0X00,0X00,0X00,
0X00,0X03,0X1F,0XFF,0XF0,0X0F,0XFF,0X00,0X0C,0X00,0X3F,0X80,0X00,0X00,0X00,0X00,
0X00,0X03,0X0F,0XFF,0XF0,0X0E,0X7F,0X80,0X0C,0X00,0X3F,0XC0,0X00,0X00,0X00,0X00,
0X00,0X03,0X07,0XFF,0XF8,0X0E,0X3F,0XC0,0X0C,0X00,0X1F,0XF0,0X00,0X00,0X00,0X00,
0X00,0X03,0X03,0XFF,0XF8,0X1E,0X1F,0XE0,0X0C,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X3F,0XFE,0X3F,0X87,0XE0,0X3C,0X00,0X0F,0XBE,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X47,0XFF,0XFF,0XF0,0X60,0X3C,0X00,0X0F,0XBF,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X31,0XFF,0XFF,0XF8,0XE0,0X38,0X00,0X0F,0XBF,0X80,0X00,0X00,0X00,
0X00,0X03,0X06,0X11,0XFF,0XFD,0XFF,0XE0,0X38,0X00,0X0F,0X1F,0X80,0X00,0X00,0X00,
0X00,0X03,0X0E,0X13,0XFF,0XF9,0XFF,0XE0,0X30,0X00,0X0E,0X07,0X80,0X00,0X00,0X00,
0X00,0X03,0X3C,0XD7,0XFD,0XFD,0XFF,0XE0,0X30,0X00,0X1C,0XC7,0X80,0X00,0X00,0X00,
0X00,0X03,0X39,0XEF,0XFB,0XFF,0XFF,0XE0,0X30,0X00,0X3D,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X73,0XEF,0XFB,0XFF,0XFF,0XE0,0X60,0X00,0X3D,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X67,0XEF,0XFB,0XFF,0XFF,0XC0,0XC0,0X00,0X3F,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X8F,0XF7,0XE3,0XFF,0XFF,0X80,0XE0,0X00,0X1B,0XFF,0X80,0X00,0X00,0X00,
0X00,0X03,0X9F,0XF3,0XE1,0XFF,0XE0,0X01,0XC0,0X00,0X3B,0XFF,0XC0,0X00,0X00,0X00,
0X00,0X03,0X3F,0XF1,0XF9,0XFF,0XC0,0X03,0X80,0X00,0X7F,0XFF,0XE0,0X00,0X00,0X00,
0X00,0X03,0X7F,0XF1,0XFD,0XFF,0X80,0X07,0X00,0X00,0X1F,0XFF,0XF8,0X00,0X00,0X00,
0X00,0X03,0XFF,0XF1,0XFF,0XFF,0X80,0X0F,0X00,0X00,0X3F,0XFF,0XFC,0X00,0X00,0X00,
0X00,0X03,0XFF,0XE0,0XFF,0XFF,0X00,0X0E,0X00,0X00,0X3F,0XFF,0X9C,0X00,0X00,0X00,
0X00,0X03,0XFF,0X80,0X0F,0XFF,0X00,0X06,0X00,0X00,0X03,0XF6,0X7C,0X00,0X00,0X00,
0X00,0X03,0XFF,0X00,0X1F,0X7C,0X00,0XF8,0X00,0X00,0X09,0XF0,0X3C,0X00,0X00,0X00,
0X00,0X03,0XFE,0X00,0X3F,0X38,0X01,0XFC,0X00,0X00,0X1C,0XF8,0X1C,0X00,0X00,0X00,
0X00,0X03,0XFC,0X00,0XFF,0X90,0X03,0XFE,0X00,0X00,0X3D,0XFF,0X9C,0X00,0X00,0X00,
0X00,0X03,0XF8,0X00,0XFF,0XC0,0X0F,0XFF,0X00,0X00,0X73,0XFF,0XCC,0X00,0X00,0X00,
0X00,0X03,0XF0,0X00,0XFF,0XE0,0X0F,0XFF,0X00,0X00,0X61,0XFF,0XFC,0X60,0X00,0X00,
0X00,0X03,0XF0,0X01,0XFF,0XF0,0X1F,0XFF,0X00,0X00,0XC0,0XFF,0XFF,0XF0,0X00,0X00,
0X00,0X07,0XE0,0X01,0XFF,0XBF,0XFF,0XFF,0X80,0X00,0XE0,0X00,0X7F,0XF0,0X00,0X00,
0X00,0X03,0XC0,0X01,0XFF,0X3F,0XFF,0XFF,0X80,0X00,0XC0,0X00,0X3F,0XF8,0X00,0X00,
0X00,0X01,0X80,0X01,0XFE,0X7F,0XFF,0XFF,0X80,0X00,0X80,0X00,0X1F,0XFC,0X00,0X00,
0X00,0X07,0X00,0X00,0XFE,0X7F,0XFF,0XFF,0X80,0X00,0X80,0X00,0X0C,0XFC,0X00,0X00,
0X00,0X03,0X00,0X00,0XF0,0X7F,0XFF,0XFF,0X00,0X00,0X80,0X00,0X04,0X7C,0X00,0X00,
0X00,0X03,0X00,0X00,0X7E,0X3F,0XFF,0XFC,0X00,0X01,0X80,0X00,0X03,0XFC,0X00,0X00,
0X00,0X03,0X00,0X00,0X7E,0X3F,0XFF,0XFE,0X00,0X01,0X80,0X00,0X01,0XF0,0X00,0X00,
0X00,0X03,0X00,0X00,0X3F,0XDF,0XBF,0XFF,0X80,0X01,0X80,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X3F,0XCF,0XBF,0XFF,0XC0,0X01,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0X7F,0XCF,0XFF,0XFF,0XE0,0X01,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X00,0XFF,0XCF,0XCF,0XFF,0XC0,0X01,0X80,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X00,0X1E,0XFF,0XEF,0XC7,0XFF,0XC0,0X01,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X01,0XFC,0XFF,0XE7,0XF0,0X7F,0X80,0X03,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X07,0X07,0XC0,0XFF,0XF3,0XF8,0XFF,0X00,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0X0F,0X80,0X7F,0XF3,0XF9,0XFE,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X01,0X1F,0X00,0X3F,0XF3,0XFB,0XFE,0X00,0X0C,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X01,0X38,0X00,0X03,0XF9,0XFB,0XF3,0XE0,0X18,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X01,0XF0,0X00,0X3C,0XF9,0XFB,0XEF,0XF0,0X30,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X01,0XC0,0X00,0X7D,0XF9,0XFB,0XCF,0XF0,0X70,0X00,0X00,0X18,0X00,0X00,0X00,
0X00,0X01,0X80,0X00,0XFF,0XF8,0XFB,0XCF,0XFC,0XC0,0X00,0X03,0XF8,0X00,0X00,0X00,
0X00,0X01,0X80,0X00,0XFF,0XFC,0XFB,0XFF,0XFC,0X00,0X00,0X07,0XF8,0X00,0X00,0X00,
0X00,0X01,0X80,0X00,0XFF,0XDE,0XFB,0XFF,0X7C,0X00,0X00,0X3F,0XFC,0X00,0X00,0X00,
0X00,0X01,0X00,0X00,0XFF,0XDE,0XF3,0XFF,0X3C,0X00,0X00,0X7F,0XFE,0X00,0X00,0X00,
0X00,0X01,0X00,0X00,0XFF,0XC2,0XF3,0XFF,0X1C,0X00,0X00,0XFF,0XFE,0X00,0X00,0X00,
0X00,0X01,0X00,0X00,0XFB,0XE3,0XFF,0XFF,0X3C,0X00,0X00,0XFF,0XFF,0X00,0X00,0X00,
0X00,0X01,0X00,0X00,0XF9,0XEF,0XFF,0XFE,0X3C,0X00,0X00,0XFF,0XFF,0X00,0X00,0X00,
0X00,0X01,0X00,0X00,0XFC,0XFF,0XFF,0XF9,0XFC,0X00,0X00,0XDC,0X7F,0X00,0X00,0X00,};
File file = LITTLEFS.open("/test", FILE_WRITE);
file.write(wbuffer, 2048);
file.close();




//unsigned char rbuffer[2048] = {0x00};
file = LITTLEFS.open("/test", FILE_READ);
file.read(wbuffer, 2048);

file.close();
printf("size of the variable..............: %i\n", sizeof(wbuffer));
//printf("content of the variable..............: \n%s\n", rbuffer);


//memcpy(storedPassword, readFile(SPIFFS, "/inputPassword.txt").c_str(), strlen(readFile(SPIFFS, "/inputPassword.txt").c_str())+1); 

memcpy(BlackImage, wbuffer, 2048); // hat funktioniert zeigt ca. 1/3 vom gesamtbild!
//memcpy(BlackImage, readFile(SPIFFS, "/picture2").c_str(), strlen(readFile(SPIFFS, "/picture2").c_str())+1); 


printf("Size of the blackimage..............: \n%i\n", sizeof(BlackImage));


Paint_DrawBitMap(BlackImage); //funktioniert



	EPD_2in13_V3_Display(BlackImage);
	//DEV_Delay_ms(2000);

	//printf("Goto Sleep...\r\n");
	EPD_2in13_V3_Sleep(); //TODO macht was?
	free(BlackImage); //TODO macht was?
	//BlackImage = NULL;
	DEV_Delay_ms(2000);//important, at least 2s
	// close 5V
	printf("close 5V, Module enters 0 power consumption ...\r\n");

  //SPIFFS.format();

}

/* The main loop -------------------------------------------------------------*/
void loop()
{
/*test*/


 String filename = "/picture";

   
  //SPIFFS.remove(filename);
    //fs::File f = LittleFS.open(filename, "w+");
    //File f = little.open(filename);
    File f = SPIFFS.open(filename , "w+");
    if (!f)
    {
      printf("file open failed\r\n");
      //return false;
    }else{
          printf("file open OK\r\n");
    }


  HTTPClient http;
  long totalSize = 0;
  boolean chone = 1;
  // configure server and url update based on your URL
  //http.begin("http://www.meteosatonline.it/riduce/nuova/ultima.jpg");  //update based on your URL
  http.begin("http://10.0.13.23/getpicture");
  //http.begin("http://10.16.10.11:8980/tempgr/ac/BaboonL.jpg");
  // start connection and send HTTP header
  long httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      // get lenght of document (is -1 when Server sends no Content-Length header)
      long len = http.getSize();
      printf("HTTP Size is: %ld Bytes\r\n",len);
      // create buffer for read
      uint8_t buff[32] = { 0 };
      //uint8_t buff[4096] = { 0 };//works 


      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();

      // read all data from server
      while (http.connected() && (len > 0 || len == -1)) {
        size_t size = stream->available();
        if (size) {

          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          f.write(buff,c);
          //memcpy (gImage_2in13_test + totalSize, buff, c);
          totalSize = totalSize + c;

          if (len > 0) {
            len -= c;
          }
          //printf("totalsize: %ld", totalSize);
          //delay (200);
        }
        yield();
      }
      printf("[HTTP] connection closed or file end.\n");

    }
  } else {
    printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
    f.close();

/*load file and store as picture variable
************************************************************************************************************************************************
*/
printf("Loading file into variable:...\n");










//printf("Size of the variable: %i\n", sizeof(testpicture));

//x.close();
//free(testpicture);

//Paint_DrawBitMap(buf);


printf("done loading file into variable\n");






//printf("size of the variable:\n %d", sizeof(image2in13));
/*end picture
*/

  //String str = (char*)gImage_2in13_test;
  //String new_str *ccp = str;
  //printf(str.c_str());

  http.end();
  yield();

  printf("list directory: \r\n");
  listDir(SPIFFS, "/", 0);
  printf("list directory done: \r\n");

  delay(10000);  //Send a request every 10 seconds

  }


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  printf("Connecting to WiFi ..\r\n");
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    delay(1000);
  }
  printf(WiFi.localIP().toString().c_str());
  printf("\r\n");
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


/*
fdsaf---------------------------asdklfjl;asjhdlkfja;lskjhdflkjasdlf
*/

String readFile(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    //Serial.println("- empty file or failed to open file");
    return String();
  }
  //Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  
  return fileContent;
}  




