#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MOUSE_BUFFER_SIZE 32

typedef struct {
  int8_t delta_x;
  int8_t delta_y;
  int8_t delta_wheel;
  bool   button_left;
  bool   button_right;
  bool   button_middle;
} MouseEvent;

typedef struct {
  MouseEvent data[MOUSE_BUFFER_SIZE];
  int head;
  int tail;
  int size;
  int maxsize;
} MouseRingBuffer;

MouseRingBuffer *MouseRingBufferCreate(); 
bool isMouseRingBufferEmpty(MouseRingBuffer *);
bool isMouseRingBufferFull(MouseRingBuffer *);
bool MouseAddEvent(MouseRingBuffer *, int8_t, int8_t, int8_t, bool, bool, bool);
bool MouseGetEvent(MouseRingBuffer *, int8_t *, int8_t *, int8_t *, bool *, bool *, bool *);
void MouseRingBufferDump(MouseRingBuffer *);
void MouseRingBufferRelease(MouseRingBuffer *);

