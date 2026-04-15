#include "IrisoledAnimation.h"

extern "C" {
#include "ssd1306.h" // Include your C driver
}

// --- Bridge Implementation ---
void PeboDisplayBridge::clearDisplay() {
    ssd1306_clear(); 
}
void PeboDisplayBridge::display() {
    ssd1306_update();
}
void PeboDisplayBridge::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    if(color == 1) ssd1306_draw_bitmap(x, y, bitmap, w, h);
}

// --- Helper: Millis for ESP-IDF ---
static uint32_t millis() {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

IrisoledAnimation::IrisoledAnimation(const unsigned char* frames[],
                                     uint8_t frameCount,
                                     const uint16_t* delays,
                                     uint16_t frameDelay,
                                     bool loop) :
  _framesRAM(frames),
  _delays(delays),
  _fallbackDelay(frameDelay),
  _frameCount(frameCount),
  _loop(loop),
  _current(0),
  _lastMillis(0),
  _running(false),
  _onFrameChange(nullptr) {}

void IrisoledAnimation::start(uint8_t startFrame) {
  if (_frameCount == 0) return;
  _current = startFrame % _frameCount;
  _lastMillis = millis();
  _running = true;
}

void IrisoledAnimation::stop() { _running = false; }
void IrisoledAnimation::resume() { _running = true; _lastMillis = millis(); }
void IrisoledAnimation::reset() { _current = 0; _running = false; }
void IrisoledAnimation::setLoop(bool loop) { _loop = loop; }

const unsigned char* IrisoledAnimation::getFramePtr(uint8_t index) {
    if (!_framesRAM) return nullptr;
    return _framesRAM[index];
}

void IrisoledAnimation::update(PeboDisplayBridge &display, int16_t x, int16_t y, uint8_t w, uint8_t h) {
  if (!_running || _frameCount == 0) return;

  uint32_t now = millis();
  const unsigned char* bmp = getFramePtr(_current);
  
  if (bmp) {
    display.clearDisplay();
    display.drawBitmap(x, y, bmp, w, h, 1);
    display.display();
  }

  uint16_t delayMs = (_delays != nullptr) ? _delays[_current] : _fallbackDelay;
  
  if (now - _lastMillis >= delayMs) {
    _lastMillis = now;
    _current++;
    if (_current >= _frameCount) {
      if (_loop) _current = 0;
      else {
        _current = _frameCount - 1;
        _running = false;
      }
    }
  }
}