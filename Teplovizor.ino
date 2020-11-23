#include <Wire.h>
#include <pin_magic.h>
#include <registers.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>
#include <SPFD5408_Util.h>

#include "Platform.h"
#include "MeasureConfigurator.h"
#include "Interpolation.h"
#include "DefinedConstants.h"
#include "MapBuilder.h"

void settings_handler(int &option, int min, int max, int single_change, void (*menu_func)(void), 
                      Adafruit_GFX_Button* buttons, uint16_t plus_button, uint16_t minus_button, uint16_t exit_button, TSPoint p);

// Init TouchScreen:
short TS_MINX = 150;
short TS_MINY = 120;
short TS_MAXX = 920;
short TS_MAXY = 940;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

uint16_t colors[COLORS_COUNT] = {RED, BLUE, BLACK, GREEN, CYAN, MAGENTA, YELLOW, WHITE};
// Init LCD

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Dimensions

uint16_t width = 0;
uint16_t height = 0;
uint16_t idx = 1;

Adafruit_GFX_Button buttons[BUTTONS]; //инициализируем кнопки
Adafruit_GFX_Button menu_buttons[MENU_OPTIONS];
Adafruit_GFX_Button grid_buttons[GRID_OPTIONS];
Adafruit_GFX_Button color_buttons[COLOR_OPTIONS];
Adafruit_GFX_Button time_buttons[TIME_OPTIONS];
Adafruit_GFX_Button angle_buttons[ANGLE_OPTIONS];
Adafruit_GFX_Button start_buttons[START_OPTIONS];


//переменные для настроек
uint16_t buttons_y = 0;
float temp = 0;
Adafruit_MLX90614 modIrTemp = Adafruit_MLX90614();
double** source_arr;
double** new_arr;
uint16_t pixel_color;
uint16_t min_color = 16;
uint16_t max_color = 24;
int grid_width = 7;
int grid_height = 7;
int grid = 3;
int color_choice = 1;
int time_choice = 1000;
int hor_angle_choice = 30;
int ver_angle_choice = 30;

//цвета для палитр
uint16_t gray_colors[32];
uint16_t rol_colors[32];
void setup() {
  initGrayPalette(gray_colors);
  initRolPalette(rol_colors);
  
  //инициализируем экранчик
  tft.reset();
  tft.begin(0x9341);
  width = tft.width() - 1;
  height = tft.height() - 1;
  //инициализируем кнопки
  initializeButtons();
  drawBorder();

  // Initial screen
  showInitScreen(tft);

  modIrTemp.begin();
  // Wait touch

  waitOneTouch();
  list_menu("MAIN_MENU", buttons, BUTTONS);

  Serial.begin(115200);
}
int flag = 0;
void loop() {
  TSPoint p;

  digitalWrite(13, HIGH);

  p = waitOneTouch();

  digitalWrite(13, LOW);

  // Map of values

  p.x = mapXValue(p);
  p.y = mapYValue(p);
  //делаем функционал кнопок для экрана таким образом, idx = 1: главный экран, idx = 2: меню настроек, idx = 3: меню с запуском измерений 4..7 - настройки для измерений
  if (idx == 1) {
    for (uint8_t b = 0; b < BUTTONS; b++) { //для каждой кнопки

      if (buttons[b].contains(p.x, p.y)) { //если на кнопку нажали
        // Action
        switch (b) {
          case BUTTON_START: //если старт, то соответствующая функция вызывается
            flag = start(flag, temp, new_arr);
            idx += 2;
            break;
          case BUTTON_MENU:
            list_menu("SETTINGS", menu_buttons, MENU_OPTIONS);
            idx++;
            break;
        }
      }
    }
  }
  else if (idx == 2) {
    for (uint8_t b = 0; b < MENU_OPTIONS; b++) {

      if (menu_buttons[b].contains(p.x, p.y)) {
        // Action
        switch (b) {
          case MENU_GRID:
            grid_menu();
            idx += 2;
            break;
          case MENU_COLOR:
            color_menu();
            idx += 3;
            break;
          case MENU_TIME:
            time_menu();
            idx += 4;
            break;
          case MENU_ANGLE:
            angle_menu();
            idx += 5;
            break;
          case MENU_BACK:
            main_menu();
            idx--;
            break;
        }
      }
    }
  }
  else if (idx == 3) {
    for (uint8_t b = 0; b < START_OPTIONS; b++) {
      if (start_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case BUTTON_BACK:
            main_menu();
            idx -= 2;
            break;
          case BUTTON_REFRESH:
            flag = start(flag, temp, new_arr);
            break;
        }
      }
    }
  }
  else if (idx == 4) {
    settings_handler(grid, 5, 20, 3, grid_menu, grid_buttons, GRID_PLUS, GRID_MINUS, GRID_BACK, p);
  }
  else if (idx == 5) {
    settings_handler(color_choice, 0, 1, 1, color_menu, color_buttons, COLOR_RIGHT, COLOR_LEFT, COLOR_BACK, p);
  }
  else if (idx == 6) {
    settings_handler(time_choice, 100, 5000, 100, time_menu, time_buttons, TIME_RIGHT, TIME_LEFT, TIME_BACK, p);
  }
  else if (idx == 7) {
    settings_handler(hor_angle_choice, 30, 90, 10, angle_menu, angle_buttons, ANGLE_RIGHT, ANGLE_LEFT, ANGLE_BACK, p);
  }

  if (flag == 1) {
    for (int r = 0; r < 215; ++r) {
      delete[] new_arr[r];
    }
    delete[] new_arr;

    //for (int r = 0; r < configurator.getVerSection(); ++r) {
    //  delete[] source_arr[r];
    //}
    delete source_arr;
  }
}

