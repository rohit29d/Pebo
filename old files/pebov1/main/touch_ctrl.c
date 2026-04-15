#include "touch_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define TOUCH1 GPIO_NUM_0
#define TOUCH2 GPIO_NUM_1

#define TAP_WINDOW_MS 400
#define HOLD_MS 1200

typedef struct {
    int tap_count;
    TickType_t first_tap;
} touch_state_t;

static touch_state_t t1 = {0};
static touch_state_t t2 = {0};

static touch_event_t detect_touch(gpio_num_t pin, touch_state_t *st)
{
    if (gpio_get_level(pin)) {
        TickType_t start = xTaskGetTickCount();

        while (gpio_get_level(pin)) {
            if ((xTaskGetTickCount() - start) > pdMS_TO_TICKS(HOLD_MS)) {
                while (gpio_get_level(pin));
                st->tap_count = 0;
                return TOUCH_HOLD;
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        st->tap_count++;
        if (st->tap_count == 1)
            st->first_tap = xTaskGetTickCount();
    }

    if (st->tap_count > 0 &&
        (xTaskGetTickCount() - st->first_tap) > pdMS_TO_TICKS(TAP_WINDOW_MS)) {
        touch_event_t evt = st->tap_count;
        st->tap_count = 0;
        return evt;
    }

    return TOUCH_NONE;
}
void touch_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << TOUCH1) | (1ULL << TOUCH2),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io);
}

touch_event_t touch1_event(void)
{
    return detect_touch(TOUCH1, &t1);
}

touch_event_t touch2_event(void)
{
    return detect_touch(TOUCH2, &t2);
}
