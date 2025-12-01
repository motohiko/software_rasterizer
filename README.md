# ソフトウェアラスタライザー

## 概要

- GPUを使用しないラスタライザーの実装


## 描画処理のフロー

頂点データ  
↓  
インプットアセンブリ  
（データを整形）  
↓  
頂点シェーダー  
（クリップ空間に変換）  
↓  
クリッピング  
（-w ～ +w の範囲）
↓  
正規化デバイス座標変換  
（w除算）
↓  
フェイスカリング  
↓  
ビューポート変換  
（ウィンドウ空間に変換）  
↓  
ラスタライズ  
↓  
フラグメントシェーダー  
（ピクセルのカラーを決定）  
↓  
デプステスト  
↓  
レンダーターゲット



## やり残したこと

- 左上ルール


## 参考資料

- 凸形状によるクリップのアルゴリズム
Sutherland-Hodgman algorithm
Ivan Sutherland, Gary W. Hodgman: Reentrant Polygon Clipping.   Communications of the ACM, vol. 17, pp. 32-42, 1974

- クリップ空間でのクリップについて
CLIPPING USING HOMOGENEOUS COORDINATES
Blinn & Newell（1978）Clipping Using Homogeneous Coordinates - section 2. CLIPPING
SIGGRAPH '78: Proceedings of the 5th annual conference on Computer graphics and interactive techniques Pages 245 - 251

- ラスタライズの並列化、重心座標  
A Parallel Algorithm for Polygon Rasterization
Juan Pineda 1988

- パースペクティブコレクト
Interpolation for Polygon Texture Mapping and Shading
Paul S. Heckbert & Henry P. Moreton, 1990

