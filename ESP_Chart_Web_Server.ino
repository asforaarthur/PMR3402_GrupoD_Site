#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHT10.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

Adafruit_AHT10 aht;

const char* ssid = "ESP_AHT10";
const char* password = "12345678";

AsyncWebServer server(80);

String readAHT10Temperature() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  
  float t = temp.temperature;
  if (isnan(t)) {    
    Serial.println("Failed to read from AHT10 sensor!");
    return "";
  } else {
    Serial.println("Temperature: " + String(t) + " °C");
    return String(t);
  }
}

String readAHT10Humidity() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float h = humidity.relative_humidity;
  if (isnan(h)) {
    Serial.println("Failed to read from AHT10 sensor!");
    return "";
  } else {
    Serial.println("Humidity: " + String(h) + " %");
    return String(h);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the AHT10 sensor
  if (!aht.begin()) {
    Serial.println("Could not find a valid AHT10 sensor, check wiring!");
    while (1);
  }

  // Configure ESP as a Wi-Fi access point
  Serial.println();
  Serial.print("Configuring access point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Failed to configure access point");
    return;
  }
  Serial.println("Access point configured");

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html; charset=UTF-8");
  });

  // Route to serve the index.html file
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html; charset=UTF-8");
  });

  // Route to serve the script.js
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/javascript; charset=UTF-8");
  });

  // Route to serve the style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css; charset=UTF-8");
  });

  // Route to serve images
  server.serveStatic("/images/", SPIFFS, "/images/");
  
  // Route to serve grafic_temp_umid.html
  server.on("/grafic_temp_umid.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/grafic_temp_umid.html", "text/html; charset=UTF-8");
  });

  // Route to serve about.html
  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/about.html", "text/html; charset=UTF-8");
  });

  // Route to serve contact.html
  server.on("/contact.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/contact.html", "text/html; charset=UTF-8");
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    String temp = readAHT10Temperature();
    request->send_P(200, "text/plain; charset=UTF-8", temp.c_str());
    Serial.println("Temperature sent: " + temp);
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    String hum = readAHT10Humidity();
    request->send_P(200, "text/plain; charset=UTF-8", hum.c_str());
    Serial.println("Humidity sent: " + hum);
  });

  server.begin();
}

void loop() {
  readAHT10Temperature(); // Chama a função de leitura da temperatura
  readAHT10Humidity(); // Chama a função de leitura da umidade
  delay(1000); // Delay de 1 segundos entre cada leitura
}
