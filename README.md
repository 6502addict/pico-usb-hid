# USB to Microsoft Serial Mouse Converter

## Overview

This project converts modern USB mice to the legacy Microsoft Serial Mouse protocol using a Raspberry Pi Pico microcontroller. It allows you to use contemporary USB mice with vintage computers or systems that only have serial mouse ports.

## Features

- Accepts standard USB mice as input
- Outputs Microsoft Serial Mouse protocol
- Configurable serial port pins
- Support for multiple button mice
- Buffer system to handle timing differences
- Debugging capabilities

## Hardware Requirements

- Raspberry Pi Pico
- USB OTG adapter (to connect USB mice to the Pico)
- Serial level converter (if connecting to vintage systems requiring true RS-232 levels)
- USB power supply

## Software Requirements

- Raspberry Pi Pico SDK
- CMake (3.13 or newer)
- Make
- Arm GCC toolchain

## Building the Project

### Setting Up the Environment

Before building, ensure you have the Raspberry Pi Pico SDK installed and properly configured:

```bash
# Set the PICO_SDK_PATH environment variable
export PICO_SDK_PATH=/path/to/pico-sdk
```

### Building

1. Clone this repository
2. Navigate to the project directory
3. Build the project:

```bash
# Generate build files
cmake .

# Compile the project
make
```

After successful compilation, you'll find `pico-usb-hid.uf2` in the build directory. This file can be flashed to your Raspberry Pi Pico.

## Flashing to the Pico

1. Hold the BOOTSEL button on the Pico while connecting it to your computer
2. The Pico will appear as a mass storage device
3. Copy the `pico-usb-hid.uf2` file to the Pico
4. The Pico will automatically reboot and run the converter

## Configuration

### Serial Port Configuration

By default, the converter uses UART0 (GP0/GP1) for serial communication. If you need to change the output pins, please refer to the documentation in the project files.

### Mouse Protocol Customization

For details on customizing the mouse protocol conversion, refer to [mouse.md](mouse.md).

## Operation

Once running:

1. Connect a USB mouse to the Pico's USB port (via an OTG adapter)
2. The onboard LED will blink to indicate successful initialization
3. Connect the serial output to your target system
4. The converter will translate USB mouse movements and button presses to Microsoft Serial Mouse protocol

### Debugging

The system includes debugging capabilities that can be enabled. For information on debugging options, please refer to the project documentation.

## Code Structure

The project consists of several key components:

- **main.c**: System initialization and main loop
- **usb_hid.c**: USB HID detection and parsing
- **mouse_decode.c**: Conversion of USB mouse events to serial protocol
- **mouse_ringbuffer.c**: Buffer implementation for mouse events
- **kbd_*.c**: Keyboard support files (if using keyboard for debugging)

## Pin Configuration

| Pin Function   | Default Pin | Description               |
|----------------|-------------|---------------------------|
| UART TX        | GP0         | Serial data output        |
| UART RX        | GP1         | Serial data input (unused)|
| Status LED     | Pico LED    | Status indication         |
| USB Host       | USB port    | Connect mice via OTG      |

## Known Issues and Limitations

For information about known issues and limitations, please refer to the project documentation.

## Troubleshooting

For troubleshooting information, please refer to the project documentation.

## Extending the Project

For information on extending the project to support additional devices:

- For keyboard support, see [keyboard.md](keyboard.md)
- For gamepad support, see [gamepad.md](gamepad.md)
- For mouse support, see [mouse.md](mouse.md)

## License

[Include license information here]

## Acknowledgments

This project builds on the Raspberry Pi Pico SDK and TinyUSB library.
