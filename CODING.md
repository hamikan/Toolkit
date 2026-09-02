# Coding Rules

## ライブラリ構成

1つの独立したアルゴリズム実装につき、1つのディレクトリを使用します。

```text
algorithm/
├── UnionFind/
│   └── main.cpp
└── RollbackUnionFind/
    └── main.cpp
```

通常版、Rollback版、永続版など、内部構造や制約が異なる実装は別の
ライブラリとして管理します。

ライブラリ本体のファイル名は`main.cpp`に統一します。
`algorithm/<LibraryName>/main.cpp`にはライブラリ実装のみを記述し、
`int main()`は記述しません。

## struct

ライブラリは`struct`で定義します。

内部データと内部関数を上部の`private`、公開APIを下部の`public`に配置します。

```cpp
struct Example {
private:
    int value_;

public:
    explicit Example(int value) : value_(value) {}

    int value() const {
        return value_;
    }
};
```

関数の宣言と定義は分離せず、構造体内に実装を記述します。

## privateメンバ

外部から変更されると不変条件が壊れるデータは`private`にします。
privateメンバ変数の名前には末尾のアンダースコアを付けます。

```cpp
vector<int> parent_;
int group_count_;
```

## コンストラクタ

引数が1つのコンストラクタには、原則として`explicit`を付けます。

```cpp
explicit UnionFind(int N) {
    init(N);
}
```

型の初期値には`T{}`を使用します。

## constとstatic

内部状態を変更しないメンバ関数には`const`を付けます。

```cpp
int group_count() const {
    return group_count_;
}
```

インスタンスの状態を使用しない演算は`static`にします。
`[[nodiscard]]`は使用しません。

## assert

ライブラリの前提条件は`assert`で検査します。

メッセージは次の形式に統一します。

```text
ClassName::function_name: expected condition
```

```cpp
assert(0 <= N && "UnionFind::init: 0 <= N");
assert(0 <= x && "UnionFind::root: 0 <= x");
assert(x < N_ && "UnionFind::root: x < N_");
```

エラーが発生した公開関数を判別できるよう、メッセージには公開関数名を
使用します。

## 整数型

要素数や添字を`int`で扱う場合は、コンテナの`size()`を明示的に変換します。

```cpp
int N = static_cast<int>(v.size());
```

## 区間

区間を扱うAPIは、原則として半開区間`[l, r)`に統一します。

## 戻り値とコンテナへの追加

返り値の型が決まっている場合は`{}`を使用します。

```cpp
return {a, b};
```

コンテナへ要素を構築しながら追加する場合は`emplace_back()`を使用します。

```cpp
result.emplace_back(a, b);
```

## 関数の並び順

同系統の既存ライブラリに合わせ、原則として次の順に配置します。

1. privateの型エイリアス、メンバ変数、内部関数
2. コンストラクタ
3. `init()`
4. 構築前の更新（`add_edge()`、`insert()`、`set()`など）
5. `build()`
6. サイズなどの基本情報
7. 更新、取得、クエリ
8. 探索、補助機能、デバッグ表示

構築前の更新を必要としない場合は、`build()`をコンストラクタの直後に
配置します。

既存ファイルを修正するときは、無関係な空行や関数順を変更しません。
