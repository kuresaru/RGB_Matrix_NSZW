#include "rgb.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include <string.h>

uc16 RGB_UpTime[] = {2, 6, 24, 80, 160, 340, 760, 3500};
// uc16 RGB_UpTime[] = {5, 10, 20, 40, 80, 160, 320, 640};
// uc16 RGB_UpTime[] = {2, 6, 10, 30, 70, 120, 500, 2500};
u32 RGB_Buffer[32 * 32] __attribute__((aligned(4)));
vu8 RGB_Displaying = 0;

void RGB_Cls()
{
    memset(RGB_Buffer, 0, sizeof(RGB_Buffer));
}

void RGB_Init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    RGB_OE = 1;
    RGB_LE = 0;
    RGB_CLK = 1;
    RGB_CLK = 0;
    RGB_Cls();
}

void RGB_TIM3Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //APB1定时器时钟=168/4*2=84MHz
    TIM_DeInit(TIM3);                                               //复位定时器
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //定时器时钟
    TIM_TimeBaseInitStructure.TIM_Prescaler = 42000 - 1;            //定时器预分频  84M/42000=2KHz 时钟周期0.5ms
    TIM_TimeBaseInitStructure.TIM_Period = 20;                      //2KHz/20=100Hz
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //每定时一次发生中断
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //定时器3初始化为1KHz
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                 //配置TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       //抢占优先1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //子优先0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //启动
    NVIC_Init(&NVIC_InitStructure);                                 //写入中断配置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                      //定时器3打开更新中断
    TIM_Cmd(TIM3, ENABLE);                                          //启动定时器3
}

void RGB_ShowLine(u16 upTime)
{
    u16 i;
    RGB_LE = 1;
    RGB_LE = 0;
    RGB_OE = 0;
    for (i = 0; i < upTime; i++) //点亮upTime个时间
        ;
    RGB_OE = 1;
    if (i < 255) //如果点亮的时间不够一个T
    {
        for (i = 256 - upTime; i > 0; i--) //关闭剩下的时间
            ;
    }
}

//TIM3的中断
void RGB_DisplayMain()
{
    u8 line, level, pos;
    u32 *color;
    RGB_Displaying = 1;
    for (line = 0; line < 8; line++)
    {
        RGB_Line(line); //设置当前扫描行
        for (level = 8; level > 0; level--)
        {

            for (pos = 0; pos < 32; pos++)
            {
                color = &RGB_Buffer[line * 32 + 31 - pos];
                RGB_R1 = ((*color) & (1 << (15 + level))) ? 1 : 0;
                RGB_G1 = ((*color) & (1 << (7 + level))) ? 1 : 0;
                RGB_B1 = ((*color) & (1 << (level - 1))) ? 1 : 0;
                color = &RGB_Buffer[(line + 8) * 32 + 31 - pos];
                RGB_R2 = ((*color) & (1 << (15 + level))) ? 1 : 0;
                RGB_G2 = ((*color) & (1 << (7 + level))) ? 1 : 0;
                RGB_B2 = ((*color) & (1 << (level - 1))) ? 1 : 0;
                color = &RGB_Buffer[(line + 16) * 32 + 31 - pos];
                RGB_R3 = ((*color) & (1 << (15 + level))) ? 1 : 0;
                RGB_G3 = ((*color) & (1 << (7 + level))) ? 1 : 0;
                RGB_B3 = ((*color) & (1 << (level - 1))) ? 1 : 0;
                color = &RGB_Buffer[(line + 24) * 32 + 31 - pos];
                RGB_R4 = ((*color) & (1 << (15 + level))) ? 1 : 0;
                RGB_G4 = ((*color) & (1 << (7 + level))) ? 1 : 0;
                RGB_B4 = ((*color) & (1 << (level - 1))) ? 1 : 0;
                RGB_CLK = 1;
                RGB_CLK = 0;
            }
            RGB_ShowLine(RGB_UpTime[level - 1]);
        }
    }
    RGB_Displaying = 0;
}