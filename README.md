# 🥊 BoxingMod — Beat Saber Quest

Beat Saber 1.37.0_9064817954 向けQuestモード。セイバーをボクシンググローブに変換し、パンチ速度でスコアが決まる新しいゲームプレイを提供します。

---

## ✨ 機能

| 機能 | 詳細 |
|------|------|
| ボクシンググローブ | 左右のセイバーをリアルなグローブ形状に置き換え（赤=右、青=左） |
| パンチスコア | 速度に応じたスコア計算（最大115点） |
| 完璧なパンチ | 約8m/s以上のパンチ速度で満点115点 |
| 速度スケール | 遅いほど得点が減少。0.5m/s以下で0点 |
| No Arrow強制 | Boxingモード中はNo Arrowモディファイヤが自動適用 |
| QuestUI設定 | メインメニューからON/OFFを切り替え可能 |

---

## 📊 スコア計算式

```
speed >= 8.0 m/s  → 115点 (満点)
speed: 0.5 ~ 8.0  → 線形補間 (0 ~ 115点)
speed < 0.5 m/s   → 0点
```

### 速度の目安
| パンチ速度 | 得点 |
|-----------|------|
| 8.0 m/s以上 | 115点 🥇 |
| 6.0 m/s | ~86点 |
| 4.0 m/s | ~57点 |
| 2.0 m/s | ~29点 |
| 0.5 m/s以下 | 0点 |

---

## 📦 インストール方法

### 必要なもの
- Beat Saber Quest版 **1.37.0_9064817954**
- [BMBF](https://bmbf.app/) または [ModsBeforeFriday](https://mbf.bsquest.xyz/)
- Scotland2 モッドローダー

### 手順
1. `BoxingMod-1.0.0.qmod` をダウンロード
2. BMBFをブラウザで開く (`http://10.0.2.2:50000` または Quest IPアドレス)
3. **Browser** タブ → QMODファイルをドラッグ＆ドロップ
4. Beat Saberを再起動
5. メインメニュー → **Mods** → **Boxing Mod** でON/OFF切り替え

---

## 🔨 ビルド方法（開発者向け）

### 必要なもの
- Windows 10/11
- [Android NDK r25c](https://developer.android.com/ndk/downloads)
- [qpm-rust](https://github.com/QuestPackageManager/QPM.CLI)
- CMake 3.22+
- Ninja

### ビルド手順

```powershell
# 1. 依存関係のインストール
qpm-rust restore

# 2. ビルド
./build.ps1

# 3. ビルド + QMODパッケージ作成
./build.ps1 -Package

# 4. クリーンビルド + パッケージ
./build.ps1 -Clean -Package
```

### プロジェクト構成

```
BoxingMod/
├── src/
│   ├── main.cpp                 # フック登録・エントリポイント
│   ├── BoxingConfig.cpp         # 設定 (config-utils)
│   ├── BoxingScoreController.cpp # パンチ速度スコア計算
│   ├── GloveManager.cpp         # グローブメッシュ生成
│   ├── UIManager.cpp            # QuestUI設定画面
│   └── NoArrowEnforcer.cpp      # No Arrow強制ロジック
├── include/
│   ├── BoxingConfig.hpp
│   ├── BoxingScoreController.hpp
│   ├── GloveManager.hpp
│   ├── NoArrowEnforcer.hpp
│   └── UIManager.hpp
├── mod.json                     # QMODメタデータ
├── qpm.json                     # 依存関係定義
├── CMakeLists.txt               # ビルド定義
└── build.ps1                    # ビルドスクリプト
```

---

## ⚙️ フック一覧

| フック | 対象 | 処理 |
|--------|------|------|
| `Hook_SaberStart` | `Saber::Start` | グローブメッシュを適用 |
| `Hook_AddNewData` | `SaberMovementData::AddNewData` | 毎フレームの速度を記録 |
| `Hook_CutScoreFinish` | `CutScoreBuffer::HandleSaberSwingRatingCounterDidChange` | スコアをパンチ速度で上書き |
| `Hook_GetGameplayModifiers` | `GameplayModifiers::get_noArrows` | Boxing中はNoArrowをtrueに強制 |

---

## 🔧 設定値

`/sdcard/ModData/com.beatgames.beatsaber/Mods/BoxingMod/BoxingConfig.json` に保存：

```json
{
  "BoxingEnabled": false,
  "MaxPunchScore": 115.0,
  "PerfectPunchSpeed": 8.0,
  "MinPunchSpeed": 0.5
}
```

---

## 📝 既知の制限

- グローブは Unity プリミティブ（Cube+Sphere）で構成されるため、カスタムセイバーほどリアルではありません。将来的にAssetBundleによる本格的な3Dグローブに対応予定。
- スコア計算はノートカット時の速度スナップショットを使用しており、ヒット直前のピーク速度を参照します。

---

## 📄 ライセンス
MIT License
