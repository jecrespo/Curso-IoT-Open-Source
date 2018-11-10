#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11

// Direccion MAC
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY };  //Sustituir YY por el número de kit

// IP del servidor
IPAddress mqtt_server(192, 168, x, x); //poner la IP de mosquitto al que conectarse

// Topic con el que trabajamos
const char* topicName = "temperatura";

DHT dht(DHTPIN, DHTTYPE);
EthernetClient ethClient;
PubSubClient client(ethClient);

void setup()
{
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

void loop()
{
  if (!client.connected()) {
    Serial.print("Connecting ...\n");
    client.connect("Arduino Client");
  }
  else {
    // Envio
    float temp = dht.readTemperature();
    char buffer[10];
    dtostrf(temp,0, 0, buffer);
    client.publish(topicName, buffer);
  }
  // Tiempo entre envios (en ms)
  delay(5000);
}
