#define STM32F411xE
#include "stdio.h"
#include "stm32l1xx.h"

#define PLL_M 4
#define PLL_N 180
#define PLL_P 0 // PLLP = 2

int transmit_char(int c) {
  while (!(USART2->SR & USART_SR_TXE)) {
  };
  USART2->DR = c;
  return c;
}

int transmit_string(const char *s) {
  while (*s) {
    transmit_char(*s++);
  }
  return 1;
}

int get_char() {
  while (!(USART2->SR & USART_SR_RXNE)) {
  };
  return USART2->DR;
}

void RCC_configure_TIM10() { RCC->APB2ENR |= RCC_APB2ENR_TIM10EN; }

void configure_RCC() {

  RCC->CR |= (0b1 << 16); // enabling HSE
  while (!(RCC->CR & (0b1 << 17))) {
  } // chacking if hse is on

  FLASH->ACR = (1 << 8) | (1 << 9) | (1 << 10) | (5 << 0); // enabling cache

  RCC->CFGR &= ~(0b1111 << 4); // AHB devided k=1

  RCC->CFGR &= ~(0b111 << 8); // APB1 devided k=1

  RCC->CFGR &= (0b111 << 11); // APB2 devided k=1

  RCC->CFGR |= (0b0010 << 18) | (0b10 << 22); // pll x6 /3

  RCC->CFGR |= (0b1 << 16); // using hse for pll source
  while (!(RCC->CFGR & (0b1 << 16))) {
  } // is pll switched to HSE?

  RCC->CR |= (1 << RCC_CR_PLLON_Pos); // enabling PLL
  while (!(RCC->CR & (1 << RCC_CR_PLLRDY_Pos))) {
  }; // wait till pll is on

  RCC->CFGR |= (0b11 << 0); // setting PLL as system clock source
  while (!(RCC->CFGR & (0b11 << 1))) {
  } // is system clock switched to PLL?

  RCC->CR |= (0b1 << 28); // enabling CSS

  RCC->CR &= ~(0b1 << 0);

  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  RCC_configure_TIM10();

  SystemCoreClockUpdate();
}

void configure_GPIO() {
  GPIOA->MODER |= GPIO_MODER_MODER2_1;
  GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos);
  GPIOA->MODER |= GPIO_MODER_MODER3_1;
  GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL3_Pos);

  GPIOA->MODER |= 0b01 << 10;
  GPIOA->OTYPER &= ~(1 << 5);
}

void configure_UART() {
  // USART2->BRR = (0x8B); // 57600
  USART2->BRR = (0x45); // 115200
  // USART2->BRR = (0x23); // 460800

  USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

void configure_timers() {
  SystemCoreClockUpdate();
  uint32_t required_timer_clock = 1000000;

  uint32_t PSC_value = (required_timer_clock / SystemCoreClock) - 1;
  // TIM10->PSC = PSC_value; // formula sysClock / (PSC + 1) as a result 1 MHz
  TIM10->PSC = 7; // formula sysClock / (PSC + 1) as a result 1 MHz

  TIM10->ARR = 999; // from 0 to 999 == 1000

  TIM10->CR1 = TIM_CR1_CEN; // enable counter
}

void delay(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i++) {
    TIM10->CNT = 0;

    while (!(TIM10->SR & TIM_SR_UIF)) {
    }

    TIM10->SR &= ~TIM_SR_UIF;
  }
}

int main(void) {
  // ClockInit();
  configure_RCC();
  configure_GPIO();
  configure_UART();
  configure_timers();

  char tempConverterToString[32] = {0};

  uint32_t _clock = HAL_RCC_GetSysClockFreq();

  sprintf(tempConverterToString, "%ld %ld \n\r", SystemCoreClock, _clock);

  transmit_string("there is no\n\r");
  delay(10000);
  transmit_string("emotion only\n\r");
  delay(10000);
  transmit_string("fucking peace\n\r");
  delay(10000);
  transmit_string(tempConverterToString);

  uint32_t current_clock = SystemCoreClock;

  while (1) {
    GPIOA->ODR |= (0b1 << 5);
    int c = get_char();
    for (uint32_t i = 0; i < c; i++) {
      GPIOA->ODR &= ~(0b1 << 5);
    }
    transmit_char(c);
  }
}
