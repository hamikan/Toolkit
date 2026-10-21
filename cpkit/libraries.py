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
_LIBRARY_URL = "https://github.com/hamikan/Toolkit/blob/main/algorithm"
_QUICK_REFERENCE_START = "<!-- quick-reference:start -->"
_QUICK_REFERENCE_END = "<!-- quick-reference:end -->"


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
        source_url = f"{_LIBRARY_URL}/{library_name}/main.cpp"
        merged = merge_cpp(current, library, source_url=source_url)
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
