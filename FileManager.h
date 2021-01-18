#pragma once

#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

#define SDSPEED 10000000

#define SCK_PIN 14
#define MISO_PIN 21
#define MOSI_PIN 13
#define CS_PIN 15

class FileManager
{
public:
   static bool sdIsDetected;

   static SPIClass spiSD;

   FileManager()
   {
   }

   void init()
   {
      pinMode(SCK_PIN,INPUT_PULLUP); 
      pinMode(MISO_PIN,INPUT_PULLUP); 
      pinMode(MOSI_PIN,INPUT_PULLUP); 
      pinMode(CS_PIN,INPUT_PULLUP); 

#if VERSION == 2
      //Need to activate mosfet
      pinMode(27, OUTPUT);
      digitalWrite(27, LOW);
      // CS bug
      pinMode(CS_PIN, OUTPUT); // wierd CS/SS bug with SPI lib
      digitalWrite(CS_PIN, LOW);

#endif

      spiSD.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);//SCK,MISO,MOSI,ss

#if VERSION == 1  
      if(SD.begin(CS_PIN, spiSD, SDSPEED))
#elif VERSION == 2
      if(SD.begin(CS_PIN, spiSD))
#endif
      {
        DBG("SD Card initialized.");
         sdIsDetected = true;
      }else{
        DBG("SD Card Initialization failed.");
      }
   }

   static File openFile(String fileName, bool forWriting = false, bool deleteIfExists = true)
   {
      if(forWriting && deleteIfExists) deleteFileIfExists(fileName);
      
      if(!fileName.startsWith("/")) fileName = "/"+fileName;
      File f = SD.open(fileName.c_str(), forWriting?FILE_WRITE:FILE_READ);
      DBG("Open file : "+String(f.name()));
      return f;
   }

   

   static void deleteFileIfExists(String path)
   {
     if(!sdIsDetected) return;

      if (SD.exists(path.c_str()))
      {
         SD.remove(path.c_str());
         DBG("Removed file "+path);
      }
   }
/*
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (server.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    DBG("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}
*/

};

SPIClass FileManager:: spiSD(HSPI);
bool FileManager::sdIsDetected = false;
