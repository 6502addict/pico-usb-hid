#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>
#include "hardware/timer.h"
#include "bsp/board.h"
#include "tusb.h"
#include "kbd_ringbuffer.h"
#include "mouse_ringbuffer.h"
#include "kbd.h"

bool debug = false;
bool hid_debug = true;
KbdRingBuffer *krb = NULL;
MouseRingBuffer *mrb = NULL;
uint8_t lang  = LANG_EN;
uint8_t term  = TERM_TVI950;

int kbd_decode_vt100(KbdRingBuffer *, uint8_t, uint8_t);
int kbd_decode_tvi950(KbdRingBuffer *, uint8_t, uint8_t);
int mouse_decode(MouseRingBuffer *, int8_t, int8_t, int8_t, bool, bool, bool);

void  dump(uint8_t *buffer, size_t size) {
  uint32_t i, a, lsize;
  int c;

  for (a = 0;;) {
    printf("%08X: ", a);
    lsize = (size >= 16) ? 16 : size;
    for (i=0 ; i<lsize; i++)
      printf("%02X ", buffer[a + i]);
    for (i = lsize; i<16; i++)
      printf("   ");
    printf("  |");
    for (i=0;i<lsize;i++) {
      c = buffer[a+i];
      printf("%c", ((c < 0x20) || (c > 126)) ? '.' : c);
    }
    for (i=size;i<32;i++)
      printf(" ");
    a += lsize;
    size -= lsize;
    printf("|\n");
    if (size <= 0)
      return;
  }
}

void process_keycode(int keycode, int modifier) {
  //  if (debug)
  //    printf("keycode = %x, modifier = %x\n", keycode, modifier); 
  switch(term) {
  case TERM_TVI950:
    kbd_decode_tvi950(krb, keycode, modifier);
    break;
  case TERM_VT100:
    kbd_decode_vt100(krb, keycode, modifier);
    break;
  }
}

void process_nintendo_gamepad(uint8_t joystick, uint8_t buttons) {
  printf("joystick = '%d', buttons '%0.2x'\n", joystick, buttons);
}

void process_mini_gamepad(uint8_t joystick, uint8_t buttons) {
  printf("joystick = '%d', buttons '%0.2x'\n", joystick, buttons);  
}

void process_mouse(int8_t dx, int8_t dy, int8_t dw, bool left, bool right, bool middle) {
  mouse_decode(mrb, dx, dy, dw, left, right, middle);
}

bool led_service (repeating_timer_t *rt) {
  static bool led_state = false;

  board_led_write(led_state);
  led_state = !led_state;
  return true;
}


/*===========================================================================
 * start here
 * ========================================================================*/
int main (void) {
  static struct repeating_timer timer_tuh;
  static struct repeating_timer timer_led;
  static int16_t pos_x, pos_y, pos_wheel;

  pos_x     = 1024/2;
  pos_y     = 768/2;
  pos_wheel = 256/2;
  stdio_init_all();
  krb = KbdRingBufferCreate();
  mrb = MouseRingBufferCreate();
  gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);
  gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
  printf("program stated\n");
  board_init();
  tusb_init();
  add_repeating_timer_ms(1000/2, led_service, NULL, &timer_led);
  while (1) {
    int8_t x, y, wheel;
    bool left, right, middle;
    uint16_t key;
    
    tuh_task();
    while(KbdGetKey(krb, &key)) {
      if (debug) {
      	printf("key = %x\n", key);
      } else
      	printf("%c", key);
    }
    while(MouseGetEvent(mrb, &x, &y, &wheel, &left, &right, &middle)) {
   //   printf("x = %d, y = %d, wheel = %d, left = %d, right = %d, middle = %d\n", x, y, wheel, left, right, middle);
      pos_x     = pos_x     + (int16_t) x;
      if (pos_x < 0) 
	pos_x = 0; 
      if (pos_x >= 1024)
	pos_x = 1023;
      pos_y     = pos_y     + (int16_t) y;
      if (pos_y < 0) 
	pos_y = 0; 
      if (pos_y >= 768)
	pos_y = 767;
      pos_wheel = pos_wheel + (int16_t) wheel;      
      if (pos_wheel < 0) 
	pos_wheel = 0; 
      if (pos_wheel >= 256)
	pos_y = 255;
      printf("position: %d %d, wheel = %d\n", pos_x, pos_y, pos_wheel);
    }
  }
  KbdRingBufferRelease(krb);
  MouseRingBufferRelease(mrb);
}

//       tight_loop_contents();
