"""C++ライブラリを既存のmain.cppへ合成する。"""

import re


_INCLUDE_PATTERN = re.compile(r"\s*(#include|#pragma)\b")
_USING_PATTERN = re.compile(r"\s*using\s+")
_MAIN_PATTERN = re.compile(r"\s*(int|signed)\s+main\s*\(")
_NUMBER_PATTERN = re.compile(r"(?:[0-9]|\.[0-9])(?:[eEpP][+-]|'\w|[\w.])*")


def _is_include(line):
    return bool(_INCLUDE_PATTERN.match(line))


def _is_using(line):
    return bool(_USING_PATTERN.match(line))


def _scan_braces(line, in_block_comment=False, quote=None, escaped=False):
    delta = 0
    i = 0
    while i < len(line):
        char = line[i]
        next_char = line[i + 1] if i + 1 < len(line) else ""
        if in_block_comment:
            if char == "*" and next_char == "/":
                in_block_comment = False
                i += 2
            else:
                i += 1
            continue
        if quote is not None:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            i += 1
            continue
        if char == "/" and next_char == "/":
            break
        if char == "/" and next_char == "*":
            in_block_comment = True
            i += 2
            continue
        if char.isalpha() or char == "_":
            i += 1
            while i < len(line) and (line[i].isalnum() or line[i] == "_"):
                i += 1
            continue
        number = _NUMBER_PATTERN.match(line, i)
        if number is not None:
            i = number.end()
            continue
        if char in "\"'":
            quote = char
        elif char == "{":
            delta += 1
        elif char == "}":
            delta -= 1
        i += 1
    return delta, in_block_comment, quote, escaped


def split_code(lines):
    blocks = []
    current = []
    balance = 0
    in_block_comment = False
    quote = None
    escaped = False
    for line in lines:
        neutral = not in_block_comment and quote is None
        if balance == 0 and neutral and not line.strip():
            if current:
                blocks.append(current)
                current = []
            continue
        previous_balance = balance
        current.append(line)
        delta, in_block_comment, quote, escaped = _scan_braces(
            line,
            in_block_comment,
            quote,
            escaped,
        )
        balance += delta
        if balance < 0:
            raise ValueError("C++コードの波括弧が対応していません")
        if previous_balance > 0 and balance == 0 and not in_block_comment and quote is None:
            blocks.append(current)
            current = []
    if balance != 0 or in_block_comment or quote is not None:
        raise ValueError(
            "C++コードの波括弧、コメント、または文字列が"
            "閉じられていません"
        )
    if current:
        blocks.append(current)
    return blocks


def _extract_library(lines):
    includes = []
    usings = []
    body = []
    header = True
    for line in lines:
        if header and not line.strip():
            continue
        if header and _is_include(line):
            includes.append(line)
            continue
        if header and _is_using(line):
            usings.append(line)
            continue
        header = False
        body.append(line)
    return includes, usings, body


def _header_positions(lines):
    include_positions = []
    using_positions = []
    for i, line in enumerate(lines):
        if not line.strip():
            continue
        if _is_include(line):
            include_positions.append(i)
            continue
        if _is_using(line):
            using_positions.append(i)
            continue
        break
    return include_positions, using_positions


def _block_key(block):
    start = 0
    while start < len(block) and block[start].strip().startswith("// Source: "):
        start += 1
    return "".join(block[start:]).strip()


def merge_cpp(main_code, library_code, source_url=None):
    main_lines = main_code.splitlines(keepends=True)
    library_lines = library_code.splitlines(keepends=True)
    library_includes, library_usings, library_body = _extract_library(library_lines)

    include_positions, using_positions = _header_positions(main_lines)
    existing_includes = {main_lines[i].strip() for i in include_positions}
    new_includes = [line for line in library_includes if line.strip() not in existing_includes]
    if new_includes:
        include_at = include_positions[-1] + 1 if include_positions else 0
        main_lines[include_at:include_at] = new_includes

    include_positions, using_positions = _header_positions(main_lines)
    existing_usings = {main_lines[i].strip() for i in using_positions}
    new_usings = [line for line in library_usings if line.strip() not in existing_usings]
    if new_usings:
        if using_positions:
            using_at = using_positions[-1] + 1
        elif include_positions:
            using_at = include_positions[-1] + 1
        else:
            using_at = 0
        main_lines[using_at:using_at] = new_usings

    main_blocks = split_code(main_lines)
    existing_blocks = {_block_key(block) for block in main_blocks}
    for block in main_blocks:
        _, _, body = _extract_library(block)
        existing_blocks.add(_block_key(body))
        if body:
            break
    new_blocks = []
    for block in split_code(library_body):
        content = "".join(block)
        key = _block_key(block)
        if not key or _MAIN_PATTERN.search(content):
            continue
        if key not in existing_blocks:
            new_blocks.append(block)
            existing_blocks.add(key)

    if new_blocks:
        main_at = next((i for i, line in enumerate(main_lines) if _MAIN_PATTERN.match(line)), None)
        if main_at is None:
            raise ValueError("main関数が見つかりません")
        added = []
        if main_at > 0 and main_lines[main_at - 1].strip():
            added.append("\n")
        if source_url:
            source = f"// Source: {source_url}"
            if not any(line.strip() == source for line in main_lines):
                added.append(source + "\n")
        for block in new_blocks:
            added.extend(block)
            if not block[-1].endswith("\n"):
                added.append("\n")
            added.append("\n")
        main_lines[main_at:main_at] = added

    return "".join(main_lines)
