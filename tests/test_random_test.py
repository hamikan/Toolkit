import importlib
import os
import shutil
import types

import pytest

from cpkit.config import Config


@pytest.fixture
def random_test_module():
    return importlib.import_module("cpkit.random_test")


def test_create_random_test_does_not_overwrite(
    monkeypatch,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    input_path, naive_path = random_test_module.create_random_test()
    input_path.write_text("keep input\n", encoding="utf-8")
    naive_path.write_text("keep naive\n", encoding="utf-8")

    random_test_module.create_random_test()

    assert input_path.read_text(encoding="utf-8") == "keep input\n"
    assert naive_path.read_text(encoding="utf-8") == "keep naive\n"


def test_create_random_test_reports_missing_template(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    mocker.patch.object(random_test_module, "ROOT", tmp_path)
    monkeypatch.chdir(tmp_path)

    with pytest.raises(FileNotFoundError, match="テンプレートが見つかりません"):
        random_test_module.create_random_test()


def test_rt_returns_none_and_preserves_existing_templates(
    monkeypatch,
    random_test_module,
    tmp_path,
):
    test_directory = tmp_path / "randomTest"
    test_directory.mkdir()
    input_path = test_directory / "input.cpp"
    naive_path = test_directory / "naive.cpp"
    input_path.write_text("input\n", encoding="utf-8")
    naive_path.write_text("naive\n", encoding="utf-8")
    monkeypatch.chdir(tmp_path)

    assert random_test_module.rt() is None

    assert input_path.read_text(encoding="utf-8") == "input\n"
    assert naive_path.read_text(encoding="utf-8") == "naive\n"


def test_random_test_creates_missing_templates(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "_compile_one", return_value=False)

    assert not random_test_module.random_test(1)

    assert (tmp_path / "randomTest" / "input.cpp").exists()
    assert (tmp_path / "randomTest" / "naive.cpp").exists()


def test_failed_case_is_saved_with_mex(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    test_directory = tmp_path / "test"
    test_directory.mkdir()
    (test_directory / "sample-1.in").write_text("old\n", encoding="utf-8")
    (test_directory / "sample-1.out").write_text("old\n", encoding="utf-8")
    (test_directory / "sample-3.in").write_text("old\n", encoding="utf-8")
    (test_directory / "sample-3.out").write_text("old\n", encoding="utf-8")
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)
    mocker.patch.object(
        random_test_module,
        "_run_program",
        side_effect=["5\n", "10\n", "11\n"],
    )

    assert not random_test_module.random_test(1, save=True)

    assert (test_directory / "sample-2.in").read_text(encoding="utf-8") == "5\n"
    assert (test_directory / "sample-2.out").read_text(encoding="utf-8") == "10\n"


def test_main_runtime_failure_is_saved(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)
    mocker.patch.object(
        random_test_module,
        "_run_program",
        side_effect=["5\n", "10\n", None],
    )

    assert not random_test_module.random_test(1, save=True)

    assert (tmp_path / "test" / "sample-1.in").read_text(encoding="utf-8") == "5\n"
    assert (tmp_path / "test" / "sample-1.out").read_text(encoding="utf-8") == "10\n"


def test_wa_output_is_labeled_and_saved_with_relative_paths(
    capsys,
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)
    mocker.patch.object(
        random_test_module,
        "_run_program",
        side_effect=["5\n", "10\n", "11\n"],
    )

    assert not random_test_module.random_test(1, save=True)

    assert capsys.readouterr().out == (
        "[WA] case 1\n\n"
        "Input:\n5\n\n"
        "Expected:\n10\n\n"
        "Actual:\n11\n\n"
        "[SAVED] test/sample-1.in\n"
        "[SAVED] test/sample-1.out\n"
    )


def test_timeout_output_identifies_program(
    capsys,
    mocker,
    random_test_module,
):
    timeout = random_test_module.subprocess.TimeoutExpired("main", 2)
    mocker.patch.object(
        random_test_module.subprocess,
        "run",
        side_effect=timeout,
    )

    assert random_test_module._run_program("main", "a.out", "", 2) is None
    assert capsys.readouterr().out == "[TLE] main (timeout: 2s)\n"


def test_runtime_error_output_identifies_program(
    capsys,
    mocker,
    random_test_module,
):
    result = types.SimpleNamespace(
        returncode=-6,
        stderr="Assertion failed: file main.cpp, line 6.\n",
        stdout="",
    )
    mocker.patch.object(random_test_module.subprocess, "run", return_value=result)

    assert random_test_module._run_program("main", "a.out", "", 2) is None
    assert capsys.readouterr().out == (
        "[RE] main (return code: -6)\n"
        "Assertion failed: file main.cpp, line 6.\n"
    )


def test_run_program_reports_start_failure(capsys, mocker, random_test_module):
    mocker.patch.object(
        random_test_module.subprocess,
        "run",
        side_effect=OSError("not executable"),
    )

    assert random_test_module._run_program("main", "a.out", "", 2) is None
    assert capsys.readouterr().out == "[ERROR] main: not executable\n"


@pytest.mark.parametrize(
    ("program", "error"),
    [
        ("naive", "RE"),
        ("naive", "TLE"),
        ("main", "RE"),
        ("main", "TLE"),
    ],
)
def test_naive_and_main_runtime_failures_show_case_and_input(
    capsys,
    error,
    monkeypatch,
    mocker,
    program,
    random_test_module,
    tmp_path,
):
    success_input = types.SimpleNamespace(returncode=0, stderr="", stdout="5\n")
    success_naive = types.SimpleNamespace(returncode=0, stderr="", stdout="10\n")
    runtime_error = types.SimpleNamespace(
        returncode=-6,
        stderr="runtime error\n",
        stdout="",
    )
    failure = (
        runtime_error
        if error == "RE"
        else random_test_module.subprocess.TimeoutExpired(program, 2)
    )
    results = [success_input, failure]
    if program == "main":
        results.insert(1, success_naive)
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)
    mocker.patch.object(
        random_test_module.subprocess,
        "run",
        side_effect=results,
    )

    assert not random_test_module.random_test(1, save=True)

    output = capsys.readouterr().out
    assert f"[{error}] {program} (case 1," in output
    assert "\n\nInput:\n5\n" in output
    if program == "naive":
        assert not (tmp_path / "test").exists()
    else:
        assert (tmp_path / "test" / "sample-1.in").read_text(
            encoding="utf-8"
        ) == "5\n"
        assert (tmp_path / "test" / "sample-1.out").read_text(
            encoding="utf-8"
        ) == "10\n"


def test_unchanged_source_is_not_recompiled(
    capsys,
    mocker,
    random_test_module,
    tmp_path,
):
    source = tmp_path / "main.cpp"
    executable = tmp_path / "a.out"
    env = tmp_path / ".env"
    source.write_text("int main() {}\n", encoding="utf-8")
    executable.write_text("binary\n", encoding="utf-8")
    env.write_text("CXX=g++\n", encoding="utf-8")
    os.utime(source, ns=(1_000_000_000, 1_000_000_000))
    os.utime(env, ns=(1_000_000_000, 1_000_000_000))
    os.utime(executable, ns=(2_000_000_000, 2_000_000_000))
    run = mocker.patch.object(random_test_module.subprocess, "run")
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(tmp_path, "g++", None, None),
    )

    assert random_test_module._compile_one("main", source, executable)

    run.assert_not_called()
    assert capsys.readouterr().out == "[BUILD] main  : cached\n"


