#include "stm32_hal/hal/hal.h"
#include <stdint.h>

#define RCC_BASE 0x40023800
#define GPIOB_BASE 0x40020400
#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOB_MODER (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_PUPDR (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))

/* Input register for gpiob
 * Read only register which represents HIGH or LOW electrical state
 */
#define GPIOB_IDR (*(volatile uint32_t *)(GPIOB_BASE + 0x10))

/* Writing register for changing state from HIGH or LOW */
#define GPIOB_ODR (*(volatile uint32_t *)(GPIOB_BASE + 0x14))

#define NUM_OF_BUTTONS 16

int matrix_buttons[NUM_OF_BUTTONS] = {0};
volatile uint8_t running = 1;

int main() {

  /* Enable GPIOB clock */
  RCC_AHB1ENR |= (1 << 1);

  /* Set PB4-7 as inputs with pull-up (rows) */
  /* Clear mode bits */
  GPIOB_MODER &= ~(0xFF << 8);

  /* Clear pullup bits first */
  GPIOB_PUPDR &= ~(0xFF << 8);  
  
  /* Set pullup for PB4-7 */
  GPIOB_PUPDR |= (0x55 << 8);   
  
  /* Set PB0-3 as outputs (columns) */
  /* Clear bits 0-7 */
  GPIOB_MODER &= ~(0xFF);

  /* Set to output mode */
  GPIOB_MODER |= 0x55; 

  while(running) {

    /* Columns */
    for(int i = 0; i < 4; i++) {
    GPIOB_ODR = ~(1 << i);  

    for(volatile uint8_t i = 0; i < 100; i++);

    uint32_t rows = GPIOB_IDR;

      /* Rows
       * Map all 4x4 button matrix to array
       * Each button is represented as 0 or 1
       * State changes to 1 while button is held
       */
      for(int j = 0; j < 4; j++) {
        if(!(rows & (1 << (j + 4)))) {
          matrix_buttons[(j * 4) + i] = 1;
          hal_led_on_off(1);
        } else {
          matrix_buttons[(j * 4) + i] = 0;
          hal_led_on_off(0);
        }
      }

      /* Test array mapping
       * All buttons tested pass test
       * Mapping success
       * Check notes.txt for pinout wiring
       */
      if(matrix_buttons[7] == 1) {
        hal_led_on_off(1);
        running = 0;
      } else {
        hal_led_on_off(0);
      }
    } 
  }

  return 0;
}
