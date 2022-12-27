#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

const char *ssid     = "IoT";
const char *password = "password";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com");
ESP8266WebServer server(80);

#define NET_TIMEOUT 10

void handle_root();
void handle_not_found();

bool ConnectToWiFi(const char *networkName, const char *networkPassword)
{
  bool result = false;
  WiFi.begin(networkName, networkPassword);
  Serial.print("Connecting to ");
  Serial.print(networkName);
  Serial.print(" ...");
  for (int i = 0; i < NET_TIMEOUT; i++)
  {
    delay(1000);
    if (WiFi.status() == WL_CONNECTED)
    {
      result = true;
      Serial.println("\nConnected");
      Serial.print("Local IP:\t");
      Serial.println(WiFi.localIP());
      Serial.print("MAC Address:\t");
      Serial.println(WiFi.macAddress());
      Serial.print("DNS IP:\t");
      Serial.println(WiFi.dnsIP());
      break;
    }
    else
    {
      Serial.print(i);
      Serial.println(' ');
    }
  }
  return result;
}

void setup(){
  Serial.begin(115200);
  Serial.println('\n');
  if (ConnectToWiFi(ssid, password))
  {
    timeClient.setUpdateInterval(600000);
    timeClient.begin();
    server.on("/", handle_root);
    server.onNotFound(handle_not_found);
    server.begin();
    Serial.println("HTTP server started");
  }
  else
  {
      Serial.println("Could not connect to the network.");
      Serial.println("The web server will not be started.");
      Serial.println("Reset the device to try again.");
  }
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println();

  timeClient.begin();
}

void loop()
{
  server.handleClient();
}

void handle_root()
{
  timeClient.update();
  server.send(200, "text/plain", timeClient.getFormattedTime());
}

void handle_not_found()
{
  server.send(404, "text/plain", "Not found");
}
