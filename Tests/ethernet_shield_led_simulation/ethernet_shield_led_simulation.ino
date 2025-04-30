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
          cliente.println("<HTML>");
          cliente.println("<BODY>");
          cliente.println("<H1>Acende LED</H1>");
          cliente.println("<hr />");
          cliente.println("<br />");
          cliente.println("<p>LED <a href=\"/?led_ligar\"><button>LIGA</button></a><a href=\"/?led_desligar\"><button>DESLIGA</button></a></p>");
          cliente.println("<p><a href=\"http://afmulti.com.br/index.php/cursos-e-treinamentos/\">Visite nosso site!</a></p>");
          cliente.println("<p><a href=\"http://facebook.com/afmultiautomacao\">Fanpage do Facebook</a></p>");
          cliente.println("</BODY>");
          cliente.println("</HTML>");

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