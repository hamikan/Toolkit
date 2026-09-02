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


def available_libraries():
    return dict(SHORTCUTS)


def _library_path(name):
    return ROOT / "algorithm" / name / "main.cpp"


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
    library_name = SHORTCUTS.get(name, name)
    if library_name not in _LIBRARY_NAMES:
        raise ValueError(f"未登録のライブラリです: {name}")
    library_path = _library_path(library_name)
    target_path = Path(target)
    if not target_path.exists():
        raise FileNotFoundError(f"追加先が見つかりません: {target_path.resolve()}")
    if not library_path.exists():
        raise FileNotFoundError(f"ライブラリが見つかりません: {library_path}")

    current = target_path.read_text(encoding="utf-8")
    library = library_path.read_text(encoding="utf-8")
    merged = merge_cpp(current, library)
    if merged == current:
        print(f"追加済み: {library_name}")
        return False
    _atomic_write(target_path, merged)
    print(f"追加: {library_name} -> {target_path}")
    return True


def dfs():
    return load_library("DFS")


def bfs():
    return load_library("BFS")


def bfs01():
    return load_library("BFS01")


def dij():
    return load_library("Dijkstra")


def BF():
    return load_library("BellmanFord")


def WF():
    return load_library("WarshallFloyd")


def Kruskal():
    return load_library("Kruskal")


def UF():
    return load_library("UnionFind")


def RUF():
    return load_library("RollbackUnionFind")


def SCC():
    return load_library("SCC")


def TwoSat():
    return load_library("TwoSat")


def seg():
    return load_library("SegTree")


def lseg():
    return load_library("LazySegTree")


def pseg():
    return load_library("PersistentSegTree")


def plseg():
    return load_library("PersistentLazySegTree")


def Treap():
    return load_library("ImplicitTreap")


def Trie():
    return load_library("Trie")


def BTrie():
    return load_library("BinaryTrie")


def Aho():
    return load_library("AhoCorasick")


def BIT():
    return load_library("BIT")


def Cum():
    return load_library("CumulativeND")


def MF():
    return load_library("MaxFlow")


def DLL():
    return load_library("DoublyLinkedList")


def Era():
    return load_library("Eratosthenes")


def GCD():
    return load_library("GCD")


def extGCD():
    return load_library("ExtendedGCD")


def LCM():
    return load_library("LCM")


def CRT():
    return load_library("CRT")


def DIG():
    return load_library("Digit")


def LCA():
    return load_library("LCA")


def HLD():
    return load_library("HLD")


def Reroot():
    return load_library("Rerooting")


def RH():
    return load_library("RollingHash")


def ZH():
    return load_library("ZobristHash")


def Mo():
    return load_library("Mo")


def MP():
    return load_library("ModPow")


def MPD():
    return load_library("ModPowDivision")


def Comb():
    return load_library("Combinatorics")


def Comp():
    return load_library("Compress")


def RS():
    return load_library("RangeSet")


def Grid():
    return load_library("Grid")


def Mat():
    return load_library("Matrix")


def Geom():
    return load_library("Geometry")
