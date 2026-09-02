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
