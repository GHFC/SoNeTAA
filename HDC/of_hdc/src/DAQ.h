#pragma once

#include "ofMain.h"

int initDAQ();
float readDAQ(int input);
void writeDAQ(float x, float y);
void trigger(int n);
void calibration();
