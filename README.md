# Audio Visualizer

<video src='https://raw.githubusercontent.com/jfcbooth/dasv/main/media/visualizer.mov' width=480></video>

https://raw.githubusercontent.com/jfcbooth/dasv/main/media/visualizer.mov

This project is a physical audio visualizer made from an 8x8 LED array.
It crudely made in high school IT class when the assignment was "make something with computer parts", which I why the LEDs are installed in a motherboard. Specifically a Gigabyte GA-78LMT-USB3, which is the mobo the backing plate is designed for.

The arduino running the LEDs updates them after sending 8 numbers through the serial interface. Ex. `1;2;3;4;5;6;7;8;` or `12345678` would set each column to that height.

The program that converts the music to these numbers is called cava and can be found on github [here](https://github.com/karlstav/cava)

## MCU setup
The visualizer can work on any MCU with serial communications and 16 outputs. I used an Arduino Nano. The row/col connections can be found in the `dasv.ino` arduino file.

For resistor values, they depend on the LEDs you have. Just make sure you use one for each row/col to prevent burning out any LEDs.


## Linux setup
The program runs natively on linux. Install with `sudo apt install cava`, then just run from the project directory `./utils/linux/run_linux.sh` after adjusting the USB tty in both `linux.conf` and `run_linux.sh`.

## Windows setup

This is complex since the audio services on windows are quite different than linux. A workaround is a program called winscap found [here](https://github.com/quantum5/winscap). This exe sends windows audio to a FIFO to read from in WSL. When I had this project finished, only WSL 1 was out and it allowed you to access COM ports through virtualized TTY interfaces. WSL 2 removed that feature. So, revert your WSL install to 1 using `wsl --set-version Ubuntu-18.04 1`, install cava with `sudo apt install cava` and run `./utils/linux/run_windows.sh` after adjusting the USB tty in both `windows.conf` and `run_windows.sh`. This will launch winscap and write to a FIFO, cava will read from that FIFO and be able to write to the appropriate TTY.

## STL files
Files for the backing plate are in `backing_plate/`

## Notes
Each respective test file sends the cava output to STDOUT instead of the USB TTY to make sure your cava installation is picking up audio properly.