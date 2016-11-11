#include "messagebus.h"
#include "menu.h"

#include "drivers/rtca.h"
#include "drivers/display.h"

static void binclock_event(enum sys_message msg) {
    if (msg & SYS_MSG_RTC_HOUR) {
        uint8_t tmp_hh = rtca_time.hour;
        if (tmp_hh > 12) { //PM
            tmp_hh -= 12;
            display_symbol(0, LCD_SYMB_PM, SEG_ON);
        } else {
            if (tmp_hh == 12) { // PM
                display_symbol(0, LCD_SYMB_PM, SEG_ON);
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

static void binclock_activated() {
    sys_messagebus_register(&binclock_event, SYS_MSG_RTC_MINUTE
                        | SYS_MSG_RTC_HOUR
    );

    binclock_event(SYS_MSG_RTC_YEAR | SYS_MSG_RTC_MONTH | SYS_MSG_RTC_DAY
                | SYS_MSG_RTC_HOUR  | SYS_MSG_RTC_MINUTE);
}

static void binclock_deactivated() {
    sys_messagebus_unregister_all(&binclock_event);

    display_symbol(0, LCD_SYMB_PM, SEG_OFF);
    display_clear(0, 1);
    display_clear(0, 2);
}

void mod_binclock_init() {
    menu_add_entry("BINCL", NULL, NULL,
            NULL,
            NULL,
            NULL, NULL,
            &binclock_activated,
            &binclock_deactivated);

}
