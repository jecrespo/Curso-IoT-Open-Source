/*
  demo-mqtt para implementar un sistema de aviso de temperaturas

  El montaje consiste en:
  -  Arduino Mega
  -  Ethernet Shield
  -  Dos sondas DHT22 conectada a los pines 6 y 7
  -  Un led bicolor (Rojo/Verde) conectado a los pines 22 y 23

  Topics suscritos:
  - umbral_1
  - umbral_2

  Topics publicados:
  - Temp_1
  - Temp_2
  - Hum_1
  - Hum_2
  - reconnect

*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Timer.h>  //librería https://github.com/JChristensen/Timer 
#include <dht.h>  //librería https://github.com/jecrespo/DHTlib

#define PIN_Sensor_Sonda_1 6
#define PIN_Sensor_Sonda_2 7
#define PIN_LED_1 3
#define PIN_LED_2 4
#define PIN_AVISO 5

Timer t;

dht sonda_1;
dht sonda_2;

unsigned long int anterior = 0;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xYY };
IPAddress server(192, 168, 1, X);

// Respuesta de los topic a los que me he subscrito en la función reconnect
void callback(char* topic, char* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  payload[length] = '\0'; //Para que pueda convertir correctamente a String

  //analizar el topic y el payload para ejecutar lo necesario con String
  String topic_S = topic;
  String payload_S = payload;

  if (topic_S == "Alarma_1") {
    if (payload_S == "true") {
      digitalWrite(PIN_LED_1, HIGH);
    }
    else
      digitalWrite(PIN_LED_1, LOW);
  }

  if (topic_S == "Alarma_2") {
    if (payload_S == "true") {
      digitalWrite(PIN_LED_2, HIGH);
    }
    else
      digitalWrite(PIN_LED_2, LOW);
  }
  if (topic_S == "Aviso") {
    if (payload_S == "ON") {
      digitalWrite(PIN_AVISO, HIGH);
    }
    else
      digitalWrite(PIN_AVISO, LOW);
  }
}

EthernetClient ethClient;
PubSubClient client(ethClient);

// Función reconexión que se ejecuta en el loop si pierdo conexión
// En reconnect también me subscribo a los topics y publico que me he reiniciado
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClientYY","curso_iot","password")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("reconnect", "Arduino Connect!!!");
      // ... and resubscribe
      client.subscribe("Umbral_1");
      client.subscribe("Umbral_2");
      client.subscribe("Alarma_1");
      client.subscribe("Alarma_2");
      client.subscribe("Aviso");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Función que publica cada 10 segundos la temperatura en los topics
void sendTemperatures () {
  Serial.println("Send Temperatures...");
  int resultado_1 = sonda_1.read22(PIN_Sensor_Sonda_1);
  int resultado_2 = sonda_2.read22(PIN_Sensor_Sonda_2);
  if (resultado_1 == DHTLIB_OK) {
    char buffer_t[6];
    char buffer_h[6];
    float temperatura_1 = sonda_1.temperature;
    float humedad_1 = sonda_1.humidity;
    float2char(temperatura_1, buffer_t);
    float2char(humedad_1, buffer_h);
    Serial.println("-- Sonda 1 --");
    Serial.print(temperatura_1);
    Serial.print(" -> ");
    Serial.println(buffer_t);
    Serial.print(humedad_1);
    Serial.print(" -> ");
    Serial.println(buffer_h);
    client.publish("Temp_1", buffer_t);
    client.publish("Hum_1", buffer_h);
  }
  else {
    //publicar en contador, ver si podría se algo como añadir 1 al topic
    switch (resultado_1)
    {
      case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
      default:
        Serial.print("Unknown error,\t");
        break;
    }
  }

  if (resultado_2 == DHTLIB_OK) {
    char buffer_t[6];
    char buffer_h[6];
    float temperatura_2 = sonda_2.temperature;
    float humedad_2 = sonda_2.humidity;
    float2char(temperatura_2, buffer_t);
    float2char(humedad_2, buffer_h);
    Serial.println("-- Sonda 2 --");
    Serial.print(temperatura_2);
    Serial.print(" -> ");
    Serial.println(buffer_t);
    Serial.print(humedad_2);
    Serial.print(" -> ");
    Serial.println(buffer_h);
    client.publish("Temp_2", buffer_t);
    client.publish("Hum_2", buffer_h);
  }
  else {
    //publicar en contador, ver si podría se algo como añadir 1 al topic
    switch (resultado_2)
    {
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Checksum error,\t");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error,\t");
        break;
      default:
        Serial.println("Unknown error,\t");
        break;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_LED_1,OUTPUT);
  pinMode(PIN_LED_2,OUTPUT);
  pinMode(PIN_AVISO,OUTPUT);

  t.every(10000, sendTemperatures);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  t.update();

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

int float2char(float number, char* buffer_c) { //Solo para número de 99.99 hasta 0. buffer[6]={'2','6','.','3','5',\0}
  if ((number > 99.99) || (number < 0))
    return 0;

  int entero = floor(number);
  int decimal = round((number - entero) * 100);

  int decenas = int(floor(entero / 10));
  int unidades = int(floor(entero - (decenas * 10)));
  int decimal_1 = int(floor(decimal / 10));
  int decimal_2 = int(floor(decimal - (decimal_1 * 10)));

  //sumo 48 para obtener el código ascii del número
  buffer_c[0] = char(decenas + 48);
  buffer_c[1] = char(unidades + 48);
  buffer_c[2] = '.';
  buffer_c[3] = char(decimal_1 + 48);
  buffer_c[4] = char(decimal_2 + 48);
  buffer_c[5] = '\0';
  return 1;
}
