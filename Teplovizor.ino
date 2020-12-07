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

// инициализация TouchScreen
short TS_MINX = 150;
short TS_MINY = 120;
short TS_MAXX = 920;
short TS_MAXY = 940;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

uint16_t colors[COLORS_COUNT] = {RED, BLUE, BLACK, GREEN, CYAN, MAGENTA, YELLOW, WHITE};

// экранчик
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// измерения
uint16_t width = 0;
uint16_t height = 0;
uint16_t idx = 1;

Adafruit_GFX_Button buttons[BUTTONS]; //инициализируем кнопки
Adafruit_GFX_Button menu_buttons[MENU_OPTIONS];
Adafruit_GFX_Button option_buttons[GRID_OPTIONS];
Adafruit_GFX_Button color_buttons[COLOR_OPTIONS];
Adafruit_GFX_Button start_buttons[START_OPTIONS];

// переменные для настроек
uint16_t buttons_y = 0;
int grid_width = 10;
int grid_height = 5;
int color_choice = 1;
int time_choice = 700;
int hor_angle_choice = 30;
int ver_angle_choice = 15;
// объект платформы
Platform platform;

//цвета для палитр
uint16_t gray_colors[32];
uint16_t rol_colors[32];
void setup() {
  initGrayPalette(gray_colors);
  initRolPalette(rol_colors);
  platform = Platform(PLATFORM_HOR_PORT, PLATFORM_VER_PORT);
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

  // демонстрация стартового экрана
  showInitScreen(tft);
  waitOneTouch();
  list_menu("MAIN_MENU", buttons, BUTTONS);

  Serial.begin(115200);
}
int flag = 0;
void loop() {
  TSPoint p;
  // ожидания нажатия на экран
  digitalWrite(13, HIGH);
  p = waitOneTouch();
  digitalWrite(13, LOW);
  p.x = mapXValue(p);
  p.y = mapYValue(p);

  // выполнение действия, соответствующего нажатой кнопке в текущем меню
  if (idx == MAIN_MENU_IDX) {
    for (uint8_t b = 0; b < BUTTONS; b++) {

      if (buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case BUTTON_START:
            flag = start(flag);
            idx = MEASURE_MENU_IDX;
            break;
          case BUTTON_MENU:
            list_menu("SETTINGS", menu_buttons, MENU_OPTIONS);
            idx = SETTINGS_MENU_IDX;
            break;
        }
      }
    }
  }
  else if (idx == SETTINGS_MENU_IDX) {
    for (uint8_t b = 0; b < MENU_OPTIONS; b++) {

      if (menu_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case MENU_VER_GRID:
            ver_grid_menu();
            idx = VER_GRID_MENU_IDX;
            break;
          case MENU_HOR_GRID:
            ver_grid_menu();
            idx = HOR_GRID_MENU_IDX;
            break;
          case MENU_VER_ANGLE:
            ver_angle_menu();
            idx = VER_ANGLE_MENU_IDX;
            break;
          case MENU_HOR_ANGLE:
            hor_angle_menu();
            idx = HOR_ANGLE_MENU_IDX;
            break;
          case MENU_COLOR:
            color_menu();
            idx = COLOR_MENU_IDX;
            break;
          case MENU_TIME:
            time_menu();
            idx = TIME_MENU_IDX;
            break;
          case MENU_BACK:
            main_menu();
            idx = MAIN_MENU_IDX;
            break;
        }
      }
    }
  }
  else if (idx == MEASURE_MENU_IDX) {
    for (uint8_t b = 0; b < START_OPTIONS; b++) {
      if (start_buttons[b].contains(p.x, p.y)) {
        switch (b) {
          case BUTTON_BACK:
            main_menu();
            idx = MAIN_MENU_IDX;
            break;
          case BUTTON_REFRESH:
            flag = start(flag);
            break;
        }
      }
    }
  }
  else if (idx == VER_GRID_MENU_IDX) {
    settings_handler(grid_height, 5, 20, 3, ver_grid_menu, option_buttons, GRID_PLUS, GRID_MINUS, GRID_BACK, p);
  }
  else if (idx == HOR_GRID_MENU_IDX) {
    settings_handler(grid_width, 5, 20, 3, hor_grid_menu, option_buttons, GRID_PLUS, GRID_MINUS, GRID_BACK, p);
  }
  else if (idx == VER_ANGLE_MENU_IDX) {
    settings_handler(ver_angle_choice, 30, 90, 10, ver_angle_menu, option_buttons, ANGLE_RIGHT, ANGLE_LEFT, ANGLE_BACK, p);
  }
  else if (idx == HOR_ANGLE_MENU_IDX) {
    settings_handler(hor_angle_choice, 30, 90, 10, hor_angle_menu, option_buttons, ANGLE_RIGHT, ANGLE_LEFT, ANGLE_BACK, p);
  }
  else if (idx == COLOR_MENU_IDX) {
    settings_handler(color_choice, 0, 1, 1, color_menu, color_buttons, COLOR_RIGHT, COLOR_LEFT, COLOR_BACK, p);
  }
  else if (idx == TIME_MENU_IDX) {
    settings_handler(time_choice, 100, 5000, 100, time_menu, option_buttons, TIME_RIGHT, TIME_LEFT, TIME_BACK, p);
  }
}

