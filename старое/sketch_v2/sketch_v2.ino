#include <ESPDMX.h>  // https://github.com/Rickgg/ESP-Dmx
DMXESPSerial dmx;

// Connect GPIO02 - TDX1 to MAX3485. D4
#define pinOut 9        //указать пин управления, где есть шим

#include <GyverPortal.h> // https://github.com/GyverLibs/GyverPortal
GyverPortal ui;

struct Data {
  bool white_led_flag;
  bool white_led_flag_check;
  bool rainbow_led_flag;
  byte mainBrightness_value;
  int palitra1_value;
  byte red1_value;
  byte green1_value;
  byte blue1_value;
  int palitra2_value;
  byte red2_value;
  byte green2_value;
  byte blue2_value;
  byte white_value;
  byte white_temp;
  byte rainbow_value;
  byte rainbow_temp;
  bool strobe_led_flag;
  byte strobe_value;
  byte strobe_temp;
  char user_ssid[25];
  char user_password[25];
};
Data data;

#include <EEManager.h>
EEManager memory(data);

byte step_slider = 5;
byte rain_min = 211;
byte white_temp;
byte connectionAttempts = 0;
byte r1, g1, b1, r2, g2, b2;

void setup() {
  EEPROM.begin(150);  // выделить память (больше или равно размеру даты)
  memory.begin(0, 'a');
  startup();
  dmx.init(16);
  dmx.write(1, data.mainBrightness_value);
  dmx.write(2, data.red1_value);
  dmx.write(3, data.green1_value);
  dmx.write(4, data.blue1_value);
  dmx.write(5, data.white_value);
  dmx.write(6, data.strobe_value);
  dmx.write(7, data.rainbow_value);

  dmx.write(9, data.mainBrightness_value);
  dmx.write(10, data.red2_value);
  dmx.write(11, data.green2_value);
  dmx.write(12, data.blue2_value);
  dmx.write(13, data.white_value);
  dmx.write(14, data.strobe_value);
  dmx.write(15, data.rainbow_value);
  dmx.update();
  data.rainbow_value = data.rainbow_temp;
  data.strobe_value = data.strobe_temp;
  ui.start();
  ui.attachBuild(build);
  ui.attach(action);
}

void build() {
  GP.BUILD_BEGIN(GP_DARK);
  GP.PAGE_TITLE("DMX WiFi-controller");
  GP.NAV_TABS("Home,Settings");

  GP.NAV_BLOCK_BEGIN();
  M_BOX(GP.BUTTON_MINI("Reset_button", "Reset", "", GP_GRAY, "", 0, 1);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("White_button", "Білий", "", GP_GRAY_B, "", 0, 1);
        GP.LED_GREEN("White_led", data.white_led_flag);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("Rainbow_button", "Радуга", "", GP_PINK, "", 0, 1);
        GP.LED_GREEN("Rainbow_led", data.rainbow_led_flag);
       );

  M_BLOCK_THIN(
    M_BOX(GP.LABEL("Головна яскравість"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("MainBrightness_slider", data.mainBrightness_value, 0, 255, step_slider, 0, GP_VIOL); );   // главная яркость
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палітра 1"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Palitra1_slider", data.palitra1_value, 0, 1530, step_slider + 1, 0, GP_ORANGE); );
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палітра 2"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Palitra2_slider", data.palitra2_value, 0, 1530, step_slider + 1, 0, GP_BLUE); );
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Білий"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("White_slider", data.white_value, 0, 255, step_slider, 0, GP_GRAY_B); );         // яркость W
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Радуга"); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Rainbow_slider", data.rainbow_value, rain_min, 255, 1, 0, GP_PINK); );            // функциональный выбор
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Стробоскоп"); GP.BUTTON_MINI("Strobe_button", "OnOff", "", GP_YELLOW, "", 0, 1); GP.LED_RED("Strobe_led", data.strobe_led_flag); GP.BREAK(); );
    M_BOX(GP.SLIDER_C("Strobe_slider", data.strobe_value, 0, 255, step_slider, 0, GP_YELLOW); );                // стробоскоп
  );
  GP.NAV_BLOCK_END();

  GP.NAV_BLOCK_BEGIN();
  GP.TITLE("Підключення до WiFi");
  GP.FORM_BEGIN("/user_wifi");     // начать форму, передать имя
  GP.TEXT("ssid_text", "Назва:", data.user_ssid); // ввод текста, подсказка Login, текста нет
  GP.BREAK();                  // перенос строки
  GP.PASS_EYE("password_text", "Пароль:", data.user_password); // ввод текста, подсказка Login, текста нет
  GP.BREAK();                  // перенос строки
  GP.SUBMIT("Oк");         // кнопка Submit
  GP.FORM_END();
  GP.NAV_BLOCK_END();

  GP.ONLINE_CHECK();
  GP.BUILD_END();
}

