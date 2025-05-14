# USB Keyboard/Mouse Adapter Customization Guide

This document explains how to customize the provided code to create USB-to-terminal adapters with keyboard and mouse support.

## Overview

The code provides a framework for creating USB keyboard and mouse adapters for vintage computers and terminals. It maps modern USB HID keycodes to the appropriate terminal control sequences or ASCII codes. The system uses:

- TinyUSB library for USB device support
- Raspberry Pi Pico (RP2040) as the hardware platform
- Ring buffers for keyboard and mouse event handling

## Architecture

1. **USB HID Handling**: TinyUSB automatically processes USB HID events
2. **Keyboard Decoding**: Converts keycodes to terminal-specific sequences
3. **Mouse Movement Processing**: Translates mouse movements to appropriate control sequences
4. **Output Generation**: Sends the converted sequences to the target system

## Customizing the Keyboard Mapping

The core functionality is in the keyboard decoding. When a USB keyboard sends a keypress, the system:

1. The USB HID layer receives the keycode and modifier status
2. `process_keycode()` is called with this information
3. This function routes to the appropriate terminal-specific decoder
4. The decoder maps the keycode to the appropriate ASCII values or control sequences

### Steps to Create Your Own Custom Adapter

1. **Create a new decoder file**:
   Start by copying `kbd_decode_template.c` to a new file, e.g., `kbd_decode_myterm.c`

2. **Modify the key mapping table**:
   The `to_ascii` array in the template defines mappings for USB HID keycodes:
   ```c
   static int to_ascii[2][128][4] =
   {
     { // EN_US
       { 0, 0, 0, 0 },                   /* 0x00 */
       // ...more entries...
     },
     { // FR_FR
       // ...entries for French keyboard...
     },
   };
   ```

   Each entry has 4 values:
   - Flags (CAPS_LOCK, NUM_LOCK, MOD_ALT, MOD_CTRL)
   - Normal key value (no modifiers)
   - Shifted key value
   - Alt key value (when applicable)

3. **Modify the special mapping table**:
   For function keys, arrows, and other special keys, the `special[]` array defines the control sequences:
   ```c
   uint8_t *special[] = {
     //  NORMAL       SHIFT       CTRL   CTRL+SHIFT
         "\n",        "\n",       "\n",       "\n",      // KEY_ENTER    0x8000 index 00
         // ...more entries...
   };
   ```

4. **Implement your decoder function**:
   Create a function like:
   ```c
   int kbd_decode_myterm(KbdRingBuffer *krb, uint8_t keycode, uint8_t modifier) {
     // Your custom decoder logic
     // Can use kbd_decode as starting point
   }
   ```

5. **Add your terminal to main.c**:
   - Add a new terminal type in `kbd.h`
   - Modify the `process_keycode()` function in `main.c`:
   ```c
   void process_keycode(int keycode, int modifier) {
     switch(term) {
     case TERM_TVI950:
       kbd_decode_tvi950(krb, keycode, modifier);
       break;
     case TERM_VT100:
       kbd_decode_vt100(krb, keycode, modifier);
       break;
     case TERM_MYTERM:
       kbd_decode_myterm(krb, keycode, modifier);
       break;
     }
   }
   ```

## Customizing Mouse Support

Mouse events are processed through the `process_mouse()` function:

```c
void process_mouse(int8_t dx, int8_t dy, int8_t dw, bool left, bool right, bool middle) {
  mouse_decode(mrb, dx, dy, dw, left, right, middle);
}
```

To customize mouse behavior:

1. Create a custom `mouse_decode_myterm()` function based on your target system's requirements
2. Implement mouse movement tracking similar to the current implementation in `main.c`
3. Add a terminal-specific switch in the `process_mouse()` function similar to `process_keycode()`

## Ring Buffer Usage

The code uses ring buffers for both keyboard and mouse events:

- `KbdRingBuffer` for keyboard events
- `MouseRingBuffer` for mouse events

These buffers store events until they can be processed and sent to the target system. Key functions:

- `KbdAddKey()`: Add a key to the keyboard buffer
- `KbdGetKey()`: Retrieve a key from the keyboard buffer
- `MouseGetEvent()`: Retrieve a mouse event from the mouse buffer

## USB Hub Compatibility Note

**IMPORTANT**: The TinyUSB library has compatibility issues with USB 3.0 hubs. When connecting both a keyboard and mouse to the adapter:

1. **DO NOT use USB 3.0 hubs** - they will not work reliably with this adapter
2. **ONLY use pure USB 2.0 hubs** - these are fully compatible and will work as expected

This limitation is a known issue with the TinyUSB library implementation. Using a USB 3.0 hub may result in device detection failures or intermittent disconnections.

## Example: Creating a Custom Terminal Adapter

Here's a simplified example for creating a custom terminal adapter:

1. Copy `kbd_decode_template.c` to `kbd_decode_custom.c`
2. Add your terminal type definition in `kbd.h`:
   ```c
   #define TERM_CUSTOM 3
   ```
3. Implement your decoder function:
   ```c
   int kbd_decode_custom(KbdRingBuffer *krb, uint8_t keycode, uint8_t modifier) {
     // Your implementation
   }
   ```
4. Update `process_keycode()` in `main.c` to include your terminal type
5. Modify the key mapping tables to match your terminal's requirements
6. Test and refine your implementation

## Debugging Tips

1. Set `debug = true` to enable debug output with keycode information
2. Use `KbdRingBufferDump()` to view the contents of the keyboard buffer
3. Add print statements to track the decoding process
4. Check USB device enumeration using `lsusb` (Linux) or Device Manager (Windows)

## Further Customization

The code is designed to be highly customizable. You can:

1. Add support for more keyboard layouts
2. Implement additional terminal types
3. Enhance mouse support with absolute positioning
4. Add support for gamepad input (already partially implemented)
5. Implement a configuration system to switch terminal types at runtime

## Conclusion

By understanding the keyboard and mouse decoding process, you can create custom adapters for virtually any vintage terminal or computer system. The key is to understand the mapping between modern USB HID codes and the target system's input requirements.

For assistance with specific terminal types or custom implementations, refer to the terminal's technical documentation for the control codes and sequences it expects.
