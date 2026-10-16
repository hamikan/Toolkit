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


def test_load_library_does_not_duplicate_existing_body_without_header_gap(
    root, tmp_path, capsys
):
    library = root / "algorithm" / "UnionFind" / "main.cpp"
    original = library.read_text(encoding="utf-8").replace(
        "using namespace std;\n\n", "using namespace std;\n", 1
    ) + "\nint main() {}\n"
    main = tmp_path / "main.cpp"
    main.write_text(original, encoding="utf-8")

    libraries.load_library("UF", main)
    libraries.load_library("UnionFind", main)

    assert main.read_text(encoding="utf-8") == original
    assert capsys.readouterr().out == "成功\n成功\n"


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
