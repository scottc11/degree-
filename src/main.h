#ifndef __MAIN_H
#define __MAIN_H

#include <mbed.h>

#define LOW                  0
#define HIGH                 1
#define PPQ                  24

#define MIDI_BAUD            31250
#define MIDI_TX              PA_2
#define MIDI_RX              PA_3

#define I2C3_SDA             PC_9
#define I2C3_SCL             PA_8

#define SPI2_MOSI            PB_15
#define SPI2_MISO            PB_14
#define SPI2_SCK             PB_13

#define DAC_A_CS             PB_12
#define DAC_B_CS             PC_7

#define DISPLAY_CLK          PB_2
#define DISPLAY_DATA         PB_10
#define DISPLAY_LATCH        PB_1

#define ENCODER_CHAN_A       PA_1
#define ENCODER_CHAN_B       PA_7
#define ENCODER_BTN          PA_0


#define ADC_A                PC_13
#define ADC_B                PC_13
#define ADC_C                PC_13
#define ADC_D                PB_0

#define GATE_OUT_A           PA_9
#define GATE_OUT_B           PA_10
#define GATE_OUT_C           PA_11
#define GATE_OUT_D           PA_12

#define CTRL_LED_D           PC_0
#define CTRL_LED_C           PC_1
#define CTRL_LED_B           PC_2
#define CTRL_LED_A           PC_3

#define LOOP_START_LED_PIN   PC_4
#define LOOP_STEP_LED_PIN    PC_5
#define EXT_CLOCK_INPUT      PC_6

#define CHAN_INT_A           PC_11
#define CHAN_INT_B           PC_10
#define CHAN_INT_C           PA_15
#define CHAN_INT_D           PC_8

#define DEGREES_INT          PC_12
#define TOUCH_INT_CTRL       PC_13
#define TOUCH_INT_A          PB_7
#define TOUCH_INT_B          PB_6
#define TOUCH_INT_C          PB_5
#define TOUCH_INT_D          PB_4

#define TCA9544A_ADDR          0x70 // 1110000
#define CAP1208_ADDR           0x14 // 0010100  via mux
#define CAP1208_CTRL_ADDR      0x14 // 0010100

#define MCP23017_DEGREES_ADDR  0x20 // 0100000
#define MCP23017_CHAN_A_ADDR  0x21 // 0100001
#define MCP23017_CHAN_B_ADDR  0x22 // 0100010
#define MCP23017_CHAN_C_ADDR  0x23 // 0100011
#define MCP23017_CHAN_D_ADDR  0x24 // 0100100


const float QUANTIZED_VOLTAGES[8][2] = {
    { 0, 136.5 },      // I
    { 136.5, 204.75 }, // maj2, n/a
    { 204.75, 273 },   // min3, maj3
    { 341.25, 409.5 }, // per4, aug4
    { 409.5, 477.75 }, // dim5, per5
    { 546, 614.25 },   // min6, maj6
    { 682.5, 750.75 }, // min7, maj7
    { 750.75, 819 }    // VIII
  };

const float OCTAVE_VALUES[] = {0.0, 819.0, 1638.0, 2457.0, 3276.0};


#endif