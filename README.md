# Room free/busy indicators

Some Arduino code to drive our physical free/busy indicators for rooms.

These currently run on an ESP2866 with some AdaFruit Pixels. Future indicators
may vary in form and function, but to make life easier we should stick to
similar (if not identical) hardware in any future designs.

## How does it work?

The `setup()` method is the first thing called when a device is powered on
(after variable initialisation). This will attempt to connect to the wifi
network given in `settings.h`.

Once connected the device will switch the lights to their default state using
`update_lights()`, then attempt to retrieve the current status of the room it is
in from the [Room Usage Dashboard](https://github.com/dxw/room-usage-dashboard)
JSON feed for that room (`update_from_server()`). If successful,
`update_from_server()` will update variables ready for the next round of
`update_lights()`.

Once `setup()` has run, `loop()` takes over and runs constantly. This updates
the lights on every iteration (which allows for things like the pulsing
behaviour). It also keeps track of the time since the last update from the
server, and runs this on the specified interval.

## How to get going

You'll need to copy the `settings.example.h` file to `settings.h`, update the
definitions to match what you want to do, and then compile it in the [Arduino
IDE](https://www.arduino.cc/en/main/software) (with appropriate hardware
attached).