int start(int flag, float temperature, double** matrix) {
  tft.fillScreen(BLACK);

  //source_arr = configurator.measure();
  // double** test = new double*[8];
  // for (int i = 0; i < 8; ++i) {
  //   test[i] = new double[8];
  // }
  // for (int i = 0; i < 8; ++i) {
  //   for (int j  = 0; j < 8; ++j) {
  //     test[i][j] = 16 + j;
  //   }
  // }

  Platform platform = Platform(PLATFORM_HOR_PORT, PLATFORM_VER_PORT);
  MeasureConfigurator configurator = MeasureConfigurator(90 - hor_angle_choice, 90 + hor_angle_choice, grid_width, 
                                            90 - ver_angle_choice, 90 + ver_angle_choice, grid_height, time_choice, platform);
  
  double** measures = configurator.measure();
  Matrix<double> measures_matrix;
  measures_matrix.arr = measures;
  measures_matrix.rows = configurator.getVerSection();
  measures_matrix.columns = configurator.getHorSection();
  // for debugging purposes
  Serial.print("Max temp: "); Serial.println(configurator.getMaxTemp());
  Serial.print("Min temp: "); Serial.println(configurator.getMinTemp());
  printArray(measures, configurator.getVerSection(), configurator.getHorSection());
  Array<uint16_t> colors_array;
  colors_array.arr = color_choice == 0 ? gray_colors : rol_colors;
  colors_array.size = 32;
  ColorPalette palette = ColorPalette(colors_array, configurator.getMinTemp(), configurator.getMaxTemp());

  MapBuilder builder = MapBuilder(&tft, 0, 0, 240, 320, palette, measures_matrix);
  builder.draw();

  tft.setTextSize (1);
  //Array<uint16_t> rol_array;
  //rol_array.arr = rol_colors;
  //rol_array.size = 32;
  //ColorPalette palette = ColorPalette(rol_array, 16, 24);
  //Matrix<double> test_arr;
  //test_arr.arr = test;
  //test_arr.rows = 8; test_arr.columns = 8;
  //MapBuilder builder = MapBuilder(&tft, 0, 0, 240, 320, palette, test_arr);
  //builder.draw();

  // Header
  uint16_t start_colors[15] = {RED, BLUE};
  // for (int i = 0; i < 8; ++i) {
  //   delete test[i];
  // }
  // delete test;
  tft.setCursor (95, 0);
  tft.setTextColor(WHITE);
  tft.println("TEMPERATURE");
  tft.setCursor (95, 300);
  tft.println(temperature);
  // Footer

  flag = 1;

  // Buttons
  for (uint8_t i = 0; i < 2; i++) {
    start_buttons[i].drawButton();
  }
  return flag;
}


void settings_menu(const char* option_name, int option_value, Adafruit_GFX_Button* buttons, uint16_t buttons_count) {
    tft.fillScreen(BLACK);
    tft.setTextSize (1);

    // Header
    tft.fillRect(0, 0, width, 10, RED);

    tft.setCursor (95, 0);
    tft.setTextColor(WHITE);
    tft.println(option_name);
    tft.setCursor(80, 300);
    tft.print(option_name);
    tft.println(" OPTION");

    tft.setCursor(95, 200);
    tft.setTextSize(4);
    tft.println(option_value);

    // Buttons
    for (uint8_t i = 0; i < buttons_count; i++) {
      buttons[i].drawButton();
    }
}

