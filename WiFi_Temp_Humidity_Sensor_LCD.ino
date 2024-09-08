// C++ code by Jonathan Ly

//LCD Library
#include <LiquidCrystal.h>
//dht11 Library
#include <dht11.h>

//Wireless Function Library
#include "temp_control_html.h"
#include <WiFi.h>
#include <WebServer.h>
// Create a WebServer object that listens on port 80
WebServer server(80);

//Your Local WiFi info
#define LOCAL_SSID "Ricky-Alpha"
#define LOCAL_PASS "321fX941XAdg"

//rgb module setup
#define RGB_RED 15
#define RGB_GREEN 2
#define RGB_BLUE 0

//fan pin
#define fanPin 4
int pwm_Control = 0, // default off at startUp
    pwm_Mode1 = 0,   // default off at mode 1
    pwm_Mode2 = 0,   // default off at mode 2
    pwm_Mode3 = 127, // default half-fan speed at mode 3
    pwm_Mode4 = 255; // default full-fan speed at mode 4

//set rgb global colors as white initially
int red = 255, blue = 255, green = 255;

//mode1 default color - blue
int R1 = 0, G1 = 0, Bl = 255;

//mode2 default color - green
int R2 = 0, G2 = 255, B2 = 0;

//mode3 default color - orange
int R3 = 209, G3 = 15, B3 = 0;

//mode4 default color - red
int R4 = 255, G4 = 0, B4 = 0;

//default temperature text: mode1, mode2, mode3, mode4
String msgT1 = "Feels Cool", msgT2 = "Feels Good", msgT3 = "Feels Warm", msgT4 = "Feels Hot";

//default humidity text: mode1, mode2, mode3, mode4
String msgH1, msgH2, msgH3, msgH4;

//global humid & temperature text
String humidT, tempT;

bool modeChange = false, modeChangeH = false;

//dht11 module setup
#define DHTPIN 16
dht11 DHT11;
float humid, temp;

//LCD setup
const int rs = 22, 
          en = 23, 
          db4 = 21, 
          db5 = 19, 
          db6 = 18, 
          db7 = 5;
LiquidCrystal lcd(rs, en, db4, db5, db6, db7);

void setup()
{
  //serial setup
  Serial.begin(9600);
  
  //rgb output devices
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_RED, OUTPUT);

  //5v 4pin fan setup
  pinMode(fanPin, OUTPUT);
  analogWrite(fanPin, pwm_Control);

  //LCD device
  lcd.begin(16,2); //sets up LCD number of column (16) & row (2)
 
  lcd.setCursor(0,0);
  lcd.print("  Setting Up  ");
  lcd.setCursor(0,1);
  lcd.print("  Please Wait!  ");

  //set RGB to White - blink 3 times
  setColor(red, blue, green);
  delay(500);
  setColor(0,0,0);
  delay(500);
  setColor(red, blue, green);
  delay(500);
  setColor(0,0,0);
  delay(500);
  setColor(red, blue, green);

  // Start WiFi and server
  connectWiFiAndStartServer();
}

void loop()
{ 
  //handles client incoming requests
  server.handleClient();
  
  //reads from DHT11 for Temperature & Humidity
  int chk = DHT11.read(DHTPIN);
  humid = DHT11.humidity;
  temp = DHT11.temperature;

    //blue: under 20 degrees Celcius
      if (temp < 20){   
        
          //rgb set blue
          setColor(R1, G1, Bl); // Blue RGB Color
          ShiftlcdText(lcdPrintText(msgT1, setHumidityText(msgH1)));
          analogWrite(fanPin, pwm_Mode1);
          
          String serial1 = "Blue is Blinking! | Run-Time: " + String(millis()/1000) + " seconds \n \n";
          Serial.print(serial1);
      }

    //green: between 21 & 25 degrees Celcius
      else if (temp >= 20 && temp <= 25){   

          //rgb set green
          setColor(R2, G2, B2); // Green RGB Color
          ShiftlcdText(lcdPrintText(msgT2, setHumidityText(msgH2)));
          analogWrite(fanPin, pwm_Mode2);

          String serial2 = "Green is Blinking! | Run-Time: " + String(millis()/1000) + " seconds \n  \n";
          Serial.print(serial2);
      }

    //orange: between 26 & 30 degrees Celcius
      else if (temp >= 26 && temp <= 30){	

          //rgb set orange
          setColor(R3, G3, B3); 
          ShiftlcdText(lcdPrintText(msgT3, setHumidityText(msgH3)));
          analogWrite(fanPin, pwm_Mode3);
          
          String serial3 = "Orange is Blinking! | Run-Time: " + String(millis()/1000) + " seconds \n  \n";
          Serial.print(serial3);
      }

    //red: over 30 degrees Celcius
      else if (temp > 30){	

          //rgb set red
          setColor(R4, G4, B4); // Red RGB Color
          ShiftlcdText(lcdPrintText(msgT4, setHumidityText(msgH4)));
          analogWrite(fanPin, pwm_Mode4);

          String serial4 = "Red is Blinking! | Run-Time: " + String(millis()/1000) + " seconds \n  \n";
          Serial.print(serial4);
          
      }

}

