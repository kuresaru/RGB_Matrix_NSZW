#ifndef __RGB_H
#define __RGB_H

#include "common.h"

#define RGB_R1 PBout(4)
#define RGB_G1 PBout(5)
#define RGB_B1 PBout(6)
#define RGB_R2 PBout(7)
#define RGB_G2 PBout(8)
#define RGB_B2 PBout(9)

#define RGB_R3 PDout(6)
#define RGB_G3 PDout(7)
#define RGB_B3 PGout(9)
#define RGB_R4 PGout(11)
#define RGB_G4 PGout(14)
#define RGB_B4 PGout(13)

#define RGB_Line(line)            \
    {                             \
        GPIOF->BSRRH = 7 << 6;    \
        GPIOF->BSRRL = line << 6; \
    }

#define RGB_CLK PGout(15)
#define RGB_LE PBout(3)
#define RGB_OE PCout(13)

extern u32 RGB_Buffer[];
extern vu8 RGB_Displaying;

void RGB_Cls();
void RGB_Init();
void RGB_TIM3Init();
void RGB_DisplayMain();

#endif