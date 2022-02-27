from __future__ import annotations

import os
import subprocess
import sys

BUILD_PATH: str = f'{os.getcwd()}/build/'
BINARY_PATH: str = f'{os.getcwd()}/build/tek'
TARGET: str = 'tek'
EXAMPLES_PATH = f'{os.getcwd()}/tests/examples/'


def check_exit_code(result: subprocess.CompletedProcess, message: str) -> None:
    if result.returncode != 0:
        print(f'[ERROR] {message}')
        sys.exit(1)


def build_executable() -> None:
    if not os.getcwd().endswith('/build'):
        os.chdir(BUILD_PATH)
    print(f'[INFO] Generating cmake project in {BUILD_PATH}...')
    result = subprocess.run(['cmake', '..'])
    check_exit_code(result, 'Unable to generate cmake project...')
    print(f'[INFO] Building target {TARGET} in {BUILD_PATH}...')
    result = subprocess.run(['make', 'tek'])
    check_exit_code(result, f'Unable to build target {TARGET}...')
    print(f'[INFO] Built target {TARGET}...')


def find_examples() -> list[str]:
    out: list[str] = []
    print(f'[INFO] Searching examples to run in {EXAMPLES_PATH}')
    for dirpath, _, filenames in os.walk(EXAMPLES_PATH):
        for filename in [f for f in filenames if f.endswith('.tek')]:
            print(f'Found {filename}')
            out.append(os.path.join(dirpath, filename))

    return out


def run_examples(examples: list[str]) -> None:
    if not os.getcwd().endswith('/build'):
        os.chdir(BUILD_PATH)

    print('[INFO] Running tests')
    for example in examples:
        filename = example.split('/')[-1]
        with open(example) as file:
            last_line: str = file.readlines()[-1]
            expected_result: str = last_line.rsplit('e', 1)[-1].strip()
            result = subprocess.run(['./tek', example], capture_output=True)

            if expected_result == 'fail':
                try:
                    assert(result.returncode != 0)
                    print(f'[TEST] {filename}...SUCCESS')
                except AssertionError:
                    print(f'[ERROR] Test {filename} FAILED')
                    print(f'[INFO] Test was supposed to fail but exit code was {result.returncode}')  # noqa: E501
                continue

            try:
                assert(expected_result == result.stdout.decode().strip())
                print(f'[TEST] {filename}...SUCCESS')
            except AssertionError:
                print(f'[ERROR] Test {filename} FAILED')
                print(f'[NOTE] Got: {result.stdout.decode().strip().encode()}')
                print(f'[NOTE] Expected: {expected_result.encode()}')
            check_exit_code(result, f'{example} had non zero exit code')


def main() -> int:
    build_executable()
    found_examples: list[str] = find_examples()
    run_examples(found_examples)


if __name__ == '__main__':
    raise SystemExit(main())
