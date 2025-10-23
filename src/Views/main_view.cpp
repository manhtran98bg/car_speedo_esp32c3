#include "Arduino.h"
#include "lvgl.h"
#include "ui.h"

#include "main_view.h"
#include "user_config.h"
#include "Models/data_model.h"
#include "Drivers/screen_driver.h"

#include "gif_view.h"
// ==== Display driver ==== //
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[TFT_HOR_RES * 40];

static lv_disp_drv_t disp_drv;

// ==== UI Components ==== //

// ==== Private variables ==== //
static bool ready = false;
// ==== Global variables ==== //
volatile UiMode currentMode = UI_MODE_ODO;
SemaphoreHandle_t displayMutex = xSemaphoreCreateMutex();

// ==== External variables ==== //

// ==== External function ==== //

// ==== Static function ==== //

static void disp_flush_callback(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    Screen.drawRegion((uint16_t *)&color_p->full, area->x1, area->y1, area->x2, area->y2);
    lv_disp_flush_ready(disp);
}

static void meter_anim_cb(void *needle, int32_t v)
{
    int needle_angle = map(v, 0, 10000, 0, 2500);
    int arc_value = map(v, 0, 10000, 0, 93);
    lv_img_set_angle(ui_img_needle, needle_angle);
    lv_arc_set_value(ui_Arc_rpm, arc_value);
}
static void update_ui()
{
    char speed_str[12];
    static int last_rpm = 0;
    int new_rpm = SpeedoData.rpm;
    if (SpeedoData.speed_kmph == -1)
        strcpy(speed_str, "---");
    else
        sprintf(speed_str, "%d", SpeedoData.speed_kmph);
    lv_label_set_text(ui_lbl_rpm_value, speed_str);
    // --- Animate meter needle ---
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)meter_anim_cb);
    lv_anim_set_values(&a, last_rpm, new_rpm);
    lv_anim_set_time(&a, 150);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
    last_rpm = new_rpm;
}
static void updateData_task(void *param)
{
    while (true)
    {
        if (ready && currentMode == UI_MODE_ODO)
        {
            if (xSemaphoreTake(displayMutex, 0) == pdTRUE)
            {
                if (SpeedoData.speed_kmph <= 100)
                    SpeedoData.speed_kmph += 1;
                else
                    SpeedoData.speed_kmph = 0;
                SpeedoData.rpm = SpeedoData.speed_kmph * 100;
                update_ui();
                xSemaphoreGive(displayMutex);
            }
        }
        // Serial.println("update ui");
        vTaskDelay(200);
    }
}

static void mainUi_task(void *param)
{
    while (true)
    {
        if (currentMode == UI_MODE_ODO)
        {
            if (xSemaphoreTake(displayMutex, 0) == pdTRUE)
            {
                lv_timer_handler();
                xSemaphoreGive(displayMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void main_view_init()
{
    lv_init();
    // lv_color_t* buf1 = (lv_color_t*)heap_caps_malloc(240 * 40 * sizeof(lv_color_t),
    //                                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, TFT_HOR_RES * 40);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_HOR_RES;
    disp_drv.ver_res = TFT_VER_RES;
    disp_drv.flush_cb = disp_flush_callback;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = true;
    lv_disp_drv_register(&disp_drv);

    ui_init();
    gif_view_init();
    needle_Animation(uic_img_needle, 200);
    lv_timer_create([](lv_timer_t *t)
                    {
                        // Screen.fadeIn();
                        lv_timer_del(t); }, 0, NULL);
    lv_timer_create([](lv_timer_t *t)
                    { ready = true; }, 3000, NULL);
    xTaskCreate(mainUi_task, "mainUi_task", 4096, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(updateData_task, "updateData_task", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
}
