# Competitive Programming Library

競技プログラミング用の個人ライブラリです。

`startup.py`を`PYTHONSTARTUP`から読み込み、各ライブラリを作業中の
`main.cpp`へ展開して使用します。

## 必要なもの

- Python 3.10以降
- C++23に対応したコンパイラ
- [AtCoder Library](https://github.com/atcoder/ac-library)
- `pyperclip`（クリップボード機能を使う場合のみ）

`pyperclip`は次のコマンドでインストールします。

```zsh
python3 -m pip install pyperclip
```

## 環境設定

`.env.example`を`.env`へコピーします。

```zsh
cp .env.example .env
```

環境に合わせて3つの値を設定します。

```env
CXX=/path/to/g++
SYSROOT=
ACL_PATH=/path/to/ac-library
```

- `CXX`：使用するC++コンパイラ
- `SYSROOT`：SDKのパス。不要な環境では空にする
- `ACL_PATH`：`atcoder/`を含むAtCoder Libraryのディレクトリ

`SYSROOT`と`ACL_PATH`に相対パスを指定した場合は、`startup.py`がある
ディレクトリを基準に解決します。

## PYTHONSTARTUP

使用しているシェルの設定ファイルへ、`startup.py`のパスを追加します。

```zsh
export PYTHONSTARTUP="/path/to/repository/startup.py"
```

zshの場合は`~/.zshrc`へ記述し、設定を読み直します。

```zsh
source ~/.zshrc
```

以降はPythonのインタープリタを起動すると、`cpkit`の関数を使用できます。

```zsh
python3
```

## ライブラリの展開

作業中のディレクトリに`main.cpp`を用意し、Pythonのインタープリタから
使用するライブラリの関数を呼び出します。

```python
UF()
seg()
BIT()
```

正式名から追加することもできます。

```python
load_library("UnionFind")
```

ライブラリの`#include`、`using`、本体が、既存の記述順を保ったまま
`main.cpp`へ追加されます。同じ内容が既に存在する場合は追加しません。
正常に完了すると`成功`、失敗すると`失敗: ライブラリまでの相対パス`を
表示します。

ライブラリの一覧と使い方は[algorithm/README.md](./algorithm/README.md)から
確認できます。

ライブラリの関数、引数、戻り値、計算量をすぐに確認する場合は`how()`を
使用します。READMEの本文は表示せず、ファイルのパスとクイックリファレンスを
表示します。

```python
how(UF)
how(UnionFind)
```

`startup.py`または`cpkit/`を編集した後は、次の関数で読み直せます。

```python
rl()
```

## ランダムテスト

次の関数を呼ぶと、現在のディレクトリに`randomTest/input.cpp`と
`randomTest/naive.cpp`を作成します。既に存在するファイルは変更しません。

```python
rt()
```

テスト回数を指定すると、入力生成、愚直解、`main.cpp`をコンパイルして
出力を比較します。`rt(n)`は失敗ケースを保存しません。
`randomTest/input.cpp`または`randomTest/naive.cpp`が存在しない場合は、
既存ファイルを変更せず、不足しているテンプレートだけを自動作成します。

```python
rt(100)
```

失敗ケースを保存する場合は`rts()`を使用します。`main.cpp`が誤答、REまたは
TLEになった場合、その入力と愚直解の出力を`test/sample-n.in`と
`test/sample-n.out`へ保存します。`n`には、既存の`sample-n.in`または
`sample-n.out`と重ならない最小の正整数を使用します。

```python
rts(100)
```

第2引数で、各プログラムのタイムアウト秒数を変更できます。既定値は2秒です。

```python
rt(100, 5)
rts(100, 5)
```

コンパイルには`.env`の設定と、`-O2 -std=c++23`を使用します。
実行ファイルが存在し、対応する`.cpp`と`.env`のどちらも更新されていない
場合は、そのファイルの再コンパイルを省略します。
ビルドと実行結果は`BUILD`、`OK`、`WA`、`RE`、`TLE`、`CE`のラベル付きで
表示し、コンパイルに失敗した場合はその時点で中止します。
`naive.cpp`または`main.cpp`がRE・TLEになった場合は、ケース番号と入力も
表示します。

## テスト

Python側の単体テストと、C++を実際にコンパイルするランダムテストの
統合テストにはpytestを使用し、モックにはpytest-mockを使用します。
最初にリポジトリのルートで仮想環境を作成し、有効化します。

```zsh
python3 -m venv .venv
source .venv/bin/activate
```

仮想環境へ開発用の依存をインストールし、テストを実行します。

```zsh
python -m pip install --upgrade pip
python -m pip install -r requirements-dev.txt
python -m pytest
```

2回目以降は`source .venv/bin/activate`と`python -m pytest`だけで実行できます。
終了時は`deactivate`で仮想環境を抜けます。

GitHub Actionsでも、push、pull request、手動実行時にPython 3.10と3.14の
両方で同じテストを実行します。

## ディレクトリ構成

```text
algorithm/<LibraryName>/main.cpp
cpkit/
src/input.cpp
src/naive.cpp
startup.py
tests/
```

各ライブラリの実装規則は[CODING.md](./CODING.md)に記載しています。
