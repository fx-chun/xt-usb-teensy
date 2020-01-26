xt-usb-teensy
===

Teensy 3.2 code for a XT to USB keyboard adapter.  
This code is written for an 83-key keyboard! Further work is needed to make it function properly with higer-keyed boards.  
Build system entirely from [apmorton/teensy-template](https://github.com/apmorton/teensy-template).

Contents
---
- `src/main.c`: main source (pins configurable in source)
- `src/scancodes.h`: XT scancodes
- `src/keymap.h`: mapping between XT scancodes and [Teensy key definitions](https://www.pjrc.com/teensy/usb_keyboard.html) (configurable)

Recommended Reading
---
https://github.com/tmk/tmk_keyboard/wiki/IBM-PC-XT-Keyboard-Protocol
https://www.pjrc.com/teensy/usb_keyboard.html
http://www.seasip.info/VintagePC/ibm_1501105.html

Configurables / Wiring
---
Pins for the clock and data pins used on the Teensy for the XT can be configured inside `src/main.c`. Vcc+5V is attached to VUSB, and GND to Teensy GND.
Scancodes for the XT keyboard can be modified inside `src/scancodes.h` (in case I got one or two wrong).  
Keymappings between the scancodes and the Teensy key definitions can be modified inside `src/keymap.h` (warning: may contain some unorthodox mappings...) 
Build settings (in case you want to build for another Teensy) can be configured per `Makefile` and original `README.md`.

.hex Build Instructions
---
Project will build as long as you have `gcc-arm-embedded` in your path. Tested with version 7.  
Simply issue a `make` in the root directory after configuring configurables and installing the above toolchain.  
A NixOS shell definition is included for the appropriate toolchain; simply use `nix-shell` in the root directory and issue `make`.
Flash .hex using your favorite loader.

Original README:
---
```
Teensy 3.X Project Template
===========================

Purpose
-------

An easy starting point for a Teensy 3.X project which might not fit inside the
arduino build environment.


Reasons to Use
--------------

- You need to modify the teensy core
- You don't love Java IDE's
- You love Make
- Because


Setup
-----

Install the Teensy udev rule: `sudo cp tools/49-teensy.rules /etc/udev/rules.d/`

Then unplug your Teensy and plug it back in.


Using
-----

1. Put your code in `src/main.cpp`
2. Put any libraries you need in `libraries`
3. Set the TEENSY variable in `Makefile` according to your teensy version
4. Build your code ```make```
5. Upload your code ```make upload```


Make Targets
------------

- `make` alias for `make hex`
- `make build` compiles everything and produces a .elf
- `make hex` converts the elf to an intel hex file
- `make post_compile` opens the launcher with the correct file
- `make upload` uploads the hex file to a teensy board
- `make reboot` reboots the teensy


Where everything came from
--------------------------

- The `teensy3` sub-folder is taken from [The Teensy 3 Cores](https://github.com/PaulStoffregen/cores/tree/master/teensy3)
- The `tools` sub-folder is taken from [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
- The `src/main.cpp` file is moved, unmodified from `teensy3/main.cpp`
- The `Makefile` file is moved, modified from `teensy3/Makefile`
- The `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)

Modifications to `Makefile` include
- Add support for arduino libraries
- Change tools directory
- Calculate target name from current directory
- Prettify rule output
- Do not upload by default, only build
```
