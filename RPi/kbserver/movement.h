#pragma once

#include "pi_lib.h"

enum class commands {center_up, center_down, left_fw, left_bw, right_fw, right_bw};
extern int pins[6];
extern bool command_active[6];

void init_pins();
void destroy_gpio();
void command_press(char c);
void command_release(char c);
void do_commands();