#include "stm32f10x.h"
#include "lvgl_lcd_drv.h"
#include "lv_conf.h"
#include "lv_hal.h"
#include <string.h>
#include "lcd.h"




void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
           // put_px(x, y, *color_p)
					 LCD_DrawPoint_GUI(x, y,  ((color_p->full) | 0xFF0000));
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}



static lv_disp_buf_t disp_buf;

/**
 * Initialize your display here
 */
void tft_init(void)
{
	//LCD_Init();
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);

	

	disp_drv.buffer = &disp_buf; 
	disp_drv.flush_cb = my_flush_cb;

	lv_disp_drv_register(&disp_drv);
}

