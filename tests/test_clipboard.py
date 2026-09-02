import importlib
import sys
import types

import pytest

import cpkit.clipboard as clipboard


@pytest.mark.parametrize(
    ("operations", "expected"),
    [
        (
            clipboard.LRUD,
            ("if(op == 'L')", "if(op == 'R')", "if(op == 'U')", "if(op == 'D')"),
        ),
        (
            clipboard.NEWS,
            ("if(op == 'N')", "if(op == 'E')", "if(op == 'W')", "if(op == 'S')"),
        ),
    ],
)
def test_clip_query_supports_direction_presets(mocker, operations, expected):
    copy = mocker.Mock()
    mocker.patch.dict(sys.modules, {"pyperclip": types.SimpleNamespace(copy=copy)})

    clipboard.clipQ(operations)

    copy.assert_called_once()
    copied = copy.call_args.args[0]
    assert all(line in copied for line in expected)


@pytest.mark.parametrize(
    ("mode", "expected"),
    [
        (
            4,
            "vector<pair<int, int>> dir = "
            "{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};",
        ),
        (
            8,
            "vector<pair<int, int>> dir = "
            "{{1, 0}, {0, 1}, {-1, 0}, {0, -1}, "
            "{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};",
        ),
    ],
)
def test_clip_dir_copies_expected_directions(mocker, mode, expected):
    copy = mocker.Mock()
    mocker.patch.dict(sys.modules, {"pyperclip": types.SimpleNamespace(copy=copy)})

    clipboard.clip_dir(mode)

    copy.assert_called_once_with(expected)


@pytest.mark.parametrize(
    ("operations", "declaration", "conditions"),
    [
        ("ab", "char op; cin >> op;", ("if(op == 'a')", "if(op == 'b')")),
        (12, "int op; cin >> op;", ("if(op == 1)", "if(op == 2)")),
    ],
)
def test_clip_query_uses_matching_operation_type(
    mocker,
    operations,
    declaration,
    conditions,
):
    copy = mocker.Mock()
    mocker.patch.dict(sys.modules, {"pyperclip": types.SimpleNamespace(copy=copy)})

    clipboard.clip_query(operations)

    copy.assert_called_once()
    copied = copy.call_args.args[0]
    assert declaration in copied
    assert all(condition in copied for condition in conditions)


def test_clipboard_dependency_is_loaded_only_when_copying(mocker):
    original_import = __import__

    def import_without_pyperclip(name, *args, **kwargs):
        if name == "pyperclip":
            raise ImportError
        return original_import(name, *args, **kwargs)

    mocker.patch("builtins.__import__", side_effect=import_without_pyperclip)

    importlib.reload(clipboard)

    with pytest.raises(RuntimeError, match="pyperclip"):
        clipboard.clip_dir()


def test_invalid_direction_mode():
    with pytest.raises(ValueError, match="4 or 8"):
        clipboard.clip_dir(6)
