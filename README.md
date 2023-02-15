# TapeSorter

### How to build and start?

Config example:
```
$ cat <CONFIG_DIR>/config.cfg
N = 19
M = 65
delay_for_read = 0
delay_for_put = 0
delay_for_shift = 0
path_in = <PATH_TO_INPUT_TAPE>
path_out = <PATH_TO_OUTPUT_TAPE>
'''

Commands:
'''
$ git clone 'https://github.com/maladetska/TapeSorter'
$ cd TapeSorter
$ cmake CMakeLists.txt
$ make
$ ./bin/TapeSorter <CONFIG_DIR>/config.cfg
```
