#include <WiFiS3.h>

WiFiClient client;

void create_ap() {
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  WiFi.config(IPAddress(192,48,56,2));
  if (!WiFi.beginAP("robot_team_3", "12345678")) {
    DEBUG_SERIAL.println("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  DEBUG_SERIAL.print("AP IP address: ");
  DEBUG_SERIAL.println(myIP);
}

void request_pdf(String page) {
  while (!client.connect("192.48.56.3", 8000)) {
    Serial.println("Connection failed");
    delay(500);
  }
  Serial.println("Connected to server");
  // HTTP 요청 보내기
  client.println((String) "GET /" + page + " HTTP/1.1");
  client.println("Host: 192.48.56.3");
  client.println();
  delay(500);
}