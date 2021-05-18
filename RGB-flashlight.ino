//RGB Flashlight

// пины для подключения шим
#define R_LED 6  //Красный
#define G_LED 5  //Зеленый
#define B_LED 3  //Синий

// пины кнопок управления
#define R_BTN 9    //Красный                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
#define G_BTN 10   //Зеленый
#define B_BTN 11   //Синий
#define MODE_BTN 8   // кнопка смены режима работы
#define POT_BTN 0   // потенциометр А0

//Тайминги и количества
#define LED_RENEW 300           // тайминг смены режимов
#define BTN_PROTECT 100         // защита дребезга кнопки 
#define MODE_CNT 6              // количество режимов работы

//объявление переменных
byte R_BRIGHT, G_BRIGHT, B_BRIGHT, F_BRIGHT,RB_BRIGHT, GB_BRIGHT, BB_BRIGHT; //яркость свечения каждого пина\фонаря\блютуз пинов
byte R_MNL, G_MNL, B_MNL, C_MNL, W_MNL; //счетчики увеличения яркости
byte R_BtnVal, G_BtnVal, B_BtnVal, mode_BtnVal, mode; //считывание состояний кнопок
int LEDcolor, k = 0; //переменные
int state = 0; //переменные
long now_millis, led_millis, r_millis, g_millis, b_millis, mode_millis; //счетчики таймеров
String modes[MODE_CNT] = {"MANUALRGB","RAINBOW","RANDOM","COLD","WARM","BT"}; // массив названий режимов 

