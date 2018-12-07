Krishna IIITD, [Dec 7, 2018 at 1:31:29 PM]:
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
float hum;  //Stores humidity value
float temp; //Stores temperature value
float moist;
int level;
float prev_hum=12;
float prev_temp=37;
const int trigPin = 12;
const int echoPin = 13;
const int VAL_PROBE = 17; //Analog pin 0 // the value after the LED goes on 
const char* ssid = "YU5040";
const char* password = "12345678";
int duration;
int distance;
int prev_distance=0;
int flag=0;
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 16
DHT dht(dht_dpin, DHTTYPE);
void setup() {
  
  Serial.begin(115200);                            
  //Serial connection
   WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
  }
ESP.wdtDisable();
ESP.wdtEnable(WDTO_8S);
//  ESP.wdtDisable();
//ESP.wdtEnable(WDTO_8S);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  yield();
  ESP.wdtFeed();
  pinMode(echoPin, INPUT);
  yield(); 
  ESP.wdtFeed();
 dht.begin();
   yield(); 
  ESP.wdtFeed();
}
void loop() {
//  ESP.wdtFeed();
  temp=temperature();
  hum=humidity();
  moist=moisture();
 level=waterlevelcontrol();
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   
      StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
      JsonObject& JSONencoder = JSONbuffer.createObject(); 
   
      //JSONencoder["sensorType1"] = "Temperature";
    JSONencoder["Id"] = "8450eca01665516d9aeb5317764902b78495502637c96192c81b1683d32d691a0965cf037feca8b9ed9ee6fc6ab8f27fce8f77c4fd9b4a442a00fc317b8237e6";
    JSONencoder["Temprature"] = temp;
      //JSONencoder["sensorType2"] = "Humidity";
    JSONencoder["Humidity"] =hum;
      //JSONencoder["sensorType3"] = "Moisture";
    JSONencoder["Soil_Moisture"] = moist;
      //JSONencoder["sensorType4"] = "Waterlevel";
    JSONencoder["Water_Level"] = level;    
      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println(JSONmessageBuffer);
   
      HTTPClient http;    //Declare object of class HTTPClient
   
      http.begin("http://192.168.43.246:8080/update");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
      http.end();  //Close connection
    } else {
      Serial.println("Error in WiFi connection");
    }
    delay(3000);//Send a request every 30 seconds

    if(level<=100){
      flag=0;}
    if(level>=120 && level<=150 && flag==0){
      flag=1;
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   
      StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
      JsonObject& JSONencoder = JSONbuffer.createObject(); 
   
      //JSONencoder["sensorType1"] = "Temperature";
      JSONencoder["Id"] = "8450eca01665516d9aeb5317764902b78495502637c96192c81b1683d32d691a0965cf037feca8b9ed9ee6fc6ab8f27fce8f77c4fd9b4a442a00fc317b8237e6";
      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println(JSONmessageBuffer);
      HTTPClient http;    //Declare object of class HTTPClient
      http.begin("http://192.168.43.246:8080/push");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
      http.end();  //Close connection
    } else {
      Serial.println("Error in WiFi connection");
    }
      }
}
int waterlevelcontrol(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  if(distance>=2000){
    distance=prev_distance;
    return distance;
    }
    prev_distance=distance;
  // Prints the distance on the Serial Monitor
  yield();
  delay(1000);
  return distance;
}
float temperature(){
  temp= dht.readTemperature();
//  Serial.print(" %, Temp: ");
//  Serial.print(temp);
//  Serial.println(" Celsius");
  if(isnan(temp))
    return prev_temp;
  prev_temp = temp;
  return temp;
}
float moisture(){
  int moisture = analogRead(VAL_PROBE);
  yield();
  return moisture;
}
float humidity(){
  hum = dht.readHumidity();
    //Print temp and humidity values to serial monitor
  //Serial.print("Humidity: ");
  //Serial.print(hum);
  if(isnan(hum))
    return prev_hum;
  prev_hum = hum;
  return hum;
}
