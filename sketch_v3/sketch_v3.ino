// Для прошивки Wemos D1 mini:
//  - тип платы: LOLIN(WEMOS) D1 R2 & mini

// Для прошивки ESP8266 node MCU v3:
//  - тип платы: NodeMCU 1.0 (ESP-12E Module)

#include <ESPDMX.h>  // https://github.com/Rickgg/ESP-Dmx
DMXESPSerial dmx;

// Connect GPIO02 - TDX1 to MAX3485. D4
#define pinOut 9        //указать пин управления, где есть шим

#include <GyverPortal.h> // https://github.com/GyverLibs/GyverPortal
GyverPortal ui;

struct Data {
  bool white_led_flag[4];
  bool white_led_flag_check[4];
  bool rainbow_led_flag[4];
  byte mainBrightness_value[4];
  int palitra1_value[4];
  byte red1_value[4];
  byte green1_value[4];
  byte blue1_value[4];
  int palitra2_value[4];
  byte red2_value[4];
  byte green2_value[4];
  byte blue2_value[4];
  byte white_value[4];
  byte white_temp[4];
  byte rainbow_value[4];
  byte rainbow_temp[4];
  bool strobe_led_flag[4];
  byte strobe_value[4];
  byte strobe_temp[4];
  char user_ssid[30];
  char user_password[30];
  char first_mode_description[30];
  char second_mode_description[30];
  char third_mode_description[30];
  char fourth_mode_description[30];
  byte num_of_mode;
  bool first_mode_led_flag;
  bool second_mode_led_flag;
  bool third_mode_led_flag;
  bool fourth_mode_led_flag;
};
Data data;

#include <EEManager.h>
EEManager memory(data, 3000);

byte step_slider = 5;

#define wifi_led 12  // D6
#define ap_led 13  // D7

