#include <stdio.h>
#include "bsp/board.h"
#include "tusb.h"
#include "kbd_ringbuffer.h"
#include "kbd.h"

static int to_ascii[2][128][4] =
  {
    { // EN_US
      { 0,                                0,           0,           0 }, /* 0x00 */	
      { 0,                                0,           0,           0 }, /* 0x01 */  
      { 0,                                0,           0,           0 }, /* 0x02 */  
      { 0,                                0,           0,           0 }, /* 0x03 */  
      { CAPS_LOCK | MOD_CTRL,           'a',         'A',        0x01 }, /* 0x04 */	 
      { CAPS_LOCK | MOD_CTRL,           'b',         'B',        0x02 }, /* 0x05 */	 
      { CAPS_LOCK | MOD_CTRL,           'c',         'C',        0x03 }, /* 0x06 */	 
      { CAPS_LOCK | MOD_CTRL,           'd',         'D',        0x04 }, /* 0x07 */ 
      { CAPS_LOCK | MOD_CTRL,           'e',         'E',        0x05 }, /* 0x08 */	
      { CAPS_LOCK | MOD_CTRL,           'f',         'F',        0x06 }, /* 0x09 */	
      { CAPS_LOCK | MOD_CTRL,           'g',         'G',        0x07 }, /* 0x0a */	
      { CAPS_LOCK | MOD_CTRL,           'h',         'H',        0x08 }, /* 0x0b */	
      { CAPS_LOCK | MOD_CTRL,           'i',         'I',        0x09 }, /* 0x0c */	
      { CAPS_LOCK | MOD_CTRL,           'j',         'J',        0x0A }, /* 0x0d */	
      { CAPS_LOCK | MOD_CTRL,           'k',         'K',        0x0B }, /* 0x0e */		
      { CAPS_LOCK | MOD_CTRL,           'l',         'L',        0x0C }, /* 0x0f */			
      { CAPS_LOCK | MOD_CTRL,           'm',         'M',        0x0D }, /* 0x10 */				
      { CAPS_LOCK | MOD_CTRL,           'n',         'N',        0x0E }, /* 0x11 */				
      { CAPS_LOCK | MOD_CTRL,           'o',         'O',        0x0F }, /* 0x12 */				
      { CAPS_LOCK | MOD_CTRL,           'p',         'P',        0x10 }, /* 0x13 */				
      { CAPS_LOCK | MOD_CTRL,           'q',         'Q',        0x11 }, /* 0x14 */				
      { CAPS_LOCK | MOD_CTRL,           'r',         'R',        0x12 }, /* 0x15 */				
      { CAPS_LOCK | MOD_CTRL,           's',         'S',        0x13 }, /* 0x16 */				
      { CAPS_LOCK | MOD_CTRL,           't',         'T',        0x14 }, /* 0x17 */				
      { CAPS_LOCK | MOD_CTRL,           'u',         'U',        0x15 }, /* 0x18 */
      { CAPS_LOCK | MOD_CTRL,           'v',         'V',        0x16 }, /* 0x19 */
      { CAPS_LOCK | MOD_CTRL,           'w',         'W',        0x17 }, /* 0x1a */
      { CAPS_LOCK | MOD_CTRL,           'x',         'X',        0x18 }, /* 0x1b */
      { CAPS_LOCK | MOD_CTRL,           'y',         'Y',        0x19 }, /* 0x1c */
      { CAPS_LOCK | MOD_CTRL,           'z',         'Z',        0x1A }, /* 0x1d */
      { 0,                              '1',         '!',        0x00 }, /* 0x1e */
      { 0,                              '2',         '@',        0x00 }, /* 0x1f */
      { 0,                              '3',         '#',        0x00 }, /* 0x20 */
      { 0,                              '4',         '$',        0x00 }, /* 0x21 */
      { 0,                              '5',         '%',        0x00 }, /* 0x22 */				
      { 0,                              '6',         '^',        0x00 }, /* 0x23 */				
      { 0,                              '7',         '&',        0x00 }, /* 0x24 */				
      { 0,                              '8',         '*',        0x00 }, /* 0x25 */				
      { 0,                              '9',         '(',        0x00 }, /* 0x26 */				
      { 0,                              '0',         ')',        0x00 }, /* 0x27 */				
      { 0,                        KEY_ENTER,   KEY_ENTER,   KEY_ENTER }, /* 0x28 */				
      { 0,                        ASCII_ESC,   ASCII_ESC,   ASCII_ESC }, /* 0x29 */	  
      { 0,                         ASCII_BS,    ASCII_BS,    ASCII_BS }, /* 0x2a */		    
      { 0,                         ASCII_HT,    ASCII_HT,    ASCII_HT }, /* 0x2b */	    
      { 0,                              ' ',         ' ',        0x00 }, /* 0x2c */		    
      { 0,                              '-',         '_',        0x00 }, /* 0x2d */			    
      { 0,                              '=',         '+',        0x00 }, /* 0x2e */			    
      { 0,                              '[',         '{',        0x00 }, /* 0x2f */			    
      { 0,                              ']',         '}',        0x00 }, /* 0x30 */			    
      { 0,                             '\\',         '|',        0x00 }, /* 0x31 */			    
      { 0,                              '#',         '~',        0x00 }, /* 0x32 */			    
      { 0,                              ';',         ':',        0x00 }, /* 0x33 */			    
      { 0,                             '\'',        '\"',        0x00 }, /* 0x34 */			    
      { 0,                              '`',         '~',        0x00 }, /* 0x35 */			
      { 0,                              ',',         '<',        0x00 }, /* 0x36 */			    
      { 0,                              '.',         '>',        0x00 }, /* 0x37 */			    
      { 0,                              '/',         '?',        0x00 }, /* 0x38 */			    
      { 0,                      KEY_CAPSLCK, KEY_CAPSLCK, KEY_CAPSLCK }, /* 0x39 */				
      { 0,                           KEY_F1,      KEY_F1,      KEY_F1 }, /* 0x3a */				
      { 0,                           KEY_F2,      KEY_F2,      KEY_F2 }, /* 0x3b */				
      { 0,                           KEY_F3,      KEY_F3,      KEY_F3 }, /* 0x3c */				
      { 0,                           KEY_F4,      KEY_F4,      KEY_F4 }, /* 0x3d */				
      { 0,                           KEY_F5,      KEY_F5,      KEY_F5 }, /* 0x3e */				
      { 0,                           KEY_F6,      KEY_F6,      KEY_F6 }, /* 0x3f */				
      { 0,                           KEY_F7,      KEY_F7,      KEY_F7 }, /* 0x40 */				
      { 0,                           KEY_F8,      KEY_F8,      KEY_F8 }, /* 0x41 */				
      { 0,                           KEY_F9,      KEY_F9,      KEY_F9 }, /* 0x42 */				
      { 0,                          KEY_F10,     KEY_F10,     KEY_F10 }, /* 0x43 */				
      { 0,                          KEY_F11,     KEY_F11,     KEY_F11 }, /* 0x44 */				
      { 0,                          KEY_F12,     KEY_F12,     KEY_F12 }, /* 0x45 */				
      { 0,                       KEY_PRTSCR,  KEY_PRTSCR,  KEY_PRTSCR }, /* 0x46 */				
      { 0,                       KEY_SCRLCK,  KEY_SCRLCK,  KEY_SCRLCK }, /* 0x47 */				
      { 0,                        KEY_PAUSE,   KEY_PAUSE,   KEY_PAUSE }, /* 0x48 */				
      { 0,                       KEY_INSERT,  KEY_INSERT,  KEY_INSERT }, /* 0x49 */				
      { 0,                         KEY_HOME,    KEY_HOME,    KEY_HOME }, /* 0x4a */				
      { 0,                         KEY_PGUP,    KEY_PGUP,    KEY_PGUP }, /* 0x4b */				
      { 0,                       KEY_DELETE,  KEY_DELETE,  KEY_DELETE }, /* 0x4c */
      { 0,                          KEY_END,     KEY_END,     KEY_END }, /* 0x4d */				
      { 0,                         KEY_PGDN,    KEY_PGDN,    KEY_PGDN }, /* 0x4e */
      { 0,                        KEY_RIGHT,   KEY_RIGHT,   KEY_RIGHT }, /* 0x4f */
      { 0,                         KEY_LEFT,    KEY_LEFT,    KEY_LEFT }, /* 0x50 */
      { 0,                         KEY_DOWN,    KEY_DOWN,    KEY_DOWN }, /* 0x51 */				
      { 0,                           KEY_UP,      KEY_UP,      KEY_UP }, /* 0x52 */				
      { 0,                       KEY_NUMLCK,  KEY_NUMLCK,  KEY_NUMLCK }, /* 0x53 */				
      { 0,                              '/',         '/',        0x00 }, /* 0x54 */
      { 0,                              '*',         '*',        0x00 }, /* 0x55 */
      { 0,                              '-',         '-',        0x00 }, /* 0x56 */
      { 0,                              '+',         '+',        0x00 }, /* 0x57 */
      { 0,                        KEY_ENTER,   KEY_ENTER,   KEY_ENTER }, /* 0x58 */
      { NUM_LOCK,                       '1',     KEY_END,     KEY_END }, /* 0x59 */
      { NUM_LOCK,                       '2',    KEY_DOWN,    KEY_DOWN }, /* 0x5a */
      { NUM_LOCK,                       '3',    KEY_PGDN,    KEY_PGDN }, /* 0x5b */
      { NUM_LOCK,                       '4',    KEY_LEFT,    KEY_LEFT }, /* 0x5c */
      { NUM_LOCK,                       '5',         '5',        0x00 }, /* 0x5d */
      { NUM_LOCK,                       '6',   KEY_RIGHT,   KEY_RIGHT }, /* 0x5e */
      { NUM_LOCK,                       '7',    KEY_HOME,    KEY_HOME }, /* 0x5f */
      { NUM_LOCK,                       '8',      KEY_UP,      KEY_UP }, /* 0x60 */
      { NUM_LOCK,                       '9',    KEY_PGUP,    KEY_PGUP }, /* 0x61 */
      { NUM_LOCK,                       '0',  KEY_INSERT,  KEY_INSERT }, /* 0x62 */
      { NUM_LOCK,                       '.',  KEY_DELETE,  KEY_DELETE }, /* 0x63 */
      { NUM_LOCK,                       '=',         '=',        0x00 }, /* 0x64 */
    },
    { // FR_FR
      { 0,                                0,           0,        0x00 }, /* 0x00 */	
      { 0,                                0,           0,        0x00 }, /* 0x01 */  
      { 0,                                0,           0,        0x00 }, /* 0x02 */  
      { 0,                                0,           0,        0x00 }, /* 0x03 */  
      { CAPS_LOCK | MOD_CTRL,           'q',         'Q',        0x11 }, /* 0x04 */	 
      { CAPS_LOCK | MOD_CTRL,           'b',         'B',        0x02 }, /* 0x05 */	 
      { CAPS_LOCK | MOD_CTRL,           'c',         'C',        0x03 }, /* 0x06 */	 
      { CAPS_LOCK | MOD_CTRL,           'd',         'D',        0x04 }, /* 0x07 */ 
      { CAPS_LOCK | MOD_CTRL,           'e',         'E',        0x05 }, /* 0x08 */	
      { CAPS_LOCK | MOD_CTRL,           'f',         'F',        0x06 }, /* 0x09 */	
      { CAPS_LOCK | MOD_CTRL,           'g',         'G',        0x07 }, /* 0x0a */	
      { CAPS_LOCK | MOD_CTRL,           'h',         'H',        0x08 }, /* 0x0b */	
      { CAPS_LOCK | MOD_CTRL,           'i',         'I',        0x09 }, /* 0x0c */	
      { CAPS_LOCK | MOD_CTRL,           'j',         'J',        0x0A }, /* 0x0d */	
      { CAPS_LOCK | MOD_CTRL,           'k',         'K',        0x0B }, /* 0x0e */		
      { CAPS_LOCK | MOD_CTRL,           'l',         'L',        0x0C }, /* 0x0f */			
      { CAPS_LOCK | MOD_CTRL,           ',',         '?',        0x00 }, /* 0x10 */				
      { CAPS_LOCK | MOD_CTRL,           'n',         'N',        0x0E }, /* 0x11 */
      { CAPS_LOCK | MOD_CTRL,           'o',         'O',        0x0F }, /* 0x12 */
      { CAPS_LOCK | MOD_CTRL,           'p',         'P',        0x10 }, /* 0x13 */
      { CAPS_LOCK | MOD_CTRL,           'a',         'A',        0x01 }, /* 0x14 */
      { CAPS_LOCK | MOD_CTRL,           'r',         'R',        0x12 }, /* 0x15 */
      { CAPS_LOCK | MOD_CTRL,           's',         'S',        0x13 }, /* 0x16 */
      { CAPS_LOCK | MOD_CTRL,           't',         'T',        0x14 }, /* 0x17 */
      { CAPS_LOCK | MOD_CTRL,           'u',         'U',        0x15 }, /* 0x18 */
      { CAPS_LOCK | MOD_CTRL,           'v',         'V',        0x16 }, /* 0x19 */				
      { CAPS_LOCK | MOD_CTRL,           'z',         'Z',        0x1A }, /* 0x1a */				
      { CAPS_LOCK | MOD_CTRL,           'x',         'X',        0x18 }, /* 0x1b */				
      { CAPS_LOCK | MOD_CTRL,           'y',         'Y',        0x19 }, /* 0x1c */				
      { CAPS_LOCK | MOD_CTRL,           'w',         'W',        0x17 }, /* 0x1d */				
      { 0,                              '&',         '1',        0x00 }, /* 0x1e */				
      { 0,                             0xE9,         '2',        0x7e }, /* 0x1f */				
      { 0,                              '"',         '3',         '#' }, /* 0x20 */				
      { 0,                             '\'',         '4',         '{' }, /* 0x21 */				
      { 0,                              '(',         '5',         '[' }, /* 0x22 */			
      { 0,                              '-',         '6',         '|' }, /* 0x23 */				
      { 0,                             0xE8,         '7',         '`' }, /* 0x24 */				
      { 0,                              '_',         '8',        '\\' }, /* 0x25 */				
      { 0,                             0xE7,         '9',         '^' }, /* 0x26 */				
      { 0,                             0xE0,         '0',         '@' }, /* 0x27 */				
      { 0,                        KEY_ENTER,   KEY_ENTER,   KEY_ENTER }, /* 0x28 */				
      { 0,                        ASCII_ESC,   ASCII_ESC,   ASCII_ESC }, /* 0x29 */	  
      { 0,                         ASCII_BS,    ASCII_BS,    ASCII_BS }, /* 0x2a */		    
      { 0,                         ASCII_HT,    ASCII_HT,    ASCII_HT }, /* 0x2b */	    
      { 0,                              ' ',         ' ',        0x00 }, /* 0x2c */		    
      { 0,                              ')',        0xB0,         ']' }, /* 0x2d */			    
      { 0,                              '=',         '+',         '}' }, /* 0x2e */			    
      { 0,                              '^',        0xA8,        0x00 }, /* 0x2f */			    
      { 0,                              '$',        0xA3,        0xA4 }, /* 0x30 */			    
      { 0,                              '*',        0xB5,        0x00 }, /* 0x31 */			    
      { 0,                              '#',        0x7E,        0x00 }, /* 0x32 */			    
      { 0,                              'm',         'M',        0x00 }, /* 0x33 */			    
      { 0,                             0xF9,         '%',        0x00 }, /* 0x34 */			    
      { 0,                             0xB2,        0x00,        0x00 }, /* 0x35 */			
      { 0,                              ';',         '.',        0x00 }, /* 0x36 */			    
      { 0,                              ':',         '/',        0x00 }, /* 0x37 */			    
      { 0,                              '!',        0xA7,        0x00 }, /* 0x38 */			    
      { 0,                      KEY_CAPSLCK, KEY_CAPSLCK,        0x00 }, /* 0x39 */				
      { 0,                           KEY_F1,      KEY_F1,      KEY_F1 }, /* 0x3a */				
      { 0,                           KEY_F2,      KEY_F2,      KEY_F2 }, /* 0x3b */				
      { 0,                           KEY_F3,      KEY_F3,      KEY_F3 }, /* 0x3c */				
      { 0,                           KEY_F4,      KEY_F4,      KEY_F4 }, /* 0x3d */				
      { 0,                           KEY_F5,      KEY_F5,      KEY_F5 }, /* 0x3e */				
      { 0,                           KEY_F6,      KEY_F6,      KEY_F6 }, /* 0x3f */				
      { 0,                           KEY_F7,      KEY_F7,      KEY_F7 }, /* 0x40 */				
      { 0,                           KEY_F8,      KEY_F8,      KEY_F8 }, /* 0x41 */				
      { 0,                           KEY_F9,      KEY_F9,      KEY_F9 }, /* 0x42 */				
      { 0,                          KEY_F10,     KEY_F10,     KEY_F10 }, /* 0x43 */				
      { 0,                          KEY_F11,     KEY_F11,     KEY_F11 }, /* 0x44 */				
      { 0,                          KEY_F12,     KEY_F12,     KEY_F12 }, /* 0x45 */				
      { 0,                       KEY_PRTSCR,  KEY_PRTSCR,  KEY_PRTSCR }, /* 0x46 */				
      { 0,                       KEY_SCRLCK,  KEY_SCRLCK,  KEY_SCRLCK }, /* 0x47 */				
      { 0,                        KEY_PAUSE,   KEY_PAUSE,   KEY_PAUSE }, /* 0x48 */				
      { 0,                       KEY_INSERT,  KEY_INSERT,  KEY_INSERT }, /* 0x49 */				
      { 0,                         KEY_HOME,    KEY_HOME,    KEY_HOME }, /* 0x4a */				
      { 0,                         KEY_PGUP,    KEY_PGUP,    KEY_PGUP }, /* 0x4b */				
      { 0,                       KEY_DELETE,  KEY_DELETE,  KEY_DELETE }, /* 0x4c */				
      { 0,                          KEY_END,     KEY_END,     KEY_END }, /* 0x4d */				
      { 0,                         KEY_PGDN,    KEY_PGDN,    KEY_PGDN }, /* 0x4e */				
      { 0,                        KEY_RIGHT,   KEY_RIGHT,   KEY_RIGHT }, /* 0x4f */				
      { 0,                         KEY_LEFT,    KEY_LEFT,    KEY_LEFT }, /* 0x50 */				
      { 0,                         KEY_DOWN,    KEY_DOWN,    KEY_DOWN }, /* 0x51 */			
      { 0,                           KEY_UP,      KEY_UP,      KEY_UP }, /* 0x52 */				
      { 0,                       KEY_NUMLCK,  KEY_NUMLCK,  KEY_NUMLCK }, /* 0x53 */				
      { 0,                              '/',         '/',        0x00 }, /* 0x54 */				
      { 0,                              '*',         '*',        0x00 }, /* 0x55 */				
      { 0,                              '-',         '-',        0x00 }, /* 0x56 */				
      { 0,                              '+',         '+',        0x00 }, /* 0x57 */				
      { 0,                        KEY_ENTER,   KEY_ENTER,   KEY_ENTER }, /* 0x58 */				
      { NUM_LOCK,                       '1',     KEY_END,     KEY_END }, /* 0x59 */				
      { NUM_LOCK,                       '2',    KEY_DOWN,    KEY_DOWN }, /* 0x5a */				
      { NUM_LOCK,                       '3',    KEY_PGDN,    KEY_PGDN }, /* 0x5b */				
      { NUM_LOCK,                       '4',    KEY_LEFT,    KEY_LEFT }, /* 0x5c */				
      { NUM_LOCK,                       '5',         '5',        0x00 }, /* 0x5d */				
      { NUM_LOCK,                       '6',   KEY_RIGHT,   KEY_RIGHT }, /* 0x5e */				
      { NUM_LOCK,                       '7',    KEY_HOME,    KEY_HOME }, /* 0x5f */				
      { NUM_LOCK,                       '8',      KEY_UP,      KEY_UP }, /* 0x60 */				
      { NUM_LOCK,                       '9',    KEY_PGUP,    KEY_PGUP }, /* 0x61 */				
      { NUM_LOCK,                       '0',  KEY_INSERT,  KEY_INSERT }, /* 0x62 */				
      { NUM_LOCK,                       '.',  KEY_DELETE,  KEY_DELETE }, /* 0x63 */				
      { NUM_LOCK,                       '<',         '>',        0x00 }, /* 0x64 */				
    },
    /*
      {  // DE_DE
      }
    */
  };

