#include <SPI.h>
#include <Ethernet.h>

// ---------------- Configuração de Modo ----------------
bool modoComRede = true; // true: Ethernet / false: Monitor Serial

// ---------------- Ethernet Configuration ----------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 2);
EthernetServer servidor(80);

// ---------------- Motor Configuration ----------------
#define directionPin1 2
#define stepPin1 3
#define directionPin2 4
#define stepPin2 5

#define stepsPerRevolution 6400
#define stepsPerDegree (stepsPerRevolution / 360.0) // com 6400 fica ~17.78 passos por grau
#define delayMicrosecondsDesired 5000

// ---------------- Ângulos dos Motores ----------------
float anguloMotor1 = 0;
float maxAngleMotor1 = 181.0;
float minAngleMotor1 = 0.1;

float anguloMotor2 = 0;
float maxAngleMotor2 = 361.0;
float minAngleMotor2 = 0.1;

// ---------------- Status de Motores ----------------
String statusMotor1 = "";
String statusMotor2 = "";


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

// ---------------- Motor Function Atualizada ----------------
void rotateMotor(int motor, int degrees) {
  if (degrees == 0 || degrees % 5 != 0) return;

  float* anguloAtual = (motor == 1) ? &anguloMotor1 : &anguloMotor2;
  float maxAngulo = (motor == 1) ? maxAngleMotor1 : maxAngleMotor2;
  float minAngulo = (motor == 1) ? minAngleMotor1 : minAngleMotor2;

  float novoAngulo = *anguloAtual + degrees;


  if (novoAngulo >= maxAngulo || novoAngulo <= minAngulo) {
    Serial.print("❌ Trajetória não pode ser executada no motor ");
    Serial.print(motor);
    Serial.println(" pois ultrapassa os limites permitidos.");

    if (motor == 1) statusMotor1 = "❌ Rotação ultrapassa limites estipulados";
    if (motor == 2) statusMotor2 = "❌ Rotação ultrapassa limites estipulados";
    return;
  }

  int steps = round(abs(degrees) * stepsPerDegree);
  float degreesPerStep = 1.0 / stepsPerDegree;
  int dirPin = (motor == 1) ? directionPin1 : directionPin2;
  int stepPin = (motor == 1) ? stepPin1 : stepPin2;

  digitalWrite(dirPin, degrees > 0 ? HIGH : LOW);

  for (int i = 0; i < steps; i++) {
    *anguloAtual += (degrees > 0 ? degreesPerStep : -degreesPerStep);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicrosecondsDesired);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicrosecondsDesired);
    //if (anguloAtual <= 0) break;
  }

  if (motor == 1) statusMotor1 = "✅ Motor 1 movido com sucesso!";
  if (motor == 2) statusMotor2 = "✅ Motor 2 movido com sucesso!";

  Serial.print("✅ Estado Atual - Motor 1: ");
  Serial.print(anguloMotor1);
  Serial.print(" graus | Motor 2: ");
  Serial.print(anguloMotor2);
  Serial.println(" graus");
}

