#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "matriz_led.pio.h"

#define LED_COUNT 25
#define LED_PIN 7      // GPIO7 para a matriz de LEDs
#define LED_R 13       // LED Vermelho
#define LED_G 11       // LED Verde
#define LED_B 12       // LED Azul
#define BUTTON_A 5
#define BUTTON_B 6

// Estrutura para os LEDs
struct pixel_t { uint8_t G, R, B; };
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

// Variáveis globais
npLED_t leds[LED_COUNT];
PIO np_pio = pio0;
uint sm = 0;
volatile int counter = 0;
volatile uint32_t last_press_time = 0;
volatile bool update_display = false;

// Padrões numéricos (0-9) para a matriz 5x5
const uint8_t numbers[10][25] = {
    // 0
    {1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,0,0,0,1,
     1,1,1,1,1},
    // 1
    {0,1,1,1,0,
     0,0,1,0,0,
     0,0,1,0,0,
     0,1,1,0,0,
     0,0,1,0,0},
    // 2
    {1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 3
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 4
    {1,0,0,0,0,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,0,0,0,1},
    // 5
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,0,
     1,1,1,1,1},
    // 6
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1},
    // 7
    {0,0,0,1,0,
     0,0,1,0,0,
     0,1,0,0,0,
     0,0,0,0,1,
     1,1,1,1,1},
    // 8
    {1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1},
    // 9
    {1,1,1,1,1,
     0,0,0,0,1,
     1,1,1,1,1,
     1,0,0,0,1,
     1,1,1,1,1}
};

// Declare function prototypes
void np_set_led(unsigned int index, uint8_t r, uint8_t g, uint8_t b); // Add this line
void np_clear(void);
void np_init(unsigned int pin);
void np_write();

// Interrupção para tratamento dos botões
void button_irq(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Debounce (200ms)
    if (current_time - last_press_time < 200) return;
    last_press_time = current_time;

    if(gpio == BUTTON_A) counter = (counter + 1) % 10;
    else if(gpio == BUTTON_B) counter = (counter - 1 + 10) % 10;
    
    update_display = true;
}

void init_buttons() {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_irq);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_irq);
}

void init_rgb() {
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
}

void np_clear(void) {
    for(unsigned int i = 0; i < LED_COUNT; i++) 
        np_set_led(i, 0, 0, 0);
}

// Funções para controle da matriz de LEDs
void np_init(unsigned int pin) {
    uint offset = pio_add_program(np_pio, &matriz_led_program);
    sm = pio_claim_unused_sm(np_pio, true);
    
    // Configuração específica do programa PIO
    matriz_led_program_init(np_pio, sm, offset, pin, 800000); // 800 KHz
    np_clear();
}

void np_set_led(unsigned int index, uint8_t r, uint8_t g, uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void np_write() {
    for(unsigned int i = 0; i < LED_COUNT; i++) {
        // Envia os bytes na ordem GRB
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

int main() {
    stdio_init_all();
    init_buttons();
    init_rgb();
    np_init(LED_PIN);

    absolute_time_t next_blink = get_absolute_time();
    bool led_state = false;

    while(true) {
        if(absolute_time_diff_us(next_blink, get_absolute_time()) >= 100000) {
            led_state = !led_state;
            gpio_put(LED_R, led_state);
            next_blink = make_timeout_time_ms(100);
        }

        if(update_display) {
            np_clear();
            for(int i = 0; i < LED_COUNT; i++) {
                if(numbers[counter][i]) 
                    np_set_led(i, 0, 50, 0); // Verde 20%
            }
            np_write();
            update_display = false;
        }
    }
}