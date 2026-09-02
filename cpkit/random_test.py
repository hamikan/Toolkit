"""入力生成・愚直解・提出コードによるランダムテストを実行する。"""

from pathlib import Path
import shutil
import subprocess

from .config import CONFIG, ROOT


def create_random_test():
    directory = Path.cwd()
    test_directory = directory / "randomTest"
    test_directory.mkdir(parents=True, exist_ok=True)
    paths = []
    for filename in ("input.cpp", "naive.cpp"):
        source = ROOT / "src" / filename
        destination = test_directory / filename
        if not source.exists():
            raise FileNotFoundError(f"テンプレートが見つかりません: {source}")
        if not destination.exists():
            shutil.copy(source, destination)
        paths.append(destination)
    return tuple(paths)


def _run_program(name, program, input_data, timeout, case=None):
    case_label = "" if case is None else f"case {case}, "
    try:
        process = subprocess.run(
            [str(Path(program).resolve())],
            input=input_data,
            text=True,
            capture_output=True,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired:
        print(f"[TLE] {name} ({case_label}timeout: {timeout}s)")
        return None
    except OSError as error:
        print(f"[ERROR] {name}: {error}")
        return None
    if process.returncode != 0:
        print(
            f"[RE] {name} "
            f"({case_label}return code: {process.returncode})"
        )
        if process.stderr:
            print(process.stderr, end="" if process.stderr.endswith("\n") else "\n")
        return None
    return process.stdout


def _needs_compile(source, executable):
    source = Path(source)
    executable = Path(executable)
    if not source.exists() or not executable.exists():
        return True
    executable_time = executable.stat().st_mtime_ns
    dependencies = (source, CONFIG.root / ".env")
    return any(
        dependency.exists() and executable_time <= dependency.stat().st_mtime_ns
        for dependency in dependencies
    )


def _compile_one(name, source, executable):
    if not _needs_compile(source, executable):
        print(f"[BUILD] {name:<5} : cached")
        return True
    if not CONFIG.cxx:
        print("[ERROR] .envのCXXを設定してください")
        return False
    try:
        result = subprocess.run(
            [CONFIG.cxx, *CONFIG.compile_flags, str(source), "-o", str(executable)],
            capture_output=True,
            text=True,
        )
    except OSError as error:
        print(f"[ERROR] compiler: {error}")
        return False
    if result.returncode == 0:
        print(f"[BUILD] {name:<5} : compiled")
        return True
    print(f"[CE] {Path(source).name}")
    if result.stderr:
        print(result.stderr, end="" if result.stderr.endswith("\n") else "\n")
    return False


def _save_failed_case(directory, input_data, expected):
    test_directory = directory / "test"
    test_directory.mkdir(parents=True, exist_ok=True)
    n = 1
    while (
        (test_directory / f"sample-{n}.in").exists()
        or (test_directory / f"sample-{n}.out").exists()
    ):
        n += 1
    input_path = test_directory / f"sample-{n}.in"
    output_path = test_directory / f"sample-{n}.out"
    input_path.write_text(input_data, encoding="utf-8")
    output_path.write_text(expected, encoding="utf-8")
    print()
    print(f"[SAVED] {input_path.relative_to(directory)}")
    print(f"[SAVED] {output_path.relative_to(directory)}")


def _print_output(name, output):
    print()
    print(f"{name}:")
    print(output, end="" if output.endswith("\n") else "\n")


def random_test(n, timeout=2, save=False):
    if n < 0:
        raise ValueError("n must be non-negative")
    directory = Path.cwd()
    create_random_test()
    test_directory = directory / "randomTest"
    input_source = test_directory / "input.cpp"
    naive_source = test_directory / "naive.cpp"
    main_source = directory / "main.cpp"
    input_program = test_directory / "input"
    naive_program = test_directory / "naive"
    main_program = directory / "a.out"

    compile_targets = (
        ("input", input_source, input_program),
        ("naive", naive_source, naive_program),
        ("main", main_source, main_program),
    )
    for name, source, executable in compile_targets:
        if not _compile_one(name, source, executable):
            return False

    for i in range(n):
        test_input = _run_program("input", input_program, "", timeout)
        if test_input is None:
            return False
        expected = _run_program(
            "naive",
            naive_program,
            test_input,
            timeout,
            i + 1,
        )
        if expected is None:
            _print_output("Input", test_input)
            return False
        actual = _run_program(
            "main",
            main_program,
            test_input,
            timeout,
            i + 1,
        )
        if actual is None:
            _print_output("Input", test_input)
            if save:
                _save_failed_case(directory, test_input, expected)
            return False
        if expected != actual:
            print(f"[WA] case {i + 1}")
            _print_output("Input", test_input)
            _print_output("Expected", expected)
            _print_output("Actual", actual)
            if save:
                _save_failed_case(directory, test_input, expected)
            return False
    print(f"[OK] {n} cases")
    return True


def rt(n=None, timeout=2):
    if n is None:
        create_random_test()
        return
    random_test(n, timeout, False)


def rts(n, timeout=2):
    random_test(n, timeout, True)
