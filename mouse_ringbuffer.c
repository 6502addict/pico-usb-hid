#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "mouse_ringbuffer.h"

MouseRingBuffer *MouseRingBufferCreate() {
  MouseRingBuffer *mrb = (MouseRingBuffer *) calloc(1, sizeof(MouseRingBuffer));

  mrb->head = 0;
  mrb->tail = 0;
  mrb->size = 0;
  mrb->maxsize = MOUSE_BUFFER_SIZE;
  return mrb;
}

bool isMouseRingBufferEmpty(MouseRingBuffer *mrb) {
  return mrb->size == 0;
}

bool isMouseRingBufferFull(MouseRingBuffer *mrb) {
  return mrb->size == mrb->maxsize;
}

bool MouseAddEvent(MouseRingBuffer *mrb, int8_t dx, int8_t dy, int8_t dw, bool left, bool right, bool middle) {
  if (isMouseRingBufferFull(mrb)) 
    false;
  mrb->data[mrb->tail].delta_x       = dx;
  mrb->data[mrb->tail].delta_y       = dy;
  mrb->data[mrb->tail].delta_wheel   = dw;
  mrb->data[mrb->tail].button_left   = left;
  mrb->data[mrb->tail].button_right  = right;
  mrb->data[mrb->tail].button_middle = middle;
  mrb->tail = (mrb->tail + 1) % mrb->maxsize;
  mrb->size++;
  return true;
}

bool MouseGetEvent(MouseRingBuffer *mrb, int8_t *dx, int8_t *dy, int8_t *dw, bool *left, bool *right, bool *middle) {
  if (isMouseRingBufferEmpty(mrb)) 
    return false;
  *dx     = mrb->data[mrb->head].delta_x;
  *dy     = mrb->data[mrb->head].delta_y;
  *dw     = mrb->data[mrb->head].delta_wheel;
  *left   = mrb->data[mrb->head].button_left;
  *right  = mrb->data[mrb->head].button_right;
  *middle = mrb->data[mrb->head].button_middle;
  mrb->head = (mrb->head + 1) % mrb->maxsize;
  mrb->size--;
  return true;
}

void MouseRingBufferDump(MouseRingBuffer *mrb) {
  if (isMouseRingBufferEmpty(mrb)) 
    return;
  int index = mrb->head;
  for (int i = 0; i < mrb->size; i++) {
    printf("%d %d %d %d %d %d\n",
	   mrb->data[index].delta_x, 
	   mrb->data[index].delta_y, 
	   mrb->data[index].delta_wheel, 
	   mrb->data[index].button_left, 
	   mrb->data[index].button_right, 
	   mrb->data[index].button_middle);
    index = (index + 1) % mrb->maxsize;
  }
}

void MouseRingBufferRelease(MouseRingBuffer *mrb) {
  free(mrb);
}
