#include "FS.h"
#include "SD_MMC.h"

void setup() {
  const char *root_path = "/";
  Serial.begin(115200);
  while(!Serial){delay(100);}
  Serial.println("getNextFileName() test");

  Serial.println("Initializing SD card");
  if (!SD_MMC.begin()){
    Serial.println("SD card initialization failed!");
    while(1) delay(1000);
  }
  Serial.println("SD card initialization success.");

  Serial.print("opening root: \"");
  Serial.print(root_path);
  Serial.println("\"");
  File root = SD_MMC.open(root_path);
  if(root == NULL){
    Serial.println("Failed to open SD; Halt!");
    while(1) delay(1000);
  }

  Serial.println("Test reading speed with new getNextFileName");
  String filename;
  //Return true if have next file, false if not
  unsigned long startTime = millis();
  int folderCnt = 0;
  while(root.getNextFileName(filename)) {
    //Serial.println(filename);
    ++folderCnt;
  }
  unsigned long endTime = millis();
  Serial.print("Done reading ");
  Serial.print(folderCnt);
  Serial.print(" folders in ");
  Serial.print(endTime - startTime);
  Serial.print(" ms = ");
  Serial.print((float)(endTime - startTime)/folderCnt);
  Serial.println(" ms/folder");

//////////////////////////////////////////////////
  Serial.println("Test reading speed with old openNextFile");
  folderCnt = 0;
  root = SD_MMC.open(root_path);
  if(root == NULL){
    Serial.println("Failed to open SD; Halt!");
    while(1) delay(1000);
  }

  File file = root.openNextFile();
  startTime = millis();
  while(file){
    if(file.isDirectory()){
      ++folderCnt;
      //Serial.println(folderCnt);
    }
    file = root.openNextFile();
  }
  endTime = millis();
  Serial.print("Done reading ");
  Serial.print(folderCnt);
  Serial.print(" folders in ");
  Serial.print(endTime - startTime);
  Serial.print(" ms = ");
  Serial.print((float)(endTime - startTime)/folderCnt);
  Serial.println(" ms/folder");

  while(1) delay(1000);
}

void loop() {
}
