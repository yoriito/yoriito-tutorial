# pocket-pal

A pocket-sized PAL implementation.

# setup 

```bash
# load vcan module
$ sudo modprobe vcan

# create virtual can device(vcan0) as network device
$ sudo ip link add dev vcan0 type vcan

# set `UP` status
$ sudo ip link set up vcan0
```

or 

```bash
$ bash script/setup_vcan.sh
```

# run pocket-pal

```bash
$ cd <this dir>/pocket-pa-cc
$ cmake -S . -B build
$ cmake --build build -j
$ ./build/pocket_pal
```

# send can frame to pocket-pal

```bash
# send specific can frame
$ cansend vcan0 123#1122334455667788
# send random can frame
$ cangen vcan0
```

# monitor can frame send by pocket-pal

```bash
$ candump vcan0
```

# send vss signal to pocket-pal

use tui client for kura

# monitor vss signal send by pocket-pal

use tui client for kura
