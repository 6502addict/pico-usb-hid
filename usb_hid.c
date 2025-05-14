#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>
#include "bsp/board.h"
#include "tusb.h"
#include "gamepad.h"

#define HOTSPOT __inline__ __attribute__ ((always_inline, hot))

extern void process_keycode(uint8_t, uint8_t);
extern void process_mouse(int8_t, int8_t, int8_t, bool, bool, bool);
extern void process_nintendo_gamepad(uint8_t, uint8_t);
extern void process_mini_gamepad(uint8_t, uint8_t);
extern void dump(const uint8_t*, const size_t);
extern bool hid_debug;

bool numlock_state           = false;
bool capslock_state          = false;
bool scrolllock_state        = false;
bool velocityone_flightstick = false;
bool nintendo_gamepad        = false;
bool mini_gamepad            = false;
bool wireless_gamepad        = false;
bool glab_gamepad            = false;

void tuh_hid_mount_cb (uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  static bool initialized = false;
  static uint8_t leds;
  uint16_t vid, pid;
  
  if (hid_debug)
    printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  if (hid_debug) {
    printf("VID = %04x, PID = %04x\r\n", vid, pid);
    printf("ITF PROTOCOL = %d\n", itf_protocol);
  }
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD) {
    tuh_hid_receive_report (dev_addr, instance);
    if (hid_debug)
      printf("keyboard %0.4x %0.4x connected\n", vid, pid);      
    if (!initialized) {
      leds |= KEYBOARD_LED_NUMLOCK;
      tuh_hid_set_report(dev_addr, instance, 0, HID_REPORT_TYPE_OUTPUT, &leds, sizeof(leds));
      initialized = true;
    }
  }

  if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
    tuh_hid_receive_report (dev_addr, instance);
    if (hid_debug)
      printf("mouse %0.4x %0.4x connected\n", vid, pid);      
  }
  
  // if velocityone flightstick enable protocol HID_ITF_PROTOCOL_NONE
  if (itf_protocol == HID_ITF_PROTOCOL_NONE) { 
    if ((vid == 0x10f5) && (pid == 0x7055)) {
      velocityone_flightstick = true;
      if (hid_debug)
	printf("flightsick %0.4x %0.4x connected\n", vid, pid);      
      tuh_hid_receive_report (dev_addr, instance);
    }
    // nintendo gamepad
    if ((vid == 0x081f) && (pid == 0xe401)){
      nintendo_gamepad = true;
      if (hid_debug)
	printf("nintendo gamepad %0.4x %0.4x connected\n", vid, pid);      
      tuh_hid_receive_report (dev_addr, instance);
    }
    // mini gamepad 
    if ((vid == 0x0079) && (pid == 0x0011)){
      mini_gamepad = true;
      if (hid_debug)
    	printf("mini gamepad %0.4x %0.4x connected\n", vid, pid);      
      tuh_hid_receive_report (dev_addr, instance);
    }
    // wirless gamepad olimex bullshit
    if ((vid == 0x0079) && (pid == 0x0126)) {       
      wireless_gamepad = true;
      if (hid_debug)
	printf("wireless gamepad %0.4x %0.4x connected\n", vid, pid);      
      tuh_hid_receive_report (dev_addr, instance);
    }
    if ((vid == 0x2563) && (pid == 0x575)) {  
      glab_gamepad = true;
      if (hid_debug)
	printf("glab gamepad %0.4x %0.4x connected\n", vid, pid);      
      tuh_hid_receive_report (dev_addr, instance);
    } else {
      printf("unknown VID = %0.4x PID = %0.4x device\n" ,vid, pid);
    }
  }
}

HOTSPOT static bool key_pressed(hid_keyboard_report_t const *report, uint8_t keycode) {
  for (uint8_t i = 0; i < sizeof(report->keycode); i++) 
    if (report->keycode[i] == keycode) 
      return true;
  return false;
}

