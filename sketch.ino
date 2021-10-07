//PROJECT(ПРОЕКТ): ARDUINO-WATCH(ЧАСЫ НА АРДУИНО)
//DATE(ДАТА): 07 OCTOBER 2021. (07 ОКТЯБРЯ 2021)
//AUTHOR(АВТОР): letharicimport@gmail.com
//GITHUB: https://github.com/Lethar1c/arduinowatch
#include <LiquidCrystal_I2C.h>;

LiquidCrystal_I2C lcd(0x27,16,2);

//Символы: звонок и значок градуса
byte bell[8] = {0b00100, 0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b11111};
byte degreesc[8] = {0b00111, 0b00101, 0b00111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};

uint64_t ledtimer = 0;
uint64_t lcdtimer = 0; //переменные таймера

uint32_t time = 86395; //время в секундах (в 1сут. 86400сек)

uint32_t hours = 0;
uint32_t minutes = 0;
uint32_t seconds = 0; //переменные времени, выводимые НА ЭКРАН

byte year = 21; //год, от 10 до 99
byte month = 10; //месяц (1-янв, 2-февр и тд)
byte day = 7;
byte weekday = 6; //ДАТА (день, месяц, год, день недели(0-пн, 1-вт, 2-ср и т.п.))

bool ledflag = 0; //флаг мигания светодиодом встроенным на ардуине

const float BETA = 3950; // should match the Beta Coefficient of the thermistor
String weekdays[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"}; //массив из названий дней недели для вывода на экран

int countofdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //количество дней в месяцах, для корректного изменения даты
//Февраль обновляется с 28 на 29 при чётном годе.
//ЕСЛИ ВЫ ПРОШИВАЕТЕ В ВИСОКОСНОМ ГОДУ, ЗАМЕНИТЕ 28 НА 29, ЕСЛИ НЕТ - ОСТАВЬТЕ КАК ЕСТЬ


void setup() {
  pinMode(13, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0); //подготовка дисплея к работе

  lcd.createChar(0, bell); //создание своих символов
  lcd.createChar(1, degreesc);
}

void loop() {
  int analogValue = analogRead(A0);
  int celsius = round(1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15);
//перевод значений с аналогового входа в градусы Цельсия

  if (millis() - ledtimer >= 300) {
    digitalWrite(13, ledflag = !ledflag);
    ledtimer = millis();
  } //мигание светодиодом. ПРИ ЖЕЛАНИИ МОЖНО ОТКЛЮЧИТЬ



  if (millis() - lcdtimer >= 1000) { //таймер на millis()
    lcd.clear(); //очистка дисплея перед выводом инфы

    hours = time/3600;
    minutes = (time - (hours*3600))/60;
    seconds = time - hours*3600 - minutes*60; //подсчёт часов, минут и секунд исходя из времени в секундах (time)

    lcd.print(
      (hours <= 9 ? ("0"+String(hours)) : String(hours))+":"+
      (minutes <= 9 ? ("0"+String(minutes)) : String(minutes))+":"+
      (seconds <= 9 ? ("0"+String(seconds)) : String(seconds)));
      lcd.print(" "+weekdays[weekday] + "  " + char(0)); //вывод на экран дня недели и звоночка

      if (time<=86398) { //+1сек
        time++;
      } else {  //обновление с 23:59:59 до 00:00:00
        time=0;
        weekday = weekday >= 6 ? 0 : weekday+1; //обновление дня недели
        if (day < countofdays[month-1]) {
        //проверка: ЕСЛИ день меньше, чем макс. кол-во в месяце то увеличить день на 1. 
        
          day++;
        } else { //ИНАЧЕ день=1, месяц увеличить на 1
          day = 1;
          if (month < 12) //если месяц меньше ДЕКАБРЯ то месяц увеличить на 1
          {
          month++;
          } else { //иначе  месяц=январь, год увеличить на 1.
            month = 1;
            year++;
            countofdays[1] = year % 4 == 0 ? 29 : 28; //проверка на високостный год.
          }
        }
      }
    
    lcdtimer = millis(); //продолжение таймера на millis()
    lcd.setCursor(0, 1);
    lcd.print(String(celsius) + char(1) +"C "+String(day)+"/"+String(month)+"/"+String(year)); //вывод даты
    lcd.setCursor(0, 0);
  }
  
}

long mapf(float x, float in_min, float in_max, float out_min, float out_max) { //округление float до long (для температуры)
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
