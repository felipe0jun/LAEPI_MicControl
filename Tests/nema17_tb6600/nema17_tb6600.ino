// Vídeo para se ter como base na hora da compreensão da execução do processo todo
// https://www.youtube.com/watch?v=nLV0fjUWI-g&ab_channel=HenryCawley


#define directionPin1 2
#define stepPin1 3
#define directionPin2 4
#define stepPin2 5


#define stepsPerRevolution 6400


void setup() {
    pinMode(directionPin1,OUTPUT);
    pinMode(stepPin1,OUTPUT);
    pinMode(directionPin2,OUTPUT);
    pinMode(stepPin2,OUTPUT);
}


void loop() {
    for (int i = 0; i < 5 * stepsPerRevolution; i++){ // Número 5 indica quantidade de rotações
        digitalWrite(directionPin1, HIGH); // HIGH pra uma direção / LOW para a outra direção
        digitalWrite(directionPin2, HIGH); // HIGH pra uma direção / LOW para a outra direção
       
        digitalWrite(stepPin1, HIGH);
        digitalWrite(stepPin2, HIGH);
        delayMicroseconds(50); // Diminuir o delay aumenta a frequência do sinal do degrau, que vai fazer o degrau do motor mais rápido


        digitalWrite(stepPin1,LOW);
        digitalWrite(stepPin2,LOW);
        delayMicroseconds(50); // Diminuir o delay aumenta a frequência do sinal do degrau, que vai fazer o degrau do motor mais rápido
    }


    // Adicionado para parar as rotações do motor (o loop fica preso dentro do while)
    while(1){
    }
}

