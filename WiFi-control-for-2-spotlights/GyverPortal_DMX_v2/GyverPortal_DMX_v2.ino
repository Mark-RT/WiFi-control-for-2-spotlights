#include <GRGB.h>

#include <ESPDMX.h>
DMXESPSerial dmx;

// Connect GPIO02 - TDX1 to MAX3485. D4
#define AP_SSID "DoNotConnect"
#define AP_PASS "Netparolja123"

//#define AP_SSID "Krop9"
//#define AP_PASS "0964946190"

#include <GyverPortal.h>
GyverPortal ui;

#define pinOut 9        //указать пин управления, где есть шим
#define lights_pcs 2    //количество прожекторов
#define number_channels 8    //количество каналов

byte min_val = 0;
byte max_val = 255;
byte step_slider = 5;
byte rain_min = 211;

byte changer1 = 0;
byte changer2 = 0;

int main_brightness = 210;
int palitra_value_1 = 0;
int palitra_value_2 = 0;
int white_brightness = 0;
int strobe = 0;
int function_choice = 215;

bool flagLed1 = 1;
bool flagLed2 = 1;
bool flagRainbowLed = 0;
bool flag_strobe = 1;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
  dmx.init(lights_pcs * number_channels);
  dmx.write(1, main_brightness);
  dmx.write(1 + number_channels, main_brightness);
  dmx.write(7, function_choice);
  dmx.write(15, function_choice);
  dmx.update();
}

void build() {
  GP.BUILD_BEGIN(GP_DARK);

  M_BOX(GP.BUTTON_MINI("Button_1_light", "1", "", GP_GREEN, "", 0, 1);
        GP.LED_GREEN("Led_1_light", flagLed1);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("Button_2_light", "2", "", GP_GREEN, "", 0, 1);
        GP.LED_GREEN("Led_2_light", flagLed2);
       );

  M_BOX(GP.BUTTON_MINI("Switchoff_button", "Выкл.всё", "", GP_GRAY, "", 0, 1);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("White_button", "Белый", "", GP_GRAY_B, "", 0, 1);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("Rainbow_button", "Радуга", "", GP_PINK, "", 0, 1);
        GP.LED_RED("Led_rainbow", !flagRainbowLed);
       );

  M_BLOCK_THIN(
    M_BOX(GP.LABEL("Главная яркость"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("MainBrightness_slider", main_brightness, min_val, max_val, step_slider, 0, GP_VIOL); );   // яркость RGBW
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палитра 1"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Palitra_slider_1", palitra_value_1, min_val, 1530, step_slider, 0, GP_ORANGE); );            // функциональный выбор
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палитра 2"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Palitra_slider_2", palitra_value_2, min_val, 1530, step_slider, 0, GP_BLUE); );            // функциональный выбор
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Белый"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("White_slider", white_brightness, min_val, max_val, step_slider, 0, GP_GRAY_B); );         // яркость W
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Радуга"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Function_slider", function_choice, rain_min, max_val, 1, 0, GP_PINK); );            // функциональный выбор
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Стробоскоп"); GP.BUTTON_MINI("Button_strobe", "OnOff", "", GP_YELLOW, "", 0, 1); GP.LED_RED("Led_strobe", !flag_strobe); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Strobe_slider", strobe, min_val, max_val, step_slider, 0, GP_YELLOW); );                // стробоскоп
  );

  GP.ONLINE_CHECK();
  GP.BUILD_END();
}

