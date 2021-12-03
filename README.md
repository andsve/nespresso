# nespresso
Base for  implementing a very basic NES emulator. Done for an internal presentation at King, in Dec 2021. Slides can be found here: https://slides.com/andsve/writing-a-nes-emulator

The goal of this project was to share knowledge and increase interest in emulation. It should act as a starting point when looking into creating your own NES emulator.

(There is also branches corresponding to different stages in the presentation linked above.)

## How to build;
> $ ./setup.sh # will download genie binaries and start a new shell with some ENV vars

> $ ./generate.sh # will call genie to generate ninja make files inside build folder

> $ cd build

> $ make

## Running
> $ ../bin/nespresso <rom_file>

Only supports very basic games, such as Donkey Kong currently. Doesn't have gamepad input at the moment, or audio, and the rendering is very basic.

You can pass along `nestest.nes` inside the `data` folder to see that it passes the nestest log validation.