//prints text into lcd
int lcdPrintText(String msgT, String msgH){

  humid = DHT11.humidity;
  temp = DHT11.temperature;
  
  String serial0 = "Humidity (%): " + String(humid,2) + " | Temperature (*C): " + String(temp, 2); 
  Serial.println(serial0);

  String tempMsg = msgT + ": " + String(temp, 0) + " *C";
  String humidMsg = msgH + ": " + String(humid, 0) + " %";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tempMsg);
  
  lcd.setCursor(0, 1);
  lcd.print(humidMsg);

  if (tempMsg.length() >= humidMsg.length()){
    return tempMsg.length();
  }

  else return humidMsg.length();
}

//sets custom humidity text at default depending on humidity percentage
String setHumidityText(String msg){

  if (modeChangeH == false){
      
      if (humid <= 30) //humidity is 30 and under
      return "Feels Dry";

      else if (humid > 30 && humid < 40) //humidity is between 30 and 40
      return "Feels Comfortable";

      else if (humid >= 40 && humid < 70 ) //humidity is between 40 and 70
      return "Feels Stuffy";

      else if (humid >= 70) //humidity is 70 and over
      return "Feels Wet";
  }

  else {

    return msg; 

  }

}

//sets rgb color
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(RGB_RED, redValue);
  analogWrite(RGB_GREEN, greenValue);
  analogWrite(RGB_BLUE, blueValue);
}

// Shifts text in LCD to the right and left
void ShiftlcdText(int stringLength){

    if (stringLength <= 16){
      delay(500);
      return;
    }

    int scrollAmount = stringLength - 16;

    for (int positionCounter = 0; positionCounter <= scrollAmount*2; positionCounter++) {
        
        server.handleClient();
        
        if (modeChange == false){ 
            

            if (positionCounter < scrollAmount) {
              lcd.scrollDisplayLeft(); // Scroll left
            }
            
            else if (positionCounter > scrollAmount){
              lcd.scrollDisplayRight(); // Scroll right}
            }
            delay(225);
        }
        else {break;}   
    }
}

//Establish WiFi connection and start server, runs in setup()
void connectWiFiAndStartServer() {
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Connecting   ");
  lcd.setCursor(0,1);
  lcd.print("    to WiFi");
  lcd.blink();

  int attempts = 0;
  const int maxAttempts = 15;
  const int delayTime = 1000;

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    Serial.print(".");
    delay(delayTime);
    attempts++;
  }

  lcd.noBlink();  

  if (WiFi.status() == WL_CONNECTED) {
    
    String serialConnectionMsg = "Connected to WiFi! Paste IP Address into Web-Browser! \nIP Address: ";
    Serial.print(serialConnectionMsg);
    Serial.print(WiFi.localIP());
    Serial.print("  \n");
    
    String lcdConnectionMsg = "Connected to WiFi: " + String(LOCAL_SSID); 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdConnectionMsg);
    lcd.setCursor(0, 1);
    lcd.print("IP Address: ");
    lcd.print(WiFi.localIP());
    ShiftlcdText(lcdConnectionMsg.length());
    }

  else {

    String lcdConnectionFailedMsg = "Cannot Connect to WiFi: " + String(LOCAL_SSID);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connection Failed");
    lcd.setCursor(0, 1);
    lcd.print(lcdConnectionFailedMsg);
    ShiftlcdText(lcdConnectionFailedMsg.length());
  }

  server.on("/", SendWebsite);
  server.on("/HUM", ProcessHumidTextChange);
  server.on("/TEMP", ProcessTempTextChange);
  server.on("/COLOR", HTTP_PUT, ProcessSetColor);
  server.on("/FAN", HTTP_PUT, ProcessFanSpeedChange);
  server.on("/MODEa", ProcessModeChange);
  server.on("/MODEb", ProcessModeChange);
  server.on("/MODEc", ProcessModeChange);
  server.begin();
}

//Send website
void SendWebsite(){
  server.send(200, "text/html", PAGE_MAIN);
}

//Process rgb color from website
void ProcessSetColor(){
  red = server.arg("R").toInt();
  delay(350);
  green = server.arg("G").toInt();
  delay(350);
  blue = server.arg("B").toInt();
  delay(350);
  
}

