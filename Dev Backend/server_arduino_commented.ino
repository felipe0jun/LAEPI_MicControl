// Importa a biblioteca SPI (usada para comunicação com o módulo Ethernet)
#include <SPI.h>

// Importa a biblioteca Ethernet para conexão à rede
#include <Ethernet.h>

// Define o endereço MAC do dispositivo (único na rede)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Define o endereço IP fixo do Arduino na rede local
IPAddress ip(192, 168, 1, 177);

// Cria o servidor Ethernet na porta 80 (padrão HTTP)
EthernetServer server(80);

// Define os pinos de direção (DIR) e passo (STEP) para dois motores de passo
const int dir1 = 2;   // Pino direção do Motor 1
const int step1 = 3;  // Pino passo do Motor 1
const int dir2 = 4;   // Pino direção do Motor 2
const int step2 = 5;  // Pino passo do Motor 2

// Função de configuração inicial (executada uma vez)
void setup() {
  // Configura todos os pinos como saída
  pinMode(dir1, OUTPUT);
  pinMode(step1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(step2, OUTPUT);

  // Inicializa a comunicação Ethernet com o MAC e IP definidos
  Ethernet.begin(mac, ip);

  // Inicia o servidor web
  server.begin();

  // Inicializa a comunicação serial para depuração
  Serial.begin(9600);
}

// Função principal que roda continuamente
void loop() {
  // Verifica se há um cliente conectado
  EthernetClient client = server.available();

  // Se houver um cliente
  if (client) {
    String req = "";                  // Variável para armazenar a requisição
    bool currentLineIsBlank = true;    // Controle para detectar fim do cabeçalho HTTP

    // Enquanto o cliente estiver conectado
    while (client.connected()) {
      // Se o cliente enviou dados
      if (client.available()) {
        char c = client.read();   // Lê o caractere enviado
        req += c;                 // Adiciona o caractere à requisição

        // Se encontrou uma linha em branco (fim do cabeçalho HTTP)
        if (c == '\n' && currentLineIsBlank) {
          // Trata a requisição recebida
          handleRequest(req, client);
          break;  // Sai do loop de leitura
        }

        // Atualiza se a linha atual é vazia ou não
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);     // Pequena pausa
    client.stop(); // Finaliza a conexão com o cliente
  }
}

// Função que analisa a requisição e executa o comando solicitado
void handleRequest(String req, EthernetClient& client) {
  int motor = 0;   // Variável para armazenar o motor escolhido
  int angle = 0;   // Variável para armazenar o ângulo a ser movimentado

  // Localiza o índice onde o comando "/motor/" aparece
  int mIndex = req.indexOf("/motor/");
  if (mIndex != -1) {
    // Pega o número do motor (logo após "/motor/")
    motor = req.charAt(mIndex + 7) - '0';
  }

  // Localiza o índice onde o comando "angle=" aparece
  int aIndex = req.indexOf("angle=");
  if (aIndex != -1) {
    // Pega o valor do ângulo (convertendo a substring para inteiro)
    angle = req.substring(aIndex + 6).toInt();
  }

  // Validação: só permite motores 1 ou 2, ângulos múltiplos de 15° e dentro de ±90°
  if ((motor == 1 || motor == 2) && angle % 15 == 0 && abs(angle) <= 90) {
    moveMotor(motor, angle);  // Move o motor conforme o comando
    sendResponse(client, "Motor " + String(motor) + " movido " + String(angle) + " graus");
  } else {
    sendResponse(client, "Comando inválido");  // Retorna erro se parâmetros inválidos
  }
}

// Função que move o motor de acordo com o ângulo solicitado
void moveMotor(int motor, int angle) {
  int stepsPer15 = 8; // Quantidade de passos para cada 15° (definido por calibração)
  int steps = abs(angle / 15) * stepsPer15; // Calcula total de passos necessários

  // Define qual motor será movimentado
  int dirPin = (motor == 1) ? dir1 : dir2;
  int stepPin = (motor == 1) ? step1 : step2;

  // Define a direção do motor (horário ou anti-horário)
  digitalWrite(dirPin, angle >= 0 ? HIGH : LOW);

  // Gera os pulsos de passo para o motor
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);   // Pulso de 500 microssegundos
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);   // Pulso de 500 microssegundos
  }
}

// Função que envia a resposta HTTP ao cliente
void sendResponse(EthernetClient& client, String msg) {
  client.println("HTTP/1.1 200 OK");         // Resposta de sucesso HTTP
  client.println("Content-Type: text/plain"); // Tipo de conteúdo: texto puro
  client.println("Connection: close");        // Fecha a conexão depois da resposta
  client.println();
  client.println(msg);                        // Mensagem de resposta enviada ao navegador
}
