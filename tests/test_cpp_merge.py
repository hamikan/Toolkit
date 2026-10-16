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
