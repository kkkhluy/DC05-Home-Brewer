String menuItems[] = { "5L 2lbs", "7L 3lbs", "Custom", "DEMO", "Cleaning", "Drain" };


// Navigation button variables
int readKey;
int savedDistance = 0;

// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100,  //   *
  0b00100,  //   *
  0b00100,  //   *
  0b00100,  //   *
  0b00100,  //   *
  0b10101,  // * * *
  0b01110,  //  ***
  0b00100   //   *
};

byte upArrow[8] = {
  0b00100,  //   *
  0b01110,  //  ***
  0b10101,  // * * *
  0b00100,  //   *
  0b00100,  //   *
  0b00100,  //   *
  0b00100,  //   *
  0b00100   //   *
};

byte menuCursor[8] = {
  B01000,  //  *
  B00100,  //   *
  B00010,  //    *
  B00001,  //     *
  B00010,  //    *
  B00100,  //   *
  B01000,  //  *
  B00000   //
};

#include <Wire.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>


#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
RTC_DS1307 rtc;

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int relay1 = 3;
int relay2 = 11;
int relay3 = 12;
//relay 4 = A2



const int relayON = HIGH;   // do not change
const int relayOFF = LOW;   //do not chage
int relayState = relayOFF;  //initial state of relay

float Mashing_Temperature;
float Wortchilling_Temperature;

int Mashing_Time_Day;
int Mashing_Time_Hour;
int Mashing_Time_Minute;
int Mashing_Time_Second;

int Mashing_Pump_Drain_Time;  //millisec

int Boiling_Time_Day;
int Boiling_Time_Hour;
int Boiling_Time_Minute;
int Boiling_Time_Second;

int Pump1Delay;
int MashDelay;
int Pump2Delay;

String status;

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

DateTime now;
unsigned long nowBuffer;

DateTime Finish_Mashing;
unsigned long Finish_MashingBuffer;

DateTime Finish_Boiling;
unsigned long Finish_BoilingBuffer;


float temperature;
// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


void setup() {



#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif
  // Initializes serial communication


  Serial.begin(115200);  //All ESP8266 serial communication at 115200 baud
  sensors.begin();
  lcd.begin(16, 2);
  lcd.clear();

  Serial.write("0000");
  Serial.flush();


  if (!rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    //Serial.flush();
    lcd.setCursor(0, 0);
    lcd.print("Cant find RTC");
    delay(50);
    while (1) delay(10);
  }

  if (!rtc.isrunning()) {
    //Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }



  /*if ( sensors.isConnected()) {
    Serial.println("Couldn't find Sensor");
    Serial.flush();
    lcd.setCursor(0, 0);
    lcd.print("Cant find Sensor");
    while (1) delay(10);
  }*/




  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(A2, OUTPUT);

  digitalWrite(relay1, 1);
  digitalWrite(relay2, 1);
  digitalWrite(relay3, 1);
  digitalWrite(A2, 1);
}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  //Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {  // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}


void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 800) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:  // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) {  // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          case 4:
            menuItem5();
            break;
          case 5:
            menuItem6();
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.

