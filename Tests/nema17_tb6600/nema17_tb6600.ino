// Vídeo para se ter como base na hora da compreensão da execução do processo todo
// https://www.youtube.com/watch?v=nLV0fjUWI-g&ab_channel=HenryCawley

#define directionPin 2
#define stepPin 3

#define stepsPerRevolution 6400

void setup() {
    pinMode(directionPin,OUTPUT);
    pinMode(stepPin,OUTPUT);
}

void loop() {
    for (int i = 0; i < 5 * stepsPerRevolution; i++){ // Número 5 indica quantidade de rotações
        digitalWrite(directionPin, HIGH) // HIGH pra uma direção / LOW para a outra direção
        
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(50); // Diminuir o delay aumenta a frequência do sinal do degrau, que vai fazer o degrau do motor mais rápido

        digitalWrite(stepPin,LOW);
        delayMicroseconds(50); // Diminuir o delay aumenta a frequência do sinal do degrau, que vai fazer o degrau do motor mais rápido
    }

    // Adicionado para parar as rotações do motor (o loop fica preso dentro do while)
    while(1){
    }
}