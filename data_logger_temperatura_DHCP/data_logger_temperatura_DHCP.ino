#include <Ethernet.h>
#include <SPI.h>
#include "Timer.h"

#define NUM_ARDUINO X  //Sustituir X por el numero de Arduino correcto

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x70, 0xYY};  //Sustituir YY por el numero de MAC correcto

char url[] = "www.aprendiendoarduino.com";

EthernetClient client;
Timer t;
String webString = "";

void setup()
{

  Serial.begin(9600);
  Serial.println("inicializando red...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }
  Serial.println("Iniciando datalogger...");
  delay(1000);
  t.every(5000,grabaDatos);
}

void loop()
{
  webString = "";
  t.update();
  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()){
      char c = client.read();
      webString += c;
    }
    Serial.println(webString);
    if (webString.indexOf("GRABADOS") > 0) Serial.println("Datos guardados correctamente");
    else Serial.println("Error al guardar los datos");
    
   client.stop();
  }
}

void grabaDatos(){
  Serial.println("leyendo temperatura... ");
  int sensorVal = analogRead(A0);
  Serial.print("Valor Leido: ");
  Serial.println(sensorVal);
  
  Serial.println("connecting to server...");
  if (client.connect(url, 80)) {
    Serial.println("connected");
    client.print("GET /servicios/datos/grabaDatos.php?arduino="+(String)NUM_ARDUINO+"&dato=");
    client.print(sensorVal);
    client.println(" HTTP/1.1");
    client.println("Host: www.aprendiendoarduino.com");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed");
  }
}
