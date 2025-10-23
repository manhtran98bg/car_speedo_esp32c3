#ifndef _DATA_MODEL_H_
#define _DATA_MODEL_H_

#pragma once
#include <Arduino.h>

struct Speedo {
    uint8_t flag;
    int speed_kmph;
    uint16_t rpm;
    int engine_temp;
};

// ==== Global Instances ==== //
extern Speedo SpeedoData;

// ==== Config ==== //
const int MAX_KMPH = 160;

#endif