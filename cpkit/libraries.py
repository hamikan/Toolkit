"""登録済みC++ライブラリを作業中のmain.cppへ追加する。"""

import os
from pathlib import Path
import tempfile

from .config import ROOT
from .cpp_merge import merge_cpp


SHORTCUTS = {
    "dfs": "DFS",
    "bfs": "BFS",
    "bfs01": "BFS01",
    "dij": "Dijkstra",
    "BF": "BellmanFord",
    "WF": "WarshallFloyd",
    "Kruskal": "Kruskal",
    "UF": "UnionFind",
    "RUF": "RollbackUnionFind",
    "SCC": "SCC",
    "TwoSat": "TwoSat",
    "seg": "SegTree",
    "lseg": "LazySegTree",
    "pseg": "PersistentSegTree",
    "plseg": "PersistentLazySegTree",
    "Treap": "ImplicitTreap",
    "Trie": "Trie",
    "BTrie": "BinaryTrie",
    "Aho": "AhoCorasick",
    "BIT": "BIT",
    "Cum": "CumulativeND",
    "MF": "MaxFlow",
    "DLL": "DoublyLinkedList",
    "Era": "Eratosthenes",
    "GCD": "GCD",
    "extGCD": "ExtendedGCD",
    "LCM": "LCM",
    "CRT": "CRT",
    "DIG": "Digit",
    "LCA": "LCA",
    "HLD": "HLD",
    "Reroot": "Rerooting",
    "RH": "RollingHash",
    "ZH": "ZobristHash",
    "Mo": "Mo",
    "MP": "ModPow",
    "MPD": "ModPowDivision",
    "Comb": "Combinatorics",
    "Comp": "Compress",
    "RS": "RangeSet",
    "Grid": "Grid",
    "Mat": "Matrix",
    "Geom": "Geometry",
}
_LIBRARY_NAMES = frozenset(SHORTCUTS.values())
_QUICK_REFERENCE_START = "<!-- quick-reference:start -->"
_QUICK_REFERENCE_END = "<!-- quick-reference:end -->"
_SOLVED_PROBLEMS_START = "<!-- solved-problems:start -->"
_SOLVED_PROBLEMS_END = "<!-- solved-problems:end -->"


def available_libraries():
    return dict(SHORTCUTS)


def _library_name(library):
    if callable(library):
        name = library.__name__
    elif isinstance(library, str):
        name = library
    else:
        raise TypeError("ライブラリ名または呼び出し関数を指定してください")
    library_name = SHORTCUTS.get(name, name)
    if library_name not in _LIBRARY_NAMES:
        raise ValueError(f"未登録のライブラリです: {name}")
    return library_name


def _library_path(name):
    return ROOT / "algorithm" / name / "main.cpp"


def _readme_path(name):
    return ROOT / "algorithm" / name / "README.md"


def _quick_reference(readme):
    content = readme.read_text(encoding="utf-8")
    start = content.find(_QUICK_REFERENCE_START)
    end = content.find(_QUICK_REFERENCE_END)
    if start == -1 or end == -1 or end < start:
        raise ValueError(f"クイックリファレンスが見つかりません: {readme}")
    start += len(_QUICK_REFERENCE_START)
    reference = content[start:end].strip()
    if not reference:
        raise ValueError(f"クイックリファレンスが見つかりません: {readme}")
    return reference


def _solved_problem_content(content, entry, problem_url, readme):
    if (
        content.count(_SOLVED_PROBLEMS_START) != 1
        or content.count(_SOLVED_PROBLEMS_END) != 1
    ):
        raise ValueError(f"solved-problemsマーカーが見つかりません: {readme}")
    start = content.index(_SOLVED_PROBLEMS_START) + len(_SOLVED_PROBLEMS_START)
    end = content.index(_SOLVED_PROBLEMS_END)
    if end < start:
        raise ValueError(f"solved-problemsマーカーの順番が不正です: {readme}")
    body = content[start:end].strip()
    if f"]({problem_url})" in body:
        return content
    if not body or body == "まだ登録されていません。":
        body = entry
    else:
        body = f"{body}\n{entry}"
    return f"{content[:start]}\n{body}\n{content[end:]}"


def _problem_field(name, value):
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{name}を指定してください")
    if "\n" in value or "\r" in value:
        raise ValueError(f"{name}に改行は使用できません")
    return value.strip()


