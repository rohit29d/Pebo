#ifndef IRISOLED_C_API_H
#define IRISOLED_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

void irisoled_init(void);
void irisoled_play(int emotion_id);

// --- ADD THIS LINE ---
void irisoled_update(void); 

#ifdef __cplusplus
}
#endif

#endif