int start(int flag) {
  tft.fillScreen(BLACK);
  // инициализация конфигуратора
  MeasureConfigurator configurator = MeasureConfigurator(90 - hor_angle_choice, 90 + hor_angle_choice, grid_width, 
                                            90 - ver_angle_choice, 90 + ver_angle_choice, grid_height, time_choice, platform);
  
  // инициализация и заполнение матрицы измерений
  Matrix<double> measures;
  measures.arr = configurator.measure();
  measures.rows = configurator.getVerSection();
  measures.columns = configurator.getHorSection();
  //Serial.print("Max temp: "); Serial.println(configurator.getMaxTemp());
  //Serial.print("Min temp: "); Serial.println(configurator.getMinTemp());
  //printArray(measures.arr, measures.rows, measures.columns);

  // инициализация цветовой палитры
  Array<uint16_t> colors_array;
  colors_array.arr = color_choice == 0 ? gray_colors : rol_colors;
  colors_array.size = 32;
  ColorPalette palette = ColorPalette(colors_array, configurator.getMinTemp(), configurator.getMaxTemp());
  // инициализация построителя тепловой карты и ее отрисовка
  MapBuilder builder = MapBuilder(&tft, 0, 0, 320, 240, palette, measures);
  builder.draw();

  // очистка матрицы измерений
  for(int r = 0; r < measures.rows; ++r) {
    delete[] measures.arr[r];
  }
  delete* measures.arr;

  tft.setTextSize (1);

  uint16_t start_colors[15] = {RED, BLUE};
  print_in_header("TEMPERATURE", 0, WHITE);
  print_in_header(platform.takeAmbientMeasure(), 300, WHITE);

  flag = 1;
  // Buttons
  for (uint8_t i = 0; i < 2; i++) {
    start_buttons[i].drawButton();
  }
  return flag;
}

template <typename T>
void print_in_header(T info, uint16_t y, uint16_t color) {
  tft.setCursor (95, y);
  tft.setTextColor(color);
  tft.println(info);
}

// функция, отрисовывающая меню для конкретной настройки тепловизора
void settings_menu(char* option_name, int option_value, Adafruit_GFX_Button* buttons, uint16_t buttons_count) {
    tft.fillScreen(BLACK);
    tft.setTextSize (1);

    // заголовок
    tft.fillRect(0, 0, width, 10, RED);
    //print_in_header(option_name, 0, WHITE);
    //print_in_header(option_name, 300, " OPTION");

    // значение параметра
    tft.setCursor(95, 200);
    tft.setTextSize(4);
    tft.println(option_value);

    // кнопки
    for (uint8_t i = 0; i < buttons_count; i++) {
      buttons[i].drawButton();
    }
}

void ver_grid_menu() { settings_menu("VER.GRID", grid_height, option_buttons, GRID_OPTIONS); }
void hor_grid_menu() { settings_menu("HOR.GRID", grid_width, option_buttons, GRID_OPTIONS); }
void ver_angle_menu() { settings_menu("VER.ANGLE", ver_angle_choice, option_buttons, ANGLE_OPTIONS); }
void hor_angle_menu() { settings_menu("HOR.ANGLE", hor_angle_choice, option_buttons, ANGLE_OPTIONS); }
void color_menu() { settings_menu("COLOR", color_choice, color_buttons, COLOR_OPTIONS); }
void time_menu() { settings_menu("TIME", time_choice, option_buttons, TIME_OPTIONS); }

