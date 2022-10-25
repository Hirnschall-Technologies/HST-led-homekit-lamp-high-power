//---------------------------------------------------
#include <ESP8266WiFi.h>
const char *ssid = "DLan";
const char *password = "PW2015dIr615!";
//---------------------------------------------------
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);  //listen to port 80
//---------------------------------------------------
#define LED_1 12
#define LED_2 14
#define LED_3 16




int currentStatus = 0;
int statusChanged = 0;
int currentBrightness = 100;


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
    server.send ( 200, "text/html",  String(analogRead(A0)*(2/1024.0))  );
  else
    server.send ( 200, "text/html",  String(currentStatus)  );
}

void setup() {
  analogWriteFreq(100);
  
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(LED_3,OUTPUT);
  analogWrite(LED_1,0);
  analogWrite(LED_2,0);
  analogWrite(LED_3,0);
  
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
  if(statusChanged){
    statusChanged = 0;
    if(currentStatus){
      analogWrite(LED_1,(int)(currentBrightness/100.0*255));
      delay(25);
      analogWrite(LED_2,(int)(currentBrightness/100.0*255));
      delay(25);
      analogWrite(LED_3,(int)(currentBrightness/100.0*255));
    }else{
      analogWrite(LED_3,0);
      delay(25);
      analogWrite(LED_2,0);
      delay(25);
      analogWrite(LED_1,0);
    }
  }
}
