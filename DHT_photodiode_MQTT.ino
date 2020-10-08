#include <ESP8266WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

const char* ssid     = "Shuvi";
const char* password = "phung250";

const char* server = "moscatestv1.herokuapp.com";

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;

long now         = millis();
long lastMeasure = 0;

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected !");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());  
  }
void callback(String topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Measage: ");
  String messageTemp;

  for(int i = 0; i < length; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
    }
  Serial.println();
  }
void reconnect(){
  while(!client.connected()){
    Serial.print("Attemping MQTT connection ...");

    if(client.connect("ESP8266Client")){
      Serial.println("connected");
     }else{
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println("Try again in 10 seconds");

      delay(10000);
       }
    }
  }
void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 Test!");
//init DHT sensor
  dht.setup(2, DHTesp::DHT11);
  setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    reconnect();
    }
  if(!client.loop()) client.connect("ESP8266Client");

  now = millis();

  if(now - lastMeasure > 30000){
    lastMeasure = now;

    float h = dht.getHumidity();
    float t = dht.getTemperature();
//  read Photodiode 
    int  p  = analogRead(A0); 
  if(isnan(h) || isnan(t)){
    Serial.println("Failed to read form DHT sensor!");
    return;
   }
  float hic = dht.computeHeatIndex(t, h, false);
  static char temperatureTemp[7];
  dtostrf(hic, 6, 2, temperatureTemp);
  client.publish("home/temp", temperatureTemp);
  
  static char humidityTemp[7];
  dtostrf(h, 6, 2, humidityTemp);  
  client.publish("home/humd", humidityTemp);
  
  static char photodiodeTemp[7];
  dtostrf(p, 6, 2, photodiodeTemp);  
  client.publish("home/pho", photodiodeTemp);
  
  Serial.println();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t Photodiode: ");
  Serial.print(p);
  Serial.println();
    }
}
