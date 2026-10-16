import runpy


def test_startup_exports_only_public_api(monkeypatch, root):
    startup = root / "startup.py"
    monkeypatch.setenv("PYTHONSTARTUP", str(startup))

    namespace = runpy.run_path(startup)

    assert "UF" in namespace
    assert "UnionFind" in namespace
    assert "how" in namespace
    assert "rt" in namespace
    assert "rts" in namespace
    assert "gcd" in namespace
    assert "rl" in namespace
    assert "os" not in namespace
    assert namespace["base2"](5) == "101"
