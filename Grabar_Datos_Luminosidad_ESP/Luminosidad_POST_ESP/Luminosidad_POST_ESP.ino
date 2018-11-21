#include <ESP8266WiFi.h>
#define NUM_ARDUINO "X" //Poner en X el número de Arduino
const int LDRPin = A0;

byte ip_raspberrypi[] = {192, 168, 6, Z}; //Sustituir Z por la IP de la Raspberry Pi

const char* ssid     = "ssid";
const char* password = "password";

WiFiClient client;

void setup()
{
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
}

void loop()
{
  //Leo datos de luminosidad
  int luminosidad = analogRead(LDRPin);
  String post_string = "arduino=" + String(NUM_ARDUINO) + "&IntensidadLuminosa=" + String(luminosidad);

  //Conecto a Raspberry Pi
  if (client.connect(ip_raspberrypi, 80)) {
    Serial.println("connected");
    client.println("POST /Graba_POST.php HTTP/1.1");
    client.println("Host: miraspberry");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(post_string.length());
    client.println("");
    client.println(post_string);
    client.println("");
    Serial.println("Peticion enviada---->");
    Serial.println("POST /Graba_POST.php HTTP/1.1");
    Serial.println("Host: miraspberry");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("Connection: close");
    Serial.print("Content-Length: ");
    Serial.println(post_string.length());
    Serial.println("");
    Serial.println(post_string);
    Serial.println("");
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
