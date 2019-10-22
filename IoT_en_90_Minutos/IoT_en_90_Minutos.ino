#include "ThingSpeak.h"
#include "secrets2.h"
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password

WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = AUTH_TOKEN;

int pinDHT11 = D2;
SimpleDHT11 dht11(pinDHT11);

unsigned long tiempo_envio;

boolean estado_boton;

void setup() {
  Serial.begin(9600);

  Serial.println("inicializando red...");
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);

  WiFi.mode(WIFI_STA);

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Iniciando datalogger...");

  ThingSpeak.begin(client);
  Blynk.begin(auth, SECRET_SSID, SECRET_PASS);

  tiempo_envio = millis();

  pinMode(D4, OUTPUT);  //BUILTIN LED
  estado_boton = digitalRead(D3);
}

void loop() {
  Blynk.run();

  // reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  boolean boton = digitalRead(D3);
  if (boton != estado_boton) {
    if (boton == HIGH) {
      Serial.print("Puerta Abierta");
      grabaEvento(1);
    }
    else {
      Serial.print("Puerta Cerrada");
      grabaEvento(0);
    }
    estado_boton = boton;
  }

  //llamar a grabar datos cada 30 segundos...
  if ((millis() - tiempo_envio) > 30000) {
    grabaDatos();
    tiempo_envio = millis();
  }
}

void grabaDatos() {
  Serial.println("=================================");
  Serial.println("Leyendo DHT11...");

  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }

  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" H");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  //Mando datos a Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
}

void grabaEvento(int evento) {
  Serial.println("=================================");
  Serial.println("Mandando Evento...");

  // write to the ThingSpeak channel
  ThingSpeak.setField(3, evento);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
