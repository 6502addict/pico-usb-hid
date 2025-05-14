# USB to Mouse Adapter Customization Guide

This document explains how to customize the code to create USB to mouse adapters using the TinyUSB stack on the Raspberry Pi Pico (RP2040) platform.

## Overview

The system works by following the HID_ITF_PROTOCOL_MOUSE standard, which is automatically handled by the `usb-hid.c` implementation. When mouse movements or button presses are detected, the system calls the following function:

```c
void process_mouse(int8_t dx, int8_t dy, int8_t dw, bool left, bool right, bool middle) {
  mouse_decode(mrb, dx, dy, dw, left, right, middle);
}
```

Where:
- `dx`: Horizontal movement (positive = right, negative = left)
- `dy`: Vertical movement (positive = down, negative = up)
- `dw`: Wheel movement (scroll)
- `left`, `right`, `middle`: Boolean values indicating button states

## Customization Options

### Option 1: Simple Mouse Adapter

If you want to create a basic USB to mouse adapter, the `mouse_decode.c` file is where you should implement your custom handling logic. In the current implementation, this file serves as a placeholder for specific processing.

Here's how to customize it:

1. Locate `mouse_decode.c` in your project
2. Modify the `mouse_decode()` function to implement your specific translation logic
3. Common customizations include:
   - Adjusting sensitivity (multiplying movement values)
   - Inverting axes
   - Mapping buttons differently
   - Adding acceleration/deceleration

Example customization:

```c
void mouse_decode(MouseReportBuffer* mrb, int8_t dx, int8_t dy, int8_t dw, 
                 bool left, bool right, bool middle) {
  // Adjust sensitivity
  dx = dx * 2;  // Double horizontal sensitivity
  dy = dy * 2;  // Double vertical sensitivity
  
  // Invert Y axis if needed
  // dy = -dy;
  
  // Process buttons
  uint8_t buttons = 0;
  if (left) buttons |= MOUSE_BUTTON_LEFT;
  if (right) buttons |= MOUSE_BUTTON_RIGHT;
  if (middle) buttons |= MOUSE_BUTTON_MIDDLE;
  
  // Fill report buffer
  mrb->report.buttons = buttons;
  mrb->report.x = dx;
  mrb->report.y = dy;
  mrb->report.wheel = dw;
  
  // Set report as ready to be sent
  mrb->ready = true;
}
```

### Option 2: USB-to-Serial-Mouse Project

For adapting USB mice to work with older computers that expect serial mice, check out the **usb-to-serial-mouse** project. This is particularly useful for vintage computers that have serial ports but no USB support.

The key differences in this approach:
- You'll need to implement serial protocol conversion
- Common serial mouse protocols include Microsoft, Mouse Systems, and Logitech
- Serial communication typically uses 1200-9600 baud rate

## USB Hub Compatibility

**Important Note on USB Hub Compatibility**:

Most USB hubs are incompatible with TinyUSB. To ensure proper functioning:

- **Avoid USB 3.0 hubs completely** - they will crash TinyUSB
- The Olimex hubs, despite claims, have the same compatibility issues
- Use only pure USB 2.0 hubs without any USB 3.0 functionality
- Recommended working hub: Targus USB 2.0 hub available on Amazon

## Implementation Details

### Key Files

1. **usb-hid.c**: Core implementation of HID protocol handling
2. **mouse_decode.c**: Custom logic for translating mouse inputs
3. **tusb_config.h**: Configuration settings for TinyUSB

### Mouse Report Structure

```c
typedef struct {
  uint8_t buttons; // Bitmap of button states
  int8_t  x;       // X-axis movement
  int8_t  y;       // Y-axis movement
  int8_t  wheel;   // Vertical wheel movement
} MouseReport;

typedef struct {
  MouseReport report;
  bool ready;      // Indicates if report is ready to be sent
} MouseReportBuffer;
```

### Button Bitmasks

```c
#define MOUSE_BUTTON_LEFT    0x01
#define MOUSE_BUTTON_RIGHT   0x02
#define MOUSE_BUTTON_MIDDLE  0x04
#define MOUSE_BUTTON_BACK    0x08
#define MOUSE_BUTTON_FORWARD 0x10
```

## Example Implementations

### Microsoft Serial Mouse Protocol Conversion

```c
void mouse_decode(MouseReportBuffer* mrb, int8_t dx, int8_t dy, int8_t dw, 
                 bool left, bool right, bool middle) {
  // Microsoft Serial Mouse protocol conversion
  uint8_t packet[3];
  
  // First byte: button states and high bits of movement
  packet[0] = 0x40;  // Start bit
  if (left) packet[0] |= 0x20;
  if (right) packet[0] |= 0x10;
  packet[0] |= ((dx & 0xC0) >> 6);  // High X bits
  packet[0] |= ((dy & 0xC0) >> 4);  // High Y bits
  
  // Second byte: X movement low bits
  packet[1] = dx & 0x3F;
  
  // Third byte: Y movement low bits
  packet[2] = dy & 0x3F;
  
  // Send packet over serial
  send_serial_packet(packet, 3);
  
  // Still update report buffer for internal use
  mrb->report.buttons = (left ? MOUSE_BUTTON_LEFT : 0) |
                        (right ? MOUSE_BUTTON_RIGHT : 0) |
                        (middle ? MOUSE_BUTTON_MIDDLE : 0);
  mrb->report.x = dx;
  mrb->report.y = dy;
  mrb->report.wheel = dw;
  mrb->ready = true;
}

## Building and Testing

1. Clone the repository
2. Make your customizations to `mouse_decode.c`
3. Build using:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Flash to your Raspberry Pi Pico:
   ```
   picotool load -x mouse_adapter.uf2
   ```
   
## Debugging Tips

- Use `printf` debugging by enabling USB CDC serial output
- Monitor mouse input values before and after your processing
- Test with different types of mice to ensure compatibility
- If using a USB hub, remember to use only compatible USB 2.0 hubs

## Common Issues

1. **No mouse detection**: Ensure your USB host configuration is correct
2. **Erratic movement**: Check your sensitivity and processing logic
3. **TinyUSB crashes**: Make sure you're using a compatible USB 2.0 hub
4. **Buttons not working**: Verify your button bitmask handling

## Further Resources

- TinyUSB Documentation: [https://github.com/hathach/tinyusb/wiki](https://github.com/hathach/tinyusb/wiki)
- Raspberry Pi Pico SDK: [https://github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- HID Protocol Specification: [https://www.usb.org/hid](https://www.usb.org/hid)
