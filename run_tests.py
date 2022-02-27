from __future__ import annotations

import os
import subprocess
import sys

BUILD_PATH: str = f'{os.getcwd()}/build/'
BINARY_PATH: str = f'{os.getcwd()}/build/tek'
TARGET: str = 'tek'
EXAMPLES_PATH = f'{os.getcwd()}/tests/'


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


def print_results(
    actual_result: subprocess.CompletedProcess,
    expected_result: str,
) -> None:
    print(f'[NOTE] Got:      {actual_result.stdout}')
    print(f'[NOTE] Expected: {expected_result}')


def run_examples(examples: list[str]) -> None:
    if not os.getcwd().endswith('/build'):
        os.chdir(BUILD_PATH)

    succeeding = 0
    failing = 0
    skipped = 0

    print('[INFO] Running tests')
    for example in examples:
        filename = example.split('/')[-1]
        with open(example) as file:
            lines = file.readlines()
            first_line: str = lines[0]

            if 'ignore' in first_line.lower():
                skipped += 1
                continue

            last_line: str = lines[-1]
            expected_result: str = last_line[
                last_line.find(
                    '\'',
                ) + 1:-2
            ].rstrip().replace(':', '\n')
            result = subprocess.run(['./tek', example], capture_output=True)

            if expected_result == '(fail)':
                try:
                    assert(result.returncode != 0)
                    succeeding += 1
                    print(f'[TEST] {filename}...SUCCESS')
                except AssertionError:
                    failing += 1
                    print(f'[ERROR] Test {filename} FAILED')
                    print(f'[INFO] Test was supposed to fail but exit code was {result.returncode}')  # noqa: E501
                    print_results(result, expected_result)
                    sys.exit(1)
                continue

            try:
                assert(expected_result == result.stdout.decode())
                succeeding += 1
                print(f'[TEST] {filename}...SUCCESS')
            except AssertionError:
                failing += 1
                print(f'[ERROR] Test {filename} FAILED')
                print_results(result, expected_result)
                sys.exit(1)

    print(f'\n[TESTS RECAP] succeeding: {succeeding}, failing: {failing}, skipped: {skipped}\n')  # noqa: E501


def main() -> int:
    build_executable()
    found_examples: list[str] = find_examples()
    run_examples(found_examples)


if __name__ == '__main__':
    raise SystemExit(main())
