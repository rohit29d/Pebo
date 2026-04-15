#ifndef IRISOLEDANIMATION_H
#define IRISOLEDANIMATION_H

#include <stdint.h>
#include <stddef.h>
#include "esp_timer.h"

// Compatibility macros
#ifndef PROGMEM
#define PROGMEM
#endif

// Bridge class to link C++ engine to C driver
class PeboDisplayBridge {
public:
    void clearDisplay();
    void display();
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
};

class IrisoledAnimation {
public:
  typedef void (*FrameCallback)(uint8_t newIndex);

  IrisoledAnimation(const unsigned char* frames[],
                    uint8_t frameCount,
                    const uint16_t* delays = nullptr,
                    uint16_t frameDelay = 200,
                    bool loop = true);

  void start(uint8_t startFrame = 0);
  void stop();
  void resume();
  void reset();
  void setLoop(bool loop);
  
  // This drives the animation
  void update(PeboDisplayBridge &display, int16_t x, int16_t y, uint8_t w, uint8_t h);

  bool isRunning() const { return _running; }

private:
  const unsigned char** _framesRAM;
  const uint16_t* _delays;
  uint16_t _fallbackDelay;
  uint8_t _frameCount;
  bool _loop;
  uint8_t _current;
  uint32_t _lastMillis; 
  bool _running;
  FrameCallback _onFrameChange;

  const unsigned char* getFramePtr(uint8_t index);
};

#endif