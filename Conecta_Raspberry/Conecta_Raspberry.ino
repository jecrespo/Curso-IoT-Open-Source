#include <Ethernet.h>
#include <SPI.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xYY};  //Sustituir YY por el numero de MAC correcto

byte ip_raspberrypi[] = {192, 168, 6, Z}; //Sustituir Z por la IP de la Raspberry Pi

EthernetClient client;

void setup()
{
  Serial.begin(9600);
  // start the Ethernet connection:
  Serial.println("inicializando red...");
  //Ethernet.begin(mac, ip, DNS, gateway, subnet);

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }

  delay(1000);
}

void loop()
{
  //Conecto a Raspberry Pi
  if (client.connect(ip_raspberrypi, 80)) {
    Serial.println("connected");
    client.println("GET /prueba.html HTTP/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }

//Espero respuesta raspberry Pi
  while (client.available() == 0) {
    //espero
    Serial.print(".");
    delay(100);
  }

  String webString = "";

//Leo respuesta Raspberry Pi
  do {
    char caracter_leido = client.read();
    webString += caracter_leido;
    delay(5);
  }  while (client.available() > 0);

  Serial.print("He recibido de Raspberry Pi: ");
  Serial.println(webString);
  client.stop();

  delay(5000);
}
