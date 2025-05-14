#include <stdio.h>
#include "bsp/board.h"
#include "tusb.h"
#include "mouse_ringbuffer.h"

#define HOTSPOT __inline__ __attribute__ ((always_inline, hot))

int mouse_decode(MouseRingBuffer *mrb, int8_t x, int8_t y, int8_t wheel, bool left, bool right, bool middle) {
  // there is nothing to do until we decide to implement some protocol
  // we just add the data to the mouse ring buffer


  if (MouseAddEvent(mrb, x, y, wheel, left, right, middle) == false) 
    printf("failed to add the mouse event to the mouse ring buffer\n");
}

