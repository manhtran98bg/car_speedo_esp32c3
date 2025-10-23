SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_1834619086.c
    images/ui_img_951008474.c
    images/ui_img_1979378346.c
    images/ui_img_images_car_icon_240_png.c
    images/ui_img_images_temperature_png.c
    fonts/ui_font_t20.c
    fonts/ui_font_t25.c
    fonts/ui_font_t13.c
    fonts/ui_font_t40.c)

add_library(ui ${SOURCES})