void setup() {
  EEPROM.begin(memory.blockSize());  // выделить память (больше или равно размеру даты)
  memory.begin(0, 'b');
  startup();

  dmx.init(16);
  recall_all_channels();

  dmx.update();

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
        GP.LED_GREEN("White_led", data.white_led_flag[data.num_of_mode]);
        GP.LABEL_BLOCK("  ");
        GP.BUTTON_MINI("Rainbow_button", "Радуга", "", GP_PINK, "", 0, 1);
        GP.LED_GREEN("Rainbow_led", data.rainbow_led_flag[data.num_of_mode]);
       );

  M_BLOCK_THIN(
    M_BOX(GP.LABEL("Головна яскравість"); GP.BREAK(); );
    M_BOX(GP.SLIDER("MainBrightness_slider", data.mainBrightness_value[data.num_of_mode], 0, 255, step_slider, 0, GP_VIOL); );   // главная яркость
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палітра 1"); GP.BREAK(); );
    M_BOX(GP.SLIDER("Palitra1_slider", data.palitra1_value[data.num_of_mode], 0, 1530, step_slider + 4, 0, GP_ORANGE); );
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Палітра 2"); GP.BREAK(); );
    M_BOX(GP.SLIDER("Palitra2_slider", data.palitra2_value[data.num_of_mode], 0, 1530, step_slider + 4, 0, GP_BLUE); );
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Білий"); GP.BREAK(); );
    M_BOX(GP.SLIDER("White_slider", data.white_value[data.num_of_mode], 0, 255, step_slider, 0, GP_GRAY_B); );         // яркость W
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Радуга"); GP.BREAK(); );
    M_BOX(GP.SLIDER("Rainbow_slider", data.rainbow_value[data.num_of_mode], 211, 255, 2, 0, GP_PINK); );            // функциональный выбор
    M_BOX(GP.HR(); );
    M_BOX(GP.LABEL("Стробоскоп"); GP.BUTTON_MINI("Strobe_button", "OnOff", "", GP_YELLOW, "", 0, 1); GP.LED_RED("Strobe_led", data.strobe_led_flag[data.num_of_mode]); GP.BREAK(); );
    M_BOX(GP.SLIDER("Strobe_slider", data.strobe_value[data.num_of_mode], 0, 255, step_slider, 0, GP_YELLOW); );                // стробоскоп
  );

  M_BLOCK_THIN(
    M_BOX(
      GP.BUTTON_MINI("First_mode", "1 режим", "", GP_GRAY_B, "", 0, 1);
      GP.BREAK();
      GP.FORM_BEGIN("/first_mode");     // начать форму, передать имя
      GP.TEXT("first_mode_desc", "опис...", data.first_mode_description);
      //GP.BREAK();
      //GP.SUBMIT_MINI("save1");         // кнопка Submit
      GP.FORM_END();
      GP.LED_GREEN("First_mode_led", data.first_mode_led_flag);
    );
    M_BOX(GP.HR(); );
    M_BOX(
      GP.BUTTON_MINI("Second_mode", "2 режим", "", GP_GRAY_B, "", 0, 1);
      GP.BREAK();
      GP.FORM_BEGIN("/second_mode");     // начать форму, передать имя
      GP.TEXT("second_mode_desc", "опис...", data.second_mode_description);
      //GP.BREAK();
      //GP.SUBMIT_MINI("save2");         // кнопка Submit
      GP.FORM_END();
      GP.LED_GREEN("Second_mode_led", data.second_mode_led_flag);
    );
    M_BOX(GP.HR(); );
    M_BOX(
      GP.BUTTON_MINI("Third_mode", "3 режим", "", GP_GRAY_B, "", 0, 1);
      GP.BREAK();
      GP.FORM_BEGIN("/third_mode");     // начать форму, передать имя
      GP.TEXT("third_mode_desc", "опис...", data.third_mode_description);
      //GP.BREAK();
      //GP.SUBMIT_MINI("save3");         // кнопка Submit
      GP.FORM_END();
      GP.LED_GREEN("Third_mode_led", data.third_mode_led_flag);
    );
    M_BOX(GP.HR(); );
    M_BOX(
      GP.BUTTON_MINI("Fourth_mode", "4 режим", "", GP_GRAY_B, "", 0, 1);
      GP.BREAK();
      GP.FORM_BEGIN("/fourth_mode");     // начать форму, передать имя
      GP.TEXT("fourth_mode_desc", "опис...", data.fourth_mode_description);
      //GP.BREAK();
      //GP.SUBMIT_MINI("save4");         // кнопка Submit
      GP.FORM_END();
      GP.LED_GREEN("Fourth_mode_led", data.fourth_mode_led_flag);
    );
  );
  GP.NAV_BLOCK_END();

  GP.NAV_BLOCK_BEGIN();
  GP.TITLE("Підключення до WiFi");
  GP.FORM_BEGIN("/user_wifi");     // начать форму, передать имя
  GP.TEXT("ssid_text", "Назва:", data.user_ssid); // ввод текста, подсказка Назва, текста нет
  GP.BREAK();                  // перенос строки
  GP.PASS_EYE("password_text", "Пароль:", data.user_password); // ввод текста, подсказка Login, текста нет
  GP.BREAK();                  // перенос строки
  //GP.SPAN(myIp);
  //GP.BREAK();                  // перенос строки
  GP.SUBMIT("Oк");         // кнопка Submit
  GP.FORM_END();

  GP.BREAK();

  M_BLOCK_THIN(
    GP.TITLE("Як внести в пам'ять режима його налаштування:");
    GP.SPAN("1. Натиснути на кнопку режима. Має світитись зелений індикатор.");
    GP.BREAK();
    GP.SPAN("2. Робити будь-які налаштування і зачекати 3 секунди. Після цього все автоматично запишеться в активний режим. Для збереження нічого натискати не треба.");
    GP.BREAK();
    GP.SPAN("3. Щоб назвати режим, треба вписати назву у поле (не більше 20 символів) і на клавіатурі натиснути Enter. Сторінка перезавантажиться.");
    GP.BREAK();
    GP.HR();
    GP.PLAIN("*кожні 3 секунди в пам'ять АКТИВНОГО режима автоматично записуються будь-які зміни!");
  );
  GP.BREAK();

  /*M_BLOCK_THIN(
    GP.TITLE("Прошивка плати:");
    GP.SPAN("1. Перейти за адресою x.x.x.x/ota_update");
    GP.BREAK();
    GP.SPAN("2. Завантажити бінарний файл");
  );*/

  
  GP.NAV_BLOCK_END();

  GP.ONLINE_CHECK();
  GP.BUILD_END();
}

