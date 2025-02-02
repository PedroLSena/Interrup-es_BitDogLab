# Projeto de Controle de LEDs com RP2040 🎮

![Badge Versão](https://img.shields.io/badge/Versão-1.0-blue)
![Licença](https://img.shields.io/badge/Licença-MIT-green)
Video de ensaio:https://youtube.com/shorts/jPSkSF9_XM0

Projeto desenvolvido para a placa BitDogLab utilizando o microcontrolador RP2040. Implementa um contador interativo com display em matriz LED 5x5 e controle por botões com tratamento de bouncing.

## 📋 Funcionalidades Principais

### 🕹️ Controle por Botões
- **Botão A**: Incrementa o valor (0-9)
- **Botão B**: Decrementa o valor (0-9)
- Debounce de 200ms implementado via software

### 💡 Sistema de LEDs
- Matriz LED 5x5 WS2812B (NeoPixel)
  - Display numérico com padrões personalizados
  - Controle via PIO para timing preciso
- LED RGB integrado
  - Piscar automático do LED vermelho (5Hz)

### ⚙️ Técnicas Utilizadas
- Interrupções GPIO para tratamento de botões
- Programação direta de hardware (PIO)
- Controle de LEDs endereçáveis
- Pull-up interno para botões

## 🛠️ Configuração de Hardware

| Componente          | GPIO  | Cor/Função       |
|---------------------|-------|------------------|
| Matriz LED 5x5      | 7     | WS2812B (GRB)    |
| LED RGB Vermelho    | 13    | Indicador estado |
| LED RGB Verde       | 11    | (Reservado)      |
| LED RGB Azul        | 12    | (Reservado)      |
| Botão A             | 5     | Incremento       |
| Botão B             | 6     | Decremento       |

## 📂 Estrutura do Código

### Principais Componentes
```c
// Configuração de Hardware
#define LED_COUNT 25       // Matriz 5x5
#define LED_PIN 7          // GPIO para NeoPixel
#define BUTTON_A 5         // Botão incremento
#define BUTTON_B 6         // Botão decremento

// Estrutura de dados para LEDs
typedef struct {
    uint8_t G, R, B;       // Ordem GRB para WS2812B
} pixel_t;

// Variáveis globais
volatile int counter = 0;            // Valor atual
volatile bool update_display = false;// Flag de atualização

🔌 Instalação e Uso
Pré-requisitos

SDK do Raspberry Pi Pico

Toolchain para ARM Cortex-M0

VS Code com extensão Cortex-Debug (opcional)

Compilação

bash
Copy
mkdir build && cd build
cmake ..
make
Gravação na Placa

Mantenha pressionado BOOTSEL

Conecte via USB

Copie interrupts_bitdoglab.uf2 para a unidade montada

🎥 Demonstração
Vídeo de Demonstração

📌 Notas Técnicas
Timing de Comunicação
Frequência: 800 KHz

Protocolo: WS2812B (GRB)

Resolução temporal via PIO

Debouncing
c
Copy
void button_irq(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Debounce de 200ms
    if (current_time - last_press_time < 200) return;
    last_press_time = current_time;
    
    // Lógica de contagem...
}
Padrões Numéricos
Matriz 5x5 representando números com:

c
Copy
const uint8_t numbers[10][25] = {
    // Padrão para número 0
    {1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,1,1,1,1},
    // ... outros números
};
📝 Licença
Este projeto está licenciado sob a Licença MIT - veja o arquivo LICENSE para detalhes.

Desenvolvido por Pedro Sena
Suporte BitDogLab