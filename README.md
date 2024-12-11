# Software Rasterizer


# 概要

- GPUを使用しないラスタライザーの実装
- ３Ｄモデルを描画
- GUIに Win32 API を使用


# やり残したこと

- ラスタライザの左上ルール（left top rule）
- バイリニアフィルタ


## 参考資料

Sutherland-Hodgman algorithm
Ivan Sutherland, Gary W. Hodgman: Reentrant Polygon Clipping. Communications of the ACM, vol. 17, pp. 32-42, 1974
凸形状によるクリップのアルゴリズム

CLIPPING USING HOMOGENEOUS COORDINATES
Blinn & Newell（1978）Clipping Using Homogeneous Coordinates - section 2. CLIPPING
SIGGRAPH '78: Proceedings of the 5th annual conference on Computer graphics and interactive techniques Pages 245 - 251
クリップ空間でのクリップについて

A Parallel Algorithm for Polygon Rasterization 
Juan Pineda 1988 
ラスタライズの並列化、あと重心座標も


## メモ(知らなかったこと)

Wで割ったあとは正規化デバイス座標（NDC）と呼ぶ

視体積クリップはクリップ空間でやる
（NDC以降では出来ない）

クリップ空間座標系で線形補間をしても良い。
（W値は=-zなので線形）

パースペクティブコレクトは、デプス値に対しては不要。

フェイスカリングはクリップ空間では出来ない。
W除算前のクリップ空間では、表裏が逆になるケースがある。
スクリーン空間は処理系によっては上下逆になる。

ラスタライズ時、ピクセルの中心は 0.5 。
ピクセルの中心がポリゴンの上にあれば色を打ち込む
ただし、右端（下辺）が 0.5 丁度だと、1ピクセル多くなるので、調整（左上ルール）が必要。

