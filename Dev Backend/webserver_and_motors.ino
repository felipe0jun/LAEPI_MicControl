#include <SPI.h>
#include <Ethernet.h>

// ---------------- Informativos para desenvolvimento ----------------
// 2 variáveis para coletar informação dos 2 sensores (ou botões) no fim de curso de cada motor (para dizer se chegou no angulo máximo ou não)
// 2 variáveis para armazenar a quantidade de angulos que cada motor andou para já verificar se é possível movimentar mais (ou não) além de sempre, antes de mover cada motor, verificar se o sensor de fim de curso (ou botão) deles está como True
// Refatorar código de rotação do motor para girar por passo, verificando sempre o angulo / sensor de fim de curso / botão para saber se pode dar mais um passo para completar os ângulos.

// Criar um mapeador de passos com seus respectivos angulos andados e salvar num arquivo no Cartão SD
// O 1 motor vai começar com ângulo de 270° (ou -90°), ou seja, apontado para baixo e o motor 2 pode começar em 0°.

// Colocar Cartão SD com código .ino e index.html mais bonito.

// ---------------- Configuração de Modo ----------------
bool modoComRede = false; // Alterar para 'true' para usar o modo com rede (usar ethernet shield). 'false' para usar Serial Monitor para enviar comando como "motor_1_50" para girar motor 1 em 50°.

// ---------------- Ethernet Configuration ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);
EthernetServer servidor(80);

// ---------------- Motor Configuration ----------------
#define directionPin1 2
#define stepPin1 3
#define directionPin2 4
#define stepPin2 5

#define stepsPerRevolution 6400
#define stepsPerDegree (stepsPerRevolution / 360.0)  // ~17.78 passos por grau

#define delayMicrosecondsDesired 300

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  pinMode(directionPin1, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);

  if (modoComRede) {
    Ethernet.begin(mac, ip);
    servidor.begin();
    delay(2000);
    Serial.println("Modo COM rede iniciado.");
    Serial.print("IP do Arduino: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Modo SEM rede iniciado. Digite comandos no formato: motor_1_90");
  }
}

// ---------------- Motor Functions ----------------
void rotateMotor(int motor, int degrees, bool direction) {
  if (degrees % 5 != 0) return; // Apenas múltiplos de 5°
  int steps = round(abs(degrees) * stepsPerDegree); // Converte graus em passos

  if (motor == 1) {
    digitalWrite(directionPin1, direction ? HIGH : LOW);
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin1, HIGH);
      delayMicroseconds(delayMicrosecondsDesired);
      digitalWrite(stepPin1, LOW);
      delayMicroseconds(delayMicrosecondsDesired);
    }
  } else if (motor == 2) {
    digitalWrite(directionPin2, direction ? HIGH : LOW);
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(delayMicrosecondsDesired);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(delayMicrosecondsDesired);
    }
  }
}

// ---------------- Loop ----------------
void loop() {
  if (modoComRede) {
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
            // ----------- HTML Response -----------
            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-Type: text/html");
            cliente.println("Connection: close");
            cliente.println();
            cliente.println("<!DOCTYPE HTML>");
            cliente.println("<html lang=\"pt-br\">");
            cliente.println("<head><title>Controle de Motores</title></head>");
            cliente.println("<body>");
            cliente.println("<h1>Controle de Motores via Ethernet</h1>");

            // Slider Motor 1
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor1\">Motor 1<br>-180&deg; a 180&deg;:</label><br>");
            cliente.println("<input type=\"range\" id=\"motor1\" name=\"motor_1\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"Mover\">");
            cliente.println("</form><br><br>");

            // Slider Motor 2
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor2\">Motor 2<br>-180&deg; a 180&deg;:</label><br>");
            cliente.println("<input type=\"range\" id=\"motor2\" name=\"motor_2\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"Mover\">");
            cliente.println("</form>");
            cliente.println("</body></html>");

            // ----------- Command Parsing -----------
            if (primeiraLinha.indexOf("GET /?motor_1=") >= 0) {
              int pos = primeiraLinha.indexOf("motor_1=") + 8;
              String valorStr = primeiraLinha.substring(pos, primeiraLinha.indexOf(" ", pos));
              int valor = valorStr.toInt();
              Serial.print("Comando Motor 1: ");
              Serial.print(valor);
              Serial.println(" graus");

              if (valor != 0 && valor % 5 == 0) {
                rotateMotor(1, valor, valor > 0);
              } else {
                Serial.println("Valor inválido para Motor 1.");
              }

            } else if (primeiraLinha.indexOf("GET /?motor_2=") >= 0) {
              int pos = primeiraLinha.indexOf("motor_2=") + 8;
              String valorStr = primeiraLinha.substring(pos, primeiraLinha.indexOf(" ", pos));
              int valor = valorStr.toInt();
              Serial.print("Comando Motor 2: ");
              Serial.print(valor);
              Serial.println(" graus");

              if (valor != 0 && valor % 5 == 0) {
                rotateMotor(2, valor, valor > 0);
              } else {
                Serial.println("Valor inválido para Motor 2.");
              }

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
  } else {
    // -------------- Serial Mode --------------
    if (Serial.available()) {
      String comando = Serial.readStringUntil('\n');
      comando.trim();

      if (comando.startsWith("motor_")) {
        int sub1 = comando.indexOf('_', 6);
        if (sub1 != -1) {
          int motor = comando.substring(6, sub1).toInt();
          int valor = comando.substring(sub1 + 1).toInt();
          if ((motor == 1 || motor == 2) && valor % 5 == 0) {
            Serial.print("Movendo motor ");
            Serial.print(motor);
            Serial.print(" em ");
            Serial.print(valor);
            Serial.println(" graus");
            rotateMotor(motor, valor, valor > 0);
          } else {
            Serial.println("Comando inválido: valor ou motor incorreto.");
          }
        }
      } else {
        Serial.println("Formato inválido. Use: motor_1_90");
      }
    }
  }
}
