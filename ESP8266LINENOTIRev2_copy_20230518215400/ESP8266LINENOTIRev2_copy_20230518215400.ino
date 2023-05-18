#include <TridentTD_LineNotify.h>

#define SSID "quay's 12T"                                         // บรรทัดที่ 11 ให้ใส่ ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD "khluy913"                                       // บรรทัดที่ 12 ใส่ รหัส Wifi
#define LINE_TOKEN "gEm3bCL7kZPLJOvxoJHLO0gxweDjsXWCbyH4vXHA6Ag"  // บรรทัดที่ 13 ใส่ รหัส TOKEN ที่ได้มาจากข้างบน
String incomingString;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(LINE.getVersion());

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);

  // ตัวอย่างส่งข้อความ
  LINE.notify("Welcome to HomeBrewer");
}

void loop() {
  /*if (Serial.available() > 0) {*/
    // read the incoming string from the Arduino
    incomingString = Serial.readString();
{
    // If the incoming string from the Arduino equals "change" then we toggle the LED
    if (incomingString.equals("0000")) {
      LINE.notify("Line Notification Confirmed");
    }
    if (incomingString.equals("1111")) {
      LINE.notify("Mashing Start");
    }
    if (incomingString.equals("2222")) {
      LINE.notify("Mashing Finished");
    }
    if (incomingString.equals("3333")) {
      LINE.notify("Boiling Started");
    }
    if (incomingString.equals("4444")) {
      LINE.notify("Boiling Finished");
    }
    if (incomingString.equals("5555")) {
      LINE.notify("WortChilling Started");
    }
    if (incomingString.equals("6666")) {
      LINE.notify("WortChilling Finished");
    }
    if (incomingString.equals("7777")) {
      LINE.notify("Please prepare Eqt. for Boiling");
    }
    if (incomingString.equals("8888")) {
      LINE.notify("Please prepare Eqt. for Wort Chilling");
    }
  }
  delay(50);  // Wait for half second
}
