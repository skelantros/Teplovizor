#include "Platform.h"
#include "MeasureConfigurator.h"
#include "Interpolation.h"

Platform platform;
MeasureConfigurator configurator;

// функция вывода массива в консоль Arduino
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

  // инициализация конфигуратора
  configurator = MeasureConfigurator(horMin, horMax, horSection,
                                     verMin, verMax, verSection,
                                     singleTime, platform);

  Serial.begin(9600);
  // произведение замеров
  double** src_arr = configurator.measure();
  Serial.println("Source array: ");
  printArray(src_arr, configurator.getVerSection(), configurator.getHorSection());
  
  int new_rows = 7;
  int new_columns = 12;
  // интерполяция тепловой карты
  double** new_arr = interpolateArray(src_arr, configurator.getVerSection(), configurator.getHorSection(),
                                      new_rows, new_columns);
  Serial.println("Interpolated array: ");
  printArray(new_arr, new_rows, new_columns);

  for(int r = 0; r < new_rows; ++r) {
    delete[] new_arr[r];
  }
  delete[] new_arr;

  for(int r = 0; r < configurator.getVerSection(); ++r) {
    delete[] src_arr[r];
  }
  delete src_arr;
}

void loop() {
   
  delay(1000); // can adjust this for faster/slower updates
}
