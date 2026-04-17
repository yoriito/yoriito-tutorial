# ref_pal

[English README is here](./README.md)

## 概要
Unity WebGLで生成した3D車両ウィジェットをElectronアプリとして動作させるためのリファレンス実装です。

## ディレクトリ構成
```
ref_pal/
  ├── main.js            # Electronのエントリポイント
  ├── preload.js         # ページが読み込まれた直後に実行されるスクリプト
  ├── assets             # 雑多なjsなどをまとめたディレクトリ
  ├── config/            # 設定・マップ類（VSS→Unity, ターゲット定義, 定数など）
  ├── fsm/               # Door/Trunk/Window/Mirror等の状態遷移（FSM）
  ├── viss/              # VISS(gRPC)接続・Subscribe処理関連のスクリプト
  ├── window/            # BrowserWindow生成など、Window関連のスクリプト
  ├── utils/             # 補助的な関数群（Unityメッセージ生成など）
  ├── TemplateData/      # ../external/widget-3d-car-unity/TemplateDataへのシンボリックリンク
  ├── Build/             # ../external/widget-3d-car-unity/Buildへのシンボリックリンク
  └── proto/             # ../external/yoriito-viss/protoへのシンボリックリンク
```

## セットアップ/デバッグ実行

```bash
$ npm install
$ ./init_pal.sh
$ npx electron . --no-sandbox
```

## ビルド・配布

以下のコマンドでdistディレクトリに*.debを生成します  

```bash
$ npm run dist
$ sudo dpkg -i dist/unity-electron-app_1.0.0_amd64.deb
$ /opt/UnityElectronApp/unity-electron-app
```

## 車両のふるまいについて

`ref_pal` では、ひとつの装備に対して、複数の信号で操作できます。  
信号の組み合わせと装備の動きの対応を明確にするため、本節では状態遷移表として車両のふるまいをまとめます。

### IsLocked/IsOpen/Positionが共存する信号の状態遷移

#### 対象信号

- Vehicle.Body.Trunk.Front
- Vehicle.Body.Trunk.Rear
- Vehicle.Cabin.Door.Row1.DriverSide
- Vehicle.Cabin.Door.Row1.PassengerSide
- Vehicle.Cabin.Door.Row2.DriverSide
- Vehicle.Cabin.Door.Row2.PassengerSide

#### 取りうる状態

- **S0**: (IsLocked=F, IsOpen=F, Position=0) → アンロック閉
- **S3**: (IsLocked=F, IsOpen=T, Position>0) → アンロック開
- **S4**: (IsLocked=T, IsOpen=F, Position=0) → ロック閉

#### 状態遷移表

| Current            | Target: IsLocked = F | Target: IsLocked = T | Target: IsOpen = F | Target: IsOpen = T | Target: Position = 0 | Target: Position > 0  |
| ------------------ | -------------------- | -------------------- | ------------------ | ------------------ | -------------------- | --------------------- |
| S0（アンロック閉） | S0                   | S4                   | S0                 | S3（Position=100） | S0                   | S3（Position=指定値） |
| S3（アンロック開） | S3                   | S3                   | S0（Position=0）   | S3（Position=100） | S0                   | S3（Position=指定値） |
| S4（ロック閉）     | S0                   | S4                   | S4                 | S4                 | S4                   | S4                    |

#### 補足情報

- `Set(IsOpen=true)` は 「全開要求」として扱う（結果は Position=100）
- `Set(Position=p)` は p をそのまま採用（途中開け可能）
- ロック中（S4）に開く要求（IsOpen=true / Position>0）は拒否
- 開いてる間（S3）はロック要求（IsLocked=true）を拒否（維持）

### IsOpen/Positionが共存する信号の状態遷移

#### 対象信号

- Vehicle.Cabin.Door.Row1.DriverSide.Window
- Vehicle.Cabin.Door.Row1.PassengerSide.Window
- Vehicle.Cabin.Door.Row2.DriverSide.Window
- Vehicle.Cabin.Door.Row2.PassengerSide.Window

#### 取りうる状態

- **W0**: (IsOpen=F, Position=0) → 全閉
- **W1**: (IsOpen=T, Position>0) → 開（途中〜全開）

#### 状態遷移表

| Current                | Target: IsOpen = F | Target: IsOpen = T | Target: Position = 0 | Target: Position > 0  |
| ---------------------- | ------------------ | ------------------ | -------------------- | --------------------- |
| W0（全閉）             | W0                 | W1（Position=100） | W0                   | W1（Position=指定値） |
| W1（開（途中〜全開）） | W0                 | W1                 | W0                   | W1（Position=指定値） |

#### 補足事項

- `Set(IsOpen=true)` は「全開要求」として扱う
- 途中開けにしたいなら `Set(Position=p)` を使う

### IsLocked/IsFoldedが共存する信号の状態遷移

#### 対象信号

- Vehicle.Body.Mirrors.DriverSide
- Vehicle.Body.Mirrors.PassengerSide

#### 取りうる状態

- **M0**: (IsLocked=F, IsFolded=F) → 展開状態でアンロック
- **M1**: (IsLocked=F, IsFolded=T) → 格納状態でアンロック
- **M2**: (IsLocked=T, IsFolded=F) → 展開状態でロック
- **M3**: (IsLocked=T, IsFolded=T) → 格納状態でロック

#### 状態遷移表

| Current                    | Target: IsLocked = F | Target: IsLocked = T | Target: IsFolded = F | Target: IsFolded = T |
| -------------------------- | -------------------- | -------------------- | -------------------- | -------------------- |
| M0（展開状態でアンロック） | M0                   | M2                   | M0                   | M1                   |
| M1（格納状態でアンロック） | M1                   | M3                   | M0                   | M1                   |
| M2（展開状態でロック）     | M0                   | M2                   | M2                   | M2                   |
| M3（格納状態でロック）     | M1                   | M3                   | M3                   | M3                   |

#### 補足事項

- ロック中は Fold/Unfold を受け付けない
- ロック自体はミラーの格納/展開状態を変えない