static uint8_t *special[] = {
  //  NORMAL       SHIFT       CTRL   CTRL+SHIFT
      "^M",        "^M",       "^M",       "^M",      // KEY_ENTER    0x8000 index 00
      "^K",        "^K",       "^K",       "^K",      // KEY_UP       0x8001 index 04
      "^V",        "^V",       "^V",       "^V",      // KEY_DOWN     0x8002 index 08
      "^L",        "^L",       "^L",       "^L",      // KEY_RIGHT    0x8003 index 0C 
      "^H",        "^H",       "^H",       "^H",      // KEY_LEFT     0x8004 index 10
        "",          "",         "",         "",      // KEY_INSERT   0x8005 index 14
        "",          "",         "",         "",      // KEY_DELETE   0x8006 index 18
        "",          "",         "",         "",      // KEY_PGUP     0x8007 index 1C
        "",          "",         "",         "",      // KEY_PGDN     0x8008 index 20
        "",          "",         "",         "",      // KEY_HOME     0x8009 index 24 
        "",          "",         "",         "",      // KEY_END      0x800A index 28
        "",          "",         "",         "",      // KEY_PAUSE    0x800B index 2C 
   "^A@\r",    "^A\\\r",         "",         "",      // KEY_F1       0x800C index 30  
   "^AA\r",     "^Aa\r",         "",         "",      // KEY_F2       0x800D index 34 
   "^AB\r",     "^Ab\r",         "",         "",      // KEY_F3       0x800E index 38  
   "^AC\r",     "^Ac\r",         "",         "",      // KEY_F4       0x800F index 3C
   "^AD\r",     "^Ad\r",         "",         "",      // KEY_F5       0x8010 index 40
   "^AE\r",     "^Ae\r",         "",         "",      // KEY_F6       0x8011 index 44
   "^AF\r",     "^Af\r",         "",         "",      // KEY_F7       0x8012 index 48
   "^AG\r",     "^Ag\r",         "",         "",      // KEY_F8       0x8013 index 4C
   "^AH\r",     "^Ah\r",         "",         "",      // KEY_F9       0x8014 index 50
   "^AI\r",     "^Ai\r",         "",         "",      // KEY_F10      0x8015 index 54
   "^AJ\r",     "^Aj\r",         "",         "",      // KEY_F11      0x8016 index 58
        "",          "",         "",         "",      // KEY_F12      0x8017 index 5C
};