void grid_menu() { settings_menu("GRID", grid, grid_buttons, GRID_OPTIONS); }
void color_menu() { settings_menu("COLOR", color_choice, color_buttons, COLOR_OPTIONS); }
void time_menu() { settings_menu("TIME", time_choice, color_buttons, TIME_OPTIONS); }
void angle_menu() { settings_menu("ANGLE", hor_angle_choice, color_buttons, ANGLE_OPTIONS); }

void settings_handler(int &option, int min, int max, int single_change, void (*menu_func)(void), 
                      Adafruit_GFX_Button* buttons, uint16_t plus_button, uint16_t minus_button, uint16_t exit_button, TSPoint p) {
  for (uint8_t b = 0; b < 3; b++) {
      if (grid_buttons[b].contains(p.x, p.y)) {
        if(b == plus_button) {
              option += single_change;
              if(option > max) option = max;
              (*menu_func)();
        }
        else if(b == minus_button) {
              option -= single_change;
              if(option < min) option = min;
              (*menu_func)();
        }
        else if(b == exit_button) {
              menu();
              idx = 2;
              break;
        }
      }
  }
}

void list_menu(const char* header, Adafruit_GFX_Button* buttons, uint16_t buttons_count) {
    tft.fillScreen(BLACK);


    tft.setTextSize (1);

    // Header

    tft.fillRect(0, 0, width, 10, RED);

    tft.setCursor (95, 0);
    tft.setTextColor(WHITE);
    tft.println("SETTINGS");

    // Buttons
    for (uint8_t i = 0; i < buttons_count; i++) {
      buttons[i].drawButton();
    }

}

void menu() { list_menu("SETTINGS", menu_buttons, MENU_OPTIONS); }
void main_menu() { list_menu("MAIN MENU", buttons, BUTTONS); }


TSPoint waitOneTouch() {

  TSPoint p;

  do {
    p = ts.getPoint();

    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);

  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));

  return p;
}

// Draw a border

void drawBorder () {

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);

}




// Initialize buttons

void initializeButtons() {

  uint16_t x = 120;
  uint16_t y = 170;
  uint16_t w = 200;
  uint16_t h = 70;
  uint16_t set_y = 50;
  uint16_t set_h = 40;
  uint8_t textSize = 1;

  char main_buttonlabels[2][20] = {"START", "SETTINGS"};
  char menu_buttonlabels[5][20] = {"GRID", "COLOR", "TIME", "ANGLE", "BACK"};
  char grid_buttonlabels[3][20] = {"-", "+", "BACK"};
  char color_buttonlabels[3][20] = {"<-", "->", "BACK"};
  char time_buttonlabels[3][20] = {"-", "+", "BACK"};
  char angle_buttonlabels[3][20] = {"-", "+", "BACK"};
  char start_buttonlabels[2][20] = {"BACK", "REFRESH"};

  uint16_t main_buttoncolors[15] = {RED, BLUE};

  for (uint8_t b = 0; b < 2; b++) {
    buttons[b].initButton(&tft,                           // TFT object
                          x,  y + b * 100,              // x, y,
                          w, h, WHITE, main_buttoncolors[b], WHITE,    // w, h, outline, fill,
                          main_buttonlabels[b], textSize);             // text
  }
  for (uint8_t b = 0; b < 5; b++) {
    menu_buttons[b].initButton(&tft,
                               x, set_y + b * 60,
                               w, set_h, WHITE, BLUE, WHITE,
                               menu_buttonlabels[b], textSize);
  }
  // Save the y position to avoid draws

  for (uint8_t b = 0; b < 3; b++) {
    grid_buttons[b].initButton(&tft,
                               x, set_y + b * 60,
                               w, set_h, WHITE, BLUE, WHITE,
                               grid_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 3; b++) {
    color_buttons[b].initButton(&tft,
                                x, set_y + b * 60,
                                w, set_h, WHITE, BLUE, WHITE,
                                color_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 3; b++) {
    time_buttons[b].initButton(&tft,
                               x, set_y + b * 60,
                               w, set_h, WHITE, BLUE, WHITE,
                               time_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 3; b++) {
    angle_buttons[b].initButton(&tft,
                                x, set_y + b * 60,
                                w, set_h, WHITE, BLUE, WHITE,
                                angle_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 2; b++) {
    start_buttons[b].initButton(&tft,
                                x, 230 + b * 60,
                                w, 45, WHITE, BLUE, WHITE,
                                start_buttonlabels[b], textSize);
  }

  buttons_y = y;

}

// Map the coordinate X
uint16_t mapXValue(TSPoint p) {

  uint16_t x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());

  //Correct offset of touch. Manual calibration
  //x+=1;

  return x;

}

// Map the coordinate Y

uint16_t mapYValue(TSPoint p) {

  uint16_t y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  //Correct offset of touch. Manual calibration
  //y-=2;

  return y;
}
