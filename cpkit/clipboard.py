"""C++の定型コードをクリップボードへコピーする。"""

LRUD = "LRUD"
NEWS = "NEWS"


def _copy(text):
    try:
        import pyperclip
    except ImportError as error:
        raise RuntimeError("クリップボード機能にはpyperclipが必要です") from error
    pyperclip.copy(text)


def clip_dir(mode=4):
    if mode == 4:
        text = "vector<pair<int, int>> dir = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};"
    elif mode == 8:
        text = (
            "vector<pair<int, int>> dir = {{1, 0}, {0, 1}, {-1, 0}, "
            "{0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};"
        )
    else:
        raise ValueError("mode must be 4 or 8")
    _copy(text)


def clip_query(operations):
    if isinstance(operations, int):
        operations = str(operations)
    is_number = operations.isdigit()
    lines = [
        "while(Q--) {",
        f"\t\t{'int' if is_number else 'char'} op; cin >> op;",
    ]
    lines.extend(
        f"\t\tif(op == {operation}) {{}}" if is_number else f"\t\tif(op == '{operation}') {{}}"
        for operation in operations
    )
    lines.append("\t}")
    _copy("\n".join(lines))


def clipDir(mode=4):
    return clip_dir(mode)


def clipQ(operations):
    return clip_query(operations)
