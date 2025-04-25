#ifndef __MAIN_H_
#define __MAIN_H_

#include <REG52.H>
#include "stdint.h"
#include "Systemstate.h"
#include "bmp.h"
#include "Beep.h"
#include "Key.h"
#include "Delay.h"
#include "AT24C1024B.h"
#include "Timer1.h"
#include "Musictable.h"
#include "Beep.h"
#include "oled12864.h"
#include "menu.h"
#include "Game.h"
#include "Timer0.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "oledfont.h"


void Menu_HandleInput(unsigned char key);

#endif