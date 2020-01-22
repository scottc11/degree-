#ifndef __MAIN_H
#define __MAIN_H

#include <mbed.h>

#define LOW                  0
#define HIGH                 1
#define PPQ                  24
#define MIDI_BAUD            31250

#define ERROR_LED            PC_8
#define DISPLAY_CLK          PB_2
#define DISPLAY_DATA         PB_1
#define DISPLAY_LATCH        PB_10
#define LOOP_START_LED_PIN   PC_2
#define LOOP_STEP_LED_PIN    PC_3
#define ENCODER_CHAN_A       PA_5
#define ENCODER_CHAN_B       PA_6
#define ENCODER_BTN          PA_7
#define SHIFT_REG_DATA       PB_13
#define SHIFT_REG_CLOCK      PB_14
#define SHIFT_REG_LATCH      PA_10
#define EXT_CLOCK_INPUT      PA_9
#define CHANNEL_GATE         PA_7
#define MIDI_TX              PA_11
#define MIDI_RX              PA_12


#define TLC59116_CHAN_A_ADDR   0x60 // 1100000
#define TLC59116_CHAN_B_ADDR   0x61 // 1100001
#define TLC59116_CHAN_C_ADDR   0x62 // 1100010
#define TLC59116_CHAN_D_ADDR   0x64 // 1100100

#define CAP1208_CNTRL_ADDR     0x14 // 0010100
#define TCA9544A_ADDR          0x70 // 1110000
#define MCP23017_DEGREES_ADDR  0x20 // 0100000


#endif