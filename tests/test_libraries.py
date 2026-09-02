import pytest

import cpkit.libraries as libraries


def test_load_library_from_any_working_directory(tmp_path):
    main = tmp_path / "main.cpp"
    main.write_text(
        "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {}\n",
        encoding="utf-8",
    )

    assert libraries.load_library("UF", main)
    first = main.read_text(encoding="utf-8")
    assert "struct UnionFind" in first
    assert not libraries.load_library("UnionFind", main)
    assert main.read_text(encoding="utf-8") == first

    with pytest.raises(ValueError, match="未登録"):
        libraries.load_library("../outside", main)


@pytest.mark.parametrize(("shortcut", "library"), libraries.SHORTCUTS.items())
def test_every_shortcut_targets_expected_library(mocker, shortcut, library):
    load = mocker.patch.object(libraries, "load_library")

    getattr(libraries, shortcut)()

    load.assert_called_once_with(library)


@pytest.mark.parametrize("shortcut", libraries.SHORTCUTS)
def test_every_registered_library_can_be_merged(tmp_path, shortcut):
    main = tmp_path / "main.cpp"
    main.write_text(
        "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {}\n",
        encoding="utf-8",
    )

    assert libraries.load_library(shortcut, main)