// функция, обрабатывающая изменения настройки тепловизора в соответствующем меню
void settings_handler(int &option, int min, int max, int single_change, void (*menu_func)(void), 
                      Adafruit_GFX_Button* buttons, uint16_t plus_button, uint16_t minus_button, uint16_t exit_button, TSPoint p) {
  for (uint8_t b = 0; b < 3; b++) {
      if (option_buttons[b].contains(p.x, p.y)) {
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

// функция, отрисовывающая меню, содержащее только кнопки
void list_menu(const char* header, Adafruit_GFX_Button* buttons, uint16_t buttons_count) {
    tft.fillScreen(BLACK);


    tft.setTextSize (1);

    // заголовок
    tft.fillRect(0, 0, width, 10, RED);
    print_in_header("SETTINGS", 0, WHITE);

    // кнопки
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

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));

  return p;
}

void drawBorder () {

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);

}

void initializeButtons() {

  uint16_t x = 120;
  uint16_t y = 170;
  uint16_t w = 200;
  uint16_t h = 70;
  uint16_t set_y = 50;
  uint16_t set_h = 40;
  uint8_t textSize = 1;

  char main_buttonlabels[2][20] = {"START", "SETTINGS"};
  char menu_buttonlabels[7][20] = {"HOR. GRID", "VER. GRID", "HOR.ANGLE", "VER.ANGLE", "COLOR", "TIME", "BACK"};
  char option_buttonlabels[3][20] = {"-", "+", "BACK"};
  char color_buttonlabels[3][20] = {"<-", "->", "BACK"};
  char start_buttonlabels[2][20] = {"BACK", "REFRESH"};

  uint16_t main_buttoncolors[15] = {RED, BLUE};

  for (uint8_t b = 0; b < 2; b++) {
    buttons[b].initButton(&tft,                           // TFT object
                          x,  y + b * 100,              // x, y,
                          w, h, WHITE, main_buttoncolors[b], WHITE,    // w, h, outline, fill,
                          main_buttonlabels[b], textSize);             // text
  }

  uint16_t curr_y = set_y;
  for(uint8_t b = 0; b < 4; b += 2) {
    menu_buttons[b].initButton(&tft,
                               x / 2, curr_y,
                               w / 2, set_h, WHITE, BLUE, WHITE,
                               menu_buttonlabels[b], textSize);
    menu_buttons[b+1].initButton(&tft,
                               x + x / 2, curr_y,
                               w / 2, set_h, WHITE, BLUE, WHITE,
                               menu_buttonlabels[b+1], textSize);
    curr_y += 60;
  }

  for (uint8_t b = 4; b < 7; b++) {
    menu_buttons[b].initButton(&tft,
                               x, curr_y,
                               w, set_h, WHITE, BLUE, WHITE,
                               menu_buttonlabels[b], textSize);
    curr_y += 60;
  }
  // Save the y position to avoid draws

  for (uint8_t b = 0; b < 3; b++) {
    option_buttons[b].initButton(&tft,
                               x, set_y + b * 60,
                               w, set_h, WHITE, BLUE, WHITE,
                               option_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 3; b++) {
    color_buttons[b].initButton(&tft,
                                x, set_y + b * 60,
                                w, set_h, WHITE, BLUE, WHITE,
                                color_buttonlabels[b], textSize);
  }
  for (uint8_t b = 0; b < 2; b++) {
    start_buttons[b].initButton(&tft,
                                x, 230 + b * 60,
                                w, 45, WHITE, BLUE, WHITE,
                                start_buttonlabels[b], textSize);
  }

  buttons_y = y;

}

uint16_t mapXValue(TSPoint p) {

  uint16_t x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());

  //Correct offset of touch. Manual calibration
  //x+=1;

  return x;

}
uint16_t mapYValue(TSPoint p) {

  uint16_t y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  //Correct offset of touch. Manual calibration
  //y-=2;

  return y;
}
