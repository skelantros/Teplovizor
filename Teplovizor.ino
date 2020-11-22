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


Platform platform;
MeasureConfigurator configurator;
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
int grid_width;
int grid_height;
int grid;
int color_choice;
int time_choice;
int hor_angle;
int ver_angle;
//цвета для палитр
uint16_t gray_colors[32];
uint16_t rol_colors[32];
void setup() {
  initGrayPalette(gray_colors);
  initRolPalette(rol_colors);
  

  int horPort = 24;
  int verPort = 22;

  platform = Platform(horPort, verPort);
  platform.setVerAngle(90);
  platform.setHorAngle(90);
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
}
int flag = 0;
void loop() {

  delay(1000); // can adjust this for faster/slower updates
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
    for (uint8_t b = 0; b < GRID_OPTIONS; b++) {
      if (grid_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case GRID_MINUS:
            grid_menu();
            break;
          case GRID_PLUS:
            grid_menu();
            break;
          case GRID_BACK:
            menu();
            idx -= 2;
            break;
        }
      }
    }
  }
  else if (idx == 5) {
    for (uint8_t b = 0; b < COLOR_OPTIONS; b++) {
      if (color_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case COLOR_LEFT:
            color_menu();
            break;
          case COLOR_RIGHT:
            color_menu();
            break;
          case COLOR_BACK:
            menu();
            idx -= 3;
            break;
        }
      }
    }
  }
  else if (idx == 6) {
    for (uint8_t b = 0; b < TIME_OPTIONS; b++) {
      if (time_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case TIME_LEFT:
            time_menu();
            break;
          case TIME_RIGHT:
            time_menu();
            break;
          case TIME_BACK:
            menu();
            idx -= 4;
            break;
        }
      }
    }
  }
  else if (idx == 7) {
    for (uint8_t b = 0; b < ANGLE_OPTIONS; b++) {
      if (angle_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case ANGLE_LEFT:
            angle_menu();
            break;
          case ANGLE_RIGHT:
            angle_menu();
            break;
          case ANGLE_BACK:
            menu();
            idx -= 5;
            break;
        }
      }
    }
  }

  if (flag == 1) {
    for (int r = 0; r < 215; ++r) {
      delete[] new_arr[r];
    }
    delete[] new_arr;

    for (int r = 0; r < configurator.getVerSection(); ++r) {
      delete[] source_arr[r];
    }
    delete source_arr;
  }
}

int start(int flag, float temperature, double** matrix) {
  tft.fillScreen(BLACK);

  //source_arr = configurator.measure();
  double** test = new double*[8];
  for (int i = 0; i < 8; ++i) {
    test[i] = new double[8];
  }
  for (int i = 0; i < 8; ++i) {
    for (int j  = 0; j < 8; ++j) {
      test[i][j] = 16 + j;
    }
  }
  new_arr = interpolateArray(test, configurator.getVerSection(), configurator.getHorSection(),
                             35, 25);
  tft.setTextSize (1);

  // Header
  uint16_t start_colors[15] = {RED, BLUE};

  for (uint8_t i = 0; i < 35; ++i) {
    for (uint8_t j = 0; j < 25; ++j) {
      pixel_color = rol_colors[int(32.0 * (new_arr[i][j] - 16) / (24 - 16))];
      tft.fillRect(0 + 6 * i, 0 + 12 * j, 9, 9, pixel_color);
    }
  }
  for (int i = 0; i < 8; ++i) {
    delete test[i];
  }
  delete test;
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
void angle_menu() { settings_menu("ANGLE", hor_angle, color_buttons, ANGLE_OPTIONS); }

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
