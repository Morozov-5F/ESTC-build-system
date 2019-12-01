#include "main.h"

#define SWITCH_DELAY 500000

static void setupClock(void)
{
  // Try to enable the HSE oscillator
  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
      RCC_HSEConfig(RCC_HSE_ON);
      if (SUCCESS != RCC_WaitForHSEStartUp())
      {
        // IF HSE fails - do nothing
        goto hse_fail;
      }
  }

  // Switch to HSE instead of PLL and wait until it switches
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
  while (RCC_GetSYSCLKSource() != 0x04);

  // Disable PLL
  RCC_PLLCmd(DISABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET);
  
  // Configure PLL
  RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 6);
  RCC_PLLCmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET);
  
  // Switch to PLL instead of HSE
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  while (RCC_GetSYSCLKSource() != 0x08);

hse_fail:
  return;
}

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  setupClock();
  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Turn all the leds off */
  GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);

  while (1)
  {
    int i;

    /* Switch the LED on */
    GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
    for (i = 0; i < SWITCH_DELAY; i++);

    /* Switch the LED off */
    GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
    for (i = 0; i < SWITCH_DELAY; i++);
  }
}
