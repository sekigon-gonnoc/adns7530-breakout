# adns7530-breakout
マウス用のICであるADNS-7530とSTM32マイコンをセットにした評価基板です。  
デフォルトのファームウェアではSTM32がI2Cスレーブ・SPIマスターとして動作し、SPIでADNS-7530から読み取ったxy方向の動きをI2Cで送信します。  
3Dプリンタ等でボールの保持部を作成することでトラックボールモジュールとして使うことも出来ます。

## 注意事項
- レーザーの出力部を直視しないでください

## 内容物
- 基板
- マウスIC(ADNS-7530)
- レンズ
- ピンヘッダ
- おまけ
  - セラミックボール
  - ネジ類

## 組み立て
- マウスICを基板に取り付けてはんだ付けする
- マウスICの保護シールを剥がし、レンズを取り付ける
- 必要に応じてレンズを接着剤等で固定する


## 使い方
- I2Cスレーブ(7bitアドレス:0x1E)として動作します
  - I2Cでデータを5byte読み出すと以下の情報を送信します。データの詳細についてはADNS-7530のデータシートを参照してください。
  
    | バイト | 内容              |
    | ------ | ----------------- |
    | 1      | 0                 |
    | 2      | 動作状態(Motion)  |
    | 3      | x方向(下位8bit)   |
    | 4      | y方向(下位8bit)   |
    | 5      | 5方向(各上位4bit) |
    
- QMKで使う場合、keymap.cに通信用のコードとポインティングデバイスの送信用コードを追加してください
  - Pro Micro/BLE Micro Proで使う場合のサンプルコードは以下にあります
    - [Pro Micro用](https://github.com/sekigon-gonnoc/qmk_firmware/tree/dev/ble_micro_pro/keyboards/helix/rev2/keymaps/tb_module)
    - [BLE Micro Pro用](https://github.com/sekigon-gonnoc/qmk_firmware/tree/dev/ble_micro_pro/keyboards/ble_micro_pro/keymaps/tb_module)

## 作例
### Helixのトラックボール拡張として使う
HelixのOLEDモジュール取り付け用のピンヘッダに本モジュールを取り付けてトラックボール拡張として動作させます

<img src="https://user-images.githubusercontent.com/43873124/71624362-7ebdc700-2c25-11ea-87c7-bde34b2fab53.png" width="320">

- タミヤのボールキャスターをボールホルダーとして使い、ネジで取り付けます
- 右手に付ける場合には基板のRで囲われた穴に、左手に付ける場合はLの穴にピンヘッダを差し込みます
- モジュールの取り付けを安定させるため、また、Pro Micro等のピンヘッダと基板上の端子等が接触しないようにするため、モジュールとPro Microの間に薄く切ったゴム足などを貼り付けて高さを調整します
- 調整ができたらピンヘッダをはんだ付けします
- サンプルプログラムを書き込んで動作を確認します
- 反応が悪い場合ボールキャスターの位置を調整してください
