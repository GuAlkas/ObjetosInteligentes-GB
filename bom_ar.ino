#include <WiFi.h> //biblioteca de uso do wifi no ESP32 (autenticação)
#include <PubSubClient.h> //biblioteca do MQTT
#include <iostream>
#include <string> //biblioteca de manipulação de strings
#define COUNT_LOW 0
 #define COUNT_HIGH 8888
 #define TIMER_WIDTH 16
#include "esp32-hal-ledc.h"

IPAddress local_IP(192, 168, 0, 231); //ip local estatico
IPAddress gateway(192, 168, 0, 1); //ip do gateway da conexão
IPAddress subnet(255, 255, 255, 0); // mascara de rede
WiFiClient BomAr; //nome de identificação do dispositivo na rede
const char* ssid = "nome da rede "; //SSID da rede wireless
const char* password = "senha da rede"; //PASSWORD da rede wireless
const char* mqtt_server = "rangecomp.com.br"; //ip do servidor mqtt
const char* mqtt_topic = "Alarme/feedback"; //topico a ser ouvido
const uint16_t mqtt_port = 1889; // porta do servidor mqtt
const char* mqtt_name = "BomAr"; //nome do dispositivo mqtt
const char* mqtt_user = ""; //nome de usuario do servidor mqtt
const char* mqtt_password = ""; // senha do servidor mqtt (client)

const int SensorPin = 12;     // the number of the pushbutton pin
const int MotorPin =  13;      // the number of the LED pin
int SensorState = 0;   
int LED = 2; // variable for reading the pushbutton status

PubSubClient client(BomAr); //nome do dispositivo para o servidor mqtt

String recebido, toprint, buff, lasttemp;
double lastMsg, timeset;

void checarwifi(){ //função responsável por monitorar a conexão wifi e refazer a conexão
  if(WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
}

void callback(char* topic, byte* message, unsigned int length) { //função de callback do mqtt (responsável por receber as mensagens
  String messageTemp;
    for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (String(topic) == mqtt_topic) {
    Serial.println(messageTemp);
    recebido = messageTemp;
   }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqtt_name)) {
      client.subscribe(mqtt_topic);
      delay(2000);
    } else {
      checarwifi();
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callback);
      delay(2500);
    }
  }
}


void setup_wifi() { //função que faz a conexão a rede wifi
WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

  }
}

void setup() {

  ledcSetup(1, 50, TIMER_WIDTH); // channel 1, 50 Hz, 16-bit width
  ledcAttachPin(13, 1);   // GPIO 22 assigned to channel 1
  Serial.begin(115200);
  //servo1.attach(MotorPin, 1000, 2000);
  pinMode(SensorPin, INPUT);
  pinMode(LED, OUTPUT);
  Serial.println("iniciando...");
  
setup_wifi();
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
}

void SendMQTTMessage(){
  client.publish("Alarme", recebido.c_str());
}


void loop(){ 
  
  if (!client.connected()) {
  reconnect();
  }

  client.loop();
  long now = millis();

    Serial.println("lendo sensor");
  SensorState = digitalRead(SensorPin);
  if (SensorState == HIGH) {

   // servo1.write(20);
   ledcWrite(1, 1000); 
    digitalWrite(LED, HIGH); 
    Serial.println("sensor ligado!");
    client.publish("Alarme", "Movimento detectado!");
    delay(1000);

  } else {
   // servo1.write(160);
   ledcWrite(1, 50000);
    digitalWrite(LED, LOW); 
    Serial.println("sensor desligado!");
    
 
    }          
}

 
