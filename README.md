# Room free/busy indicators

Some Arduino code to drive our physical free/busy indicators for rooms.

These currently run on an ESP2866 with some AdaFruit Pixels. Future indicators
may vary in form and function, but for sanity we should try stick to similar
hardware.

## How to get going

You'll need to copy the `settings.example.h` file to `settings.h`, update the
definitions to match what you want to do, and then compile it in the [Arduino
IDE](https://www.arduino.cc/en/main/software) (with appropriate hardware
attached).