void recall_all_channels() {
  dmx.write(1, data.mainBrightness_value[data.num_of_mode]);
  dmx.write(2, data.red1_value[data.num_of_mode]);
  dmx.write(3, data.green1_value[data.num_of_mode]);
  dmx.write(4, data.blue1_value[data.num_of_mode]);
  dmx.write(5, data.white_value[data.num_of_mode]);
  
  dmx.write(9, data.mainBrightness_value[data.num_of_mode]);
  dmx.write(10, data.red2_value[data.num_of_mode]);
  dmx.write(11, data.green2_value[data.num_of_mode]);
  dmx.write(12, data.blue2_value[data.num_of_mode]);
  dmx.write(13, data.white_value[data.num_of_mode]);

  if (data.strobe_led_flag[data.num_of_mode]) {
    dmx.write(6, data.strobe_value[data.num_of_mode]);
    dmx.write(14, data.strobe_value[data.num_of_mode]);
  }

  if (data.rainbow_led_flag[data.num_of_mode]) {
    dmx.write(7, data.rainbow_value[data.num_of_mode]);
    dmx.write(15, data.rainbow_value[data.num_of_mode]);
  }
}

void action() {
  serial_print();
  if (ui.form()) {
    if (ui.form("/user_wifi")) {
      ui.copyStr("ssid_text", data.user_ssid, 25);
      ui.copyStr("password_text", data.user_password, 25);
    }

    else if (ui.form("/first_mode")) ui.copyStr("first_mode_desc", data.first_mode_description, 20);

    else if (ui.form("/second_mode")) ui.copyStr("second_mode_desc", data.second_mode_description, 20);

    else if (ui.form("/third_mode")) ui.copyStr("third_mode_desc", data.third_mode_description, 20);

    else if (ui.form("/fourth_mode")) ui.copyStr("fourth_mode_desc", data.fourth_mode_description, 20);

    memory.update();
  }

  if (ui.click()) {
    if (ui.click("Reset_button")) {
      data.white_led_flag[data.num_of_mode] = 0;
      data.rainbow_led_flag[data.num_of_mode] = 0;
      data.palitra1_value[data.num_of_mode] = 0;
      colorWheel_1(data.palitra1_value[data.num_of_mode]);
      data.palitra2_value[data.num_of_mode] = 0;
      colorWheel_2(data.palitra2_value[data.num_of_mode]);
      data.white_value[data.num_of_mode] = 0;
      dmx.write(5, data.white_value[data.num_of_mode]);
      dmx.write(13, data.white_value[data.num_of_mode]);
      data.rainbow_value[data.num_of_mode] = 0;
      dmx.write(7, data.rainbow_value[data.num_of_mode]);
      dmx.write(15, data.rainbow_value[data.num_of_mode]);
      data.strobe_led_flag[data.num_of_mode] = 0;
      data.strobe_value[data.num_of_mode] = 0;
      dmx.write(6, data.strobe_value[data.num_of_mode]);
      dmx.write(14, data.strobe_value[data.num_of_mode]);
    }

    else if (ui.click("First_mode")) {
      data.first_mode_led_flag = 1;
      data.second_mode_led_flag = 0;
      data.third_mode_led_flag = 0;
      data.fourth_mode_led_flag = 0;
      data.num_of_mode = 0;
      recall_all_channels();
    }

    else if (ui.click("Second_mode")) {
      data.first_mode_led_flag = 0;
      data.second_mode_led_flag = 1;
      data.third_mode_led_flag = 0;
      data.fourth_mode_led_flag = 0;
      data.num_of_mode = 1;
      recall_all_channels();
    }

    else if (ui.click("Third_mode")) {
      data.first_mode_led_flag = 0;
      data.second_mode_led_flag = 0;
      data.third_mode_led_flag = 1;
      data.fourth_mode_led_flag = 0;
      data.num_of_mode = 2;
      recall_all_channels();
    }

    else if (ui.click("Fourth_mode")) {
      data.first_mode_led_flag = 0;
      data.second_mode_led_flag = 0;
      data.third_mode_led_flag = 0;
      data.fourth_mode_led_flag = 1;
      data.num_of_mode = 3;
      recall_all_channels();
    }

    else if (ui.click("White_button")) {
      data.white_led_flag[data.num_of_mode] = !data.white_led_flag[data.num_of_mode];
      if (data.white_led_flag[data.num_of_mode]) {
        if (data.rainbow_led_flag[data.num_of_mode]) {
          data.rainbow_led_flag[data.num_of_mode] = 0;
          data.rainbow_value[data.num_of_mode] = 0;
          data.white_led_flag_check[data.num_of_mode] = 1;
          dmx.write(7, 0);
          dmx.write(15, 0);
        }
        data.white_temp[data.num_of_mode] = data.white_value[data.num_of_mode];
        data.white_value[data.num_of_mode] = 255;
        data.red1_value[data.num_of_mode] = 255;
        data.green1_value[data.num_of_mode] = 255;
        data.blue1_value[data.num_of_mode] = 255;
        data.red2_value[data.num_of_mode] = 255;
        data.green2_value[data.num_of_mode] = 255;
        data.blue2_value[data.num_of_mode] = 255;

        dmx.write(2, data.red1_value[data.num_of_mode]);
        dmx.write(3, data.green1_value[data.num_of_mode]);
        dmx.write(4, data.blue1_value[data.num_of_mode]);
        dmx.write(5, data.white_value[data.num_of_mode]);
        dmx.write(10, data.red2_value[data.num_of_mode]);
        dmx.write(11, data.green2_value[data.num_of_mode]);
        dmx.write(12, data.blue2_value[data.num_of_mode]);
        dmx.write(13, data.white_value[data.num_of_mode]);
      }
      else {
        if (data.white_led_flag_check[data.num_of_mode]) {
          data.rainbow_led_flag[data.num_of_mode] = 1;
          data.rainbow_value[data.num_of_mode] = data.rainbow_temp[data.num_of_mode];
          dmx.write(7, data.rainbow_value[data.num_of_mode]);
          dmx.write(15, data.rainbow_value[data.num_of_mode]);
          data.white_led_flag_check[data.num_of_mode] = 0;
        }
        colorWheel_1(data.palitra1_value[data.num_of_mode]);
        colorWheel_2(data.palitra2_value[data.num_of_mode]);
        data.white_value[data.num_of_mode] = data.white_temp[data.num_of_mode];
        dmx.write(5, data.white_value[data.num_of_mode]);
        dmx.write(13, data.white_value[data.num_of_mode]);
      }
    }

    else if (ui.click("Rainbow_button")) {
      data.rainbow_led_flag[data.num_of_mode] = !data.rainbow_led_flag[data.num_of_mode];
      if (data.rainbow_led_flag[data.num_of_mode]) {
        data.rainbow_value[data.num_of_mode] = data.rainbow_temp[data.num_of_mode];
        dmx.write(7, data.rainbow_value[data.num_of_mode]);
        dmx.write(15, data.rainbow_value[data.num_of_mode]);
      }
      else {
        data.rainbow_value[data.num_of_mode] = 0;
        dmx.write(7, data.rainbow_value[data.num_of_mode]);
        dmx.write(15, data.rainbow_value[data.num_of_mode]);
      }
    }


    else if (ui.clickInt("MainBrightness_slider", data.mainBrightness_value[data.num_of_mode])) {
      dmx.write(1, data.mainBrightness_value[data.num_of_mode]);
      dmx.write(9, data.mainBrightness_value[data.num_of_mode]);
    }

    else if (ui.clickInt("Palitra1_slider", data.palitra1_value[data.num_of_mode])) {
      colorWheel_1(data.palitra1_value[data.num_of_mode]);
    }

    else if (ui.clickInt("Palitra2_slider", data.palitra2_value[data.num_of_mode])) {
      colorWheel_2(data.palitra2_value[data.num_of_mode]);
    }

    else if (ui.clickInt("White_slider", data.white_value[data.num_of_mode])) {
      dmx.write(5, data.white_value[data.num_of_mode]);
      dmx.write(13, data.white_value[data.num_of_mode]);
    }

    else if (ui.clickInt("Rainbow_slider", data.rainbow_value[data.num_of_mode])) {
      data.rainbow_temp[data.num_of_mode] = data.rainbow_value[data.num_of_mode];
      if (data.rainbow_led_flag[data.num_of_mode]) {
        dmx.write(7, data.rainbow_value[data.num_of_mode]);
        dmx.write(15, data.rainbow_value[data.num_of_mode]);
      }
    }

    else if (ui.click("Strobe_button")) {
      data.strobe_led_flag[data.num_of_mode] = !data.strobe_led_flag[data.num_of_mode];
      if (data.strobe_led_flag[data.num_of_mode]) {
        data.strobe_value[data.num_of_mode] = data.strobe_temp[data.num_of_mode];
        dmx.write(6, data.strobe_value[data.num_of_mode]);
        dmx.write(14, data.strobe_value[data.num_of_mode]);
      }
      else {
        data.strobe_value[data.num_of_mode] = 0;
        dmx.write(6, data.strobe_value[data.num_of_mode]);
        dmx.write(14, data.strobe_value[data.num_of_mode]);
      }
    }

    else if (ui.clickInt("Strobe_slider", data.strobe_value[data.num_of_mode])) {
      data.strobe_temp[data.num_of_mode] = data.strobe_value[data.num_of_mode];
      if (data.strobe_led_flag[data.num_of_mode]) {
        dmx.write(6, data.strobe_value[data.num_of_mode]);
        dmx.write(14, data.strobe_value[data.num_of_mode]);
      }
    }
    memory.update();
    dmx.update();
    //serial_print();
  }
}

