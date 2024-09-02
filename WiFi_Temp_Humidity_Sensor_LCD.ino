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
#define LOCAL_SSID "Ricky-Guest"
#define LOCAL_PASS "564us90OP0"

//rgb module setup
#define RGB_RED 15
#define RGB_GREEN 2
#define RGB_BLUE 0
//set rgb global colors as white initially
int red = 255,
    blue = 255, 
    green = 255;

// state of LED, initially ON
bool LED_State = true;

//dht11 module setup
#define DHTPIN 16
dht11 DHT11;

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
  
  //LCD device
  lcd.begin(16,2); //sets up LCD number of column (16) & row (2)
  lcd.setCursor(0,0);
  lcd.print("   Setting Up  ");
  lcd.setCursor(0,1);
  lcd.print("  Please Wait!  ");

  //set RGB to White - blink 3 times
  setColor(red,green,blue);
  delay (500);
  setColor(0,0,0);
  delay(500);
  setColor(red,green,blue);
  delay (500);
  setColor(0,0,0);
  delay(500);
  setColor(red,green,blue);

  // Start WiFi and server
  connectWiFiAndStartServer();

}

void loop()
{  

  // Regularly handle client incoming requests
  server.handleClient();
  
  //reads from DHT11 for Temperature & Humidity
  int chk = DHT11.read(DHTPIN);
  float humid = DHT11.humidity;
  float temp = DHT11.temperature;

  Serial.print("Humidity (%): ");
  Serial.println(humid, 2);

  Serial.print("Temperature  (C): ");
  Serial.println(temp, 2);

  for (int positionCounter = 0; positionCounter < 19; positionCounter++) {
    // scroll one position Left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(300);
  }     
    
  for (int positionCounter = 0; positionCounter < 19; positionCounter++) {
    // scroll one position Right:
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(300);
  }   

  delay (1500);  

    //blue: 20 degrees & under Celcius
    if (temp <= 20){          

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Feels Cold: ");
      lcd.print(temp, 1);
      lcd.print(" *C");
      lcd.setCursor(0,1);
      lcd.print("Humidity: ");
      lcd.print(humid, 1);
      lcd.print(" %");
      Serial.print("blue is blinking! \n");
      Serial.print("Run-Time: ");
      Serial.print(millis()/ 1000);
      Serial.print(" seconds\n");
          
      //rgb set blue
      setColor(0, 0, 180); // Blue RGB Color
      delay(50);
      }

    //green: between 21 & 25 degrees Celcius
      else if (temp >= 21 && temp <= 25){   

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Feels Good: ");
          lcd.print(temp,1);
          lcd.print(" *C");
          lcd.setCursor(0,1);
          lcd.print("Humidity: ");
          lcd.print(humid, 1);
          lcd.print(" %");
          Serial.print("green is blinking! \n");
          Serial.print("Run-Time: ");
          Serial.print(millis()/ 1000);
          Serial.print(" seconds\n"); 
          
          //rgb set green
          setColor(0, 180, 0); // Green RGB Color
          delay(50);
      }

    //orange: between 26 & 30 degrees Celcius
      else if (temp >= 26 && temp <= 29){		 

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Feels Warm: ");
          lcd.print(temp, 1);
          lcd.print(" *C");
          lcd.setCursor(0,1);
          lcd.print("Humidity: ");
          lcd.print(humid, 1);
          lcd.print(" %");
          Serial.print("orange is blinking! \n"); 
          Serial.print("Run-Time: ");
          Serial.print(millis()/ 1000);
          Serial.print(" seconds\n");
          
          //rgb set yellow
          setColor(200, 70, 0); //yellow
          delay(50);
      }


    //red: 32 degrees Celcius and over
      else if (temp >= 30){	

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Feels Hot: ");
          lcd.print(temp, 1);
          lcd.print(" *C");
          lcd.setCursor(0,1);
          lcd.print("Humidity: ");
          lcd.print(humid, 1);
          lcd.print(" %");
          Serial.print("red is blinking! \n");
          Serial.print("Run-Time: ");
          Serial.print(millis()/ 1000);
          Serial.print(" seconds\n");
          
          //rgb set red
          setColor(180, 0, 0); // Red RGB Color
          delay(50);  
      }
}

//sets rgb color
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(RGB_RED, redValue);
  analogWrite(RGB_GREEN, greenValue);
  analogWrite(RGB_BLUE, blueValue);
}

//Establish WiFi connection and start server, runs in setup()
void connectWiFiAndStartServer() {
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Connecting to  ");
  lcd.setCursor(0,1);
  lcd.print("      WiFi");
  lcd.blink();

  delay(500);

  int wifiWaitTimeCounter = 0;
  bool wifiStatus = false;

  while (WiFi.status() != WL_CONNECTED && !wifiStatus) {
    Serial.print(".");
    
    wifiWaitTimeCounter++;
    delay(1000);

    if (wifiWaitTimeCounter == 15){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Connection Failed!");
      lcd.setCursor(0,1);
      lcd.print("WiFi: ");
      lcd.print(LOCAL_SSID);
      delay(2000);
      wifiStatus = true;
    }
  }
  
  lcd.noBlink();  
  
  if (!wifiStatus){
    Serial.println("Connected to WiFi!");
    Serial.println("Paste IP address into web-browser!");
    Serial.print("IP address: "); 
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connected to WiFi: ");
    lcd.print(LOCAL_SSID);
    lcd.setCursor(0,1);
    lcd.print("IP address: "); 
    lcd.print(WiFi.localIP());
  }

  delay(1000);

  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/BUTTON_ON", ProcessHumidityText);
  server.on("/BUTTON_OFF", ProcessTemperatureText);
  server.on("/COLOR", HTTP_PUT, ProcessSetColor);
  server.on("/TOGGLEMODE", ProcessModeChange);
  server.begin();
}

//Send website
void SendWebsite() {
  server.send(200, "text/html", PAGE_MAIN);
}

//SendXML
void SendXML() {
  String xml = "<?xml version='1.0'?>\n<Data>\n";
  xml += "<LED>" + String(LED_State ? "1" : "0") + "</LED>\n";
  xml += "</Data>\n";
  server.send(200, "text/xml", xml);
}

//Process rgb color from website
void ProcessSetColor() {
  String redValue = server.arg("R");
  String greenValue = server.arg("G");
  String blueValue = server.arg("B");

  red = redValue.toInt();
  green = greenValue.toInt();
  blue = blueValue.toInt();

  if (red >= 0 && red <= 255 && green >= 0 && green <= 255 && blue >= 0 && blue <= 255) {
    setColor(red, green, blue);
    server.send(200, "text/plain", "Color set");
  } else {
    server.send(400, "text/plain", "Invalid color values");
  }
}

//Process changing Humidity Text
void ProcessHumidityText(){
}

//Process changing Temperature Text
void ProcessTemperatureText(){
}

//Process changing in between 4 modes
void ProcessModeChange(){

}


