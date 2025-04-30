// Inclui a biblioteca Ethernet para conectar o Arduino à rede
//#include <Ethernet.h>
#include <EthernetENC.h>

// Define o endereço MAC (identificador único de rede)
// Você pode mudar esse MAC se tiver conflito na rede
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Define o endereço IP fixo do Arduino na rede local
//byte ip[] = { 192, 168, 1, 177 };
byte ip[] = { 192, 168, 0, 177 };

// Cria o objeto do servidor Ethernet na porta 80 (HTTP padrão)
EthernetServer servidor(80);

// String que vai armazenar a requisição que o navegador enviar
String readString;

void setup() {
  // Define o pino 8 como saída (para ligar ou desligar o LED)
  //pinMode(8, OUTPUT);
  Serial.begin(9600);

  // Inicializa a conexão Ethernet com o MAC e IP definidos
  Ethernet.begin(mac, ip);

  // Inicia o servidor web
  servidor.begin();
  
  delay(2000);
  Serial.println("Setup done");
}

void loop() {
  // Cria um objeto cliente para verificar se existe conexão de navegador
  EthernetClient cliente = servidor.available();

  // Se houver cliente conectado
  if (cliente) {
    // Enquanto o cliente estiver conectado
    while (cliente.connected()) {
      // Se o cliente enviar dados
      if (cliente.available()) {
        // Lê o caractere enviado
        char c = cliente.read();

        // Acumula o caractere lido na variável readString (limite de 100 caracteres)
        if (readString.length() < 100) {
          readString += c;
        }

        // Se o caractere lido for '\n' (fim da requisição HTTP)
        if (c == '\n') {
          // Começa a enviar a resposta HTTP para o navegador
          cliente.println("HTTP/1.1 200 OK"); // Resposta de sucesso
          cliente.println("Content-Type: text/html"); // Informa que o conteúdo é HTML
          cliente.println(); // Linha em branco para separar cabeçalhos do corpo HTML
          
          // Envia o conteúdo da página HTML
          cliente.println("<!DOCTYPE html>");
          cliente.println("<html lang="pt-br">");
          cliente.println("<head>");
          cliente.println("<meta charset="UTF-8">");
          cliente.println("<title>Controle de Motores</title>");
          cliente.println("<style>");
          cliente.println("body {font-family: Arial, sans-serif;text-align: center;background: #f4f4f4;padding-top: 50px;}");
          cliente.println("h1 {color: #333;}");
          cliente.println(".motor-control {margin: 20px auto;padding: 20px;border-radius: 10px;background: #fff;box-shadow: 0 2px 5px  rgba(0,0,0,0.1);width: 350px;}");
          cliente.println("button {padding: 10px 15px;margin: 5px;border: none;border-radius: 5px;background: #3498db;color: white;cursor: pointer;}");
          cliente.println("button:hover {background: #2980b9;}");
          cliente.println("input[type="range"] {width: 100%;}");
          cliente.println(".angle-display {font-weight: bold;margin: 10px 0;}");
          cliente.println("</style>");
          cliente.println("</head>");
          cliente.println("<body>");
  
          cliente.println("<div class="motor-control">");
          cliente.println("<h2>Motor 1</h2>");
          cliente.println("<button onclick="sendAngle(1, -15)">-15°</button>");
          cliente.println("<button onclick="sendAngle(1, 15)">+15°</button>");
          cliente.println("<div class="angle-display">Ângulo: <span id="angle1Value">0</span>°</div>");
          cliente.println("<input type="range" id="angle1" min="-90" max="90" step="15" value="0" oninput="updateSlider(1)">");
          cliente.println("<button onclick="sendSlider(1)">Mover para ângulo</button>");
          cliente.println("</div>");
          
          cliente.println("<div class="motor-control">");
          cliente.println("<h2>Motor 2</h2>");
          cliente.println("<button onclick="sendAngle(2, -15)">-15°</button>");
          cliente.println("<button onclick="sendAngle(2, 15)">+15°</button>");
          cliente.println("<div class="angle-display">Ângulo: <span id="angle2Value">0</span>°</div>");
          cliente.println("<input type="range" id="angle2" min="-90" max="90" step="15" value="0" oninput="updateSlider(2)">");
          cliente.println("<button onclick="sendSlider(2)">Mover para ângulo</button>");
          cliente.println("</div>");
  
          cliente.println("<script>");
          cliente.println("const ip = "192.168.0.177";");
  
          cliente.println("function sendAngle(motor, angle) {");
          cliente.println("fetch(`http://${ip}/motor/${motor}/move?angle=${angle}`)");
          cliente.println(".then(res => res.text())");
          cliente.println(".then(text => alert(text))");
          cliente.println(".catch(err => alert("Erro: " + err));}");

          cliente.println("function sendSlider(motor) {");
          cliente.println("const slider = document.getElementById(`angle${motor}`);");
          cliente.println("const angle = parseInt(slider.value, 10);");
          cliente.println("sendAngle(motor, angle);}");

          cliente.println("function updateSlider(motor) {");
          cliente.println("const slider = document.getElementById(`angle${motor}`);");
          cliente.println("const display = document.getElementById(`angle${motor}Value`);");
          cliente.println("display.textContent = slider.value;}");

          cliente.println("window.onload = () => {updateSlider(1);updateSlider(2);};");
          cliente.println("</script>");

          cliente.println("</body>");
          cliente.println("</html>");

          // Espera um pouco para garantir que todos os dados sejam enviados
          delay(1);

          // Fecha a conexão com o cliente
          cliente.stop();

          // Processa a string da requisição para ver se o botão de LIGAR foi pressionado
          if (readString.indexOf("?led_ligar") > 0) {
            //digitalWrite(8, HIGH); // Liga o LED (pino 8 em nível alto)
            Serial.println("LED ON");
          }
          else {
            // Se não, verifica se o botão de DESLIGAR foi pressionado
            if (readString.indexOf("?led_desligar") > 0) {
              //digitalWrite(8, LOW); // Desliga o LED (pino 8 em nível baixo)
              Serial.println("LED OFF");
            }
          }

          // Limpa a variável readString para a próxima requisição
          readString = "";
        }
      }
    }
  }
}