void loop() {
  ui.tick();
  memory.tick();
}

void colorWheel_1(int color) {
  byte r1, g1, b1;
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
  data.red1_value[data.num_of_mode] = 255 - r1;
  data.green1_value[data.num_of_mode] = 255 - g1;
  data.blue1_value[data.num_of_mode] = 255 - b1;
  dmx.write(2, data.red1_value[data.num_of_mode]);
  dmx.write(3, data.green1_value[data.num_of_mode]);
  dmx.write(4, data.blue1_value[data.num_of_mode]);
}

void colorWheel_2(int color) {
  byte r2, g2, b2;
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
  data.red2_value[data.num_of_mode] = 255 - r2;
  data.green2_value[data.num_of_mode] = 255 - g2;
  data.blue2_value[data.num_of_mode] = 255 - b2;
  dmx.write(10, data.red2_value[data.num_of_mode]);
  dmx.write(11, data.green2_value[data.num_of_mode]);
  dmx.write(12, data.blue2_value[data.num_of_mode]);
}

void startup() {
  Serial.begin(115200);
  pinMode(wifi_led, OUTPUT);
  pinMode(ap_led, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(data.user_ssid, data.user_password);
  byte connectionAttempts = 0;
  digitalWrite(ap_led, LOW);
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 25) {
    Serial.print(".");
    connectionAttempts++;
    delay(100);
    digitalWrite(wifi_led, LOW);
    delay(400);
    digitalWrite(wifi_led, HIGH);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("DMX WiFi-controller");
    digitalWrite(ap_led, HIGH);
    digitalWrite(wifi_led, LOW);
  }
  Serial.println(WiFi.localIP());
  serial_print();
}

