#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

Servo servo1;
Servo servo2;

// ---------------- Configuração de Modo ----------------
bool modoComRede = true; // true: Ethernet / false: Monitor Serial

// ---------------- Ethernet Configuration ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 2);
EthernetServer servidor(80);

// ---------------- Motor Configuration ----------------
#define servoPin1 3  // PWM
#define servoPin2 5  // PWM

// ---------------- Ângulos dos Motores ----------------
float anguloMotor1 = 0;
float maxAngleMotor1 = 181.0;
float minAngleMotor1 = 0.0;

float anguloMotor2 = 0;
float maxAngleMotor2 = 181.0;
float minAngleMotor2 = 0.0;

// ---------------- Status de Motores ----------------
String statusMotor1 = "";
String statusMotor2 = "";

void setup() {
  Serial.begin(9600);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write((int)anguloMotor1);
  servo2.write((int)anguloMotor2);


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

void rotateMotor(int motor, int degrees) {
  if (degrees == 0 || degrees % 5 != 0) return;

  float* anguloAtual = (motor == 1) ? &anguloMotor1 : &anguloMotor2;
  float maxAngulo = (motor == 1) ? maxAngleMotor1 : maxAngleMotor2;
  float minAngulo = (motor == 1) ? minAngleMotor1 : minAngleMotor2;

  float novoAngulo = *anguloAtual + degrees;

  if (novoAngulo > maxAngulo || novoAngulo < minAngulo) {
    Serial.print("❌ Trajetória não pode ser executada no motor ");
    Serial.print(motor);
    Serial.println(" pois ultrapassa os limites permitidos.");

    if (motor == 1) statusMotor1 = "❌ Rotação ultrapassa limites estipulados";
    if (motor == 2) statusMotor2 = "❌ Rotação ultrapassa limites estipulados";
    return;
  }

  *anguloAtual = novoAngulo;

  if (motor == 1) {
    servo1.write((int)*anguloAtual);
    statusMotor1 = "✅ Motor 1 movido com sucesso!";
  } else {
    servo2.write((int)*anguloAtual);
    statusMotor2 = "✅ Motor 2 movido com sucesso!";
  }

  Serial.print("✅ Estado Atual - Motor 1: ");
  Serial.print(anguloMotor1);
  Serial.print(" graus | Motor 2: ");
  Serial.print(anguloMotor2);
  Serial.println(" graus");
}

void resetMotor(int motor) {
  if (motor == 1) {
    anguloMotor1 = 0.0;
    servo1.write(0);
    statusMotor1 = "✅ Motor 1 resetado com sucesso!";
    Serial.println("🔁 Motor 1 resetado para 0 graus.");
  } else if (motor == 2) {
    anguloMotor2 = 0.0;
    servo2.write(0);
    statusMotor2 = "✅ Motor 2 resetado com sucesso!";
    Serial.println("🔁 Motor 2 resetado para 0 graus.");
  }
}

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
          cliente.read(); // consome '\n'


          if (!requisicaoRecebida && linha.startsWith("GET")) {
            primeiraLinha = linha;
            requisicaoRecebida = true;
          }
          if (linha.length() == 0) {
            // Limpa status para nova ação
            statusMotor1 = "";
            statusMotor2 = "";

            // ----------- Command Parsing -----------
            if (primeiraLinha.indexOf("GET /?motor_1=") >= 0) {
              int pos = primeiraLinha.indexOf("motor_1=") + 8;
              String valorStr = primeiraLinha.substring(pos, primeiraLinha.indexOf(" ", pos));
              int valor = valorStr.toInt();
              Serial.print("Comando Motor 1: ");
              Serial.print(valor);
              Serial.println(" graus");

              if (valor != 0 && valor % 5 == 0) {
                rotateMotor(1, valor);
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
                rotateMotor(2, valor);
              } else {
                Serial.println("Valor inválido para Motor 2.");
              }

            } else if (primeiraLinha.indexOf("GET /motor_1_reset") >= 0) {
              Serial.println("🧭 Resetando Motor 1...");
              resetMotor(1);

            } else if (primeiraLinha.indexOf("GET /motor_2_reset") >= 0) {
              Serial.println("🧭 Resetando Motor 2...");
              resetMotor(2);

            } else {
              Serial.println("Comando não reconhecido.");
            }

            // ----------- HTML Response (Após execução de comandos!) -----------

            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-Type: text/html; charset=UTF-8");
            cliente.println("Connection: close");
            cliente.println();
            cliente.println("<!DOCTYPE HTML>");
            cliente.println("<html lang=\"pt-br\">");

            cliente.println("<head>");
            cliente.println("<meta charset=\"UTF-8\">");
            cliente.println("<title>Controle de Motores</title>");
            cliente.println("</head>");

            cliente.println("<style>");
            cliente.println(".status-ok { color: green; font-weight: bold; }");
            cliente.println(".status-erro { color: red; font-weight: bold; }");
            cliente.println(".status-movendo { color: orange; font-weight: bold; }");
            cliente.println("body { font-family: Arial, sans-serif; }");
            cliente.println("label { font-weight: bold; }");
            cliente.println("</style>");

            cliente.println("<body>");
            cliente.println("<h1>Controle de Motores via Ethernet</h1>");

            // Controle Motor 1
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor1\">Motor 1: <b>"+ String(anguloMotor1, 1) +"&deg;</b><br>-180&deg; a 180&deg;:</label><br>");
            if (statusMotor1.length() > 0) {
              String classe = "status-ok";
              if (statusMotor1.startsWith("❌")) classe = "status-erro";
              else if (statusMotor1.startsWith("🔄")) classe = "status-movendo";
              cliente.println("<p class='" + classe + "'> " + statusMotor1 + "</p>");
            }
            cliente.println("<input type=\"range\" id=\"motor1\" name=\"motor_1\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"➡️ Mover\">");
            cliente.println("</form><br><br>");

            // Controle Motor 2
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor2\">Motor 2: <b>" + String(anguloMotor2, 1) + "&deg;</b><br>-180&deg; a 180&deg;:</label><br>");
            if (statusMotor2.length() > 0) {
              String classe = "status-ok";
              if (statusMotor2.startsWith("❌")) classe = "status-erro";
              else if (statusMotor2.startsWith("🔄")) classe = "status-movendo";
              cliente.println("<p class='" + classe + "'> " + statusMotor2 + "</p>");
            }
            cliente.println("<input type=\"range\" id=\"motor2\" name=\"motor_2\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"➡️ Mover\">");
            cliente.println("</form><br><br>");

            // Botões de Reset
            cliente.println("<form action=\"/motor_1_reset\" method=\"GET\">");
            cliente.println("<input type=\"submit\" value=\"🔁 Resetar Motor 1\">");
            cliente.println("</form><br>");

            cliente.println("<form action=\"/motor_2_reset\" method=\"GET\">");
            cliente.println("<input type=\"submit\" value=\"🔁 Resetar Motor 2\">");
            cliente.println("</form>");

            cliente.println("</body></html>");

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
        if (comando.endsWith("_reset")) {
          // Comando de reset
          int motor = comando.charAt(6) - '0'; // motor_1_reset ou motor_2_reset
          if (motor == 1 || motor == 2) {
            Serial.print("🔧 Resetando motor ");
            Serial.println(motor);
            resetMotor(motor);
          } else {
            Serial.println("Comando de reset inválido. Use: motor_1_reset ou motor_2_reset");
          }

        } else {
          // Comando de movimento
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
              rotateMotor(motor, valor);
            } else {
              Serial.println("Comando inválido: valor ou motor incorreto.");
            }
          } else {
            Serial.println("Formato inválido. Use: motor_1_90");
          }
        }

      } else {
        Serial.println("Formato inválido. Use: motor_1_90 ou motor_1_reset");
      }
    }
  }
}
