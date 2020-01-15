#include <ESP8266WiFi.h>
#include <DHT.h>  
#include <SDS011.h>

//서버에 전송하기 위한 초기설정
const char* server = "api.thingspeak.com";
String apiKey = "8K86QM6F7GST6OHZ";
const char* MY_SSID = "GENYF WIFI MAIN";//와이파이 이름
const char* MY_PWD = "genyfwifi";//와이파이 비밀번호

float P10, P25;
float Temp, Humi;

SDS011 my_sds;

#define DHTPIN D4
 
DHT dht(DHTPIN, DHT22);

//// 초기 설정(시리얼 통신, 핀 설정)
void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  my_sds.begin(D5,D6);// 핀 설정

//와이파이 연결
  Serial.print("Connecting to " + *MY_SSID);

  WiFi.begin(MY_SSID, MY_PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected\n");
}

void GetData(){
  my_sds.read(&P25,&P10);//미세먼지 측정
  Temp = dht.readTemperature();//온도측정
  Humi = dht.readHumidity();//습도측정
  Serial.println("Measuring...");
  delay(5000);
}

void loop() {
  GetData();
  
  while (isnan(Humi) || isnan(Temp) || isnan(P10) || isnan(P25))
  {
    GetData();
  }
  
  WiFiClient client;//Wifi 연결

  Serial.println("P10:  "+ String(P10) + "ug/m3");
  Serial.println("P2.5: "+ String(P25) + "ug/m3");

  Serial.println("Temp :" + String(Temp) + "C");
  Serial.println("Humi :" + String(Humi) + "%");
 
  if (client.connect(server, 80)) { // 서버에 전송
    Serial.println("WiFi Client connected");

    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(P10);
    postStr +="&field2=";
    postStr += String(P25);
    postStr +="&field3=";
    postStr += String(Temp);
    postStr +="&field4=";
    postStr += String(Humi);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  Serial.print("Waiting...\n\n\n");
  client.stop();
}