@pytest.mark.parametrize("newer", ["source", "env"])
def test_newer_source_or_env_is_recompiled(
    mocker,
    newer,
    random_test_module,
    tmp_path,
):
    source = tmp_path / "main.cpp"
    executable = tmp_path / "a.out"
    env = tmp_path / ".env"
    source.write_text("int main() {}\n", encoding="utf-8")
    executable.write_text("binary\n", encoding="utf-8")
    env.write_text("CXX=g++\n", encoding="utf-8")
    os.utime(source, ns=(1_000_000_000, 1_000_000_000))
    os.utime(env, ns=(1_000_000_000, 1_000_000_000))
    os.utime(executable, ns=(2_000_000_000, 2_000_000_000))
    path = source if newer == "source" else env
    os.utime(path, ns=(3_000_000_000, 3_000_000_000))
    run = mocker.patch.object(
        random_test_module.subprocess,
        "run",
        return_value=types.SimpleNamespace(returncode=0, stderr=""),
    )
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(tmp_path, "g++", None, None),
    )

    assert random_test_module._compile_one("main", source, executable)

    run.assert_called_once()


def test_compile_error_is_labeled(
    capsys,
    mocker,
    random_test_module,
    tmp_path,
):
    source = tmp_path / "main.cpp"
    executable = tmp_path / "a.out"
    source.write_text("invalid\n", encoding="utf-8")
    result = types.SimpleNamespace(returncode=1, stderr="syntax error\n")
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(tmp_path, "g++", None, None),
    )
    mocker.patch.object(
        random_test_module.subprocess,
        "run",
        return_value=result,
    )

    assert not random_test_module._compile_one("main", source, executable)

    assert capsys.readouterr().out == "[CE] main.cpp\nsyntax error\n"


