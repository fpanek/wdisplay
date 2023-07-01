/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>


//new includes
#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SPIFFS.h"

/***** Settings ******/
const char* ssid = "wdisplay";
const char* password = "wdisplay";

uint8_t MAC_array[6];
char MAC_char[18];

String serverName = "http://10.0.13.23:80/getpicture";
//uint8_t PicArray[80750] = {0}; //Array for picture make sure under 15K
unsigned char gImage_2in13_test[100000] = {0};

#define FORMAT_SPIFFS_IF_FAILED true


/* Entry point ----------------------------------------------------------------*/
void setup()
{
          if(psramInit()){
        printf("\nPSRAM is correctly initialized");
        }else{
        printf("PSRAM not available");
        }
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
	Paint_DrawBitMap(gImage_2in13);

	EPD_2in13_V3_Display(BlackImage);
	//DEV_Delay_ms(2000);



	//printf("Clear...\r\n");
	//EPD_2in13_V3_Init();
	//EPD_2in13_V3_Clear();

	//printf("Goto Sleep...\r\n");
	EPD_2in13_V3_Sleep(); //TODO macht was?
	free(BlackImage); //TODO macht was?
	//BlackImage = NULL;
	DEV_Delay_ms(2000);//important, at least 2s
	// close 5V
	printf("close 5V, Module enters 0 power consumption ...\r\n");

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        printf("SPIFFS Mount Failed\r\\n");
        return;
    }



  //SPIFFS.format();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
/*test*/

 String filename = "/testfileesp";

  /*
  fs::Dir dir = SPIFFS.openDir(F("/"));
  while (dir.next())
  {
    fileName = dir.fileName();

    Serial.println(fileName);
  }
  */
  
  SPIFFS.remove(filename);
    fs::File f = SPIFFS.open(filename, "w+");
    if (!f)
    {
      printf("file open failed\r\n");
      //return false;
    }else{
          printf("file open OK\r\n");
    }
/*test*/

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
      uint8_t buff[2048] = { 0 };
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


  //String str = (char*)gImage_2in13_test;
  //String new_str *ccp = str;
  //printf(str.c_str());
  f.close();
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
