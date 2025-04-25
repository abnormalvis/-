/* menu.c */
#include "menu.h"
#include "oled12864.h"
#include "key.h"
#include "Game.h"
#include "Beep.h"
#include <string.h>
#include <stdio.h>

/* 全局菜单状态 */
Menu_State menu =
    {
        MENU_MAIN, /* current_page */
        4,         /* max_items */
        0,         /* selected_item */
        1,         /* language */
        0          /* song_selection */
};

static unsigned short last_refresh = 0; /* 最后刷新时间戳 */

/* 菜单配置 */
static const MenuConfig menu_config[] = {
    /* MENU_MAIN */
    {
        "MAIN MENU",
        {"Play Game", "Settings", "Help", "Exit"},
        4,
        MENU_MAIN},
    /* MENU_SETTINGS */
    {
        "SETTINGS",
        {"Language", "Back"},
        2,
        MENU_MAIN},
    /* MENU_PLAY */
    {
        "PLAY MODE",
        {"Song Select", "Back"},
        2,
        MENU_MAIN},
    /* MENU_SONG_SELECT */
    {
        "SONG SELECT",
        {"Song 1", "Song 2", "Song 3", "Back"},
        4,
        MENU_PLAY}};

/* 私有函数声明 */
static void DrawMenuPage(void);
static void HandleInput(unsigned char key);
static void TransitionState(MenuState new_state);
static void HandleSelection(void);

// 初始化菜单
void Menu_Init(void)
{
    OLED_Init();                   // 先初始OLED
    OLED_Clear();                  // 清屏
    menu.current_page = MENU_MAIN; // 初始菜单页面为主菜单
    menu.selected_item = 0;        // 初始选中项为0
    menu.max_items = 4;
}

// 展示菜单
void Menu_Display(void)
{
    /* 200ms间隔或状态变化时刷新OLED */
    if ((SystemTick - last_refresh > MENU_REFRESH_INTERVAL) ||
        (menu.current_page != *(volatile unsigned char *)&menu.current_page))
    {                              // 检测当前时间戳和上一次刷新时间的间隔 或者当前页码是否发生变化
        OLED_Clear();              // 先清屏
        DrawMenuPage();            // 绘制当前页
        last_refresh = SystemTick; // 更新刷新时间
    }
}

void Menu_Update(void)
{
    static unsigned char last_key = 0;     // 上一次按键
    unsigned char current_key = Get_Key(); // 获取当前按键

    /* 按键去抖动处理 */
    if (current_key != last_key)
    {
        HandleInput(current_key); // 处理按键
        last_key = current_key;   // 更新上一次按键
    }
}

static void DrawMenuPage(void)
{
    unsigned char i;
    unsigned char y_pos;

    // 读取当前页面状态
    const unsigned char current_state = menu.current_page;
    uint8_t len;
    len = strlen(menu_config[current_state].title);
    // 获取当前页面的标题长度
    OLED_ShowString((128 - len * 8) / 2, 0, menu_config[current_state].title, 8);
    /* 显示菜单项 */
    for (i = 0; i < menu_config[current_state].item_count; i++)
    {
        y_pos = 2 + i * 2;

        /* 选中指示   原本想用反色显式 感觉又太麻烦了 不过反色显示可能会好看一点*/
        if (i == menu.selected_item)
        { // 用箭头提示选
            OLED_ShowString(4, y_pos, ">", 16);
        }
        if (current_state == MENU_SETTINGS && i == 0)
        {
            // sprintf(char *, const char *, ...)
            OLED_ShowString(12, y_pos, "Language", 8);
            OLED_ShowString(80, y_pos, (menu.language == 1) ? "[EN]" : "[CN]", 8);
        }
        else
        { // 显示菜单项
            OLED_ShowString(12, y_pos, menu_config[current_state].items[i], 8);
        }
    }

    OLED_Refresh();
}

/* 处理按键输入 */
static void HandleInput(unsigned char key)
{
    switch (key) {
    case KEY_PREV:  // 上移选项
        if (menu.selected_item > 0) {
            menu.selected_item--;
        }
        break;

    case KEY_NEXT:  // 下移选项
        if (menu.selected_item < menu_config[menu.current_page].item_count - 1) {
            menu.selected_item++;
        }
        break;

    case KEY_CONFIRM:  // 确认选择
        HandleSelection();
        break;
    }
}
/* 处理菜单选择 */
static void HandleSelection(void)
{
    const unsigned char state = menu.current_page;      /* 当前菜单状态 */
    const unsigned char selection = menu.selected_item; /* 当前选中项 */

    switch (state)
    {
    case MENU_MAIN: /* 主菜单 */
        if (selection == 0)
        { /* 打游戏 */
            TransitionState(MENU_PLAY);
        }
        else if (selection == 1)
        { /* 设置 */
            TransitionState(MENU_SETTINGS);
        }
        else if (selection == 3)
        { /* 退出 */
            OLED_Clear();
            OLED_Refresh();
        }
        break;

    case MENU_SETTINGS:
        if (selection == 0)
        {
            menu.language = (menu.language == 1) ? 2 : 1;
        }
        else
        {
            TransitionState(menu_config[state].parent_state);
        }
        break;

    case MENU_PLAY:
        if (selection == 0)
        {
            TransitionState(MENU_SONG_SELECT);
        }
        else
        {
            TransitionState(menu_config[state].parent_state);
        }
        break;

    case MENU_SONG_SELECT:
        if (selection < 3)
        { /* 前3项是歌曲 */
            menu.song_selection = selection + 1;
            Game_Start(menu.song_selection);
            PlayMusic();
            SystemState_Transition(SYSTEM_GAMEPLAY);
        }
        else
        {
            TransitionState(menu_config[state].parent_state);
        }
        break;
    }
}

static void TransitionState(MenuState new_state)
{
    menu.current_page = new_state;
    menu.selected_item = 0;
    menu.max_items = menu_config[new_state].item_count;
}

/* 新增接口实现 */
void Menu_TransitionTo(MenuState new_state)
{
    TransitionState(new_state);
}

// void Menu_ReturnToParent(void)
// {
//     if (menu.current_page != MENU_MAIN)
//     {
//         TransitionState(menu_config[menu.current_page].parent_state);
//     }
// }

// Menu_State *GetMenuState(void)
// {
//     return &menu;
// }

// uint8_t GetCurrentSongID(void)
// {
//     return menu.song_selection;
// }