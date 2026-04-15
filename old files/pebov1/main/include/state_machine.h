#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MODE_1 = 0,
    MODE_CHATGPT,
    MODE_SETTINGS
} pebo_mode_t;

void pebo_set_mode(pebo_mode_t mode);
pebo_mode_t pebo_get_mode(void);

#ifdef __cplusplus
}
#endif
