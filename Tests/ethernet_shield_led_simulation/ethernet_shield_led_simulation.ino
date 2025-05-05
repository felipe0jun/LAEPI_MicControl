#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);

EthernetServer servidor(80);

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  Serial.print("IP do Arduino: ");
  Serial.println(Ethernet.localIP());
  servidor.begin();
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

        // Armazena a primeira linha da requisição
        if (!requisicaoRecebida && linha.startsWith("GET")) {
          primeiraLinha = linha;
          requisicaoRecebida = true;
        }

        if (linha.length() == 0) {
          // Envia resposta
          cliente.println("HTTP/1.1 200 OK");
          cliente.println("Content-Type: text/html");
          cliente.println("Connection: close");
          cliente.println();
          cliente.println("<!DOCTYPE HTML>");
          cliente.println("<html>");
          cliente.println("<h1>Controle do LED</h1>");
          cliente.println("<p><a href=\"/?led_ligar\"><button>LIGAR</button></a></p>");
          cliente.println("<p><a href=\"/?led_desligar\"><button>DESLIGAR</button></a></p>");
          cliente.println("</html>");

          // Verifica comando baseado somente na primeira linha da requisição
          if (primeiraLinha.indexOf("GET /?led_ligar") >= 0) {
            Serial.println("Comando: LIGAR");
          } else if (primeiraLinha.indexOf("GET /?led_desligar") >= 0) {
            Serial.println("Comando: DESLIGAR");
          } else {
            Serial.println("Requisição padrão ou desconhecida.");
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
