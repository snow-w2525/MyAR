## MyARの使い方
	- 設定はすべてcamera-config.jsonで行うこと。
	　このファイルの読み込みは、起動時だけなので、設定を変更して反映したい場合は、一度アプリを落とすこと。

### JSONファイルの設定

#### 共通設定
	- BACKGROUND_TYPE : string
		windowの背景を何にするか。cameraかimageかを選択する
	
	- CAMERA_TITLE : string
		windowのタイトル。

	- CAMERA_NO : int
		BACKGROUND_TYPEがcameraのときに、どのカメラを使用するかを番号で選択する。

	- BACKGROUND_IMAGE_NAME : string
		BACKGROUND_TYPEがimageのときに、どの画像を背景に挿入するかを設定する。

	- BACKGROUND_WIDTH : int
		windowの幅を設定する。

	- BACKGROUND_HEIGHT : int
		windowの高さを設定する。

#### ARリソース設定
	- COMBINE : string
		windowに。ARリソースをどう配置するか。
		chromakey、perspective、perspective + chromakeyから選択する。

	- ARUCO : bool
		BACKGROUND_TYPEがcameraのとき、ARUCOマーカー上にレンダリングするかを選択する。
		ARUCOマーカー上にレンダリングするので、COMBINEをperspective + chromakeyにすること。

	- ARUCO_VERTICAL : bool
		ARUCOがtrueのとき、リソースをマーカー上に対して垂直に配置するか、
		水平に配置するかを選択する。


