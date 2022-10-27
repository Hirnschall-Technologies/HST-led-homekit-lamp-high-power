//---------------------------------------------------
#include <ESP8266WiFi.h>
const char *ssid = "";
const char *password = "";
//---------------------------------------------------
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);  //listen to port 80
//---------------------------------------------------
#define LED_1 12
#define LED_2 14
#define LED_3 16

#define FAN_PIN 10    //T2.1
#define REMOTE_PIN 5    //SW

#define MAX_SAFE_TEMP_WO_FAN 75 
#define COOLDOWN_TEMP_WO_FAN 65 
#define MAX_SAFE_TEMP 80    //cannot measure higher using 0-2V
#define COOLDOWN_TEMP 70    //if MAX_SAFE_TEMP is hit, reduce max power to MAX_SAFE_PWM until temperature reaches COOLDOWN_TEMP
#define MAX_SAFE_PWM 153    // 60%

#define TEMP_READ_INTERVALL_IN_MS 10000

int remotePinHigh = 0;
int fanStatus = 0;
int overheated = 0;
int currentStatus = 0;
int statusChanged = 0;
int currentBrightness = 100;

unsigned long previousMillis =0;

float getTemperatureInDegC(){
  //see datasheet for TMP236 for details on conversion
    return -(5/98.0)*400+analogRead(A0)*(2/1024.0)*1000*(5/98.0);
}


void setPosition() {
  Serial.println("New request for \"/\"");
  if(server.hasArg("setenabled")){
    statusChanged = 1;
    currentStatus = (server.arg("setenabled").toInt() > 0);
    server.send ( 200, "text/html",  String(currentStatus)  );
  }
  else if(server.hasArg("getstatus"))
    server.send ( 200, "text/html",  String(currentStatus)  );
  else if(server.hasArg("setbrightness")){
    statusChanged = 1;
    if(server.arg("setbrightness").toInt() >=0 && server.arg("setbrightness").toInt() <=100)
      currentBrightness = server.arg("setbrightness").toInt();
    server.send ( 200, "text/html",  String(currentBrightness)  );
  }
  else if(server.hasArg("getbrightness"))
    server.send ( 200, "text/html",  String(currentBrightness)  );
  else if(server.hasArg("temperature"))
    server.send ( 200, "application/json",  "{\"temperature\": " + String(getTemperatureInDegC()) + "}" );
  else
    server.send ( 200, "text/html",  String(currentStatus)  );
}

void setup() {
  analogWriteFreq(100);    //less than 500 Hz for linear dimming with AL8860Q
  
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(LED_3,OUTPUT);
  analogWrite(LED_1,0);
  analogWrite(LED_2,0);
  analogWrite(LED_3,0);

  
  pinMode(FAN_PIN,OUTPUT);
  digitalWrite(FAN_PIN,LOW);
  
  Serial.begin(115200); //start the serial output
  Serial.println();
  Serial.println("Starting up");
  
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Print local IP address and start web server
  Serial.println("");
  //set mac address
  uint8_t mac[6] {0x5C, 0x26, 0x19, 0x40, 0x86, 0x62}; 
  wifi_set_macaddr(0, const_cast<uint8*>(mac));
  //disable ap advertising 
  WiFi.mode(WIFI_STA);
  //connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  
  Serial.println("------------------------------");
  server.on ( "/", setPosition );
  server.begin(); //start the webserver
  Serial.println("Webserver started");

  Serial.println("Startup finished");
  Serial.println("****************");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  server.handleClient();  //process all the requests for the Webserver
  unsigned long currentMillis = millis();
  if (previousMillis > currentMillis || currentMillis - previousMillis >= TEMP_READ_INTERVALL_IN_MS)
  {
    previousMillis = currentMillis;
    float temp = getTemperatureInDegC();
    Serial.print("Temp at ");
    Serial.print(currentMillis);
    Serial.print("ms: ");
    Serial.println(temp);

    //control fan based on temp 
    if (temp > MAX_SAFE_TEMP_WO_FAN && currentStatus && !fanStatus){
      fanStatus = 1;
      digitalWrite(FAN_PIN, HIGH);
    }

    if(temp < COOLDOWN_TEMP_WO_FAN && currentStatus && fanStatus){
      fanStatus= 0;
      digitalWrite(FAN_PIN, LOW);
    }
    if( temp > MAX_SAFE_TEMP && currentStatus){
      overheated = 1;
      statusChanged = 1;
      Serial.println("overheating! - limiting to 60% power");
    }
    if( temp < COOLDOWN_TEMP && overheated){
      overheated = 0;
      statusChanged = 1;
      Serial.println("overheating avoided - allowing 100% power");
    }
  }
  if(statusChanged){
    statusChanged = 0;
    if(currentStatus){
      int safePWM;
      if(overheated)
        safePWM = (currentBrightness/100.0*255)<MAX_SAFE_PWM?(currentBrightness/100.0*255):MAX_SAFE_PWM;
      else
        safePWM = currentBrightness/100.0*255;
      analogWrite(LED_1,safePWM);
      delay(25);
      analogWrite(LED_2,safePWM);
      delay(25);
      analogWrite(LED_3,safePWM);
    }else{
      analogWrite(LED_3,0);
      delay(25);
      analogWrite(LED_2,0);
      delay(25);
      analogWrite(LED_1,0);
    }
  }

  if(!remotePinHigh && digitalRead(REMOTE_PIN) == HIGH){
    Serial.println("remote input - toggle on/off");
    remotePinHigh = 1;
    statusChanged = 1;
    currentStatus = !currentStatus;
  }
  if(remotePinHigh && digitalRead(REMOTE_PIN) == LOW){
    Serial.println("reset remote input");
    remotePinHigh = 0;
    }
}
