/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

/* Definition of used ports */
#define RAIN_PORT 2 //PD2
#define DOOR_PORT 3 //PD3
#define VELOCITY_PORT_ANALOG 0 // IOPORT3

typedef struct sensor_events {
  bool is_door_opened;
  int bus_velocity;
  bool is_bus_stoped;
  bool is_raining;
} sensor_events_t;

/* Thread for reading sensors.
 * Read primary sensors:
 * 1. (HP) Velocity speed. -> check speed and speed limit and then activate or not buzzer.
 * 2. (HP) Door sensor. -> check door sensor and if it is opened it is not possible to speed the bus.
 * 3. Rain sensor. -> check raining sensor and reduces limit speed.
 * 4. Ultrasonic sensor -> check ???
 */
static THD_WORKING_AREA(waThread1, 32);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;

  char buffer[200];

  chRegSetThreadName("read-high-priority-sensors");
  while (true) {
    /* Read velocity speed */
      //This will determine wether the bus has stoped or not. 
      //Do analog reading here

    /* Read Door sensor */
    palReadPad(IOPORT4, RAIN_PORT) == PAL_HIGH ? strncpy(buffer,"HIGH!\r\n",sizeof(buffer)) : strncpy(buffer,"LOW!\r\n",sizeof(buffer));
    chnWrite(&SD1, (const uint8_t *)buffer, strlen(buffer));
    chThdSleepMilliseconds(1000);
  }
}

void init_ports() {
  palSetPadMode(IOPORT4, RAIN_PORT, PAL_MODE_INPUT);
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  init_ports();
  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /*
   * Starts the LED blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  chnWrite(&SD1, (const uint8_t *)"Guard Angel Started\r\n", 14);

  while (TRUE) {
    chThdSleepMilliseconds(1000);
  }
}