//Process changing Humidity Text
void ProcessHumidTextChange(){
  if (!server.arg("H").isEmpty()){  
    humidT = server.arg("H");
    delay(350);
  }

}

//Process changing Temperature Text
void ProcessTempTextChange(){
  if (!server.arg("T").isEmpty()){  
    tempT = server.arg("T");
    delay(350);
  } 
  
}

void ProcessFanSpeedChange(){
  pwm_Control = server.arg("F").toInt();
  delay(350);
}

//Process changing in between 4 modes
void ProcessModeChange(){
  
  if (server.arg("Ma").isEmpty() && server.arg("Mb").isEmpty() && server.arg("Mc").isEmpty()){
    modeChange = false;
    return;
  }
  
  else {

      for (int i = 0; i <= 15; i++){
          delay(425);

          modeChange = true;     
          
          int mT = server.arg("Ma").toInt();
          int mC = server.arg("Mb").toInt();
          int mF = server.arg("Mc").toInt();

          String serialmsg = "nothing";

          switch (mT) {
              case 1:
                if (!tempT.isEmpty()){
                  msgT1 = tempT;                
                }
                if (!humidT.isEmpty()){
                  modeChangeH = true;
                  msgH1 = humidT;
                  }  
                                  
                serialmsg = "mT1 Recieved | tempT: " + msgT1 + ", humidT: " + msgH1;
                Serial.println(serialmsg);

                modeChange = false;
                break;

              case 2:
                if (!tempT.isEmpty()){
                  msgT2 = tempT;                
                }
                if (!humidT.isEmpty()){
                  modeChangeH = true;
                  msgH2 = humidT;
                  }

                serialmsg = "mT2 Recieved | tempT: " + msgT2 + ", humidT: " + msgH2;
                Serial.println(serialmsg);

                modeChange = false;
                break;

              case 3:
                if (!tempT.isEmpty()){
                  msgT3 = tempT;                
                }
                if (!humidT.isEmpty()){
                  modeChangeH = true;
                  msgH3 = humidT;
                  }
                
                serialmsg = "mT3 Recieved | tempT: " + msgT3 + ", humidT: " + msgH3;
                Serial.println(serialmsg);

                modeChange = false;
                break;

              case 4:
                if (!tempT.isEmpty()){
                  msgT4 = tempT;                
                }
                if (!humidT.isEmpty()){
                  modeChangeH = true;
                  msgH4 = humidT;
                  }

                serialmsg = "mT4 Recieved | tempT: " + msgT4 + ", humidT: " + msgH4;
                Serial.println(serialmsg);

                modeChange = false;
                break;
          }

          switch (mC) {
              case 1:
                R1 = red;
                G1 = green;
                Bl = blue;                
                serialmsg = "mc1 Recieved | Red: " + String(R1) + ", Green: " + String(G1) + ", Blue: " + String(Bl);
                Serial.println(serialmsg);
                modeChange = false;
                break;

              case 2:
                R2 = red;
                G2 = green;
                B2 = blue;
                serialmsg = "mc2 Recieved | Red: " + String(R2) + ", Green: " + String(G2) + ", Blue: " + String(B2);
                Serial.println(serialmsg);
                modeChange = false;
                break;

              case 3:
                R3 = red;
                G3 = green;
                B3 = blue;
                serialmsg = "mc3 Recieved | Red: " + String(R3) + ", Green: " + String(G3) + ", Blue: " + String(B3);
                Serial.println(serialmsg);
                modeChange = false;
                break;

              case 4:
                R4 = red;
                G4 = green;
                B4 = blue;
                serialmsg = "mc1 Recieved | Red: " + String(R4) + ", Green: " + String(G4) + ", Blue: " + String(B4);
                Serial.println(serialmsg);
                modeChange = false;
                break;
          }

          

          switch (mF){

                  case 1:
                    serialmsg = "mF1 Recieved | Fan Speed: " + String(pwm_Control);
                    Serial.println(serialmsg);
                    pwm_Mode1 = pwm_Control;
                    modeChange = false;
                    break;

                  case 2:
                    serialmsg = "mF2 Recieved | Fan Speed: " + String(pwm_Control);
                    Serial.println(serialmsg);
                    pwm_Mode2 = pwm_Control;
                    modeChange = false;
                    break;

                  case 3:
                    serialmsg = "mF3 Recieved | Fan Speed: " + String(pwm_Control);
                    Serial.println(serialmsg);
                    pwm_Mode3 = pwm_Control;
                    modeChange = false;
                    break;

                  case 4:
                    serialmsg = "mF4 Recieved | Fan Speed: " + String(pwm_Control);
                    Serial.println(serialmsg);
                    pwm_Mode4 = pwm_Control;
                    modeChange = false;
                    break;
          }

      } // for loop bracket
  }// else bracket

}



