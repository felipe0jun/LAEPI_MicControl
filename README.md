# 🎤 Suporte de microfone com posicionamento automatizado

Este projeto consiste em um **suporte inteligente para microfone** com **posicionamento automático** utilizando **3 motores de passo**.  

- **2 motores superiores (invertidos)**: responsáveis pela angulação do microfone (um motor tem a lógica invertida via cabeamento).  
- **1 motor inferior**: responsável pela rotação do eixo completo.  

O sistema pode ser controlado via **rede Ethernet (servidor web embarcado)** ou via **Monitor Serial**, possibilitando ajustes em tempo real do ângulo do microfone.

---

## 🚀 Funcionalidades

- Controle de até **3 motores de passo** com precisão em graus.  
- **Interface Web** para controle remoto pelo navegador.  
- **Modo Serial** alternativo para enviar comandos diretamente pelo PC.  
- Limites de segurança de rotação para evitar danos mecânicos.  
- Função de **reset automático** para retornar os motores à posição inicial (0°).  
- Interface HTML responsiva com **sliders**, **botões de ajuste rápido** (±5°) e mensagens de status.  

---

## 📦 Hardware necessário

- **Arduino UNO R4 Minima**.  
- **Ethernet Shield W5100** (para uso em rede).  
- **3 Motores de passo NEMA 17 (ou equivalentes)**.  
- **Drivers de motor (`TB6600`)**.  
- Fonte de alimentação 12V 3,5A.

---

## 🔌 Conexões (Arduino)

| Motor | Pino Direção | Pino Passo | Limite de Ângulo | Observação |
|-------|--------------|------------|------------------|------------|
| Motor 1.1 Superior | D2 | D3 | 0° → 180° | Controlado por software |
| Motor 1.2 Superior | D2 | D3 | 0° → 180° | Cabeamento invertido |
| Motor 2 Inferior (rotação) | D4 | D5 | 0° → 360° | Controlado por software |

> ⚠️ O código controla **Motor Superior (1)** e **Motor Inferior (eixo)**. O segundo motor superior (**Motor superior (2)**) funciona de forma inversa ao primeiro motor superior devido à configuração do cabeamento.

---

## Cabeamento

Abaixo, pode-se observar um esquema de ligações de fios:

