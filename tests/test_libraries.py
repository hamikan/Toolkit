import pytest

import cpkit.libraries as libraries


def write_readme(root, library, quick_reference=None):
    directory = root / "algorithm" / library
    directory.mkdir(parents=True)
    readme = directory / "README.md"
    content = f"# {library}\n"
    if quick_reference is not None:
        content += (
            "\n<!-- quick-reference:start -->\n"
            f"{quick_reference}\n"
            "<!-- quick-reference:end -->\n"
        )
    readme.write_text(content, encoding="utf-8")
    return readme


def write_solved_readme(root, library, body="まだ登録されていません。"):
    directory = root / "algorithm" / library
    directory.mkdir(parents=True)
    readme = directory / "README.md"
    readme.write_text(
        f"# {library}\n\n説明\n\n"
        "<!-- solved-problems:start -->\n"
        f"{body}\n"
        "<!-- solved-problems:end -->\n\n"
        "注意点\n",
        encoding="utf-8",
    )
    return readme


def test_load_library_from_any_working_directory(tmp_path, capsys):
    main = tmp_path / "main.cpp"
    main.write_text(
        "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {}\n",
        encoding="utf-8",
    )

    assert libraries.load_library("UF", main) is None
    first = main.read_text(encoding="utf-8")
    assert "struct UnionFind" in first
    assert libraries.load_library("UnionFind", main) is None
    assert main.read_text(encoding="utf-8") == first
    assert capsys.readouterr().out == "成功\n成功\n"

    with pytest.raises(ValueError, match="未登録"):
        libraries.load_library("../outside", main)


def test_load_library_reports_relative_library_path_on_failure(
    tmp_path, monkeypatch, mocker, capsys
):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    main = tmp_path / "main.cpp"
    main.write_text("int main() {}\n", encoding="utf-8")
    monkeypatch.chdir(tmp_path)

    assert libraries.load_library("UF", main) is None

    assert capsys.readouterr().out == "失敗: algorithm/UnionFind/main.cpp\n"


def test_load_library_reports_library_path_when_target_is_missing(
    tmp_path, monkeypatch, mocker, capsys
):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    library = tmp_path / "algorithm" / "UnionFind" / "main.cpp"
    library.parent.mkdir(parents=True)
    library.write_text("struct UnionFind {};\n", encoding="utf-8")
    monkeypatch.chdir(tmp_path)

    assert libraries.load_library("UF") is None

    assert capsys.readouterr().out == "失敗: algorithm/UnionFind/main.cpp\n"


def test_load_library_reports_library_path_when_merge_fails(
    tmp_path, monkeypatch, mocker, capsys
):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    library = tmp_path / "algorithm" / "UnionFind" / "main.cpp"
    library.parent.mkdir(parents=True)
    library.write_text("struct UnionFind {};\n", encoding="utf-8")
    (tmp_path / "main.cpp").write_text("int main() {}\n", encoding="utf-8")
    mocker.patch.object(libraries, "merge_cpp", side_effect=RuntimeError)
    monkeypatch.chdir(tmp_path)

    assert libraries.load_library("UF") is None

    assert capsys.readouterr().out == "失敗: algorithm/UnionFind/main.cpp\n"


def test_save_problem_adds_entry_to_every_library(tmp_path, mocker, capsys):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    union_find = write_solved_readme(tmp_path, "UnionFind")
    segment_tree = write_solved_readme(tmp_path, "SegTree")

    result = libraries.save_problem(
        libraries=[libraries.UF, libraries.seg],
        title="ABC431 E",
        problem_url="https://atcoder.jp/contests/abc431/tasks/abc431_e",
        submission_url="https://atcoder.jp/contests/abc431/submissions/12345678",
    )

    assert result is None
    assert capsys.readouterr().out == "成功\n"
    entry = (
        "- [ABC431 E](https://atcoder.jp/contests/abc431/tasks/abc431_e) "
        "([提出](https://atcoder.jp/contests/abc431/submissions/12345678))"
    )
    for readme in (union_find, segment_tree):
        content = readme.read_text(encoding="utf-8")
        assert entry in content
        assert "まだ登録されていません。" not in content
        assert "説明" in content
        assert "注意点" in content


def test_save_problem_does_not_add_same_problem_twice(tmp_path, mocker, capsys):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    readme = write_solved_readme(tmp_path, "UnionFind")
    arguments = {
        "libraries": [libraries.UF],
        "title": "ABC431 E",
        "problem_url": "https://atcoder.jp/contests/abc431/tasks/abc431_e",
        "submission_url": "https://atcoder.jp/contests/abc431/submissions/12345678",
    }

    libraries.save_problem(**arguments)
    libraries.save_problem(**arguments)

    content = readme.read_text(encoding="utf-8")
    assert content.count(arguments["problem_url"]) == 1
    assert capsys.readouterr().out == "成功\n成功\n"


