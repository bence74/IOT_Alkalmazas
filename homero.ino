#include <WiFi.h>
#include <DHT.h>


const char* ssid = "sajat_wifi";
const char* password = "jelszo";

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(1000);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi csatlakoztatva!");
  Serial.print("IP cím: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            float h = dht.readHumidity();
            float t = dht.readTemperature();

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"5\">");
            client.println("<title>ESP32 Hőmérő</title>");
            client.println("<style>body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #f4f4f4; }</style>");
            client.println("</head><body>");
            client.println("<h1>ESP32 IoT Hőm&eacute;rs&eacute;klet</h1>");

            if (isnan(h) || isnan(t)) {
              client.println("<p style='color:red;'>Hiba a szenzor olvasásakor!</p>");
            } else {
              client.println("<p style='font-size: 28px;'>Hőmérséklet: <strong>" + String(t, 1) + " &deg;C</strong></p>");
              client.println("<p style='font-size: 28px;'>Páratartalom: <strong>" + String(h, 1) + " %</strong></p>");
            }
            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}