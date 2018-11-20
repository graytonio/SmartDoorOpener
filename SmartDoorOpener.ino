#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#define D0 16
#define D1 5

const char* ssid     = "YOUR SSID";
const char* password = "YOUR PASSWORD";
WiFiServer server(80);
WiFiClient client;
String header;

bool openDoorState;
bool closeDoorState;
String apiKey = "Your API Key";

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  HTTPClient http;
  String url = "http://maker.ifttt.com/trigger/new_ip/with/key/" + apiKey + "?value1=";
  url = url + WiFi.localIP().toString() ;
  Serial.println(url);
  http.begin(url);
  http.GET();
  delay(500);
  http.end();
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  delay(15000);
  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
}

void openDoor(){
  digitalWrite(D0, LOW);
  digitalWrite(D1, HIGH);
  delay(15000);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  delay(15000);
  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
}

void closeDoor(){
  digitalWrite(D0, LOW);
  digitalWrite(D1, HIGH);
}

void sendControlPage(){
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #77878A;}</style></head>");
  client.println("<body><h1>The Amazing Smart Door Opener </h1>");       
  client.println("<p><a href=\"/open\"><button class=\"button\">Open</button></a></p>");
  client.println("</body></html>");
  client.println();
}

void loop(){
  client = server.available();   

  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();          
        Serial.write(c);                   
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /open") >= 0) {
                openDoorState = true;
            }else if(header.indexOf("GET /close") >= 0){
                closeDoorState = true;
            }
            
            sendControlPage();

            if(openDoorState){
              openDoor();
              openDoorState = false;
            }else if(closeDoorState){
              closeDoor();
              closeDoorState = false;
            }
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