def test_save_problem_validates_every_readme_before_writing(tmp_path, mocker):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    union_find = write_solved_readme(tmp_path, "UnionFind")
    original = union_find.read_text(encoding="utf-8")
    write_readme(tmp_path, "SegTree")

    with pytest.raises(ValueError, match="solved-problems"):
        libraries.save_problem(
            libraries=[libraries.UF, libraries.seg],
            title="ABC431 E",
            problem_url="https://atcoder.jp/contests/abc431/tasks/abc431_e",
            submission_url="https://atcoder.jp/contests/abc431/submissions/12345678",
        )

    assert union_find.read_text(encoding="utf-8") == original


def test_save_problem_rejects_library_name_string():
    with pytest.raises(TypeError, match="呼び出し関数を指定してください"):
        libraries.save_problem(
            libraries=["UF"],
            title="ABC431 E",
            problem_url="https://atcoder.jp/contests/abc431/tasks/abc431_e",
            submission_url="https://atcoder.jp/contests/abc431/submissions/12345678",
        )


def test_save_problem_rejects_empty_library_list():
    with pytest.raises(ValueError, match="1つ以上"):
        libraries.save_problem(
            libraries=[],
            title="ABC431 E",
            problem_url="https://atcoder.jp/contests/abc431/tasks/abc431_e",
            submission_url="https://atcoder.jp/contests/abc431/submissions/12345678",
        )


@pytest.mark.parametrize(
    ("field", "value"),
    [
        ("title", "ABC431 E\n不正な行"),
        ("problem_url", "https://example.com/problem\n不正な行"),
        ("submission_url", "https://example.com/submission\n不正な行"),
    ],
)
def test_save_problem_rejects_line_breaks(field, value):
    arguments = {
        "libraries": [libraries.UF],
        "title": "ABC431 E",
        "problem_url": "https://atcoder.jp/contests/abc431/tasks/abc431_e",
        "submission_url": "https://atcoder.jp/contests/abc431/submissions/12345678",
    }
    arguments[field] = value

    with pytest.raises(ValueError, match=field):
        libraries.save_problem(**arguments)


def test_how_accepts_shortcut_function_and_displays_quick_reference(
    tmp_path, monkeypatch, mocker, capsys
):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    write_readme(
        tmp_path,
        "UnionFind",
        "bool unite(int x, int y)\n"
        "  引数: 頂点番号 x, y\n"
        "  戻り値: 新しく繋がったら true\n"
        "  計算量: O(α(N))",
    )
    monkeypatch.chdir(tmp_path)

    assert libraries.how(libraries.UF) is None

    assert capsys.readouterr().out == (
        "UnionFind\n"
        "README: algorithm/UnionFind/README.md\n\n"
        "bool unite(int x, int y)\n"
        "  引数: 頂点番号 x, y\n"
        "  戻り値: 新しく繋がったら true\n"
        "  計算量: O(α(N))\n"
    )


def test_how_accepts_formal_library_function(tmp_path, mocker, capsys):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    write_readme(tmp_path, "UnionFind", "UnionFind(int N)")

    libraries.how(libraries.UnionFind)

    assert "UnionFind(int N)" in capsys.readouterr().out


def test_formal_library_function_loads_same_library(mocker):
    load = mocker.patch.object(libraries, "load_library")

    result = libraries.UnionFind()

    load.assert_called_once_with("UnionFind")
    assert result is None


def test_how_rejects_string():
    with pytest.raises(TypeError, match="呼び出し関数を指定してください"):
        libraries.how("UF")


def test_how_rejects_unregistered_library():
    def Unknown():
        pass

    with pytest.raises(ValueError, match="未登録のライブラリです"):
        libraries.how(Unknown)


def test_how_reports_missing_readme(tmp_path, mocker):
    mocker.patch.object(libraries, "ROOT", tmp_path)

    with pytest.raises(FileNotFoundError, match="READMEが見つかりません"):
        libraries.how(libraries.UF)


def test_how_reports_missing_quick_reference(tmp_path, mocker):
    mocker.patch.object(libraries, "ROOT", tmp_path)
    write_readme(tmp_path, "UnionFind")

    with pytest.raises(ValueError, match="クイックリファレンスが見つかりません"):
        libraries.how(libraries.UF)


@pytest.mark.parametrize(("shortcut", "library"), libraries.SHORTCUTS.items())
def test_every_shortcut_targets_expected_library(mocker, shortcut, library):
    load = mocker.patch.object(libraries, "load_library")

    result = getattr(libraries, shortcut)()

    load.assert_called_once_with(library)
    assert result is None


@pytest.mark.parametrize("shortcut", libraries.SHORTCUTS)
def test_every_registered_library_can_be_merged(tmp_path, shortcut):
    main = tmp_path / "main.cpp"
    main.write_text(
        "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {}\n",
        encoding="utf-8",
    )

    assert libraries.load_library(shortcut, main) is None
