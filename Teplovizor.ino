#include "Platform.h"
#include "MeasureConfigurator.h"

Platform platform;
MeasureConfigurator configurator;

void printArray(double** arr, int rows, int columns) {
  for(int r = 0; r < rows; ++r) {
    for(int c = 0; c < columns; ++c) {
      Serial.print(arr[r][c]); Serial.print(' ');
    }
    Serial.println();
  }
}

void setup() {
  int horPort = 22;
  int verPort = 24;
  
  platform = Platform(horPort, verPort);
  platform.setVerAngle(90);
  platform.setHorAngle(90);

  int singleTime = 500;
  int horMin = 60;
  int horMax = 120;
  int verMin = 60;
  int verMax = 120;
  int horSection = 5;
  int verSection = 5;
  
  configurator = MeasureConfigurator(horMin, horMax, horSection,
                                     verMin, verMax, verSection,
                                     singleTime, platform);

  Serial.begin(9600);
  printArray(configurator.measure(), verSection, horSection);
}

void loop() {
   
  delay(1000); // can adjust this for faster/slower updates
}
