#include "irisoled_c_api.h"
#include "Irisoled.h"

static Irisoled eyes;

extern "C" {

    void irisoled_init(void)
    {
        eyes.begin();
    }

    void irisoled_play(int emotion_id)
    {
        eyes.play(emotion_id);
    }

    // --- ADD THIS FUNCTION ---
    void irisoled_update(void)
    {
        eyes.update();
    }

}