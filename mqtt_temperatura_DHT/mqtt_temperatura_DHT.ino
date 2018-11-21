#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 6
#define DHTTYPE DHT11

// Direccion MAC
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY };  //Sustituir YY por el número de kit

// IP del servidor
IPAddress mqtt_server(192, 168, X, X); //poner la IP de mosquitto al que conectarse

// Topic con el que trabajamos
const char* topicNameT = "temperaturaXX/temp"; //Sustituir XX por el número de kit
const char* topicNameH = "temperaturaXX/hum"; //Sustituir XX por el número de kit

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
    client.connect("arduinoClientXX", "curso_iot", "raspberry");  //Sustituir XX por número de puesto
  }
  else {
    // Envio
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    char bufferT[10];
    char bufferH[10];
    dtostrf(temp, 0, 0, bufferT); //solo para Arduinos con MCU AVR https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__stdlib_1ga060c998e77fb5fc0d3168b3ce8771d42.html
    client.publish(topicNameT, bufferT);
    dtostrf(hum, 0, 0, bufferH);
    client.publish(topicNameH, bufferH);
  }
  // Tiempo entre envios (en ms)
  delay(5000);
}
