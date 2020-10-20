<!--
Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
Distributed under the terms of the GNU General Public License, version 3
-->

# Serial port data protocol

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![License: CC BY-NC-ND 4.0](https://img.shields.io/badge/License-CC%20BY--NC--ND%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc-nd/4.0/)

![Compile report](https://github.com/dmfrodrigues/feup-rcom-l1/workflows/Compile%20report/badge.svg)
![Test](https://github.com/dmfrodrigues/feup-rcom-l1/workflows/Test/badge.svg)
![Test stats](https://github.com/dmfrodrigues/feup-rcom-l1/workflows/Test%20stats/badge.svg)
![Documentation](https://github.com/dmfrodrigues/feup-rcom-l1/workflows/Documentation/badge.svg)

- **Project name:** Serial port data protocol
- **Short description:** Serial port data protocol and API, for communicating between two computers
- **Environment:** Unix
- **Tools:** C
- **Institution:** [FEUP](https://sigarra.up.pt/feup/en/web_page.Inicial)
- **Course:** [RCOM](https://sigarra.up.pt/feup/en/UCURR_GERAL.FICHA_UC_VIEW?pv_ocorrencia_id=459483) (Computer Networks)
<!-- - **Project grade:** ??.?/20.0 -->
- **Group members:**
    - [Breno Accioly de Barros Pimentel](https://github.com/BrenoAccioly) (<up201800170@fe.up.pt>)
    - [Diogo Miguel Ferreira Rodrigues](https://github.com/dmfrodrigues) (<dmfrodrigues2000@gmail.com> / <diogo.rodrigues@fe.up.pt>)

## Computers

FEUP, room I321, bench 3:
- Computer connected to FEUP network: 192.168.109.129
- Computer in rack: 192.168.109.222

## Compile

To compile, run `make`. Here are some other interesting `make` options:

```sh
make test                       # Run tests
make stats                      # Run statistics
make doc                        # Generate documentation
make doc/report/report.pdf      # Generate report
make clean                      # Cleanup directory
```

## Use

```txt
transmitter COM FILE <-b baudrate> <-d pd> <-h ph> <-r retransmissions> <-s size> <-t timeout> <-T tau> <-v verbosity>
receiver    COM      <-b baudrate> <-d pd> <-h ph> <-r retransmissions> <-s size> <-t timeout> <-T tau> <-v verbosity>

COM                     Communications channel to use (COM1 - ttyS0, COM2 - ttyS1, ...)
FILE                    Path to file to be transferred by the transmitter

-b, --baudrate          Baud rate; bit per second (default is 38400)
-d, --prob-error-data   Data error probability
-h, --prob-error-head   Header error probability
-r, --retransmissions   Retransmissions (default is 3)
-s, --size              Block size, in bytes (default is LL_MAX_SIZE, which is 4096)
-t, --timeout           Timeout, in seconds (default is 3)
-T, --tau               Tau; extra time before analysing, in microseconds (default is 0)
-v, --verbosity         Verbosity (0 to 4)
```

# License

© 2020 Diogo Rodrigues, Breno Pimentel

All files are licensed under [GNU General Public License v3](LICENSE) by **© 2020 Diogo Rodrigues, Breno Pimentel**, to the exception of:
- all files inside directory `doc/guidelines`, which were authored by the lecturers;
- `pinguim.gif`, which was made available by the lecturers;
- all files inside directory `doc/report` and all artifacts deriving from them, which are licensed under the [Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International](doc/report/LICENSE) public license.

The files not authored by me are presented as a fundamental complement to the presented solutions, and are made available under *fair use for education*.
