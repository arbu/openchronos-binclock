/*
    modules/binclock.c: binary clock module for openchronos-ng

    Copyright (C) 2012 Angelo Arrifano <miknix@gmail.com>

                http://www.openchronos-ng.sourceforge.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "messagebus.h"
#include "menu.h"

/* drivers */
#include "drivers/rtca.h"
#include "drivers/display.h"

typedef enum display_segment disp_seg_t;
static const disp_seg_t line1[] = {
    LCD_SEG_L1_0,
    LCD_SEG_L1_1,
    LCD_SEG_L1_2,
    LCD_SEG_L1_3,
};
static const disp_seg_t line2[] = {
    LCD_SEG_L2_0,
    LCD_SEG_L2_1,
    LCD_SEG_L2_2,
    LCD_SEG_L2_3,
    LCD_SEG_L2_4,
    LCD_SEG_L2_5,
};
#define line_n(n) (n == 1 ? line1 : line2)

void display_binary(uint8_t line, uint8_t number) {
    int8_t i = line == 1 ? 3 : 5;
    for(; i >= 0; i++) {
        if(number & (1 << i))
            display_char(0, line_n(line)[i], '1', SEG_SET);
        else
            display_char(0, line_n(line)[i], ' ', SEG_OFF);
    }
}

static void binclock_event(enum sys_message msg)
{
    if (msg & SYS_MSG_RTC_HOUR) {
        uint8_t tmp_hh = rtca_time.hour;
        if (tmp_hh > 12) { //PM
            tmp_hh -= 12;
            display_symbol(0, LCD_SYMB_PM, SEG_SET);
        } else {
            if (tmp_hh == 12) { // PM
                display_symbol(0, LCD_SYMB_PM, SEG_SET);
            } else { // AM
                display_symbol(0, LCD_SYMB_PM, SEG_OFF);
            }
            if (tmp_hh == 0)
                tmp_hh = 12;
        }
        char bin[] = "    ";
        for(int i = 0; i < 4; i++)
            if(tmp_hh & (1 << i))
                bin[3 - i] = '1';
        display_chars(0, LCD_SEG_L1_3_0, bin, SEG_SET);
    }
    if (msg & SYS_MSG_RTC_MINUTE) {
        char bin[] = "      ";
        for(int i = 0; i < 6; i++)
            if(rtca_time.min & (1 << i))
                bin[5 - i] = '1';
        display_chars(0, LCD_SEG_L2_5_0, bin, SEG_SET);
    }
}

/* update screens with fake event */
static inline void update_screen()
{
    binclock_event(SYS_MSG_RTC_YEAR | SYS_MSG_RTC_MONTH | SYS_MSG_RTC_DAY
                | SYS_MSG_RTC_HOUR  | SYS_MSG_RTC_MINUTE);
}

