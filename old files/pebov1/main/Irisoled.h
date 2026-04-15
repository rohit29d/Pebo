#ifndef IRISOLED_H
#define IRISOLED_H

#include "IrisoledAnimation.h"

// --- FIX: Renamed Namespace to avoid conflict with Class ---
namespace IrisoledBitmaps {
    extern const unsigned char alert[] PROGMEM;
    extern const unsigned char angry[] PROGMEM;
    extern const unsigned char blink_down[] PROGMEM;
    extern const unsigned char blink_up[] PROGMEM;
    extern const unsigned char blink[] PROGMEM;
    extern const unsigned char bored[] PROGMEM;
    extern const unsigned char despair[] PROGMEM;
    extern const unsigned char disoriented[] PROGMEM;
    extern const unsigned char excited[] PROGMEM;
    extern const unsigned char focused[] PROGMEM;
    extern const unsigned char furious[] PROGMEM;
    extern const unsigned char happy[] PROGMEM;
    extern const unsigned char look_down[] PROGMEM;
    extern const unsigned char look_left[] PROGMEM;
    extern const unsigned char look_right[] PROGMEM;
    extern const unsigned char look_up[] PROGMEM;
    extern const unsigned char normal[] PROGMEM;
    extern const unsigned char sad[] PROGMEM;
    extern const unsigned char scared[] PROGMEM;
    extern const unsigned char sleepy[] PROGMEM;
    extern const unsigned char surprised[] PROGMEM;
    extern const unsigned char wink_left[] PROGMEM;
    extern const unsigned char wink_right[] PROGMEM;
    extern const unsigned char worried[] PROGMEM;
    extern const unsigned char battery_full[] PROGMEM;
    extern const unsigned char battery_low[] PROGMEM;
    extern const unsigned char battery[] PROGMEM;
    extern const unsigned char left_signal[] PROGMEM;
    extern const unsigned char logo[] PROGMEM;
    extern const unsigned char mode[] PROGMEM;
    extern const unsigned char right_signal[] PROGMEM;
    extern const unsigned char warning[] PROGMEM;
}

// --- Class Definition ---
class Irisoled {
public:
    Irisoled();
    ~Irisoled(); 
    void begin();
    void play(int emotion_id);
    void update(); 

private:
    PeboDisplayBridge _display;
    IrisoledAnimation* _anim;
    
    // Pointers for animation sequences
    const unsigned char* _seq_blink[4]; 
    const unsigned char* _seq_angry[1];
    const unsigned char* _seq_happy[1];
    const unsigned char* _seq_sad[1];
};

#endif