int evaluateButton(int x) {
  int result = 0;
  if (x < 60) {
    result = 1;  // right
  } else if (x < 200) {
    result = 2;  // up
  } else if (x < 400) {
    result = 3;  // down
  } else if (x < 600) {
    result = 4;  // left
  } else if (x < 800) {
    result = 5;  // select
  }
  return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.

void drawInstructions() {
  lcd.setCursor(0, 1);  // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1));  // Up arrow
  lcd.print("/");
  lcd.write(byte(2));  // Down arrow
  lcd.print(" buttons");
}

void menuItem1() {  // Function executes when you select the Preset1 item from main menu
  int activeButton = 0;
  Mashing_Temperature = 70;
  Wortchilling_Temperature = 30.0;
  Pump1Delay = 95000;
  MashDelay = 120000;
  Pump2Delay = 150000;

  Mashing_Time_Day = 0;
  Mashing_Time_Hour = 2;
  Mashing_Time_Minute = 0;
  Mashing_Time_Second = 0;


  Boiling_Time_Day = 0;
  Boiling_Time_Hour = 1;
  Boiling_Time_Minute = 0;
  Boiling_Time_Second = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("5L 2lbs");
  PreBoil();
  Mashing();
  WaitforPrepBoilng();
  Boiling();
  WaitforPrepWortChilling();
  WortChill();
}

void menuItem2() {  // Function executes when you select the Preset2 item from main menu
  int activeButton = 0;
  Mashing_Temperature = 70;
  Wortchilling_Temperature = 35.0;
  Pump1Delay = 80000;
  MashDelay = 120000;
  Pump2Delay = 150000;

  Mashing_Time_Day = 0;
  Mashing_Time_Hour = 2;
  Mashing_Time_Minute = 0;
  Mashing_Time_Second = 0;


  Boiling_Time_Day = 0;
  Boiling_Time_Hour = 1;
  Boiling_Time_Minute = 0;
  Boiling_Time_Second = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("7L 3lbs");
  PreBoil();
  Mashing();
  WaitforPrepBoilng();
  Boiling();
  WaitforPrepWortChilling();
  WortChill();
}

void menuItem3() {  // Function executes when you select the Preset3 item from main menu
  int activeButton = 0;
  Mashing_Temperature = 70;
  Wortchilling_Temperature = 35.0;
  Pump1Delay = 80000;
  MashDelay = 120000;
  Pump2Delay = 150000;

  Mashing_Time_Day = 0;
  Mashing_Time_Hour = 1;
  Mashing_Time_Minute = 30;
  Mashing_Time_Second = 0;


  Boiling_Time_Day = 0;
  Boiling_Time_Hour = 1;
  Boiling_Time_Minute = 0;
  Boiling_Time_Second = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Custom");
  PreBoil();
  Mashing();
  WaitforPrepBoilng();
  Boiling();
  WaitforPrepWortChilling();
  WortChill();
}

void menuItem4() {  // Function executes when you select the Preset4 item from main menu
  int activeButton = 0;
  Mashing_Temperature = 25.0;
  Wortchilling_Temperature = 30.0;
  Pump1Delay = 10000;
  MashDelay = 10000;
  Pump2Delay = 10000;

  Mashing_Time_Day = 0;
  Mashing_Time_Hour = 0;
  Mashing_Time_Minute = 2;
  Mashing_Time_Second = 0;

  Boiling_Time_Day = 0;
  Boiling_Time_Hour = 0;
  Boiling_Time_Minute = 1;
  Boiling_Time_Second = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("DEMO");
  PreBoil();
  Mashing();
  WaitforPrepBoilng();
  Boiling();
  WaitforPrepWortChilling();
  WortChill();
}

void menuItem5() {
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Cleaning");
  Cleaning();
}

void menuItem6() {
  int activeButton = 0;

  lcd.clear();
  Drain();
}

void Mashing() {

  Serial.write("1111");
  Serial.flush();
  delay(1000);

  DateTime now = rtc.now();
  DateTime Finish_Mashing(now + TimeSpan(Mashing_Time_Day, Mashing_Time_Hour, Mashing_Time_Minute, Mashing_Time_Second));
  Finish_MashingBuffer = Finish_Mashing.unixtime();

  do {
    DateTime now = rtc.now();
    nowBuffer = now.unixtime();
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    Serial.print(temperature);
    Serial.print(" C");
    Serial.println();
    //Serial.println("Mashing in Prog");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Mashing in Prog");
    lcd.setCursor(7, 1);
    lcd.print(temperature);
    lcd.print(" C");


    if (Mashing_Temperature < temperature) {
      Timecheck();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Pumping...");
      digitalWrite(relay3, 1);  //ปิดเตาไม่ให้ทำความร้อนเพิ่ม
      digitalWrite(relay1, 0);  //ปั๊ม 1 ดูดน้ำเข้า
      delay(Pump1Delay);             //น้ำ 7 ลิตร
      digitalWrite(relay1, 1);  //ปั๊ม 1 หยุดดูดน้ำเข้า
      delay(MashDelay);            //รอให้น้ำตาลจาก Malt ซึม
      digitalWrite(relay2, 0);  //ปั๊ม 2 ดูดน้ำออก
      delay(Pump2Delay);
      digitalWrite(relay2, 1);  //ปั๊ม 2 หยุดดูดน้ำออก
    } else {
      Timecheck();
      digitalWrite(relay3, 0);  //เปิดเตา
      digitalWrite(relay1, 1);
      digitalWrite(relay2, 1);
      digitalWrite(A2, 1);
      //Serial.println("Heater turn on");
    }
  } while (nowBuffer < Finish_MashingBuffer);
  digitalWrite(relay1, 1);
  digitalWrite(relay3, 1);
  digitalWrite(relay2, 1);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Mashing Finished");
  delay(3000);
  Serial.write("2222");
  Serial.flush();
  delay(1000);
}

void Boiling() {
  Serial.write("3333");
  Serial.flush();

  DateTime now = rtc.now();
  DateTime Finish_Boiling(now + TimeSpan(Boiling_Time_Day, Boiling_Time_Hour, Boiling_Time_Minute, Boiling_Time_Second));
  Finish_BoilingBuffer = Finish_Boiling.unixtime();


  Timecheck();
  Finish_BoilingBuffer = Finish_Boiling.unixtime();

  do {
    Timecheck();
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    digitalWrite(relay3, 0);
    //Serial.print(temperature);
    //Serial.print(" C");
    //Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Boiling in Prog");
    lcd.setCursor(7, 1);
    lcd.print(temperature);
    lcd.print(" C");
  } while (nowBuffer < Finish_BoilingBuffer);

  digitalWrite(relay3, 1);
  Serial.write("4444");
  Serial.flush();
  delay(1000);
}

void WortChill() {
  Serial.write("5555");
  Serial.flush();

  delay(1000);
  do {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    //Serial.println("Temperature =");
    //Serial.print(temperature);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Chilling in Prog");
    lcd.setCursor(7, 1);
    lcd.print(temperature);
    lcd.print(" C");
    digitalWrite(A2, 0);  //เปิดปั๊ม
    //lcd.setCursor(0, 1);
    //lcd.print("Adding Ice");
    //Serial.println("Ready for fermented");
    delay(1000);
  } while (temperature > Wortchilling_Temperature);
  digitalWrite(A2, 1);  //ปิดปั๊ม
  //Serial.println("Ready for fermented");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WortChill Finished");
  delay(10000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready for");
  lcd.setCursor(0, 1);
  lcd.print("fermentation");
  Serial.write("6666");
  Serial.flush();
  delay(1000);
}

void Timecheck() {
  DateTime now = rtc.now();
  nowBuffer = now.unixtime();
}

void WaitforPrepBoilng() {
  Serial.write("7777");
  Serial.flush();
  delay(1000);
  readKey = analogRead(0);
  while (readKey > 800) {
    readKey = analogRead(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Turn off the");
    lcd.setCursor(5, 1);
    lcd.print("Valve");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press --> when");
    lcd.setCursor(5, 1);
    lcd.print("Ready");
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Draining hose");
  digitalWrite(relay1, 0);  //ปั๊ม 1 ดูดน้ำเข้า
  digitalWrite(relay2, 0);  //ปั๊ม 2 ดูดน้ำเข้า
  delay(10000);
  digitalWrite(relay1, 1);  //ปั๊ม 1 หยุดดูดน้ำเข้า
  digitalWrite(relay2, 1);  //ปั๊ม 2 หยุดดูดน้ำออก
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready for Boiling");
  delay(5000);
  lcd.clear();
}


void WaitforPrepWortChilling() {
  Serial.write("8888");
  Serial.flush();
  delay(1000);
  readKey = analogRead(0);
  while (readKey > 800) {
    readKey = analogRead(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press --> when");
    lcd.setCursor(5, 1);
    lcd.print("Ready");
    delay(100);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready for");
  lcd.setCursor(0, 1);
  lcd.print("WortChilling");
  delay(5000);
  lcd.clear();
}

void PreBoil() {
  do {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    //Serial.println("Temperature =");
    //Serial.print(temperature);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("Boiling To Req. T");
    lcd.setCursor(7, 1);
    lcd.print(temperature);
    lcd.print(" C");
    digitalWrite(relay3, 0);  //เปิดปั๊ม
    delay(1000);
  } while (temperature < Mashing_Temperature);
  digitalWrite(relay3, 1);  //ปิดปั๊ม
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Boiling Finished");
  delay(2000);
  lcd.clear();
}

void Cleaning() {
  while (readKey > 800) {
    readKey = analogRead(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put the hose to");
    lcd.setCursor(1, 1);
    lcd.print("Cleaning Agent");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press --> when");
    lcd.setCursor(5, 1);
    lcd.print("Ready");
    delay(100);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Draining hose");
  digitalWrite(relay1, 0);  //ปั๊ม 1 ดูดน้ำเข้า
  digitalWrite(relay2, 0);  //ปั๊ม 2 ดูดน้ำเข้า
  delay(30000);
  digitalWrite(relay1, 1);  //ปั๊ม 1 หยุดดูดน้ำเข้า
  digitalWrite(relay2, 1);  //ปั๊ม 2 หยุดดูดน้ำออก
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Brew!");
  delay(5000);
  lcd.clear();
}

void Drain() {
  lcd.clear();
  lcd.print("Draining");
  digitalWrite(relay2, 0);  //ปั๊ม 2 ดูดน้ำออก
  delay(140000);
  digitalWrite(relay2, 1);  //ปั๊ม 2 หยุดดูดน้ำออก
  lcd.clear();
  lcd.print("Finished");
  delay(3000);
  lcd.clear();
}