![Cabeamento Pjto PI](https://github.com/user-attachments/assets/dfaa6d53-20e1-4008-a6bb-268331e82438)


---

## 💻 Configuração do software

Temos dois vídeos monstrando as configurações e demonstrações de funcionamento do sistema projetado:
- [Vídeo configuração + demontração interface web + demontração via código Python](https://drive.google.com/file/d/1Jp4nhEutnwPQjifhXevI_f_mm55FSiDO/view?usp=drive_link)
- [Vídeo demonstração código Python com pipeline completa de medição](https://drive.google.com/file/d/1JzpyqOZ0_BPRxIaVZZyTceJrk7otcbLN/view?usp=drive_link)

### Dependências
- [Arduino IDE](https://www.arduino.cc/en/software)  
- Biblioteca padrão `SPI.h`  
- Biblioteca padrão `Ethernet.h`  

### Configuração de rede

Para sistemas **Windows (10)** (no qual foram realizados os testes):
1. Conecte o cabo crossover no arduino e no computador.
2. No computador, entre em *Configurações*.
3. Selecione a opção *Rede e internet*.
<img width="1430" height="450" alt="image" src="https://github.com/user-attachments/assets/ef73972f-77f5-40fc-8bd4-47a815d50b20" />

6. Selecione a opção *Ethernet*.
<img width="800" height="910" alt="image" src="https://github.com/user-attachments/assets/cfcff21e-4300-496e-862c-38b110361959" />

6. Na área de "Configurações relacionadas", selecione *Alterar opções de adaptador*.
<img width="1146" height="672" alt="image" src="https://github.com/user-attachments/assets/a7f41bc5-842b-47ed-a7ca-7f1f39b80c55" />

7. Clique com o botão direito do mouse em cima de *Ethernet* (na rede desejada).
<img width="870" height="629" alt="image" src="https://github.com/user-attachments/assets/751211b8-adf5-4cee-84e8-c80ce5189e71" />

8. Selecione *Propriedades*.
9. Procure e selecione a opção *Protocolo IP Versão 4 (TCP/IPv4)*.
<img width="688" height="561" alt="image" src="https://github.com/user-attachments/assets/a6a551ba-498f-4a5c-a2ad-8aeda4a7762c" />

10. Clique em *Propriedades*.
11. Clique em *Usar o seguinte endereço IP:*.
12. Preencha o campo *Endereço IP:* com `192.168.1.1`.
13. Preencha *Máscara de sub-rede:* com `255.255.255.0`.
<img width="562" height="619" alt="image" src="https://github.com/user-attachments/assets/4b3fd4e8-3a48-4ae2-814b-4b3a5eaf89b7" />

14. Clique em *OK*.
15. Feche as configurações.
    

### Alternando modo de operação
No início do código, escolha se o controle será por **rede** ou por **serial**:
```cpp
bool modoComRede = true; // true: Ethernet / false: Monitor Serial
```

Por padrão já está configurado para conexão via rede/cabo crossover.

---

## 🌐 Modo Web (Ethernet)

1. Conecte o Arduino com o **Ethernet Shield** à sua rede.  
2. Acesse o IP configurado (`http://192.168.1.2`).
3. Utilize a interface para:  
   - Mover motores em incrementos de ±5°.  
   - Usar sliders para definir ângulos.  
   - Resetar motores para 0°.
   - Visualizar o status de cada operação.

---

## 🖥️ Modo serial

Quando `modoComRede = false`, utilize o **Monitor Serial** (9600 baud) para enviar comandos.  

### Comandos suportados:
- Mover motor:
  ```
  motor_1_90   // Move Motor Superior em +90 graus
  motor_2_-45  // Move Motor Inferior em -45 graus
  ```
- Resetar motor:
  ```
  motor_1_reset
  motor_2_reset
  ```

  **Para utilizar comunicação com o webserver do Arduino exclusivamente por URL (API REST) você pode montar a requisição da seguinte forma:**
  - `192.168.1.2/motor_1_90`: Move o motor 1 em +90 graus.
  - `192.168.1.2/motor_2_reset`: Reseta o motor 2 (deixando-o na posição inicial de 0 graus).

*Lembre-se que motor 1 faz referência aos motores superiores (angulação geral do posicionamento do microfone) e o motor 2 faz referência ao motor debaixo (rotação do eixo).*

⚠️ Os movimentos devem ser múltiplos de **5°**.  
⚠️ Caso o valor ultrapasse os limites configurados, o comando é ignorado.  

---

## ⚙️ Parâmetros importantes

- **Passos por revolução:** `6400`  
- **Passos por grau:** ~17,78  
- **Delay entre passos:** `5000 µs`  

Esses parâmetros podem ser ajustados no código conforme seu driver e motor.

---

## 🏗️ Estrutura do projeto

```
📂 LAEPI_MicControl  
  ┣ 📂 Tests
    ┣ 📂 ethernet_shield_led_simulation
      ┣ 📜 README.md
      ┣ 📜 ethernet_shield_led_simulation.ino
      ┣ 📜 ethernet_shield_motors_simulation.ino
      ┣ 📜 html_pi.txt
    ┣ 📂 nema17_tb6600
      ┣ 📜 README.md
      ┣ 📜 nema17_tb6600.ino
      ┣ 📜 nema17_tb6600_wiring.png
      ┣ 📜 nema17_tb6600_wiring_2.png
  ┣ 📜 webserver_and_motors_final_result.ino   # Código principal
  ┣ 📜 .gitignore
  ┣ 📜 README.md
  ┣ 📜 LICENSE
```

---

## 📈 Possíveis melhorias futuras

- Adicionar **memória de posições** (presets de ângulos).  
- Implementar **feedback por sensores** (fim de curso / encoders).  
- Suporte a **Wi-Fi** (ESP32/ESP8266).  
- Criação de aplicativo mobile para controle remoto.  

---

## 👨‍🔬 Projeto desenvolvido no **LAEPI - UFSC**  

Este projeto faz parte das atividades de pesquisa e desenvolvimento em **automação e robótica aplicada à acústica**.

---

## Desenvolvedores

- [Igor Caetano de Souza](https://github.com/IgorCaetano)
- [Felipe Jun Hatsumura](https://github.com/felipe0jun)
