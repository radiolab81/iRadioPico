#include <Arduino.h>
#include "../globals.h"
#include "../player.h"

#include <Wire.h>
#include "PAJ7620U2.h"

#define PIN_SDA 16                                           
#define PIN_SCL 17  
// no PAJ7620U2 INTR pin defined, so we use polling

void gpiod_gesture_init() {
    Wire.setSDA(PIN_SDA);                        
    Wire.setSCL(PIN_SCL);
    Wire.begin();
    
    delayMicroseconds(10);
    
    while(!PAJ7620U2_init())
    { 
      SERIAL_PORT.print("GPIOD: Gesture Sensor Error\n");
      delay(500);
    }
    
    SERIAL_PORT.print("GPIOD:Gesture Sensor OK\n");
    I2C_writeByte(PAJ_BANK_SELECT, 0);//Select Bank 0
    for (uint8_t i = 0; i < Gesture_Array_SIZE; i++)
    {
      I2C_writeByte(Init_Gesture_Array[i][0], Init_Gesture_Array[i][1]);//Gesture register initializes
    }

}

void gpiod_gesture_run() {

	unsigned short Gesture_Data = I2C_readU16(PAJ_INT_FLAG1);
    if (Gesture_Data)
    {
      switch (Gesture_Data)
      {
        case PAJ_UP:                   SERIAL_PORT.print("GPIOD: Up\n");            break;
        case PAJ_DOWN:                 SERIAL_PORT.print("GPIOD: Down\n");          break;
        case PAJ_LEFT:                 SERIAL_PORT.print("GPIOD: Left\n");  prev_station();        break;
        case PAJ_RIGHT:                SERIAL_PORT.print("GPIOD: Right\n"); next_station();        break;
        case PAJ_FORWARD:              SERIAL_PORT.print("GPIOD: Forward\n");       break;
        case PAJ_BACKWARD:             SERIAL_PORT.print("GPIOD: Backward\n");      break;
        case PAJ_CLOCKWISE:            SERIAL_PORT.print("GPIOD: Clockwise\n");     break;
        case PAJ_COUNT_CLOCKWISE:      SERIAL_PORT.print("GPIOD: AntiClockwise\n"); break;
        case PAJ_WAVE:                 SERIAL_PORT.print("GPIOD: Wave\n");          break;
        default: break;
      }
    }
}
