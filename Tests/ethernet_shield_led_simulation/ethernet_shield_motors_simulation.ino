#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);

EthernetServer servidor(80);

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);  
  servidor.begin();
  delay(2000);
  Serial.println("Setup done!");
  Serial.print("IP do Arduino: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient cliente = servidor.available();
  if (cliente) {
    Serial.println("Cliente conectado.");

    String primeiraLinha = "";
    boolean requisicaoRecebida = false;

    while (cliente.connected()) {
      if (cliente.available()) {
        String linha = cliente.readStringUntil('\r');
        cliente.read(); // consome o '\n'

        if (!requisicaoRecebida && linha.startsWith("GET")) {
          primeiraLinha = linha;
          requisicaoRecebida = true;
        }

        if (linha.length() == 0) {
          // Envia página HTML
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");
          cliente.println("Connection: close");
          cliente.println();
          cliente.println("<!DOCTYPE HTML>");
          cliente.println("<html>");
          cliente.println("<head><title>Controle de Motores</title></head>");
          cliente.println("<body>");
          cliente.println("<h1>Controle de Motores via Ethernet</h1>");

          // Slider para motor 1
          cliente.println("<form action=\"/\" method=\"GET\">");
          cliente.println("<label for=\"motor1\">Motor 1 (-180° a 180°):</label><br>");
          cliente.println("<input type=\"range\" id=\"motor1\" name=\"motor_1\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
          cliente.println("<output>0</output><br>");
          cliente.println("<input type=\"submit\" value=\"Enviar\">");
          cliente.println("</form><br><br>");

          // Slider para motor 2
          cliente.println("<form action=\"/\" method=\"GET\">");
          cliente.println("<label for=\"motor2\">Motor 2 (-180° a 180°):</label><br>");
          cliente.println("<input type=\"range\" id=\"motor2\" name=\"motor_2\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
          cliente.println("<output>0</output><br>");
          cliente.println("<input type=\"submit\" value=\"Enviar\">");
          cliente.println("</form>");

          cliente.println("</body></html>");

          // Interpreta o comando
          if (primeiraLinha.indexOf("GET /?motor_1=") >= 0) {
            int pos = primeiraLinha.indexOf("motor_1=") + 8;
            String valorStr = primeiraLinha.substring(pos, primeiraLinha.indexOf(" ", pos));
            int valor = valorStr.toInt();
            Serial.print("Comando Motor 1: ");
            Serial.print(valor);
            Serial.println(" graus");
          } else if (primeiraLinha.indexOf("GET /?motor_2=") >= 0) {
            int pos = primeiraLinha.indexOf("motor_2=") + 8;
            String valorStr = primeiraLinha.substring(pos, primeiraLinha.indexOf(" ", pos));
            int valor = valorStr.toInt();
            Serial.print("Comando Motor 2: ");
            Serial.print(valor);
            Serial.println(" graus");
          } else {
            Serial.println("Comando não reconhecido.");
          }

          break;
        }
      }
    }

    delay(1);
    cliente.stop();
    Serial.println("Cliente desconectado.");
  }
}