// Функция задания базовых настроек
void setup()
{

  Serial.begin(9600); // Скорость передачи для блютуз модуля
  randomSeed(analogRead(0));  // включение случайного random

  // задание выхода на транзисторы
  pinMode(R_LED, OUTPUT); 
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);

  //задание входа на кнопки управления
  pinMode(R_BTN, INPUT_PULLUP);
  pinMode(G_BTN, INPUT_PULLUP);
  pinMode(B_BTN, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP); 
  pinMode(POT_BTN, INPUT);  
}
//*********************************************
//функция выключения светодиода
void awhiteoff() {
  R_BRIGHT = 0; //яркость красного
  G_BRIGHT = 0;  //яркость зеленого
  B_BRIGHT = 0;  //яркость синего
   //запись аналогового сигнала яркости в светодиоды
  analogWrite(R_LED, R_BRIGHT); 
  analogWrite(G_LED, G_BRIGHT);
  analogWrite(B_LED, B_BRIGHT);
}
//основная цикличная функция программы
void loop()
{
  now_millis = millis(); //функция времени работы
  // считываем двоичный сигнал состояния кнопок
  R_BtnVal = digitalRead(R_BTN);
  G_BtnVal = digitalRead(G_BTN);
  B_BtnVal = digitalRead(B_BTN);
  mode_BtnVal = digitalRead(MODE_BTN);
  
  // анализ нажатия кнопок и защита от дребезга
  if ((R_BtnVal == LOW) & (now_millis - r_millis)> BTN_PROTECT) { 
    R_MNL = (R_MNL + 1) % 6;
    R_BRIGHT = 51 * R_MNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (R)
    r_millis = now_millis + 300;
  }
  if ((G_BtnVal == LOW) & (now_millis - g_millis)> BTN_PROTECT) { 
    G_MNL = (G_MNL + 1) % 6;
    G_BRIGHT = 51 * G_MNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (G)
    g_millis = now_millis + 300;
  }
  if ((B_BtnVal == LOW) & (now_millis - b_millis)> BTN_PROTECT) { 
    B_MNL = (B_MNL + 1) % 6;
    B_BRIGHT = 51 * B_MNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (B)
    b_millis = now_millis + 300;
  }
  if ((mode_BtnVal == LOW) & (now_millis - mode_millis)> BTN_PROTECT) { 
    mode = (mode + 1) % MODE_CNT; //каждое нажатие сменяет режим работы
    awhiteoff(); //сброс свечения перед каждым режимом
    mode_millis = now_millis + 300;
  }

  //Выбор режимов работы
  switch (mode) {
  case 0: // ручной режим (управление каждым светодиодом нажатием на на кнопку)
    if (now_millis - led_millis > LED_RENEW) { //защита от дребезга
      analogWrite(R_LED, R_BRIGHT); //запись состояния яркости в светодиоды
      analogWrite(G_LED, G_BRIGHT);
      analogWrite(B_LED, B_BRIGHT);
      led_millis = now_millis;
    }    
    break;
  case 1: // радуга (смена цветов от красного к синему через потенциометр
    if (now_millis - led_millis > LED_RENEW) { //защита от дребезга
      LEDcolor = analogRead(POT_BTN); //чтение напряжения с потенциометра (от 0 до 1023)
      if (LEDcolor <= 250) { 
        k = map(LEDcolor, 0, 250, 0, 255); //перевод диапазона напряжений в байтный 0-255
        R_BRIGHT = 0; G_BRIGHT = k; B_BRIGHT = 255; //прибавление зеленого
      } 
      if (LEDcolor > 250 && LEDcolor <= 500) { 
        k = map(LEDcolor, 250, 500, 0, 255); //перевод диапазона напряжений в байтный 0-255
        R_BRIGHT = 0; G_BRIGHT = 255; B_BRIGHT = 255 - k; //убавление синего
      } 
      if (LEDcolor > 500 && LEDcolor <= 750) { 
        k = map(LEDcolor, 500, 750, 0, 255); //перевод диапазона напряжений в байтный 0-255
        R_BRIGHT = k; G_BRIGHT = 255; B_BRIGHT = 0;  //прибавление красного
      } 
      if (LEDcolor >= 750) { 
        k = map(LEDcolor, 750, 1023, 0, 255); //перевод диапазона напряжений в байтный 0-255
        R_BRIGHT = 255; G_BRIGHT = 255 - k; B_BRIGHT = 0; //убавление зеленого
      }  
      //запись в светодиоды
      analogWrite(R_LED, R_BRIGHT);
      analogWrite(G_LED, G_BRIGHT);
      analogWrite(B_LED, B_BRIGHT);
      led_millis = now_millis; 
    }
    break;
  case 2: // случайный цвет (автоматическое переключение случайного цвета из всего диапазона)
    if (now_millis - led_millis > 200 + LEDcolor) { //защита от дребезга
      R_BRIGHT = random(6)*51; //выбор случайного числа от 0 до 255 с шагом 51
      G_BRIGHT = random(6)*51;
      B_BRIGHT = random(6)*51;
      analogWrite(R_LED, R_BRIGHT); //запись в светодиоды
      analogWrite(G_LED, G_BRIGHT);
      analogWrite(B_LED, B_BRIGHT);
      led_millis = now_millis; 
    }
    break;
    case 3: // холодный свет (регулировка света от 0;0;0 до 255;255;255 потенциометром
   if (now_millis - led_millis > LED_RENEW) { //защита от дребезга
      LEDcolor = analogRead(POT_BTN); //считывание напряжения потенциометра
      LEDcolor = LEDcolor/4; //перевод в байтовое 0-255 значение
      analogWrite(R_LED, LEDcolor); //запись в светодиоды
      analogWrite(G_LED, LEDcolor);
      analogWrite(B_LED, LEDcolor);
      led_millis = now_millis; }
  break;
  case 4: // теплый свет (регулировка света от 0;0;0  до 255;200;30 потенциометром)
   if (now_millis - led_millis > LED_RENEW) { //защита от дребезга
      LEDcolor = analogRead(POT_BTN); //считывание напряжения потенциометра
      LEDcolor = LEDcolor/4; //перевод в байтовое 0-255 значение
      analogWrite(R_LED, 1*LEDcolor); //запись в светодиоды
      analogWrite(G_LED, 0.784*LEDcolor); 
      analogWrite(B_LED, 0.117*LEDcolor);
      led_millis = now_millis; }
  break;
   case 5: // блютуз (режим управления со смартфона)
   //чтение сигнала из терминала смартфона
     if (Serial.available() > 0) {
    state = Serial.read(); } //state - цифра, заданная пользователем на режим
    //режим регулировки R LED по аналогу с ручным
     if (state == '1') {
    R_MNL = (R_MNL + 1) % 6;
    R_BRIGHT = 51 * R_MNL;
    analogWrite(R_LED, R_BRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости
    state = 0;
  }
   //режим регулировки G LED по аналогу с ручным
  if (state == '2') {
    G_MNL = (G_MNL + 1) % 6;
    G_BRIGHT = 51 * G_MNL;
    analogWrite(G_LED, G_BRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости 
    state = 0;
  }
  //режим регулировки B LED по аналогу с ручным
  if (state == '3') {
    B_MNL = (B_MNL + 1) % 6;
    B_BRIGHT = 51 * B_MNL;
    analogWrite(B_LED, B_BRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости
    state = 0;
  }
   //режим случайных цветов по аналогу с ручным
  if (state == '4') {
      R_BRIGHT = random(6)*51;
      G_BRIGHT = random(6)*51;
      B_BRIGHT = random(6)*51;
      analogWrite(R_LED, R_BRIGHT);
      analogWrite(G_LED, G_BRIGHT);
      analogWrite(B_LED, B_BRIGHT);
      state=0;
  }
   //режим радуги, каждая отправка 4 меняет цвет в диапазоне от красного к синему
  if (state == '5') {
if ((GB_BRIGHT<255) && (BB_BRIGHT==0)&& (RB_BRIGHT==255)) { //если горит красный, прибавлять зеленый
     G_MNL = (G_MNL + 1) % 6;
    GB_BRIGHT = 51 * G_MNL;
    analogWrite(G_LED, GB_BRIGHT);}
    else if ((GB_BRIGHT == 255) && (BB_BRIGHT==0)&& (RB_BRIGHT >0)) { //если горит красный и зеленый, убавлять красный
      R_MNL = (R_MNL + 1) % 6;
    RB_BRIGHT = 255 -(51 * R_MNL);
    analogWrite (R_LED, RB_BRIGHT);}
    else if ((GB_BRIGHT == 255) && (BB_BRIGHT<255)&& (RB_BRIGHT ==0)) { //если горит зеленый и не горит красный и синий, прибавлять синий
      B_MNL = (B_MNL + 1) % 6;
    BB_BRIGHT = 51 * B_MNL;
    analogWrite (B_LED, BB_BRIGHT);}
    else if ((GB_BRIGHT >0) && (BB_BRIGHT==255)&& (RB_BRIGHT ==0)) { //если горит синий и зеленый, убавлвлять зеленый
      G_MNL = (G_MNL + 1) % 6;
    GB_BRIGHT =255-(51 * G_MNL);
    analogWrite (G_LED, GB_BRIGHT);}
    else { //если светодиод не горит задать красный цвет
    RB_BRIGHT=255;
       GB_BRIGHT=0;
          BB_BRIGHT=0;
    analogWrite (R_LED, RB_BRIGHT);
    analogWrite (G_LED, GB_BRIGHT);
    analogWrite (B_LED, BB_BRIGHT);}
  state=0;}
   //режим холодного цвета, каждая отправка 6 прибавляет яркость +51 от до 255;255;255 
  if (state=='6') {
  C_MNL = (C_MNL + 1) % 6;
    F_BRIGHT = 51 * C_MNL;
      analogWrite(R_LED, F_BRIGHT);
      analogWrite(G_LED, F_BRIGHT);
      analogWrite(B_LED, F_BRIGHT); 
      state = 0;}
      //режим теплого цвета, каждая отправка 7 прибавляет яркость +51 до 255;200;30
       if (state=='7') {
  W_MNL = (W_MNL + 1) % 6;
    F_BRIGHT = 51 * W_MNL;
      analogWrite(R_LED, 1*F_BRIGHT);
      analogWrite(G_LED, 0.784*F_BRIGHT);
      analogWrite(B_LED, 0.117*F_BRIGHT); 
      state = 0;}
      //режим выключения светодиода 
else if (state == '0') {
    awhiteoff();
    state = 0;
  }
  break;
  }
}
