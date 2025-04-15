#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

const int dir1 = 2;
const int step1 = 3;
const int dir2 = 4;
const int step2 = 5;

void setup() {
  pinMode(dir1, OUTPUT);
  pinMode(step1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(step2, OUTPUT);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    String req = "";
    bool currentLineIsBlank = true;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        req += c;
        if (c == '\n' && currentLineIsBlank) {
          handleRequest(req, client);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

void handleRequest(String req, EthernetClient& client) {
  int motor = 0;
  int angle = 0;

 
  int mIndex = req.indexOf("/motor/");
  if (mIndex != -1) {
    motor = req.charAt(mIndex + 7) - '0';
  }


  int aIndex = req.indexOf("angle=");
  if (aIndex != -1) {
    angle = req.substring(aIndex + 6).toInt();
  }

  if ((motor == 1 || motor == 2) && angle % 15 == 0 && abs(angle) <= 90) {
    moveMotor(motor, angle);
    sendResponse(client, "Motor " + String(motor) + " movido " + String(angle) + " graus");
  } else {
    sendResponse(client, "Comando inválido");
  }
}

void moveMotor(int motor, int angle) {
  int stepsPer15 = 8; 
  int steps = abs(angle / 15) * stepsPer15;

  int dirPin = (motor == 1) ? dir1 : dir2;
  int stepPin = (motor == 1) ? step1 : step2;

  digitalWrite(dirPin, angle >= 0 ? HIGH : LOW);

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
}

void sendResponse(EthernetClient& client, String msg) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println(msg);
}