def _atomic_write(path, content):
    mode = path.stat().st_mode
    with tempfile.NamedTemporaryFile(
        "w",
        encoding="utf-8",
        newline="",
        dir=path.parent,
        prefix=f".{path.name}.",
        delete=False,
    ) as file:
        file.write(content)
        temporary = Path(file.name)
    try:
        os.chmod(temporary, mode)
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def load_library(name, target="main.cpp"):
    library_name = _library_name(name)
    library_path = _library_path(library_name)
    relative_library_path = os.path.relpath(library_path)
    target_path = Path(target)
    try:
        if not target_path.exists() or not library_path.exists():
            raise FileNotFoundError
        current = target_path.read_text(encoding="utf-8")
        library = library_path.read_text(encoding="utf-8")
        merged = merge_cpp(current, library)
        if merged != current:
            _atomic_write(target_path, merged)
    except Exception:
        print(f"失敗: {relative_library_path}")
        return
    print("成功")


def how(library):
    if not callable(library):
        raise TypeError("ライブラリの呼び出し関数を指定してください")
    library_name = _library_name(library.__name__)
    readme = _readme_path(library_name)
    if not readme.exists():
        raise FileNotFoundError(f"READMEが見つかりません: {readme}")
    reference = _quick_reference(readme)
    relative_readme = os.path.relpath(readme)
    print(f"{library_name}\nREADME: {relative_readme}\n\n{reference}")


def save_problem(libraries, title, problem_url, submission_url):
    title = _problem_field("title", title)
    problem_url = _problem_field("problem_url", problem_url)
    submission_url = _problem_field("submission_url", submission_url)
    library_functions = tuple(libraries)
    if not library_functions:
        raise ValueError("ライブラリを1つ以上指定してください")

    library_names = []
    for library in library_functions:
        if not callable(library):
            raise TypeError("ライブラリの呼び出し関数を指定してください")
        library_name = _library_name(library.__name__)
        if library_name not in library_names:
            library_names.append(library_name)

    escaped_title = title.replace("\\", "\\\\").replace("[", "\\[").replace("]", "\\]")
    entry = f"- [{escaped_title}]({problem_url}) ([提出]({submission_url}))"
    updates = []
    for library_name in library_names:
        readme = _readme_path(library_name)
        if not readme.exists():
            raise FileNotFoundError(f"READMEが見つかりません: {readme}")
        content = readme.read_text(encoding="utf-8")
        updated = _solved_problem_content(content, entry, problem_url, readme)
        updates.append((readme, content, updated))

    for readme, content, updated in updates:
        if updated != content:
            _atomic_write(readme, updated)
    print("成功")


def dfs():
    load_library("DFS")


def bfs():
    load_library("BFS")


def bfs01():
    load_library("BFS01")


def dij():
    load_library("Dijkstra")


def BF():
    load_library("BellmanFord")


def WF():
    load_library("WarshallFloyd")


def Kruskal():
    load_library("Kruskal")


def UF():
    load_library("UnionFind")


def UnionFind():
    load_library("UnionFind")


def RUF():
    load_library("RollbackUnionFind")


def SCC():
    load_library("SCC")


def TwoSat():
    load_library("TwoSat")


def seg():
    load_library("SegTree")


def lseg():
    load_library("LazySegTree")


def pseg():
    load_library("PersistentSegTree")


def plseg():
    load_library("PersistentLazySegTree")


def Treap():
    load_library("ImplicitTreap")


def Trie():
    load_library("Trie")


def BTrie():
    load_library("BinaryTrie")


def Aho():
    load_library("AhoCorasick")


def BIT():
    load_library("BIT")


def Cum():
    load_library("CumulativeND")


def MF():
    load_library("MaxFlow")


def DLL():
    load_library("DoublyLinkedList")


def Era():
    load_library("Eratosthenes")


def GCD():
    load_library("GCD")


def extGCD():
    load_library("ExtendedGCD")


def LCM():
    load_library("LCM")


def CRT():
    load_library("CRT")


def DIG():
    load_library("Digit")


def LCA():
    load_library("LCA")


def HLD():
    load_library("HLD")


def Reroot():
    load_library("Rerooting")


def RH():
    load_library("RollingHash")


def ZH():
    load_library("ZobristHash")


def Mo():
    load_library("Mo")


def MP():
    load_library("ModPow")


def MPD():
    load_library("ModPowDivision")


def Comb():
    load_library("Combinatorics")


def Comp():
    load_library("Compress")


def RS():
    load_library("RangeSet")


def Grid():
    load_library("Grid")


def Mat():
    load_library("Matrix")


def Geom():
    load_library("Geometry")
