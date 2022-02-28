from __future__ import annotations

import os
import subprocess
import sys

BUILD_PATH: str = f'{os.getcwd()}/build/'
BINARY_PATH: str = f'{os.getcwd()}/build/tek'
TARGET: str = 'tek'
TESTS_PATH = f'{os.getcwd()}/tests/'


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def color_red(msg: str) -> str:
    return f'{bcolors.FAIL}{msg}{bcolors.ENDC}'


def color_green(msg: str) -> str:
    return f'{bcolors.OKGREEN}{msg}{bcolors.ENDC}'


def color_header(msg: str) -> str:
    return f'{bcolors.HEADER}{msg}{bcolors.ENDC}'


def check_exit_code(result: subprocess.CompletedProcess, message: str) -> None:
    if result.returncode != 0:
        print(color_red(f'[ERROR] {message}'))
        sys.exit(1)


def build_executable() -> None:
    if not os.getcwd().endswith('/build'):
        os.chdir(BUILD_PATH)

    result = subprocess.run(['cmake', '..'], capture_output=True)
    check_exit_code(result, 'Unable to generate cmake project...')

    result = subprocess.run(['make', 'tek'], capture_output=True)
    check_exit_code(result, f'Unable to build target {TARGET}...')

    print(color_green(f'[BUILD] Built target {TARGET}'))


def find_examples() -> list[str]:
    out: list[str] = []

    for dirpath, _, filenames in os.walk(TESTS_PATH):
        for filename in [f for f in filenames if f.endswith('.tek')]:
            out.append(os.path.join(dirpath, filename))

    return out


def print_results(
    actual_result: subprocess.CompletedProcess,
    expected_result: str,
) -> None:
    print(f'{color_red(f"[NOTE] Got:      {actual_result.stdout}")}')
    print(f'{color_red(f"[NOTE] Expected: {expected_result}")}')


def print_succeeding_test(filename: str) -> None:
    left_column = f'[TEST] {filename}'
    print(color_green(left_column + 'SUCCESS'.rjust(80 - len(left_column), '.')))  # noqa: E501


def print_failing_test(filename: str) -> None:
    left_column = f'[TEST] {filename}'
    print(color_red(left_column + 'FAILED'.rjust(80 - len(left_column), '.')))


def assert_results(filename, assert_expression: bool) -> bool:
    try:
        assert(assert_expression)
        print_succeeding_test(filename)
        return True
    except AssertionError:
        print_failing_test(filename)


def run_examples(examples: list[str]) -> None:
    if not os.getcwd().endswith('/build'):
        os.chdir(BUILD_PATH)

    succeeding = 0
    ignored = 0

    for example in examples:
        filename = example.split('/')[-1]
        with open(example) as file:
            lines = file.readlines()
            first_line: str = lines[0]

            if 'ignore' in first_line.lower():
                ignored += 1
                continue

            last_line: str = lines[-1]
            expected_result: str = last_line[
                last_line.find(
                    '\'',
                ) + 1:-2
            ].rstrip().replace(':', '\n')
            result = subprocess.run(['./tek', example], capture_output=True)

            if expected_result == '(fail)':
                if assert_results(filename, result.returncode != 0):
                    succeeding += 1
                else:
                    print_results(result, expected_result)
                    sys.exit(1)
                continue

            if assert_results(filename, expected_result == result.stdout.decode()):  # noqa: E501
                succeeding += 1
            else:
                print_results(result, expected_result)
                sys.exit(1)

    print(f'\n{color_header("[TESTS RECAP]")} {color_green(f"succeeding: {succeeding}")}, ignored: {ignored}\n')  # noqa: E501


def main() -> int:
    build_executable()
    found_examples: list[str] = find_examples()
    run_examples(found_examples)


if __name__ == '__main__':
    raise SystemExit(main())