void serial_print() {
  Serial.println("      Номер мода: ");
  Serial.println(data.num_of_mode);
  Serial.print("      Главная яркость: ");
  Serial.println(data.mainBrightness_value[data.num_of_mode]);
  Serial.print("Белый LED: ");
  Serial.print(data.white_led_flag[data.num_of_mode]);
  Serial.print(data.white_led_flag_check[data.num_of_mode]);
  Serial.print("     Радуга LED: ");
  Serial.println(data.rainbow_led_flag[data.num_of_mode]);

  Serial.print("Палитра 1: ");
  Serial.print(data.palitra1_value[data.num_of_mode]);
  Serial.print("          Палитра 2: ");
  Serial.println(data.palitra2_value[data.num_of_mode]);
  Serial.print("Красный 1: ");
  Serial.print(data.red1_value[data.num_of_mode]);
  Serial.print("          Красный 2: ");
  Serial.println(data.red2_value[data.num_of_mode]);
  Serial.print("Зеленый 1: ");
  Serial.print(data.green1_value[data.num_of_mode]);
  Serial.print("          Зеленый 2: ");
  Serial.println(data.green2_value[data.num_of_mode]);
  Serial.print("  Синий 1: ");
  Serial.print(data.blue1_value[data.num_of_mode]);
  Serial.print("              Синий 2: ");
  Serial.println(data.blue2_value[data.num_of_mode]);
  Serial.print("     Белый: ");
  Serial.println(data.white_value[data.num_of_mode]);
  Serial.print("     Радуга: ");
  Serial.println(data.rainbow_value[data.num_of_mode]);
  Serial.print("     Стробоскоп: ");
  Serial.print(data.strobe_value[data.num_of_mode]);
  Serial.print("   Кнопка стр: ");
  Serial.println(data.strobe_led_flag[data.num_of_mode]);
  }
