import pytest


def test_merge_preserves_header_order_and_is_idempotent():
    from cpkit.cpp_merge import merge_cpp

    main = """#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
    return 0;
}
"""
    library = """#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;
using mint = atcoder::modint998244353;

struct Example {
    string brace = "}";
};
"""
    merged = merge_cpp(main, library)

    assert merged.index("#include <bits/stdc++.h>") < merged.index(
        "#include <atcoder/modint>"
    )
    assert merged.index("using ll = long long;") < merged.index(
        "using mint = atcoder::modint998244353;"
    )
    assert merged.index("struct Example") < merged.index("int main()")
    assert merge_cpp(merged, library) == merged


def test_merge_adds_source_before_library_body():
    from cpkit.cpp_merge import merge_cpp

    main = "int main() {}\n"
    library = (
        "#include <vector>\nusing namespace std;\n\n"
        "struct Helper {\n    int value;\n};\n\n"
        "struct Example {\n    Helper helper;\n};\n"
    )
    source_url = "https://github.com/hamikan/Toolkit/blob/main/algorithm/Example/main.cpp"

    merged = merge_cpp(main, library, source_url=source_url)

    assert merged == (
        "#include <vector>\nusing namespace std;\n\n"
        f"// Source: {source_url}\n"
        "struct Helper {\n    int value;\n};\n\n"
        "struct Example {\n    Helper helper;\n};\n\n"
        "int main() {}\n"
    )
    assert merge_cpp(merged, library, source_url=source_url) == merged
    assert merge_cpp(merged, library) == merged


def test_merge_deduplicates_shared_blocks_with_sources():
    from cpkit.cpp_merge import merge_cpp

    shared = "struct Shared {\n    int value;\n};\n\n"
    first = shared + "struct First {\n    Shared shared;\n};\n"
    second = shared + "struct Second {\n    Shared shared;\n};\n"
    base_url = "https://github.com/hamikan/Toolkit/blob/main/algorithm"
    merged = merge_cpp("int main() {}\n", first, source_url=f"{base_url}/First/main.cpp")

    merged = merge_cpp(merged, second, source_url=f"{base_url}/Second/main.cpp")

    assert merged.count("struct Shared") == 1
    assert merged.count("struct First") == 1
    assert merged.count("struct Second") == 1
    assert merged.count(f"// Source: {base_url}/First/main.cpp\n") == 1
    assert f"// Source: {base_url}/Second/main.cpp\nstruct Second" in merged
    assert merge_cpp(merged, first, source_url=f"{base_url}/First/main.cpp") == merged
    assert merge_cpp(merged, second, source_url=f"{base_url}/Second/main.cpp") == merged


@pytest.mark.parametrize("gap", ["", "\n", " \n"])
@pytest.mark.parametrize(
    "headers",
    [
        "#include <vector>\nusing namespace std;\nusing ll = long long;\n",
        "#include <vector>\n\nusing namespace std;\nusing ll = long long;\n",
    ],
)
def test_merge_does_not_duplicate_body_next_to_headers(gap, headers):
    from cpkit.cpp_merge import merge_cpp

    body = "struct Example {\n    ll value;\n};\n"
    main = f"{headers}{gap}{body}\nint main() {{}}\n"
    library = f"{headers}\n{body}"

    assert merge_cpp(main, library) == main


@pytest.mark.parametrize("gap", ["", "\n"])
@pytest.mark.parametrize("has_source", [False, True])
def test_merge_with_source_does_not_duplicate_existing_body(gap, has_source):
    from cpkit.cpp_merge import merge_cpp

    headers = "#include <vector>\nusing namespace std;\n"
    body = "struct Example {\n    vector<int> values;\n};\n"
    source_url = "https://github.com/hamikan/Toolkit/blob/main/algorithm/Example/main.cpp"
    source = f"// Source: {source_url}\n" if has_source else ""
    main = f"{headers}{gap}{source}{body}\nint main() {{}}\n"

    assert merge_cpp(main, headers + "\n" + body, source_url=source_url) == main


