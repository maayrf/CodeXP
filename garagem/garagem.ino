#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <Servo.h>

int LED = 7;
int ButtonG = 6;
int ButtonL = 5;

int BttGU = LOW;
int BttLU = LOW;

int BttGE = LOW;
int BttLE = LOW;

Servo Motor;

byte mac[] = {0xDE , 0xAD , 0xBE , 0xEF , 0xAA, 0xB1};

char* servidor_mqtt = "m14.cloudmqtt.com";
int porta_mqtt = 16015;

long lastReconnectAttempt = 0;

void message_mqtt (char* topic, byte* payload, unsigned int length) {
  
  char* payload_char = payload;
  payload_char[length] = 0;
  String payload_string = String(payload_char);
  String topic_string = String(topic);
  
 // Serial.print("TOPIC:");
 // Serial.println(topic_string);
 // Serial.print("PAYLOAD:");
  //Serial.println(payload_string);

  if (topic_string == "Motor" && payload_string == "1"){
    
    Motor.write(90);
    delay (100);
    digitalWrite(LED, HIGH);
    }
    
  if (topic_string == "Motor" && payload_string == "0"){
    
    Motor.write(180);
    delay (100);
    digitalWrite(LED, LOW);
    }   

    if (topic_string == "LED" && payload_string == "1"){
    digitalWrite(LED, HIGH);
    delay(500);
    }
    
  if (topic_string == "LED" && payload_string == "0"){
    digitalWrite(LED, LOW);
    delay(500);
    }   
  }

EthernetClient ethernet_client;

PubSubClient pubsub(servidor_mqtt, porta_mqtt, message_mqtt, ethernet_client);

boolean reconnect () {
  Serial.println("Reconectando...");
  
  if (pubsub.connect("Mayara", "guivkxnq", "z9jSVqTskGy5"))
  {
    Serial.println("Conectando..");
    pubsub.publish("Motor", "Voltei");
    Serial.println("Voltei enviando");
    pubsub.subscribe("Motor");
    pubsub.subscribe("LED");
  }
  }


void setup() {
  // put your setup code here, to run once:

pinMode(LED, OUTPUT); 
pinMode(ButtonG, INPUT);
pinMode(ButtonL, INPUT);

Motor.attach(4);
Motor.write(180);

Serial.begin(9600);

if (Ethernet.begin(mac) == 0) {
  Serial.println("Não pegou DHCP");
  }
else {
  Serial.println("Pegou DHCP");
  Serial.println(Ethernet.localIP());
  }
 
if (pubsub.connect("Mayara", "guivkxnq", "z9jSVqTskGy5")){
    Serial.println("Conectado");
    pubsub.publish("Motor", "Voltei");
    Serial.println("Voltei enviando");
    pubsub.subscribe("Motor");
  }

else{
  Serial.println("MQTT não conectado");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

BttLE = digitalRead (ButtonL);
BttGE = digitalRead (ButtonG);

if (BttLE == HIGH){
    digitalWrite (LED,!digitalRead(LED));
    delay (500);
    }
if (BttGE == HIGH){
  if (Motor.read() >= 90 && Motor.read() <= 100){
    Motor.write(180);
    delay (100);
    digitalWrite(LED, HIGH);
    delay(500);
    }
    
  else {
    Motor.write(90);
    delay (100);
    digitalWrite(LED, LOW);
    delay(500);
    }  
    }


if (!pubsub.connected()){
  long now = millis();
  
  if (now - lastReconnectAttempt > 5000){
    
    Serial.println("Reconectando");
    lastReconnectAttempt = now;

    if (reconnect()){
      lastReconnectAttempt = 0;
      }
    }
  }
  else{
    pubsub.loop();
    }
}
