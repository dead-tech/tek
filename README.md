![Build Status](https://github.com/Dead-tech/dead-lang/actions/workflows/linux.yml/badge.svg)
# Tek

Simple interpreter created following [**Crafting Interpreters**](https://craftinginterpreters.com/contents.html) book.

## Building

- Clone repo `git clone https://github.com/dead-tech/tek.git`
- Change directory `cd tek`
- Create build directory and cd `mkdir build && cd build`
- Use CMake `cmake .. && make`

## Testing

This project utilizes a python script to run all the tests.

The script uses as default cmake target `tek`, as default compilation output directory `build` and as default tests folder `tests`.

You could change these settings via `python3 run_tests.py --build-dir BUILD_DIR || --target TARGET || --tests-dir TESTS_DIR`
- Capture test programs output from stdout once `python3 run_tests.py --capture`
- Run tests `python3 run_tests.py`

For more information checkout `python3 run_tests.py --help`
