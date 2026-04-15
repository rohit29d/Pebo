#ifndef PEBO_EYES_H
#define PEBO_EYES_H

typedef enum {
    PEBO_EYES_NEUTRAL = 0, // Default/Open
    PEBO_EYES_BLINK   = 0, // Same as neutral (or specific closed frame)
    PEBO_EYES_ANGRY   = 1,
    PEBO_EYES_CUTE    = 2, // Mapped to 'Happy' in C++
    PEBO_EYES_SAD     = 3
} pebo_eye_anim_t;

void pebo_eyes_init(void);
void pebo_eyes_play(pebo_eye_anim_t anim);

#endif