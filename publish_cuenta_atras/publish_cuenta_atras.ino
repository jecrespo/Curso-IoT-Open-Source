
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Direccion MAC
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x21 };  //Sustituir YY por el número de kit

// IP del servidor
IPAddress mqtt_server(192, 168, 6, 72); //poner la IP de mosquitto al que conectarse

// Topic con el que trabajamos
const char* topicName = "countdown";

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup()
{
  Serial.begin(9600);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  client.setServer(mqtt_server, 1883);
}

void loop()
{
  if (!client.connected()) {
    Serial.print("Connecting ...\n");
    client.connect("arduinoClient21", "curso_iot", "raspberry");  //Sustituir XX por número de puesto
    // Envio la cuenta atras
    for (int i = 10; i >= 0; i--) {
      char buffer[5];
      sprintf(buffer, "%i", i);
      Serial.print("Envio la cadena de caracteres: ");
      Serial.println(buffer);
      client.publish(topicName, buffer);
      delay(1000);
    }
  }
  else {
    Serial.println("Fallo conexión");
  }

  // Tiempo entre envios (en ms)
  delay(10000);
}