void action() {
  if (ui.form()) {
    if (ui.form("/user_wifi")) {
      ui.copyStr("ssid_text", data.user_ssid, 25);
      ui.copyStr("password_text", data.user_password, 25);
    }
  }

  if (ui.click()) {
    if (ui.click("Reset_button")) {
      data.white_led_flag = 0;
      data.rainbow_led_flag = 0;
      data.palitra1_value = 0;
      colorWheel_1(data.palitra1_value);
      data.palitra2_value = 0;
      colorWheel_2(data.palitra2_value);
      data.white_value = 0;
      dmx.write(5, data.white_value);
      dmx.write(13, data.white_value);
      data.rainbow_value = 0;
      dmx.write(7, data.rainbow_value);
      dmx.write(15, data.rainbow_value);
      data.strobe_led_flag = 0;
      data.strobe_value = 0;
      dmx.write(6, data.strobe_value);
      dmx.write(14, data.strobe_value);

    }

    else if (ui.click("White_button")) {
      data.white_led_flag = !data.white_led_flag;
      if (data.white_led_flag) {
        if (data.rainbow_led_flag) {
          data.rainbow_led_flag = 0;
          data.rainbow_value = 0;
          data.white_led_flag_check = 1;
          dmx.write(7, 0);
          dmx.write(15, 0);
        }
        data.white_temp = data.white_value;
        data.white_value = 255;
        data.red1_value = 255;
        data.green1_value = 255;
        data.blue1_value = 255;
        data.red2_value = 255;
        data.green2_value = 255;
        data.blue2_value = 255;

        dmx.write(2, data.red1_value);
        dmx.write(3, data.green1_value);
        dmx.write(4, data.blue1_value);
        dmx.write(5, data.white_value);
        dmx.write(10, data.red2_value);
        dmx.write(11, data.green2_value);
        dmx.write(12, data.blue2_value);
        dmx.write(13, data.white_value);
      }
      else {
        if (data.white_led_flag_check) {
          data.rainbow_led_flag = 1;
          data.rainbow_value = data.rainbow_temp;
          dmx.write(7, data.rainbow_value);
          dmx.write(15, data.rainbow_value);
          data.white_led_flag_check = 0;
        }
        colorWheel_1(data.palitra1_value);
        colorWheel_2(data.palitra2_value);
        data.white_value = data.white_temp;
        dmx.write(5, data.white_value);
        dmx.write(13, data.white_value);
      }
    }

    else if (ui.click("Rainbow_button")) {
      data.rainbow_led_flag = !data.rainbow_led_flag;
      if (data.rainbow_led_flag) {
        data.rainbow_value = data.rainbow_temp;
        dmx.write(7, data.rainbow_value);
        dmx.write(15, data.rainbow_value);
      }
      else {
        data.rainbow_value = 0;
        dmx.write(7, data.rainbow_value);
        dmx.write(15, data.rainbow_value);
      }
    }

    else if (ui.clickInt("MainBrightness_slider", data.mainBrightness_value)) {
      dmx.write(1, data.mainBrightness_value);
      dmx.write(9, data.mainBrightness_value);
    }

    else if (ui.clickInt("Palitra1_slider", data.palitra1_value)) {
      colorWheel_1(data.palitra1_value);
    }

    else if (ui.clickInt("Palitra2_slider", data.palitra2_value)) {
      colorWheel_2(data.palitra2_value);
    }

    else if (ui.clickInt("White_slider", data.white_value)) {
      dmx.write(5, data.white_value);
      dmx.write(13, data.white_value);
    }

    else if (ui.clickInt("Rainbow_slider", data.rainbow_value)) {
      data.rainbow_temp = data.rainbow_value;
      if (data.rainbow_led_flag) {
        dmx.write(7, data.rainbow_value);
        dmx.write(15, data.rainbow_value);
      }
    }

    else if (ui.click("Strobe_button")) {
      data.strobe_led_flag = !data.strobe_led_flag;
      if (data.strobe_led_flag) {
        data.strobe_value = data.strobe_temp;
        dmx.write(6, data.strobe_value);
        dmx.write(14, data.strobe_value);
      }
      else {
        data.strobe_value = 0;
        dmx.write(6, data.strobe_value);
        dmx.write(14, data.strobe_value);
      }
    }

    else if (ui.clickInt("Strobe_slider", data.strobe_value)) {
      data.strobe_temp = data.strobe_value;
      if (data.strobe_led_flag) {
        dmx.write(6, data.strobe_value);
        dmx.write(14, data.strobe_value);
      }
    }
    dmx.update();
    memory.update();
    //serial_print();
  }
}

