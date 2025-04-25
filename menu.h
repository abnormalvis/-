#ifndef __MENU_H_
#define __MENU_H_

#include "oled12864.h"
#include "key.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Systemstate.h"
#include "stdint.h"

/* 菜单状态 */
typedef enum
{
    MENU_MAIN,        /* 主菜单 */
    MENU_SETTINGS,    /* 设置 */
    MENU_PLAY,        /* 播放菜单 */
    MENU_SONG_SELECT, /* 歌曲选择 */
    MENU_STATE_COUNT  /* 用于边界检查的状态总数 */
} MenuState;

/* 菜单项配置结构 */
typedef struct
{
    const char *title;      /* 菜单标题 */
    const char *items[4];   /* 菜单项文本 */
    uint8_t item_count;     /* 有效项数 */
    MenuState parent_state; /* 父菜单状态 */
} MenuConfig;

/* 菜单状态机 */
typedef struct Menu_State
{
    MenuState current_page; /* 当前页面 */
    uint8_t max_items;      /* 页面最大项数 */
    uint8_t selected_item;  /* 当前选中项 */
    uint8_t language;       /* 语言设置 */
    uint8_t song_selection; /* 当前选中的歌曲 */
} Menu_State;

extern Menu_State menu; /* 全局菜单状态机 */


#define MENU_REFRESH_INTERVAL 200 /* 刷新间隔 */
#define MENU_ITEMS_PER_PAGE 4     /* 每页最大项数 */
#define MAX_SONG_NUM 3            /* 最大歌曲数量 */
#define MENU_TITLE_Y 0    // 标题在第0页
#define MENU_ITEM_START_Y 1 // 菜单项从第1页开始
#define CHAR_WIDTH 6      // 8号字宽度
#define LINE_HEIGHT 1     // 行间距（页单位）
extern const Menu_State *GetMenuState(void);
extern uint8_t GetCurrentSongID(void);

/* 核心功能接口（兼容原有函数） */
void Menu_Init(void);
void Menu_Display(void);
void Menu_Update(void);

/* 状态机接口 */
void Menu_TransitionTo(MenuState new_state);
void Menu_ReturnToParent(void);

#endif