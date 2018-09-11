# Conway's Game of Life #

This is a fast implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) on Linux. When available, e.g. when executing the program in a non-graphical mode (Ctrl-Alt-F1), the full framebuffer is used so that every pixel is a cell. When the framebuffer is not available, the present terminal window is used, in which every character is a cell. In both cases we have periodic boundaries.

Note that you may have to be a superuser to have permissions to use the framebuffer.

Finally, the text-based output will look better or worse depending on the terminal emulator you are using. The fastest and best looking one that I tried was `rxvt-unicode` (`urxvt`).

### Usage ###

Compile the executable by invoking `make`. This will generate the program `life`. When run with a single argument `?`, usage information is displayed, namely 

    Usage: ./life <generations> <delay (ms)> <init fraction> <seed> <prefer_txt>

* <generations> (default 10) is the number of generations you want to display
* <delay> (default 200 ms) is the time in milliseconds that you want to wait between generations. You can make this as short as you want, but when it is too short to finish drawing one generation before refreshing it, it won't look very smooth.
* <init fraction> is a number between 0 and 1 with the proportion of live cells
* <seed> is an integer that seeds the random generator
* set <prefer_txt> to 1 if you want to have text output, even if the framebuffer is available.

When executed, it will display the specified number of generations, and return to the command line (note that the framebuffer contents will not be erased before it is explicitly overwritten). Ctrl-C to exit prematurely.

If you want timing information, compile `lifetime` using `make lifetime`. This can be executed like the other, but also as

    ./lifetime <generations> <rows> <cols>

in which case it will not display anything, only compute the specified number of generations on a world of the specified size, and output how long each update took in microseconds.

### Contact ###

doetoe@protonmail.com
