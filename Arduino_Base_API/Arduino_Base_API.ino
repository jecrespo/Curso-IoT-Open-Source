#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY  //Sustituir YY por el numero de MAC correcto
};

EthernetClient client;
String webString = "";
char url[] = "aemet.es";  //Poner la URL que llama la PAI
char comando[] = ""; //comando de llamada a la API

void setup()
{
  Serial.begin(9600);
  Serial.println("Ejemplo de uso de APIs con Arduino, iniciar Arduino para lanzar una nueva llamada a la API");
  String cadenaLeida = "";
  // start the Ethernet connection:
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

  if (client.connect(url, 80)) {
    Serial.println("connected");
    client.println("GET /xml/municipios/localidad_26089.xml HTTP/1.1");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }

  void loop()
  {
    if (client.available()) {
      char c = client.read();
      webString += c;
      Serial.print(c);
    }

    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      //Serial.println(webString);
      for (;;)
        ;
    }
  }
