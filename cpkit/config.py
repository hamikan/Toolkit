"""リポジトリの.envからコンパイル設定を読み込む。"""

from dataclasses import dataclass
import os
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Config:
    root: Path
    cxx: str
    sysroot: Path | None
    acl_path: Path | None

    @property
    def compile_flags(self):
        flags = ["-O2", "-std=c++23"]
        if self.sysroot is not None:
            flags.extend(["-isysroot", str(self.sysroot)])
        if self.acl_path is not None:
            flags.extend(["-I", str(self.acl_path)])
        return flags


def load_env(path):
    path = Path(path)
    values = {}
    if not path.exists():
        return values
    with path.open("r", encoding="utf-8") as file:
        for line_number, line in enumerate(file, 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if "=" not in line:
                raise ValueError(
                    f"{path}:{line_number}: KEY=VALUEの形式で指定してください"
                )
            key, value = line.split("=", 1)
            key, value = key.strip(), value.strip()
            if not key:
                raise ValueError(f"{path}:{line_number}: KEYを指定してください")
            if len(value) >= 2 and value[0] == value[-1] and value[0] in "\"'":
                value = value[1:-1]
            values[key] = value
    return values


def _resolve_path(value, root):
    if not value:
        return None
    path = Path(value).expanduser()
    if not path.is_absolute():
        path = root / path
    return path.resolve()


def load_config(root=ROOT):
    root = Path(root).resolve()
    values = load_env(root / ".env")
    return Config(
        root=root,
        cxx=os.path.expanduser(values.get("CXX", "")),
        sysroot=_resolve_path(values.get("SYSROOT", ""), root),
        acl_path=_resolve_path(values.get("ACL_PATH", ""), root),
    )


CONFIG = load_config()
