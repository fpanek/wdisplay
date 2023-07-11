/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>


//new includes
#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "LITTLEFS.h"  // LittleFS is declared
#define SPIFFS LITTLEFS

/***** Settings ******/
const char *ssid = "wdisplay";
const char *password = "wdisplay";


String serverName = "http://10.0.13.23:80/getpicture";

#define FORMAT_LITTLEFS_IF_FAILED true

char uid [32] = {}; 


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */


/* Entry point ----------------------------------------------------------------*/
void setup() {
  initWiFi();
  print_wakeup_reason();
  DEV_Module_Init();
  EPD_2in13_V3_Init();
  sprintf(uid, "%012llx" ,ESP.getEfuseMac()); //set unique id 

  //test
    getPictureFromServer();
  clearScreen();
  printPicture();
  printf("Send ESP to sleep\r\n");
  EPD_2in13_V3_Sleep();
  //delay(10000);  //compare delay with sleep!
  delay(5000);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
  printf("will be never printed...\r\n");
  //printf("wake up ESP again..\r\n");
  //EPD_2in13_V3_Init();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
  printf("still running looop.....\r\n");
  delay(1000);
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : printf("Wakeup caused by external signal using RTC_IO\r\n"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : printf("Wakeup caused by external signal using RTC_CNTL\r\n"); break;
    case ESP_SLEEP_WAKEUP_TIMER : printf("Wakeup caused by timer\r\n"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : printf("Wakeup caused by touchpad\r\n"); break;
    case ESP_SLEEP_WAKEUP_ULP : printf("Wakeup caused by ULP program\r\n"); break;
    default : printf("Wakeup was not caused by deep sleep: %d\r\n",wakeup_reason); break;
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  printf("\r\nConnecting to WiFi..\r\n");
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    delay(1000);
  }
  printf("\r\nConnected to WiFI - my IP: %s", WiFi.localIP().toString().c_str());
  printf("\r\n");

}

void showFileContent() {
  File file = LITTLEFS.open("/picture", FILE_READ);
  unsigned char buffer[64];
  printf("start reading file..\n");
  while (file.available()) {
    int l = file.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[l] = 0;
  }
  file.close();
}

void listlittlefscontent() {
  printf("start listing directory: \r\n");
  listDir(SPIFFS, "/", 0);
  printf("list directory done: \r\n");
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
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

void differentUnusedFunctions() {
  //SPIFFS.remove("/picture2"); //remove function if required
  //SPIFFS.format();
}

void getPictureFromServer() {
  String filename = "/picture";

  File f = SPIFFS.open(filename, "w+");
  if (!f) {
    printf("file open failed\r\n");
    //return false;
  } else {
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
      printf("HTTP Size is: %ld Bytes\r\n", len);
      uint8_t buff[32] = { 0 };                             // create buffer for read
      WiFiClient *stream = http.getStreamPtr();             // get tcp stream
      while (http.connected() && (len > 0 || len == -1)) {  // read all data from server
        size_t size = stream->available();
        if (size) {
          unsigned char c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          f.write(buff, c);
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


void clearScreen() {
  EPD_2in13_V3_Clear();
}



void printPicture() {
  //Create a new image cache
  UBYTE *BlackImage;
  UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0) ? (EPD_2in13_V3_WIDTH / 8) : (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    while (1)
      ;
  }

  if (!SPIFFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LITTLEFS Mount Failed");
    return;
  }
  File file = LITTLEFS.open("/picture", FILE_READ);
  file.read(BlackImage, file.size());
  file.close();
  Paint_DrawBitMap(BlackImage);  //funktioniert
  EPD_2in13_V3_Display(BlackImage);

  free(BlackImage);
  //EPD_2in13_V3_Sleep();
  DEV_Delay_ms(2000);  //important, at least 2s
  printf("close 5V, Module enters 0 power consumption ...\r\n");
}
