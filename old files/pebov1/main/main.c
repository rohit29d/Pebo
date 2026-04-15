#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <stdlib.h> // Required for rand()

#include "i2c_ctrl.h"
#include "ssd1306.h"
#include "pebo_eyes.h"
#include "touch_ctrl.h"
#include "state_machine.h"

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    // 1. Hardware Initialization
    i2c_master_init();
    ssd1306_init();
    touch_init();
    
    // 2. Eyes Initialization 
    // (This now launches the animation background task)
    pebo_eyes_init();

    // 3. Set Initial State
    pebo_set_mode(MODE_1);
    pebo_eyes_play(PEBO_EYES_NEUTRAL);

    while (1) {

        /* -------- TOUCH 1 : MODE CONTROL -------- */
        touch_event_t t1 = touch1_event();

        if (t1 == TOUCH_1TAP) {
            pebo_set_mode(MODE_1);
            pebo_eyes_play(PEBO_EYES_NEUTRAL);
        }
        else if (t1 == TOUCH_2TAP) {
            pebo_set_mode(MODE_CHATGPT);
            pebo_eyes_play(PEBO_EYES_BLINK); // Standard blink or "Listening" face
        }
        else if (t1 == TOUCH_3TAP) {
            pebo_set_mode(MODE_SETTINGS);
            pebo_eyes_play(PEBO_EYES_BLINK); // Standard blink or "Config" face
        }
        else if (t1 == TOUCH_HOLD) {
            esp_restart();
        }

        /* -------- TOUCH 2 : EMOTIONS (Only in MODE 1) -------- */
        if (pebo_get_mode() == MODE_1) {

            touch_event_t t2 = touch2_event();

            if (t2 == TOUCH_1TAP) {
                // Explicit Blink Logic
                pebo_eyes_play(PEBO_EYES_BLINK);
                
                // Wait 150ms while the background task renders the closed eye
                vTaskDelay(pdMS_TO_TICKS(150)); 
                
                // Return to open eyes
                pebo_eyes_play(PEBO_EYES_NEUTRAL);
            }
            else if (t2 == TOUCH_2TAP) {
                pebo_eyes_play(PEBO_EYES_ANGRY);
            }
            else if (t2 == TOUCH_HOLD) {
                pebo_eyes_play(PEBO_EYES_CUTE);
            }
            else if (t2 == TOUCH_3TAP) {
                // Randomly pick Angry or Cute
                pebo_eyes_play((rand() % 2) ? PEBO_EYES_ANGRY : PEBO_EYES_CUTE);
            }
        }

        // Yield to other tasks (essential for FreeRTOS watchdog)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}