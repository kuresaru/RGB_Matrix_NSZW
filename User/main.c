#include "common.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "rgb.h"
#include "picture.h"
#include "ff.h"
#include "diskio.h"
#include "debug.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#define LED1 PFout(10)
#define LED0 PFout(9)


FIL file;

void Init_Clock()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void Init_IO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
}

void InitTIM4()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBasiInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_DeInit(TIM4);                                           
  TIM_TimeBasiInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //84MHz
  TIM_TimeBasiInitStructure.TIM_Prescaler = 42000 - 1;        //84M/42000=2KHz
  TIM_TimeBasiInitStructure.TIM_Period = 80;                  //2KHz/40=25fps
  TIM_TimeBasiInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBasiInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBasiInitStructure);
  TIM_ClearFlag(TIM4, TIM_IT_Update);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM4, ENABLE);
}

void err()
{
  while (1)
  {
    LED1 = 1;
    Delay_ms(500);
    LED1 = 0;
    Delay_ms(500);
  }
}

int main()
{
  u16 i;
  Init_Clock();
  Init_IO();
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
  LED0 = 1;
  LED1 = 3;
  RGB_Init();

  DEBUG_Init();
  while (disk_initialize(DEV_SD) != RES_OK)
  {
    Delay_ms(500);
    printf("SD init error.\r\n");
  }
  FATFS fs;
  if (f_mount(&fs, "", 1))
  {
    printf("File system mount failed.\r\n");
    err();
  }
  if (f_open(&file, "01.bin", FA_READ))
  {
    printf("File open failed.\r\n");
    err();
  }
  printf("File size : %ld.\r\n", file.obj.objsize);

  RGB_TIM3Init();
  InitTIM4();

  while (1)
  {
    // RGB_DisplayMain();
  }
}