def test_compile_reports_missing_compiler(
    capsys,
    mocker,
    random_test_module,
    tmp_path,
):
    source = tmp_path / "main.cpp"
    executable = tmp_path / "a.out"
    source.write_text("int main() {}\n", encoding="utf-8")
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(tmp_path, "", None, None),
    )

    assert not random_test_module._compile_one("main", source, executable)
    assert capsys.readouterr().out == "[ERROR] .envのCXXを設定してください\n"


def test_compile_reports_compiler_start_failure(
    capsys,
    mocker,
    random_test_module,
    tmp_path,
):
    source = tmp_path / "main.cpp"
    executable = tmp_path / "a.out"
    source.write_text("int main() {}\n", encoding="utf-8")
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(tmp_path, "missing-g++", None, None),
    )
    mocker.patch.object(
        random_test_module.subprocess,
        "run",
        side_effect=OSError("compiler not found"),
    )

    assert not random_test_module._compile_one("main", source, executable)
    assert capsys.readouterr().out == "[ERROR] compiler: compiler not found\n"


def test_compile_stops_at_first_failure(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    compile_one = mocker.patch.object(
        random_test_module,
        "_compile_one",
        return_value=False,
    )
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "create_random_test")

    assert not random_test_module.random_test(1)

    compile_one.assert_called_once_with(
        "input",
        tmp_path / "randomTest" / "input.cpp",
        tmp_path / "randomTest" / "input",
    )


def test_input_runtime_failure_stops_before_solutions(
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "create_random_test")
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)
    run_program = mocker.patch.object(
        random_test_module,
        "_run_program",
        return_value=None,
    )

    assert not random_test_module.random_test(1, save=True)

    run_program.assert_called_once_with(
        "input",
        tmp_path / "randomTest" / "input",
        "",
        2,
    )
    assert not (tmp_path / "test").exists()


def test_random_test_rejects_negative_case_count(random_test_module):
    with pytest.raises(ValueError, match="non-negative"):
        random_test_module.random_test(-1)


def test_random_test_accepts_zero_cases(
    capsys,
    monkeypatch,
    mocker,
    random_test_module,
    tmp_path,
):
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(random_test_module, "create_random_test")
    mocker.patch.object(random_test_module, "_compile_one", return_value=True)

    assert random_test_module.random_test(0)
    assert capsys.readouterr().out == "[OK] 0 cases\n"


@pytest.mark.parametrize(
    ("function_name", "args", "expected"),
    [
        ("rt", (100,), (100, 2, False)),
        ("rt", (100, 5), (100, 5, False)),
        ("rts", (100,), (100, 2, True)),
        ("rts", (100, 5), (100, 5, True)),
    ],
)
def test_rt_and_rts_select_saving_and_timeout(
    args,
    expected,
    function_name,
    mocker,
    random_test_module,
):
    random_test = mocker.patch.object(
        random_test_module,
        "random_test",
        return_value=True,
    )

    result = getattr(random_test_module, function_name)(*args)

    assert result is None
    random_test.assert_called_once_with(*expected)


@pytest.mark.skipif(shutil.which("g++") is None, reason="g++ is required")
def test_random_test_end_to_end(
    capsys,
    monkeypatch,
    mocker,
    random_test_module,
    root,
    tmp_path,
):
    test_directory = tmp_path / "randomTest"
    test_directory.mkdir()
    (test_directory / "input.cpp").write_text(
        '#include <iostream>\nint main() { std::cout << "5\\n"; }\n',
        encoding="utf-8",
    )
    solution = (
        "#include <iostream>\n"
        "int main() { int x; std::cin >> x; "
        "std::cout << x * 2 << '\\n'; }\n"
    )
    (test_directory / "naive.cpp").write_text(solution, encoding="utf-8")
    (tmp_path / "main.cpp").write_text(solution, encoding="utf-8")
    monkeypatch.chdir(tmp_path)
    mocker.patch.object(
        random_test_module,
        "CONFIG",
        Config(root, shutil.which("g++") or "", None, None),
    )

    assert random_test_module.random_test(3)

    assert "[OK] 3 cases" in capsys.readouterr().out
