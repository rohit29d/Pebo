#include "pebo_eyes.h"
#include "irisoled_c_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Background Task: Updates the OLED 30 times per second
void eyes_anim_task(void *pvParameter)
{
    while (1) {
        irisoled_update(); 
        vTaskDelay(pdMS_TO_TICKS(33)); // ~30 FPS
    }
}

void pebo_eyes_init(void)
{
    irisoled_init();

    // FIX: ESP32-C3 is Single Core! We cannot pin to Core 1.
    // We use xTaskCreate which automatically picks the available core (Core 0).
    xTaskCreate(
        eyes_anim_task,   // Function to run
        "EyesAnim",       // Task name
        4096,             // Stack size
        NULL,             // Parameters
        1,                // Priority
        NULL              // Task Handle
    );
}

void pebo_eyes_play(pebo_eye_anim_t anim)
{
    irisoled_play((int)anim);
}