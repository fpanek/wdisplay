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
  initWiFi();
	DEV_Module_Init();
	EPD_2in13_V3_Init();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  //clearScreen();
  //printPicture();
  delay(10000);  //Send a request every 10  seconds
}


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  printf("\r\nConnecting to WiFi ..\r\n");
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    delay(1000);
  }
  printf("\r\nConnected to WiFI - my IP: %s", WiFi.localIP().toString().c_str());
  printf("\r\n");
}

void showFileContent(){
  File file = LITTLEFS.open("/picture", FILE_READ);
  unsigned char buffer[64];
  printf("start reading file..\n");
  while (file.available()) {
    int l = file.readBytesUntil('\n', buffer, sizeof(buffer)-1);
    buffer[l] = 0;
  }
  file.close();
}

void listlittlefscontent(){
  printf("start listing directory: \r\n");
  listDir(SPIFFS, "/", 0);
  printf("list directory done: \r\n");
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

void differentUnusedFunctions(){
  //SPIFFS.remove("/picture2"); //remove function if required
  //SPIFFS.format();
}

void getPictureFromServer(){
   String filename = "/picture";
   
  File f = SPIFFS.open(filename , "w+");
  if (!f)
  {
    printf("file open failed\r\n");
      //return false;
  }else{
    //printf("file open OK\r\n");
  }


  HTTPClient http;
  long totalSize = 0;
  boolean chone = 1;
  // configure server and url update based on your URL
  http.begin("http://10.0.13.23/getpicture");
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
      uint8_t buff[32] = { 0 };       // create buffer for read
      WiFiClient * stream = http.getStreamPtr();       // get tcp stream
      while (http.connected() && (len > 0 || len == -1)) {       // read all data from server
        size_t size = stream->available();
        if (size) {
          unsigned char c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          f.write(buff,c);
          totalSize = totalSize + c;
          if (len > 0) {
            len -= c;
          }
        }
        yield();
      }
      printf("[HTTP] connection closed or file end.\n");
    }
  } else {
    printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  f.close();
  http.end();
  yield();
}


void clearScreen(){
	EPD_2in13_V3_Clear();
}



void printPicture(){
 
    //Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) 
	{
		printf("Failed to apply for black memory...\r\n");
		while (1);
	}

  if(!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
      Serial.println("LITTLEFS Mount Failed");
      return;
  }

  printf("open file for reading\n");
  File file = LITTLEFS.open("/picture", FILE_READ);
  printf("read file and copy it to blackimage\n");
  file.read(BlackImage, file.size());  
  file.close();

  Paint_DrawBitMap(BlackImage); //funktioniert

	EPD_2in13_V3_Display(BlackImage);

	//printf("Goto Sleep...\r\n");
	EPD_2in13_V3_Sleep(); 
	free(BlackImage); 
	//BlackImage = NULL;
	DEV_Delay_ms(2000);//important, at least 2s
	// close 5V
	printf("close 5V, Module enters 0 power consumption ...\r\n");
}


