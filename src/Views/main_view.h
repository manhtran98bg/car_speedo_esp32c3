#ifndef _MAIN_VIEW_H_
#define _MAIN_VIEW_H_

#pragma once

typedef void (*disp_flush)(uint16_t *data, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void main_view_init();
#endif