void resetMotor(int motor) {
  float* anguloAtual = (motor == 1) ? &anguloMotor1 : &anguloMotor2;
  int dirPin = (motor == 1) ? directionPin1 : directionPin2;
  int stepPin = (motor == 1) ? stepPin1 : stepPin2;

  float degrees = -*anguloAtual;
  int steps = round(abs(degrees) * stepsPerDegree);
  float degreesPerStep = 1.0 / stepsPerDegree;

  if (degrees == 0) {
    Serial.print("🔄 Motor ");
    Serial.print(motor);
    Serial.println(" já está em 0 graus.");
    return;
  }
 
  digitalWrite(dirPin, degrees > 0 ? HIGH : LOW);

  for (int i = 0; i < steps; i++) {
    *anguloAtual += (degrees > 0 ? degreesPerStep : -degreesPerStep);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayMicrosecondsDesired);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayMicrosecondsDesired);
  }

  *anguloAtual = 0.0; // Garante exatidão
  Serial.print("🔁 Motor ");
  Serial.print(motor);
  Serial.println(" resetado para 0 graus.");
 
  if (motor == 1) statusMotor1 = "✅ Motor 1 resetado com sucesso!";
  if (motor == 2) statusMotor2 = "✅ Motor 2 resetado com sucesso!";
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
            cliente.println("<title>Posicionamento Automático de Microfone</title>");
            cliente.println("<link href=\"https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap\" rel=\"stylesheet\">");

            cliente.println("<style>");
            cliente.println("body { font-family: 'Roboto', sans-serif; text-align: center; background-color: #f2f2f2; padding: 20px; }");
            cliente.println("h1 { color: #333; }");
            cliente.println("form { background: #fff; padding: 20px; margin: 20px auto; width: 300px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }");
            cliente.println("label { font-weight: bold; display: block; margin-bottom: 10px; }");
            cliente.println("input[type='range'] { width: 100%; margin-bottom: 10px; }");
            cliente.println("input[type='submit'], .btn-ajuste { background-color: #4CAF50; border: none; color: white; padding: 10px 15px; font-size: 14px; margin: 5px; cursor: pointer; border-radius: 5px; }");
            cliente.println(".btn-ajuste { background-color: #2196F3; }");
            cliente.println(".status-ok { color: green; font-weight: bold; }");
            cliente.println(".status-erro { color: red; font-weight: bold; }");
            cliente.println(".status-movendo { color: orange; font-weight: bold; }");

            // Overlay e spinner
            cliente.println("#overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.5); z-index: 9999; display: none; align-items: center; justify-content: center; flex-direction: column; }");
            cliente.println(".spinner { border: 8px solid #f3f3f3; border-top: 8px solid #3498db; border-radius: 50%; width: 60px; height: 60px; animation: spin 1s linear infinite; margin-bottom: 15px; }");
            cliente.println("@keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }");
            cliente.println(".loading-text { color: white; font-size: 18px; font-weight: bold; }");
            cliente.println("</style>");

            cliente.println("<script>");
            cliente.println("function ajustarValor(id, delta) {");
            cliente.println("  var slider = document.getElementById(id);");
            cliente.println("  var output = slider.nextElementSibling;");
            cliente.println("  var novoValor = parseInt(slider.value) + delta;");
            cliente.println("  if (novoValor > 180) novoValor = 180;");
            cliente.println("  if (novoValor < -180) novoValor = -180;");
            cliente.println("  slider.value = novoValor;");
            cliente.println("  output.value = novoValor;");
            cliente.println("}");
            cliente.println("document.addEventListener('DOMContentLoaded', function () {");
            cliente.println("  const forms = document.querySelectorAll('form');");
            cliente.println("  forms.forEach(form => {");
            cliente.println("    form.addEventListener('submit', function (e) {");
            cliente.println("      const message = form.querySelector('input[type=\"submit\"]').value;");
            cliente.println("      document.getElementById('loadingMessage').innerText = message + ' em andamento...';");
            cliente.println("      document.getElementById('overlay').style.display = 'flex';");
            cliente.println("    });");
            cliente.println("  });");
            cliente.println("});");
            cliente.println("</script>");


            cliente.println("</head>");
            cliente.println("<body>");

            // Overlay invisível inicialmente
            cliente.println("<div id=\"overlay\"><div class=\"spinner\"></div><div class=\"loading-text\" id=\"loadingMessage\">Executando...</div></div>");

            cliente.println("<h1>Controle de Posicionamento de Microfone</h1>");
            cliente.println("<h1>LAEPI - UFSC</h1>");

            // Motor 1
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor1\">Motor 1: <b>" + String(anguloMotor1, 1) + "&deg;</b><br>0&deg; a 180&deg;</label>");
            if (statusMotor1.length() > 0) {
              String classe = "status-ok";
              if (statusMotor1.startsWith("❌")) classe = "status-erro";
              else if (statusMotor1.startsWith("🔄")) classe = "status-movendo";
              cliente.println("<p class='" + classe + "'> " + statusMotor1 + "</p>");
            }
            cliente.println("<div>");
            cliente.println("<button class='btn-ajuste' type='button' onclick=\"ajustarValor('motor1', -5)\">-5&deg;</button>");
            cliente.println("<button class='btn-ajuste' type='button' onclick=\"ajustarValor('motor1', 5)\">+5&deg;</button>");
            cliente.println("</div>");
            cliente.println("<input type=\"range\" id=\"motor1\" name=\"motor_1\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value + '&deg;'\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"➡️ Mover\">");
            cliente.println("</form>");

            // Motor 2
            cliente.println("<form action=\"/\" method=\"GET\">");
            cliente.println("<label for=\"motor2\">Motor 2: <b>" + String(anguloMotor2, 1) + "&deg;</b><br>0&deg; a 360&deg;</label>");
            if (statusMotor2.length() > 0) {
              String classe = "status-ok";
              if (statusMotor2.startsWith("❌")) classe = "status-erro";
              else if (statusMotor2.startsWith("🔄")) classe = "status-movendo";
              cliente.println("<p class='" + classe + "'> " + statusMotor2 + "</p>");
            }
            cliente.println("<div>");
            cliente.println("<button class='btn-ajuste' type='button' onclick=\"ajustarValor('motor2', -5)\">-5&deg;</button>");
            cliente.println("<button class='btn-ajuste' type='button' onclick=\"ajustarValor('motor2', 5)\">+5&deg;</button>");
            cliente.println("</div>");
            cliente.println("<input type=\"range\" id=\"motor2\" name=\"motor_2\" min=\"-180\" max=\"180\" value=\"0\" step=\"5\" oninput=\"this.nextElementSibling.value = this.value + '&deg;'\">");
            cliente.println("<output>0</output><br>");
            cliente.println("<input type=\"submit\" value=\"➡️ Mover\">");
            cliente.println("</form>");

            // Resetar motores
            cliente.println("<form action=\"/motor_1_reset\" method=\"GET\">");
            cliente.println("<input type=\"submit\" value=\"🔁 Resetar Motor 1\">");
            cliente.println("</form>");
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
