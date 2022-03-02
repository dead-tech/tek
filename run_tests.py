from __future__ import annotations

import argparse
import os
import subprocess
import sys
from contextlib import contextmanager
from pathlib import Path


@contextmanager
def set_directory(path: Path):
    origin = Path().absolute()

    if origin == path:
        return

    try:
        os.chdir(path)
        yield
    finally:
        os.chdir(origin)


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


def build_executable(build_dir: str, target: str, verbose: bool) -> str:
    with set_directory(build_dir):
        result = subprocess.run(['cmake', '..'], capture_output=True)
        check_exit_code(result, 'Unable to generate cmake project...')

        result = subprocess.run(['make', target], capture_output=True)
        check_exit_code(result, f'Unable to build target {target}...')

        if verbose:
            print(color_green(f'[BUILD] Built target {target}'))

        return os.path.abspath('tek')


def find_tests(tests_dir: str) -> list[str]:
    out: list[str] = []

    for dirpath, _, filenames in os.walk(tests_dir):
        for filename in [f for f in filenames if f.endswith('.tek')]:
            out.append(os.path.join(dirpath, filename))

    return out


def print_results(
    actual_result: subprocess.CompletedProcess,
    expected_result: str,
) -> None:
    print(f'{color_red(f"[NOTE] Got:      {actual_result.stdout}")}')
    print(f'{color_red(f"[NOTE] Expected: {expected_result.encode()}")}')


def print_ignored_test(filename: str) -> None:
    left_column = f'[TEST] {filename}'
    print(color_header(left_column + 'IGNORED'.rjust(80 - len(left_column), '.')))  # noqa: E501


def print_succeeding_test(filename: str) -> None:
    left_column = f'[TEST] {filename}'
    print(color_green(left_column + 'SUCCESS'.rjust(80 - len(left_column), '.')))  # noqa: E501


def print_failing_test(filename: str) -> None:
    left_column = f'[TEST] {Path(filename).absolute().resolve()}'
    print(color_red(left_column + 'FAILED'.rjust(80 - len(left_column), '.')))


def assert_results(filename, assert_expression: bool, verbose: bool) -> bool:
    try:
        assert(assert_expression)
        if verbose:
            print_succeeding_test(filename)
        return True
    except AssertionError:
        print_failing_test(filename)


def run_tests(executable: str, tests: list[str], verbose: bool) -> None:
    succeeding = 0
    ignoring = 0

    for test in tests:
        filename = test.split('/')[-1]
        with open(test.replace('.tek', '.txt')) as file:
            lines = file.readlines()
            first_line: str = lines[0]

            try:
                lines[1]
                ignoring += 1
                continue
            except IndexError:
                pass

            expected_result: str = first_line[3:].replace(':', '\n')
            result = subprocess.run([executable, test], capture_output=True)

            if expected_result == 'fail':
                if assert_results(filename, result.returncode != 0, verbose):
                    succeeding += 1
                else:
                    print_results(result, expected_result)
                    sys.exit(1)
                continue

            if assert_results(
                filename,
                expected_result == result.stdout.decode(),
                verbose,
            ):
                succeeding += 1
            else:
                print_results(result, expected_result)
                sys.exit(1)

    succeeding_str = color_green(f'succeeding: {succeeding}')
    ignoring_str = color_header(f'ignoring: {ignoring}')
    print(f'\n{color_header("[TESTS RECAP]")} {succeeding_str}, {ignoring_str}')  # noqa: E501


def capture_tests_output(
    executable: str,
    tests: list[str],
    verbose: bool,
) -> None:
    for test in tests:
        result = subprocess.run([executable, test], capture_output=True)
        expected_result = result.stdout.decode().replace('\n', ':')
        test_out_file = test.replace('.tek', '.txt')
        with open(test_out_file, 'w') as file:
            if result.returncode == 0:
                file.write(f'e: {expected_result}')
            else:
                file.write('e: fail')

        if verbose:
            printable_test = test.split('/')[-1]
            printable_test_out_file = test_out_file.split('/')[-1]
            print(
                color_header(
                    f'[INFO] Ran {printable_test:<40} {"saved stdout ->":<14} {printable_test_out_file:<40}',  # noqa: E501
                ),
            )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--build-dir',
        help='path to build directory',
        default='./build/',
        type=str,
    )
    parser.add_argument(
        '--target',
        help='target to build',
        default='tek',
        type=str,
    )
    parser.add_argument(
        '--tests-dir',
        help='path to tests directory',
        default='./tests/',
        type=str,
    )
    parser.add_argument(
        '--capture',
        help='captures stdout and saves it as expected result for tests',
        action='store_true',
    )
    parser.add_argument(
        '--verbose',
        help='don\'t show succeeding and ignored tests',
        action='store_true',
    )
    args = parser.parse_args()

    executable = build_executable(args.build_dir, args.target, args.verbose)
    tests = find_tests(args.tests_dir)

    print(os.getcwd())
    print(os.listdir())
    if args.capture:
        capture_tests_output(executable, tests, args.verbose)
    else:
        run_tests(executable, tests, args.verbose)


if __name__ == '__main__':
    raise SystemExit(main())