void tuh_hid_report_received_cb  (uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
  static hid_keyboard_report_t last_kbd_report = { 0, 0, { 0, 0, 0, 0, 0, 0 } };
  static uint8_t last_nintendo_gamepad_report[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  static uint8_t last_mini_gamepad_report[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  static uint8_t last_glab_gamepad_report[28] = { 0 };
  hid_keyboard_report_t *kbd_report;
  hid_mouse_report_t *mouse_report;
  uint8_t button_mask;
  uint8_t i;
  (void) instance; (void) len;
  bool left, right, middle;
  static uint8_t leds = 0, last_leds = 0;

  switch (tuh_hid_interface_protocol (dev_addr, instance)) {
  case HID_ITF_PROTOCOL_NONE:
    if (velocityone_flightstick) {
      if (hid_debug) {
	printf("velocityone flightstick packet :\n");
	dump(report, len);
	printf("\n");
      }
    } else if (mini_gamepad) {
      uint16_t joystate;
      uint8_t buttons, joystick;
      int i;
      
      for (i = 0; i < len; i++) {
	if (report[i] != last_mini_gamepad_report[i]) {
	  dump(report, len);
	  printf("\n");
	  joystate = report[3] << 8 | report[4];
	  switch(joystate) {
	  case 0x7F7F:  joystick = 0;
	    break;
	  case 0x7F00:  joystick = 1;
	    break;
	  case 0xFF00:  joystick = 2;
	    break;
	  case 0xFF7F:  joystick = 3;
	    break;
	  case 0xFFFF:  joystick = 4;
	    break;
	  case 0x7FFF:  joystick = 5;
	    break;
	  case 0x00FF:  joystick = 6;
	    break;
	  case 0x007F:  joystick = 7;
	    break;
	  case 0x0000:  joystick = 8;
	    break;
	  }
	  buttons |= ((report[5] & 0x10) == 0x10) ? GAMEPAD_B      : 0;
	  buttons |= ((report[5] & 0x20) == 0x20) ? GAMEPAD_A      : 0;
	  buttons |= ((report[6] & 0x20) == 0x20) ? GAMEPAD_START  : 0;
	  buttons |= ((report[6] & 0x10) == 0x10) ? GAMEPAD_SELECT : 0;
	  process_mini_gamepad(joystick, buttons);
	  memcpy(&last_mini_gamepad_report, report, sizeof(last_nintendo_gamepad_report));
	}
      }
    } else if (nintendo_gamepad) {
      uint16_t joystate;
      uint8_t buttons, joystick;

      if (memcmp(report, last_nintendo_gamepad_report, len) != 0) {
	joystate = report[0] << 8 | report[1];
	switch(joystate) {
	case 0x7F7F:  joystick = 0;
	  break;
	case 0x7F00:  joystick = 1;
	  break;
	case 0xFF00:  joystick = 2;
	  break;
	case 0xFF7F:  joystick = 3;
	  break;
	case 0xFFFF:  joystick = 4;
	  break;
	case 0x7FFF:  joystick = 5;
	  break;
	case 0x00FF:  joystick = 6;
	  break;
	case 0x007F:  joystick = 7;
	  break;
	case 0x0000:  joystick = 8;
	  break;
	}
	buttons |= ((report[5] & 0x80) == 0x80) ? GAMEPAD_Y      : 0;
	buttons |= ((report[5] & 0x40) == 0x40) ? GAMEPAD_B      : 0;
	buttons |= ((report[5] & 0x20) == 0x20) ? GAMEPAD_A      : 0;
	buttons |= ((report[5] & 0x10) == 0x10) ? GAMEPAD_X      : 0;
	buttons |= ((report[6] & 0x20) == 0x20) ? GAMEPAD_START  : 0;
	buttons |= ((report[6] & 0x10) == 0x10) ? GAMEPAD_SELECT : 0;
	buttons |= ((report[6] & 0x02) == 0x02) ? GAMEPAD_RIGHT  : 0;
	buttons |= ((report[6] & 0x01) == 0x01) ? GAMEPAD_LEFT   : 0;
	process_nintendo_gamepad(joystick, buttons);
	memcpy(&last_nintendo_gamepad_report, report, sizeof(last_nintendo_gamepad_report));
      }
    } if (glab_gamepad) {
      int i, x;
      /*
	joy 0       byte 3 and 4 btn = byte 1 bit 2
	joy 1       byte 5 and 6 btn = byte 1 bit 3
	btn select  byte 1 bit 0
	btn start   byte 1 bit 1
	bth home    byte 1 bit 4  pressure = byte 3,4,5,6 ???
	cross right byte 2 bits 0..3 2 F pressure = byte 7
	cross left  byte 2 bits 0..3 6 F pressure = byte 8
	cross up    byte 2 bits 0..3 0 F pressure = byte 9
	cross down  byte 2 bits 0..3 4 F pressure = byte 10
	l1          byte 0 bit  4        pressure = byte 15
	r1          byte 0 bit  5        pressure = byte 16
	l2          byte 0 bit  6        pressure = byte 17
	r2          byte 0 bit  7        pressuer = byte 18
      */
      for (x = i = 0; i < len; i++) {
	//	if (i == 21)
	//	  continue;
	//	if (i == 19)
	//	  continue;

	if (report[i] != last_glab_gamepad_report[i]) {
	  printf("R[%2d]=%0.4x, L[%2d]=%04x\n", i, report[i], i, last_glab_gamepad_report[i]);
	  x++;
	}
      }
      if (x != 0) {
	//	dump(report, len);
	//	printf("\n");
	memcpy(&last_glab_gamepad_report, report, sizeof(last_glab_gamepad_report));
      }
    } else {
      if (hid_debug) {
	//	printf("undefined packet type:\n");
	//	dump(report, len);
	//	printf("\n");
      }
    }
    break;

  case HID_ITF_PROTOCOL_KEYBOARD:
    kbd_report = (hid_keyboard_report_t *) report;
    if (hid_debug) 
      printf("kbd report len = %d\n", len);
      for (i = 0; i < sizeof(kbd_report->keycode); i++) 
      if (kbd_report->keycode[i]) 
	if (!key_pressed(&last_kbd_report, kbd_report->keycode[i]))  {
	  switch(kbd_report->keycode[i]) {
	  case 0x39:
	    capslock_state = !capslock_state;	
	    if (capslock_state)
	      leds |= KEYBOARD_LED_CAPSLOCK;
	    else
	      leds &= ~KEYBOARD_LED_CAPSLOCK;
	    break;
	    
	  case 0x47:
	    scrolllock_state = !scrolllock_state;	
	    if (scrolllock_state)
	      leds |= KEYBOARD_LED_SCROLLLOCK;
	    else
	      leds &= ~KEYBOARD_LED_SCROLLLOCK;
	    break;
	    
	  case 0x53:
	    numlock_state = !numlock_state;
	    if (!numlock_state)
	      leds |= KEYBOARD_LED_NUMLOCK;
	    else
	      leds &= ~KEYBOARD_LED_NUMLOCK;
	    break;

	  default:
	    process_keycode(kbd_report->keycode[i], kbd_report->modifier); 
	    break;
	  }
	}
    if (last_leds != leds) {
      tuh_hid_set_report(dev_addr, instance, 0, HID_REPORT_TYPE_OUTPUT, &leds, sizeof(leds));
      last_leds = leds;
    }
    memcpy(&last_kbd_report, kbd_report, sizeof(last_kbd_report));
    break;

  case HID_ITF_PROTOCOL_MOUSE:
    //    printf("len = %d\n", len);
    //    dump(report, len);
    mouse_report = (hid_mouse_report_t *) report;
    left   = mouse_report->buttons & MOUSE_BUTTON_LEFT   ? true : false;
    right  = mouse_report->buttons & MOUSE_BUTTON_RIGHT  ? true : false;
    middle = mouse_report->buttons & MOUSE_BUTTON_MIDDLE ? true : false;
    process_mouse(mouse_report->x, mouse_report->y, mouse_report->wheel, left, right, middle);
    break;
  }
  tuh_hid_receive_report (dev_addr, instance);
}

void tuh_hid_umount_cb (uint8_t dev_addr, uint8_t instance)  {
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  uint16_t vid, pid;

  if (hid_debug) 
    printf("HID device address = %d, instance = %d is umounted\r\n", dev_addr, instance);
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD) {
    if (hid_debug) 
      printf("keyboard %0.4x %0.4x disconnected\n", vid, pid);  
  }
  if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
    if (hid_debug) 
      printf("mouse %0.4x %0.4x disconnected\n", vid, pid);  
  }  
  // if velocityone flightstick enable protocol HID_ITF_PROTOCOL_NONE
  if ((itf_protocol == HID_ITF_PROTOCOL_NONE) && 
      ((vid == 0x10f5) && (pid == 0x7055))) {
    if (hid_debug) 
      printf("flightstick %0.4x %0.4x disconnected\n", vid, pid);      
  }
  // mini gamepad
  if ((itf_protocol == HID_ITF_PROTOCOL_NONE) && 
      ((vid == 0x0079) && (pid == 0x0011))) {
    if (hid_debug) 
      printf("mini gameopad %0.4x %0.4x disconnected\n", vid, pid);      
  }
  // nintendo gamepad
  if ((itf_protocol == HID_ITF_PROTOCOL_NONE) && 
      ((vid == 0x081f) && (pid == 0xe401))) {
    if (hid_debug) 
      printf("nintendo gameopad %0.4x %0.4x disconnected\n", vid, pid);      
  }
  // wireless gamepad
  if ((itf_protocol == HID_ITF_PROTOCOL_NONE) && 
      ((vid == 0x0079) && (pid == 0x0126))) {       
    if (hid_debug) 
      printf("wireless gameopad %0.4x %0.4x disconnected\n", vid, pid);      
  }

}

