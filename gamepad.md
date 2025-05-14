# Gamepad Integration Guide

## Overview

Unlike keyboards and mice that use standardized HID protocols, gamepads typically utilize custom HID reports that vary between manufacturers and models. This guide explains how to integrate and customize various gamepads in your USB HID implementation.

## Required Files

- **gamepad.h**: Contains button definitions and constants
- **usb_hid.c**: Main implementation for USB HID handling

## Gamepad Button Definitions

The following standard button definitions are available in `gamepad.h`:

```c
#define GAMEPAD_A          0x80
#define GAMEPAD_B          0x40
#define GAMEPAD_X          0x20
#define GAMEPAD_Y          0x10
#define GAMEPAD_SELECT     0x08
#define GAMEPAD_START      0x04
#define GAMEPAD_LEFT       0x02
#define GAMEPAD_RIGHT      0x01
```

## Integration Process

Adding support for a new gamepad requires three main steps:

1. **Device recognition** in `tuh_hid_mount_cb()`
2. **Report decoding** in `tuh_hid_report_received_cb()`
3. **Disconnection handling** in `tuh_hid_umount_cb()`

### 1. Device Recognition

In the `tuh_hid_mount_cb()` function, add a new condition to identify your gamepad using its Vendor ID (VID) and Product ID (PID):

```c
if (itf_protocol == HID_ITF_PROTOCOL_NONE) {
    // Check for your gamepad's VID and PID
    if ((vid == 0xYOUR_VID) && (pid == 0xYOUR_PID)) {
        your_gamepad = true;
        if (hid_debug)
            printf("your gamepad %0.4x %0.4x connected\n", vid, pid);
        tuh_hid_receive_report(dev_addr, instance);
    }
}
```

### 2. Report Decoding

In the `tuh_hid_report_received_cb()` function, add a new condition to decode your gamepad's reports:

```c
else if (your_gamepad) {
    static uint8_t last_your_gamepad_report[REPORT_SIZE] = { 0 };
    
    // Only process if report has changed
    if (memcmp(report, last_your_gamepad_report, len) != 0) {
        // Decode joystick position
        uint16_t joystate = ...;  // Extract from report
        uint8_t buttons = 0;
        uint8_t joystick = 0;
        
        // Map joystick states to directions
        switch(joystate) {
            case CENTERED_VALUE: joystick = 0; break;
            case UP_VALUE:       joystick = 1; break;
            // ... additional directions
        }
        
        // Map button presses
        buttons |= ((report[x] & MASK) == MASK) ? GAMEPAD_A : 0;
        // ... additional buttons
        
        // Process the decoded data
        process_your_gamepad(joystick, buttons);
        
        // Save the report for comparison
        memcpy(&last_your_gamepad_report, report, sizeof(last_your_gamepad_report));
    }
}
```

### 3. Disconnection Handling

In the `tuh_hid_umount_cb()` function, add a new condition to handle your gamepad's disconnection:

```c
if ((itf_protocol == HID_ITF_PROTOCOL_NONE) && 
    ((vid == 0xYOUR_VID) && (pid == 0xYOUR_PID))) {
    your_gamepad = false;
    if (hid_debug)
        printf("your gamepad %0.4x %0.4x disconnected\n", vid, pid);
}
```

## Processing Function

Implement a processing function in your main application to handle the decoded gamepad data:

```c
void process_your_gamepad(uint8_t joystick, uint8_t buttons) {
    // Process joystick direction (0-8)
    // Process button presses
    
    // Example: Convert to Atari joystick signals
    // ...
}
```

## Report Format Analysis

When adding support for a new gamepad, you'll need to analyze its HID report format. Use the following approaches:

1. **Enable debug output** to view raw reports:
   ```c
   if (hid_debug) {
       dump(report, len);
       printf("\n");
   }
   ```

2. **Observe report changes** when pressing different buttons and moving the joystick.

3. **Common report formats**:
   - Joystick position often uses 2 bytes for X/Y axes
   - Buttons are typically individual bits in specific bytes
   - Some gamepads include pressure-sensitive data

## Example: Nintendo Gamepad Integration

```c
// Device recognition
if ((vid == 0x081f) && (pid == 0xe401)) {
    nintendo_gamepad = true;
    if (hid_debug)
        printf("nintendo gamepad %0.4x %0.4x connected\n", vid, pid);
    tuh_hid_receive_report(dev_addr, instance);
}

// Report decoding
if (nintendo_gamepad) {
    if (memcmp(report, last_nintendo_gamepad_report, len) != 0) {
        joystate = report[0] << 8 | report[1];
        
        // Map joystick states
        switch(joystate) {
            case 0x7F7F: joystick = 0; break;  // Centered
            case 0x7F00: joystick = 1; break;  // Up
            // ... other directions
        }
        
        // Map buttons
        buttons |= ((report[5] & 0x80) == 0x80) ? GAMEPAD_Y : 0;
        buttons |= ((report[5] & 0x40) == 0x40) ? GAMEPAD_B : 0;
        // ... other buttons
        
        process_nintendo_gamepad(joystick, buttons);
        memcpy(&last_nintendo_gamepad_report, report, sizeof(last_nintendo_gamepad_report));
    }
}
```

## Joystick Direction Mapping

Standard 8-way joystick direction mapping:

| Direction | Value | Common joystate pattern |
|-----------|-------|------------------------|
| Center    | 0     | 0x7F7F                 |
| Up        | 1     | 0x7F00                 |
| Up-Right  | 2     | 0xFF00                 |
| Right     | 3     | 0xFF7F                 |
| Down-Right| 4     | 0xFFFF                 |
| Down      | 5     | 0x7FFF                 |
| Down-Left | 6     | 0x00FF                 |
| Left      | 7     | 0x007F                 |
| Up-Left   | 8     | 0x0000                 |

## Adapter Examples

You can create adapters for various classic systems:

### Atari-style Joystick Adapter

```c
void process_gamepad_to_atari(uint8_t joystick, uint8_t buttons) {
    // Atari joystick uses active-low signals
    uint8_t atari_port = 0xFF;
    
    // Map joystick directions
    switch(joystick) {
        case 1: atari_port &= ~0x01; break;  // Up
        case 3: atari_port &= ~0x02; break;  // Right
        case 5: atari_port &= ~0x04; break;  // Down
        case 7: atari_port &= ~0x08; break;  // Left
        // Handle diagonals by combining signals
        case 2: atari_port &= ~(0x01|0x02); break;  // Up-Right
        case 4: atari_port &= ~(0x02|0x04); break;  // Down-Right
        case 6: atari_port &= ~(0x04|0x08); break;  // Down-Left
        case 8: atari_port &= ~(0x08|0x01); break;  // Up-Left
    }
    
    // Map fire button (typically A or B)
    if (buttons & (GAMEPAD_A | GAMEPAD_B))
        atari_port &= ~0x10;  // Fire button
    
    // Output to Atari port
    write_atari_port(atari_port);
}
```

## Troubleshooting

1. **Unrecognized Gamepad**: Verify VID/PID values and ensure `itf_protocol == HID_ITF_PROTOCOL_NONE`

2. **Incorrect Button Mapping**: Use debug output to analyze the report format

3. **Inconsistent Behavior**: Check for report format changes between connection sessions

4. **Performance Issues**: Use the `HOTSPOT` attribute for performance-critical functions
