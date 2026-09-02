"""競技プログラミング用の対話コマンドをPYTHONSTARTUPへ読み込む。"""

import importlib as _importlib
import os as _os
from pathlib import Path as _Path
import sys as _sys


_startup_file = globals().get("__file__") or _os.environ.get("PYTHONSTARTUP")
if not _startup_file:
    raise RuntimeError("startup.pyのパスを特定できません")
_root = _Path(_startup_file).expanduser().resolve().parent
if str(_root) not in _sys.path:
    _sys.path.insert(0, str(_root))

_exported_names = set()


def _load_cpkit(reload_modules=False):
    global _exported_names
    if reload_modules:
        module_names = [
            name
            for name in tuple(_sys.modules)
            if name == "cpkit" or name.startswith("cpkit.")
        ]
        for name in module_names:
            del _sys.modules[name]
    module = _importlib.import_module("cpkit")
    for name in _exported_names:
        globals().pop(name, None)
    for name in module.__all__:
        globals()[name] = getattr(module, name)
    _exported_names = set(module.__all__)


def rl():
    _load_cpkit(reload_modules=True)


_load_cpkit()