void loop() {
  ui.tick();
  memory.tick();
}

void colorWheel_1(int color) {
  if (color <= 255) {                       // красный макс, зелёный растёт
    r1 = 255;
    g1 = color;
    b1 = 0;
  }
  else if (color > 255 && color <= 510) {   // зелёный макс, падает красный
    r1 = 510 - color;
    g1 = 255;
    b1 = 0;
  }
  else if (color > 510 && color <= 765) {   // зелёный макс, растёт синий
    r1 = 0;
    g1 = 255;
    b1 = color - 510;
  }
  else if (color > 765 && color <= 1020) {  // синий макс, падает зелёный
    r1 = 0;
    g1 = 1020 - color;
    b1 = 255;
  }
  else if (color > 1020 && color <= 1275) {   // синий макс, растёт красный
    r1 = color - 1020;
    g1 = 0;
    b1 = 255;
  }
  else if (color > 1275 && color <= 1530) { // красный макс, падает синий
    r1 = 255;
    g1 = 0;
    b1 = 1530 - color;
  }
  data.red1_value = 255 - r1;
  data.green1_value = 255 - g1;
  data.blue1_value = 255 - b1;
  dmx.write(2, data.red1_value);
  dmx.write(3, data.green1_value);
  dmx.write(4, data.blue1_value);
}

void colorWheel_2(int color) {
  if (color <= 255) {                       // красный макс, зелёный растёт
    r2 = 255;
    g2 = color;
    b2 = 0;
  }
  else if (color > 255 && color <= 510) {   // зелёный макс, падает красный
    r2 = 510 - color;
    g2 = 255;
    b2 = 0;
  }
  else if (color > 510 && color <= 765) {   // зелёный макс, растёт синий
    r2 = 0;
    g2 = 255;
    b2 = color - 510;
  }
  else if (color > 765 && color <= 1020) {  // синий макс, падает зелёный
    r2 = 0;
    g2 = 1020 - color;
    b2 = 255;
  }
  else if (color > 1020 && color <= 1275) {   // синий макс, растёт красный
    r2 = color - 1020;
    g2 = 0;
    b2 = 255;
  }
  else if (color > 1275 && color <= 1530) { // красный макс, падает синий
    r2 = 255;
    g2 = 0;
    b2 = 1530 - color;
  }
  data.red2_value = 255 - r2;
  data.green2_value = 255 - g2;
  data.blue2_value = 255 - b2;
  dmx.write(10, data.red2_value);
  dmx.write(11, data.green2_value);
  dmx.write(12, data.blue2_value);
}

void startup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(data.user_ssid, data.user_password);
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20) {
    delay(500);
    Serial.print(".");
    connectionAttempts++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("DMX WiFi-controller");
  }
  Serial.println(WiFi.localIP());
  //serial_print();
}

void serial_print() {
  Serial.print("      Главная яркость: ");
  Serial.println(data.mainBrightness_value);
  Serial.print("Белый LED: ");
  Serial.print(data.white_led_flag);
  Serial.print(data.white_led_flag_check);
  Serial.print("     Радуга LED: ");
  Serial.println(data.rainbow_led_flag);

  Serial.print("Палитра 1: ");
  Serial.print(data.palitra1_value);
  Serial.print("          Палитра 2: ");
  Serial.println(data.palitra2_value);
  Serial.print("Красный 1: ");
  Serial.print(data.red1_value);
  Serial.print("          Красный 2: ");
  Serial.println(data.red2_value);
  Serial.print("Зеленый 1: ");
  Serial.print(data.green1_value);
  Serial.print("          Зеленый 2: ");
  Serial.println(data.green2_value);
  Serial.print("  Синий 1: ");
  Serial.print(data.blue1_value);
  Serial.print("              Синий 2: ");
  Serial.println(data.blue2_value);
  Serial.print("     Белый: ");
  Serial.println(data.white_value);
  Serial.print("     Радуга: ");
  Serial.println(data.rainbow_value);
  Serial.print("     Стробоскоп: ");
  Serial.print(data.strobe_value);
  Serial.print("   Кнопка стр: ");
  Serial.println(data.strobe_led_flag);
}