void action() {
  if (ui.click()) {
    if (ui.click("Button_1_light")) {
      flagLed1 = !flagLed1;
      if (flagLed1 == 0) changer1 = 8;
      else if (flagLed1 == 1) changer1 = 0;
    }

    else if (ui.click("Button_2_light")) {
      flagLed2 = !flagLed2;
      if (flagLed2 == 0) changer2 = 8;
      else if (flagLed2 == 1) changer2 = 0;
    }

    else if (ui.click("Switchoff_button")) {
      palitra_value_1 = 0;
      palitra_value_2 = 0;
      flagRainbowLed = 1;
      function_choice = 211;
      white_brightness = 0;
      flag_strobe = 1;
      strobe = 0;
      dmx.write(2, 0);
      dmx.write(3, 0);
      dmx.write(4, 0);
      dmx.write(5, 0);
      dmx.write(6, 0);
      dmx.write(7, 0);
      dmx.write(10, 0);
      dmx.write(11, 0);
      dmx.write(12, 0);
      dmx.write(13, 0);
      dmx.write(14, 0);
      dmx.write(15, 0);
    }

    else if (ui.click("White_button")) {
      flagRainbowLed = 1;
      white_brightness = max_val;
      dmx.write(7, 0);
      dmx.write(15, 0);
      dmx.write(2, max_val);
      dmx.write(3, max_val);
      dmx.write(4, max_val);
      dmx.write(5, max_val);
      dmx.write(10, max_val);
      dmx.write(11, max_val);
      dmx.write(12, max_val);
      dmx.write(13, max_val);
    }

    else if (ui.click("Rainbow_button")) {
      flagRainbowLed = !flagRainbowLed;
      if (flagRainbowLed == 0) {
        dmx.write(7, function_choice);
        dmx.write(15, function_choice);
      }
      else if (flagRainbowLed == 1) {
        dmx.write(7, 0);
        dmx.write(15, 0);
      }
    }
    else if (ui.clickInt("MainBrightness_slider", main_brightness)) {
      for (int i = 1 + changer1; i <= lights_pcs * 8; i = i + 8 + changer2) {
        dmx.write(i, main_brightness);
      }
    }

    else if (ui.clickInt("Palitra_slider_1", palitra_value_1)) {
        colorWheel_1(palitra_value_1);
    }
    
    else if (ui.clickInt("Palitra_slider_2", palitra_value_2)) {
        colorWheel_2(palitra_value_2);
    }

    else if (ui.clickInt("White_slider", white_brightness)) {
      for (int i = 5 + changer1; i <= lights_pcs * 8; i = i + 8 + changer2) {
        dmx.write(i, white_brightness);
      }
    }
    else if (ui.clickInt("Function_slider", function_choice)) {
      if (flagRainbowLed == 0) {
        for (int i = 7 + changer1; i <= lights_pcs * 8; i = i + 8 + changer2) {
          dmx.write(i, function_choice);
        }
      }
    }
    else if (ui.click("Button_strobe")) {
      flag_strobe = !flag_strobe;
      if (flag_strobe == 1) {
        dmx.write(6, 0);
        dmx.write(14, 0);
      }
      else if (flag_strobe == 0) {
        dmx.write(6, strobe);
        dmx.write(14, strobe);
      }
    }

    else if (ui.clickInt("Strobe_slider", strobe)) {
      if (flag_strobe == 0) {
        for (int i = 6 + changer1; i <= lights_pcs * 8; i = i + 8 + changer2) {
          dmx.write(i, strobe);
        }
      }
    }
  }
}
void loop() {
  ui.tick();
  dmx.update();
}

void colorWheel_1(int color) {
  byte _r, _g, _b;
  if (color <= 255) {                       // красный макс, зелёный растёт
    _r = 255;
    _g = color;
    _b = 0;
  }
  else if (color > 255 && color <= 510) {   // зелёный макс, падает красный
    _r = 510 - color;
    _g = 255;
    _b = 0;
  }
  else if (color > 510 && color <= 765) {   // зелёный макс, растёт синий
    _r = 0;
    _g = 255;
    _b = color - 510;
  }
  else if (color > 765 && color <= 1020) {  // синий макс, падает зелёный
    _r = 0;
    _g = 1020 - color;
    _b = 255;
  }
  else if (color > 1020 && color <= 1275) {   // синий макс, растёт красный
    _r = color - 1020;
    _g = 0;
    _b = 255;
  }
  else if (color > 1275 && color <= 1530) { // красный макс, падает синий
    _r = 255;
    _g = 0;
    _b = 1530 - color;
  }
  dmx.write(2, 255 - _r);
  dmx.write(3, 255 - _g);
  dmx.write(4, 255 - _b);
}

void colorWheel_2(int color) {
  byte _r, _g, _b;
  if (color <= 255) {                       // красный макс, зелёный растёт
    _r = 255;
    _g = color;
    _b = 0;
  }
  else if (color > 255 && color <= 510) {   // зелёный макс, падает красный
    _r = 510 - color;
    _g = 255;
    _b = 0;
  }
  else if (color > 510 && color <= 765) {   // зелёный макс, растёт синий
    _r = 0;
    _g = 255;
    _b = color - 510;
  }
  else if (color > 765 && color <= 1020) {  // синий макс, падает зелёный
    _r = 0;
    _g = 1020 - color;
    _b = 255;
  }
  else if (color > 1020 && color <= 1275) {   // синий макс, растёт красный
    _r = color - 1020;
    _g = 0;
    _b = 255;
  }
  else if (color > 1275 && color <= 1530) { // красный макс, падает синий
    _r = 255;
    _g = 0;
    _b = 1530 - color;
  }
  dmx.write(10, 255 - _r);
  dmx.write(11, 255 - _g);
  dmx.write(12, 255 - _b);
}
