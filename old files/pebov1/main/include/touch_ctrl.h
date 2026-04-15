#pragma once

typedef enum {
    TOUCH_NONE = 0,
    TOUCH_1TAP = 1,
    TOUCH_2TAP = 2,
    TOUCH_3TAP = 3,
    TOUCH_HOLD = -1
} touch_event_t;

void touch_init(void);

touch_event_t touch1_event(void);
touch_event_t touch2_event(void);

int touch2_pressed(void);
