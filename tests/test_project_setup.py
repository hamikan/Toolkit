def test_local_test_setup_uses_virtual_environment(root):
    gitignore = (root / ".gitignore").read_text(encoding="utf-8").splitlines()
    readme = (root / "README.md").read_text(encoding="utf-8")

    assert ".venv/" in gitignore
    assert "python3 -m venv .venv" in readme
    assert "source .venv/bin/activate" in readme


def test_cpkit_workflow_runs_pytest_on_supported_python_versions(root):
    workflow = (
        root / ".github" / "workflows" / "cpkit.yml"
    ).read_text(encoding="utf-8")

    assert workflow.startswith("name: CPKit\n")
    assert "permissions:\n  contents: read" in workflow
    assert "actions/checkout@v7" in workflow
    assert "actions/setup-python@v7" in workflow
    assert 'python-version: ["3.10", "3.14"]' in workflow
    assert "cache: pip" in workflow
    assert "python -m pip install -r requirements-dev.txt" in workflow
    assert "python -m pytest" in workflow
