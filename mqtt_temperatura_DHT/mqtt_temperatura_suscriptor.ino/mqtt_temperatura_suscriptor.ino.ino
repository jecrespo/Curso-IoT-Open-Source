#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Direccion MAC
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY };  //Cambiar YY por el número de kit

// IP del servidor
IPAddress mqtt_server(192, 168, x, x);  //Cambiar por la IP del servidor mosquitto

// Topic con el que trabajamos
const char* topicName = "temperatura";

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i=0;
  for (i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
      Serial.print("Connecting ...");
      if (client.connect("rece_arduino")) {
        Serial.println("connected");
        client.subscribe(topicName);
      } else {
        delay(5000);
      }
  }
  // Cliente a la escucha
  client.loop();
}
