# ObjetosInteligentes-GB
Repositório feito para guardar informações do projeto final da matéria de objetos inteligentes conectados da Universidade Presbiteriana Mackenzie

Este projeto tem como objetivo utilizar um sensor de movimento infravermelho para captar entradas e saídas de um ambiente controlado, caso o sensor capte algum movimento, um alerta será enviado para uma url via Wifi.  

O desenvolvimento do projeto será integrar o “esp32” via WiFi, utilizando um sensor de movimento “HC-sr501 pir” para disparar um sinal para a url http://rangecomp.com.br:1880/ui/#!/7 caso alguma ação seja detectada. Junto a isso, teremos a ação do atuador “servo motor mg995”, que puxará uma linha presa em um ponto fixo, exercendo pressão em um aparelho “Bom ar”.
Para o desenvolvimento do código, foi usado o software do "Arduino" https://www.arduino.cc/en/software.

Hardware utilizado:
  1- ESP32 Com o propósito de comunicar via Wifi o sinal enviado pelo sensor de movimento. A placa faz do uso de uma interface micro USB]. 

  2- Sensor de presença/movimento HC-sr501 pir. Com o propósito de captar movimento via sensor de radiação infravermelha, disparando um sinal que será captado pelo celular caso haja movimento.  

  3- Protoboard 830 furos. Este Protoboard permite que os componentes eletrônicos possam ser interligados em um número quase infinito de maneiras para produzir circuitos eletrônicos em trabalhos de prototipagem. 

  4- Jumpers 10cm macho-macho. Os jumpers são peças fundamentais com funções como, desviar, ligar ou desligar o fluxo elétrico.

  5- Jumpers 10cm macho-fêmea. Os jumpers são peças fundamentais com funções como, desviar, ligar ou desligar o fluxo elétrico. 

  6- Jumpers 10cm fêmea - fêmea. Os jumpers são peças fundamentais com funções como, desviar, ligar ou desligar o fluxo elétrico. 

 
Código utilizado:

#include <WiFi.h> //biblioteca de uso do wifi no ESP32 (autenticação)
#include <PubSubClient.h> //biblioteca do MQTT
#include <iostream>
#include <string> //biblioteca de manipulação de strings
#define COUNT_LOW 0
 #define COUNT_HIGH 8888
 #define TIMER_WIDTH 16Q
#include "esp32-hal-ledc.h"

IPAddress local_IP(192, 168, 0, 231); //ip local estatico
IPAddress gateway(192, 168, 0, 1); //ip do gateway da conexão
IPAddress subnet(255, 255, 255, 0); // mascara de rede
WiFiClient BomAr; //nome de identificação do dispositivo na rede
const char* ssid = ""; //SSID da rede wireless
const char* password = ""; //PASSWORD da rede wireless
const char* mqtt_server = "rangecomp.com.br"; //ip do servidor mqttQ
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

 