///* Hour */
//static void edit_hh_sel(void)
//{
//    lcd_screen_activate(0);
//    display_chars(0, LCD_SEG_L1_3_2, NULL, BLINK_ON);
//}
//static void edit_hh_dsel(void)
//{
//    display_chars(0, LCD_SEG_L1_3_2, NULL, BLINK_OFF);
//}
//static void edit_hh_set(int8_t step)
//{
//    helpers_loop(&rtca_time.hour, 0, 23, step);
//
//    update_screen();
//}
//
//
///* Minute */
//static void edit_mm_sel(void)
//{
//    lcd_screen_activate(0);
//    display_chars(0, LCD_SEG_L1_1_0, NULL, BLINK_ON);
//}
//
//static void edit_mm_dsel(void)
//{
//    display_chars(0, LCD_SEG_L1_1_0, NULL, BLINK_OFF);
//}
//
//static void edit_mm_set(int8_t step)
//{
//    helpers_loop(&rtca_time.min, 0, 59, step);
//
//    update_screen();
//}
//
///* 12h/24h */
//static void edit_12_24_display(void)
//{
//    if (use_CLOCK_AMPM) {
//        display_chars(2, LCD_SEG_L1_3_0 , " 12H", SEG_SET);
//    } else {
//        display_chars(2, LCD_SEG_L1_3_0 , " 24H", SEG_SET);
//    }
//}
//static void edit_12_24_sel(void)
//{
//    lcd_screen_activate(2);
//    edit_12_24_display();
//    display_chars(2, LCD_SEG_L1_3_0, NULL, BLINK_ON);
//}
//static void edit_12_24_dsel(void)
//{
//    display_chars(2, LCD_SEG_L1_3_0, NULL, BLINK_OFF);
//}
//static void edit_12_24_set(int8_t step)
//{
//    use_CLOCK_AMPM = !use_CLOCK_AMPM;
//    edit_12_24_display();
//    update_screen();
//}
//
///* Save YMDHMS */
//static void edit_save()
//{
//    /* Here we return from the edit mode, fill in the new values! */
//    rtca_time.sec = 0;
//    rtca_set_time();
//
//    /* turn off only SOME blinking segments */
//    display_chars(0, LCD_SEG_L1_3_0, NULL, BLINK_OFF);
//    display_chars(0, LCD_SEG_L2_4_0, NULL, BLINK_OFF);
//    display_chars(1, LCD_SEG_L1_3_0, NULL, BLINK_OFF);
//    display_chars(2, LCD_SEG_L1_3_0, NULL, BLINK_OFF);
//
//    /* return to main screen */
//    lcd_screen_activate(0);
//
//    /* start the RTC */
//    rtca_start();
//
//    /* update screens with fake event */
//    clock_event(SYS_MSG_RTC_YEAR | SYS_MSG_RTC_MONTH | SYS_MSG_RTC_DAY
//                | SYS_MSG_RTC_HOUR  | SYS_MSG_RTC_MINUTE | SYS_MSG_RTC_SECOND);
//}
//
///* edit mode item table */
//static struct menu_editmode_item edit_items[] = {
//    {&edit_12_24_sel, &edit_12_24_dsel, &edit_12_24_set},
//    {&edit_hh_sel, &edit_hh_dsel, &edit_hh_set},
//    {&edit_mm_sel, &edit_mm_dsel, &edit_mm_set},
//    { NULL },
//};

/************************ menu callbacks **********************************/
static void binclock_activated()
{
    sys_messagebus_register(&binclock_event, SYS_MSG_RTC_MINUTE
                        | SYS_MSG_RTC_HOUR
    );

    /* create three screens, the first is always the active one */
    //lcd_screens_create(3); // 0:time + date, 1: year + day of week, 2:temp for settings (ex 12/24h setup)

    /* update screens with fake event */
    update_screen();
}

static void binclock_deactivated()
{
    sys_messagebus_unregister_all(&binclock_event);

    /* destroy virtual screens */
    //lcd_screens_destroy();

    display_symbol(0, LCD_SYMB_PM, SEG_OFF);
    display_clear(0, 1);
    display_clear(0, 2);
}

///* Num button press callback */
//static void num_pressed()
//{
//    uint8_t nr = get_active_lcd_screen_nr();
//    if (++nr >= 2) {
//        nr = 0; // Skip 12h/24h setup screen
//    }
//
//    lcd_screen_activate(nr);
//}

///* Star button long press callback. */
//static void star_long_pressed()
//{
//    /* stop the hardware RTC */
//    rtca_stop();
//
//#ifdef CONFIG_MOD_CLOCK_BLINKCOL
//    /* the blinking dots feature might hide the two dots, we display them
//      here just in case */
//    display_symbol(0, LCD_SEG_L1_COL, SEG_ON);
//#endif
//
//    menu_editmode_start(&edit_save, edit_items);
//}

void mod_binclock_init()
{
    menu_add_entry("BINCL", NULL, NULL,
            NULL, //&num_pressed,
            NULL, //&star_long_pressed,
            NULL, NULL,
            &binclock_activated,
            &binclock_deactivated);

}
