# Statistics

This small module performs statistics on the rest of this repository's code. It allows one to run the transmitter and receiver with given parameters and collect data on their performance, so as to evaluate it as a function of the settings used in transmission.

This module is particularly important because it allows anyone to group a set of runs and run them in sequence (which would obviously be a laborious task should it be run by hand, one by one); besides, the runs are loaded from files so recompilation is not necessary, and several input files can be written to suit different needs (e.g., test the influence of frame size in a file, in another oe test the influence of baud-rate, etc.)

## Compile

To compile, run `make`. Here are some other interesting `make` options:

```sh
make test       # Test statistics
make clean      # Cleanup stats directory
```

## Use

```txt
stats_transmitter FILE COM
stats_receiver    FILE COM

FILE        Input file, listing the arguments to run the transmitter/receiver
COM         Communications channel (COM1 - ttyS0, COM2 - ttyS1, ...)
```

To use these programs, first start running `stats_receiver` and once it has started run `stats_transmitter`. Corresponding `stats_receiver` and `stats_transmitter` programs should use the same `FILE` so they use the same serial port settings.

## Input

An input file is composed of several lines, where each line is either empty (ignored) or is a runline, with the following appearance:

```txt
../testfiles/pinguim.gif 115200 1e-5 2e-5 3 4096 5000000 10000 1
```

The fields correspond to command-line arguments of the transmitter and receiver. The fields are the following, by the order they appear in a runline:

| Argument | Example                    | Meaning                                                                 |
|----------|----------------------------|-------------------------------------------------------------------------|
| `<FILE>` | `../testfiles/pinguim.gif` | File to be transferred                                                  |
| `-b`     | `115200`                   | Baud rate                                                               |
| `-d`     | `1e-5`                     | Probability of data bit randomly swapping                               |
| `-h`     | `2e-5`                     | Probability of header bit randomly swapping                             |
| `-r`     | `3`                        | Number of retransmissions                                               |
| `-s`     | `4096`                     | Maximum frame size                                                      |
| `-t`     | `5000000`                  | Timeout, in microseconds                                                |
| `-T`     | `10000`                    | Artificial propagation delay, in microseconds                           |
| `-v`     | `1`                        | Verbosity; not relevant, as the programs are not compiled in debug mode |

See also the [main README](../README.md) for more information on the arguments.

## Output

### Transmitter

`stats_transmitter` prints to `stderr` a human-readable data table, including progress so the user can easily assess how much time is left until the task is finished.

`stats_transmitter` also prints to `stdout` the data it collected. It is printed in CSV format so a user can easily redirect `stdout` to a fresh CSV file for later analysis.

### Receiver

`stats_receiver` only prints to `stderr`, and similarly to `stats_transmitter` it is a human-readable data table including progress.