static void enqueue(KbdRingBuffer *krb, uint16_t key, uint8_t mod) {
  uint8_t l, i;
  uint8_t *str;
  bool    ctrl = false;
  
  if ((key & 0x8000) == 0x8000) {
    uint8_t index;

    index = (key & 0xFF) << 2;
    if ((mod & KBD_MOD_CTRL) == KBD_MOD_CTRL) 
      index |= 0x02;
    if ((mod & KBD_MOD_SHIFT) == KBD_MOD_SHIFT) 
      index |= 0x01;
    str = special[index];
    for (i = 0, l = strlen(str); i < l; i++) {
      uint8_t ch = str[i];

      if (ch == '^') {
	ctrl = true;
	continue;
      } else {
	if (ctrl) {
	  ch &= 0x1F;
	  ctrl = false;
	}
	KbdAddKey(krb, ch);
      }
    }
  } else if (key <= 0xFF) 
    KbdAddKey(krb, key);
}

int kbd_decode_tvi950(KbdRingBuffer *krb, uint8_t keycode, uint8_t modifier) {
  uint8_t mod = (modifier & 0x0F) | ((modifier >> 4) & 0x0F);
  uint8_t map = 0;
  uint16_t flag, key, skey, akey;
  
  if ((modifier & KBD_MOD_WINDOW) == KBD_MOD_WINDOW) {
    switch(keycode) {
    case KBD_KEY_F1:
      lang = 0;
      break;
    case KBD_KEY_F2:
      lang = 1;
      break;  
      //    case KBD_KEY_F3:
      //      lang = 2;
      //      break;
    case KBD_KEY_F5:
      term = TERM_TVI950;
      break;  
    case KBD_KEY_F6:
      term = TERM_VT100;
      break;  
    case KBD_KEY_F12:
      debug = !debug;
      break;
    }
  } 
  flag = to_ascii[lang][keycode][0];
  key  = to_ascii[lang][keycode][1];
  skey = to_ascii[lang][keycode][2];
  akey = to_ascii[lang][keycode][3];
  if ((mod & KBD_MOD_CTRL) == KBD_MOD_CTRL) {
    if ((key & 0x8000) == 0x8000)
      enqueue(krb, key, mod);
    else if (key < 0x80) {
      if ((flag & MOD_CTRL) == MOD_CTRL)
	KbdAddKey(krb, key & 0x1F);
    }
  } else if ((mod & MOD_ALT) == MOD_ALT) {
    if ((akey > 0) & (akey <= 0xff)) {
      // printf("adding keycode = %0.2x key = '%c' akey = %0.2x\n", keycode, key, akey);
      KbdAddKey(krb, key & 0x1F);
    } else
      switch(keycode) {
	// add special cases
      default:
	// do nothing;
	break;
      }
  } else {
    if (capslock_state) 
      enqueue(krb, ((mod & KBD_MOD_SHIFT) == KBD_MOD_SHIFT) ? key : skey, mod);
    else if (numlock_state)
      enqueue(krb, ((mod & KBD_MOD_SHIFT) == KBD_MOD_SHIFT) ? key : skey, mod);      
    else
      enqueue(krb, ((mod & KBD_MOD_SHIFT) == KBD_MOD_SHIFT) ? skey : key, mod);      
  }
}

