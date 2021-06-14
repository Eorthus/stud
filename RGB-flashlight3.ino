//RGB Flashlight

// пины для подключения шим
#define RLED 6  //Красный
#define GLED 5  //Зеленый
#define BLED 3  //Синий

// пины кнопок управления
#define RBT 9    //Красный                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
#define GBT 10   //Зеленый
#define BBT 11   //Синий
#define MODEBT 8   // кнопка смены режима работы
#define POTBT 0   // потенциометр А0

//Тайминги и количества
#define LEDTIME 300           // тайминг смены режимов
#define BTTIME 100         // защита дребезга кнопки 
#define MODECOL 6              // количество режимов работы

//объявление переменных
byte RBRIGHT, GBRIGHT, BBRIGHT, FBRIGHT,RBBRIGHT, GBBRIGHT, BBBRIGHT; //яркость свечения каждого пина\фонаря\блютуз пинов
byte RMNL, GMNL, BMNL, CMNL, WMNL; //счетчики увеличения яркости
byte RVAL, GVAL, BVAL, MODEVAL, MODE; //считывание состояний кнопок
int color, k = 0; //переменные
int state = 0; //переменные
long nowmillis, ledmillis, rmillis, gmillis, bmillis, modemillis; //счетчики таймеров

// Функция задания базовых настроек
void setup()
{
  Serial.begin(9600); // Скорость передачи для блютуз модуля
  randomSeed(analogRead(0));  // включение случайного random

  // задание выхода на транзисторы
  pinMode(RLED, OUTPUT); 
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);

  //задание входа на кнопки управления
  pinMode(RBT, INPUT_PULLUP);
  pinMode(GBT, INPUT_PULLUP);
  pinMode(BBT, INPUT_PULLUP);
  pinMode(MODEBT, INPUT_PULLUP); 
  pinMode(POTBT, INPUT);  
}
//*********************************************
//функция выключения светодиода
void whiteoff() {
  RBRIGHT = 0; //яркость красного
  GBRIGHT = 0;  //яркость зеленого
  BBRIGHT = 0;  //яркость синего
   //запись аналогового сигнала яркости в светодиоды
  analogWrite(RLED, RBRIGHT); 
  analogWrite(GLED, GBRIGHT);
  analogWrite(BLED, BBRIGHT);
}
//основная цикличная функция программы
void loop()
{
  nowmillis = millis(); //функция времени работы
  // считываем двоичный сигнал состояния кнопок
  RVAL = digitalRead(RBT);
  GVAL = digitalRead(GBT);
  BVAL = digitalRead(BBT);
  MODEVAL = digitalRead(MODEBT);
  
  // анализ нажатия кнопок и защита от дребезга
  if ((RVAL == LOW) & (nowmillis - rmillis)> BTTIME) { 
    RMNL = (RMNL + 1) % 6;
    RBRIGHT = 51 * RMNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (R)
    rmillis = nowmillis + 300;
  }
  if ((GVAL == LOW) & (nowmillis - gmillis)> BTTIME) { 
    GMNL = (GMNL + 1) % 6;
    GBRIGHT = 51 * GMNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (G)
    gmillis = nowmillis + 300;
  }
  if ((BVAL == LOW) & (nowmillis - bmillis)> BTTIME) { 
    BMNL = (BMNL + 1) % 6;
    BBRIGHT = 51 * BMNL; //каждое нажатие прибавляет 51 (из 255) к яркости свечения светодиода (B)
    bmillis = nowmillis + 300;
  }
  if ((MODEVAL == LOW) & (nowmillis - modemillis)> BTTIME) { 
    mode = (mode + 1) % MODECOL; //каждое нажатие сменяет режим работы
    whiteoff(); //сброс свечения перед каждым режимом
    modemillis = nowmillis + 300;
  }

  //Выбор режимов работы
  switch (mode) {
  case 0: // ручной режим (управление каждым светодиодом нажатием на на кнопку)
    if (nowmillis - ledmillis > LEDTIME) { //защита от дребезга
      analogWrite(RLED, RBRIGHT); //запись состояния яркости в светодиоды
      analogWrite(GLED, GBRIGHT);
      analogWrite(BLED, BBRIGHT);
      ledmillis = nowmillis;
    }    
    break;
  case 1: // радуга (смена цветов от красного к синему через потенциометр
    if (nowmillis - ledmillis > LEDTIME) { //защита от дребезга
      color = analogRead(POTBT); //чтение напряжения с потенциометра (от 0 до 1023)
      if (color <= 250) { 
        k = map(color, 0, 250, 0, 255); //перевод диапазона напряжений в байтный 0-255
        RBRIGHT = 0; GBRIGHT = k; BBRIGHT = 255; //прибавление зеленого
      } 
      if (color > 250 && color <= 500) { 
        k = map(color, 250, 500, 0, 255); //перевод диапазона напряжений в байтный 0-255
        RBRIGHT = 0; GBRIGHT = 255; BBRIGHT = 255 - k; //убавление синего
      } 
      if (color > 500 && color <= 750) { 
        k = map(color, 500, 750, 0, 255); //перевод диапазона напряжений в байтный 0-255
        RBRIGHT = k; GBRIGHT = 255; BBRIGHT = 0;  //прибавление красного
      } 
      if (color >= 750) { 
        k = map(color, 750, 1023, 0, 255); //перевод диапазона напряжений в байтный 0-255
        RBRIGHT = 255; GBRIGHT = 255 - k; BBRIGHT = 0; //убавление зеленого
      }  
      //запись в светодиоды
      analogWrite(RLED, RBRIGHT);
      analogWrite(GLED, GBRIGHT);
      analogWrite(BLED, BBRIGHT);
      ledmillis = nowmillis; 
    }
    break;
  case 2: // случайный цвет (автоматическое переключение случайного цвета из всего диапазона)
    if (nowmillis - ledmillis > 200 + color) { //защита от дребезга
      RBRIGHT = random(6)*51; //выбор случайного числа от 0 до 255 с шагом 51
      GBRIGHT = random(6)*51;
      BBRIGHT = random(6)*51;
      analogWrite(RLED, RBRIGHT); //запись в светодиоды
      analogWrite(GLED, GBRIGHT);
      analogWrite(BLED, BBRIGHT);
      ledmillis = nowmillis; 
    }
    break;
    case 3: // холодный свет (регулировка света от 0;0;0 до 255;255;255 потенциометром
   if (nowmillis - ledmillis > LEDTIME) { //защита от дребезга
      color = analogRead(POTBT); //считывание напряжения потенциометра
      color = color/4; //перевод в байтовое 0-255 значение
      analogWrite(RLED, color); //запись в светодиоды
      analogWrite(GLED, color);
      analogWrite(BLED, color);
      ledmillis = nowmillis; }
  break;
  case 4: // теплый свет (регулировка света от 0;0;0  до 255;200;30 потенциометром)
   if (nowmillis - ledmillis > LEDTIME) { //защита от дребезга
      color = analogRead(POTBT); //считывание напряжения потенциометра
      color = color/4; //перевод в байтовое 0-255 значение
      analogWrite(RLED, 1*color); //запись в светодиоды
      analogWrite(GLED, 0.784*color); 
      analogWrite(BLED, 0.117*color);
      ledmillis = nowmillis; }
  break;
   case 5: // блютуз (режим управления со смартфона)
   //чтение сигнала из терминала смартфона
     if (Serial.available() > 0) {
    state = Serial.read(); } //state - цифра, заданная пользователем на режим
    //режим регулировки R LED по аналогу с ручным
     if (state == '1') {
    RMNL = (RMNL + 1) % 6;
    RBRIGHT = 51 * RMNL;
    analogWrite(RLED, RBRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости
    state = 0;
  }
   //режим регулировки G LED по аналогу с ручным
  if (state == '2') {
    GMNL = (GMNL + 1) % 6;
    GBRIGHT = 51 * GMNL;
    analogWrite(GLED, GBRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости 
    state = 0;
  }
  //режим регулировки B LED по аналогу с ручным
  if (state == '3') {
    BMNL = (BMNL + 1) % 6;
    BBRIGHT = 51 * BMNL;
    analogWrite(BLED, BBRIGHT); //каждая отправка единицы прибавляет +51 (из 255) яркости
    state = 0;
  }
   //режим случайных цветов по аналогу с ручным
  if (state == '4') {
      RBRIGHT = random(6)*51;
      GBRIGHT = random(6)*51;
      BBRIGHT = random(6)*51;
      analogWrite(RLED, RBRIGHT);
      analogWrite(GLED, GBRIGHT);
      analogWrite(BLED, BBRIGHT);
      state=0;
  }
   //режим радуги, каждая отправка 4 меняет цвет в диапазоне от красного к синему
  if (state == '5') {
if ((GBBRIGHT<255) && (BBBRIGHT==0)&& (RBBRIGHT==255)) { //если горит красный, прибавлять зеленый
     GMNL = (GMNL + 1) % 6;
    GBBRIGHT = 51 * GMNL;
    analogWrite(GLED, GBBRIGHT);}
    else if ((GBBRIGHT == 255) && (BBBRIGHT==0)&& (RBBRIGHT >0)) { //если горит красный и зеленый, убавлять красный
      RMNL = (RMNL + 1) % 6;
    RBBRIGHT = 255 -(51 * RMNL);
    analogWrite (RLED, RBBRIGHT);}
    else if ((GBBRIGHT == 255) && (BBBRIGHT<255)&& (RBBRIGHT ==0)) { //если горит зеленый и не горит красный и синий, прибавлять синий
      BMNL = (BMNL + 1) % 6;
    BBBRIGHT = 51 * BMNL;
    analogWrite (BLED, BBBRIGHT);}
    else if ((GBBRIGHT >0) && (BBBRIGHT==255)&& (RBBRIGHT ==0)) { //если горит синий и зеленый, убавлвлять зеленый
      GMNL = (GMNL + 1) % 6;
    GBBRIGHT =255-(51 * GMNL);
    analogWrite (GLED, GBBRIGHT);}
    else { //если светодиод не горит задать красный цвет
    RBBRIGHT=255;
       GBBRIGHT=0;
          BBBRIGHT=0;
    analogWrite (RLED, RBBRIGHT);
    analogWrite (GLED, GBBRIGHT);
    analogWrite (BLED, BBBRIGHT);}
  state=0;}
   //режим холодного цвета, каждая отправка 6 прибавляет яркость +51 от до 255;255;255 
  if (state=='6') {
  CMNL = (CMNL + 1) % 6;
    FBRIGHT = 51 * CMNL;
      analogWrite(RLED, FBRIGHT);
      analogWrite(GLED, FBRIGHT);
      analogWrite(BLED, FBRIGHT); 
      state = 0;}
      //режим теплого цвета, каждая отправка 7 прибавляет яркость +51 до 255;200;30
       if (state=='7') {
  WMNL = (WMNL + 1) % 6;
    FBRIGHT = 51 * WMNL;
      analogWrite(RLED, 1*FBRIGHT);
      analogWrite(GLED, 0.784*FBRIGHT);
      analogWrite(BLED, 0.117*FBRIGHT); 
      state = 0;}
      //режим выключения светодиода 
else if (state == '0') {
    whiteoff();
    state = 0;
  }
  break;
  }
}
