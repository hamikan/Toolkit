import pytest


def test_load_env_and_relative_paths(tmp_path):
    from cpkit.config import load_config, load_env

    (tmp_path / ".env").write_text(
        "CXX='bin/g++'\nSYSROOT=./sdk\nACL_PATH=./ac-library\n",
        encoding="utf-8",
    )

    assert load_env(tmp_path / ".env") == {
        "CXX": "bin/g++",
        "SYSROOT": "./sdk",
        "ACL_PATH": "./ac-library",
    }
    config = load_config(tmp_path)
    assert config.cxx == "bin/g++"
    assert config.sysroot == (tmp_path / "sdk").resolve()
    assert config.acl_path == (tmp_path / "ac-library").resolve()
    assert config.compile_flags == [
        "-O2",
        "-std=c++23",
        "-isysroot",
        str((tmp_path / "sdk").resolve()),
        "-I",
        str((tmp_path / "ac-library").resolve()),
    ]


def test_invalid_env_reports_line(tmp_path):
    from cpkit.config import load_env

    path = tmp_path / ".env"
    path.write_text("CXX=g++\nINVALID\n", encoding="utf-8")
    with pytest.raises(ValueError, match=r"\.env:2"):
        load_env(path)


def test_missing_env_uses_empty_configuration(tmp_path):
    from cpkit.config import load_config, load_env

    assert load_env(tmp_path / ".env") == {}

    config = load_config(tmp_path)
    assert config.cxx == ""
    assert config.sysroot is None
    assert config.acl_path is None
    assert config.compile_flags == ["-O2", "-std=c++23"]


def test_load_env_ignores_blank_lines_and_comments(tmp_path):
    from cpkit.config import load_env

    path = tmp_path / ".env"
    path.write_text("\n# comment\n CXX = g++ \n", encoding="utf-8")

    assert load_env(path) == {"CXX": "g++"}


def test_load_env_rejects_empty_key(tmp_path):
    from cpkit.config import load_env

    path = tmp_path / ".env"
    path.write_text("=value\n", encoding="utf-8")

    with pytest.raises(ValueError, match="KEYを指定してください"):
        load_env(path)