@pytest.mark.parametrize("headers", ["", "#include <vector>\n"])
def test_merge_adds_missing_headers_without_duplicating_existing_body(headers):
    from cpkit.cpp_merge import merge_cpp

    body = "struct Example {\n    vector<int> values;\n};\n"
    main = f"{headers}{body}\nint main() {{}}\n"
    library_headers = "#include <vector>\nusing namespace std;\n"
    library = f"{library_headers}\n{body}"

    merged = merge_cpp(main, library)

    assert merged == f"{library_headers}{body}\nint main() {{}}\n"
    assert merge_cpp(merged, library) == merged


def test_merge_preserves_multiline_using_declarations():
    from cpkit.cpp_merge import merge_cpp

    declaration = "using Generator = decltype([] {\n    return 1;\n});\n"
    main = f"{declaration}\nint main() {{}}\n"
    library = "struct Example {\n    int value;\n};\n"

    merged = merge_cpp(main, library)

    assert merged.count(declaration) == 1
    assert library in merged
    assert merge_cpp(merged, library) == merged


def test_body_requires_main_function():
    from cpkit.cpp_merge import merge_cpp

    with pytest.raises(ValueError, match="main"):
        merge_cpp("#include <bits/stdc++.h>\n", "struct Example {};\n")


def test_merge_ignores_braces_in_comments_and_literals():
    from cpkit.cpp_merge import merge_cpp

    main = "int main() {}\n"
    library = '''struct Example {
    string text = "}";
    char brace = '{';
    /* { ignored } */
    // } ignored
};
'''

    merged = merge_cpp(main, library)

    assert library.strip() in merged


@pytest.mark.parametrize("target", ["main", "library"])
@pytest.mark.parametrize(
    "literal",
    [
        "1'000",
        "1'000'000",
        "0xAB'CD",
        "0b1010'0101",
        "0'765",
        "1'000ULL",
        "1e+1'0",
        "1'2.3'4e+5'6",
        ".1'2e-3'4",
        "0xA'B.C'Dp+1'2",
    ],
)
def test_merge_preserves_digit_separators(target, literal):
    from cpkit.cpp_merge import merge_cpp

    declaration = f"    static constexpr auto value = {literal};\n"
    main_body = declaration if target == "main" else ""
    library_body = declaration if target == "library" else ""
    main = f"int main() {{\n{main_body}}}\n"
    library = f"struct Example {{\n{library_body}}};\n"

    merged = merge_cpp(main, library)

    assert main in merged
    assert library in merged
    assert merged.index("struct Example") < merged.index("int main()")
    assert merge_cpp(merged, library) == merged


@pytest.mark.parametrize("target", ["main", "library"])
@pytest.mark.parametrize(
    "literal",
    ["'{'", "'}'", r"'\''", r"'\\'", "L'}'", "u'}'", "U'}'", "u8'}'", "u8'0'"],
)
def test_merge_preserves_character_literals_alongside_digit_separators(target, literal):
    from cpkit.cpp_merge import merge_cpp

    declarations = (
        "    static constexpr int limit = 1'000;\n"
        f"    static constexpr auto character = {literal};\n"
    )
    main_body = declarations if target == "main" else ""
    library_body = declarations if target == "library" else ""
    main = f"int main() {{\n{main_body}}}\n"
    library = f"struct Example {{\n{library_body}}};\n"

    merged = merge_cpp(main, library)

    assert main in merged
    assert library in merged
    assert merge_cpp(merged, library) == merged


@pytest.mark.parametrize(
    "library",
    [
        "struct Example {\n",
        "/* unclosed comment\n",
        'string text = "unclosed;\n',
        "};\n",
    ],
)
def test_merge_rejects_unclosed_or_unmatched_cpp(library):
    from cpkit.cpp_merge import merge_cpp

    with pytest.raises(ValueError, match="波括弧|コメント|文字列"):
        merge_cpp("int main() {}\n", library)


def test_merge_adds_headers_to_headerless_main():
    from cpkit.cpp_merge import merge_cpp

    merged = merge_cpp(
        "int main() {}\n",
        "#include <vector>\nusing namespace std;\n\nstruct Example {};\n",
    )

    assert merged.startswith("#include <vector>\nusing namespace std;\n")
    assert merged.index("struct Example") < merged.index("int main()")


def test_merge_ignores_main_function_from_library():
    from cpkit.cpp_merge import merge_cpp

    merged = merge_cpp(
        "int main() {}\n",
        "struct Example {};\n\nint main() { return 1; }\n",
    )

    assert "struct Example" in merged
    assert merged.count("int main()") == 1
