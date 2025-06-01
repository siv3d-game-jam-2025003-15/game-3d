# pragma once
#include "Common.hpp"
#include "PhiController.hpp"
#include "ObjectController.hpp"
#include "Collision.hpp"

// ポイントライトVer.1
//struct Light
//{
//	Float3 g_pointLightPos;
//	float g_pointLightStrong;
//	float _padding;
//};

// ポイントライトVer.2
struct PSLighting
{
	static constexpr uint32 MaxPointLights = 4;

	struct Light
	{
		Float4 position{ 0, 0, 0, 0 };
		Float4 diffuseColor{ 0, 0, 0, 0 };
		Float4 attenuation{ 1.0f, 2.0f, 1.0f, 0 };
	};

	std::array<Light, MaxPointLights> pointLights;

	/// @brief 点光源を設定します。
	/// @param i 光源のインデックス。0 以上 MaxPointLights 未満である必要があります。
	/// @param pos 光源の位置
	/// @param diffuse 色
	/// @param r 強さ
	void setPointLight(uint32 i, const Vec3& pos, const ColorF& diffuse, double r)
	{
		pointLights[i].position = Float4{ pos, 1.0f };
		pointLights[i].diffuseColor = diffuse.toFloat4();
		pointLights[i].attenuation = Float4{ 1.0, (2.0 / r), (1.0 / (r * r)), 0.0 };
	}

	/// @brief 点光源を球として描画します。
	/// @param i 光源のインデックス。0 以上 MaxPointLights 未満である必要があります。
	/// @param r 球の半径
	void drawPointLightAsEmissiveSphere(uint32 i, double r)
	{
		const Vec3 pos = pointLights[i].position.xyz();
		const ColorF diffuse{ pointLights[i].diffuseColor };

		PhongMaterial phong;
		phong.ambientColor = ColorF{ 0.0 };
		phong.diffuseColor = ColorF{ 0.0 };
		phong.emissionColor = diffuse;
#ifdef _DEBUG
		Sphere{ pos, r }.draw(phong);
#endif
	}
};

// アイテムID
enum ItemID
{
	Bread,	// 0 パン
	Memo,	// 1 手記
	Key,	// 2 錆びた鍵
	Poker,	// 3 火かき棒
	Parchment, // 4 羊皮紙
	Wire, // 5 針金（ハンガー）
	DirtyCloth,	// 6 汚れた布
	Cloth,	// 7 布
	ToastedParchment,	// 8 炙った羊皮紙
	WireKey, // 9 針金製の鍵
	IronKey,	// 10 鉄製の鍵
};

// カメラの実験
class CameraTest : public App::Scene
{
public:

	// コンストラクタ
	CameraTest(const InitData& init);

	// 処理
	void update() override;

	// 描画
	void draw() const override;

private:

	// リソースを読み込む
	void loadResources();

	// ドアの数
	static const int DoorNum = 4;

	// ポイントライトVer.2
	ConstantBuffer<PSLighting> constantBuffer;

	// デバッグ表示
	void debug();

	// マウスポインタのRay
	Ray getMouseRay() const;

	// インベントリ：アイテムのミニ画像
	void drawMiniItem(
		int itemId,
		int x,
		int y
	) const;

	// インベントリ：アイテムの拡大画像
	void drawBigItem(
		int itemId,
		int x,
		int y
	) const;

	// カメラ
	BasicCamera3D camera;

	// カメラスピード
	const double cameraSpeed = 2.0f;

	// HDR（ハイダイナミックレンジ）レンダリング
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R16G16B16A16_Float, HasDepth::Yes };

	// 地面
	//Texture groundTexture;
	//Mesh groundPlane;

	// 背景色
	ColorF backgroundColor;

	//double m_fastMove = 1.0;

	//bool m_padPressed = false;

	// カメラの視点（現在）
	Vec3 m_eyePosition = Vec3{ 2.5, 1.5, 10 };

	// カメラの視点（次）
	Vec3 toCameraPos = m_eyePosition;

	// カメラの視点（1フレーム前）
	Vec3 last_eyePosition = Vec3{ 2.5, 1.5, 10 };

	// カメラの注視点（カメラが「いま見ている」場所のこと）
	Vec3 m_focusPosition = Vec3{ 0, 0, -1 };

	// カメラの上方向ベクトル
	Vec3 m_upDirection = Vec3{ 0, 1, 0 };

	//Size m_sceneSize = Scene::Size();

	// 視野角
	double m_verticalFOV = 55_deg;
	
	// 近クリッピング面（Near Clipping Plane）
	//double m_nearClip = 0.2;
	double m_nearClip = 0.01;

	// 開始時の目の角度
	double m_focusY = 0;
	double to_m_focusY = 0;

	// カメラの角度
	PhiController phiController = PhiController(m_eyePosition, m_focusPosition);

	//==============================
	// モデル
	//==============================

	// モデルの読み込み
	const Model model{ U"assets/models/Room/EV_Room01.obj" };

	// ドア
	const Model modelDoor{ U"assets/models/Room/Gimmick_FenceDoor01.obj" };

	// 錆びた鍵
	const Model modelKey{ U"assets/models/Key/Gimmick_RustedKey01.obj" };

	// 鉄製の鍵
	const Model modelIronKey{ U"assets/models/Key/Gimmick_RustedKey02.obj" };

	// パン
	const Model modelBread{ U"assets/models/Bread/Gimmick_bread01.obj" };

	// 火かき棒
	const Model modelPoker{ U"assets/models/Poker/Gimmick_Poker01.obj" };

	// 引き出し
	const Model modelDrawerChain{ U"assets/models/Drawer/Gimmick_Drawer01_Chain.obj" };
	const Model modelDrawerEye{ U"assets/models/Drawer/Gimmick_Drawer01_Eye.obj" };
	const Model modelDrawerFeather{ U"assets/models/Drawer/Gimmick_Drawer01_Feather.obj" };
	const Model modelDrawerFlower{ U"assets/models/Drawer/Gimmick_Drawer01_Flower.obj" };
	const Model modelDrawerNon{ U"assets/models/Drawer/Gimmick_Drawer01_Non.obj" };
	const Model modelDrawerSnake{ U"assets/models/Drawer/Gimmick_Drawer01_Snake.obj" };
	const Model modelShelf{ U"assets/models/Drawer/Gimmick_Shelf01.obj" };

	// ビックリマーク
	const Model modelExclamationMark{ U"assets/models/ExclamationMark/ExclamationMark.obj" };

	// 羊皮紙
	const Model modelParchment{ U"assets/models/Parchment/Gimmick_Parchment01.obj" };

	// ハンガー
	const Model modelHanger{ U"assets/models/Hangar/Gimmick_Hangar01.obj" };
	
	// 汚れた布
	const Model modelDirtyCloth{ U"assets/models/Cloth/Gimmick_Cloth01.obj" };

	// 手記
	const Model modelMemo{ U"assets/models/Memo/memo.obj" };


	//==============================
	// スケーリング
	//==============================

	// スケーリングの倍率
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };


	//==============================
	// BGM
	//==============================

	// BGMがストップしている時間
	double bgmStopCount = 0.0f;



	//==============================
	// ドア関係
	//==============================

	// ドアの回転
	Vec3 doorRot{ 0, 180_deg, 0 };	// 最初の扉
	Vec3 door2Rot{ 0, 180_deg, 0 };	// 左下の部屋の扉
	//double toDoorRotY = 0;

	// ドアの移動（横に開ける）
	double toDoorPosX = 0;
	double toDoor2PosX = 0;

	// ドアが開いているかどうかのフラグ
	bool bDoorOpen[DoorNum];

	//==============================
	// シェーダー
	//==============================

	//const PixelShader psBright = HLSL{
	//	U"example/shader/hlsl/extract_bright_linear.hlsl",
	//	U"PS"
	//}
	//	| GLSL{
	//		U"example/shader/glsl/extract_bright_linear.frag",
	//		{
	//			{U"PSConstants2D", 0}
	//		}
	//};

	// 頂点シェーダ
	const VertexShader vs3D = GLSL{
		U"example/shader/glsl/default3d_forward.vert",
		{
			{ U"VSPerView", 1 },
			{ U"VSPerObject", 2 },
			{ U"VSPerMaterial", 3 }
		}
	};

	// ピクセルシェーダ
	const PixelShader ps3D = GLSL{
		U"example/shader/glsl/PointLight.Frag",
		{
			{ U"PSPerFrame", 0 },
			{ U"PSPerView", 1 },
			{ U"PSPointLightPos", 2 },
			{ U"PSPerMaterial", 3 },
			{ U"PSLighting", 4 }
		}
	};

	//const RenderTexture gaussianA4{ renderTexture.size() / 4 }, gaussianB4{ renderTexture.size() / 4 };
	//const RenderTexture gaussianA8{ renderTexture.size() / 8 }, gaussianB8{ renderTexture.size() / 8 };
	//const RenderTexture gaussianA16{ renderTexture.size() / 16 }, gaussianB16{ renderTexture.size() / 16 };

	//bool isGlowEffect = true;
	//int glowEffectType = 0;

	// ストップウォッチ
	Stopwatch stopwatch{ StartImmediately::Yes };

	// 回転をスムーズにするときに使っている変数
	double sss = 0;

	// 回転をスムーズにするときに使っている変数
	double ccc = 0;

	// 動きをスムーズにする値
	double smooth = 0.1f;
	//double lightSmooth = 0.025f;

	// XINPUT：プレイヤーインデックス (0 - 3)
	size_t playerIndex = 0;

	// m_focusY
	double focusY_max = 2.0;

	// ライトの設定
	Vec3 lightPos;
	float lightStrong = 4.0;
	//double lightSize = 0.073;
	//double emission = 1.0;
	//double toEmission = 1.0;

	// 暖炉の明るさ
	float fireplaceStrong = 2.6;
	float fireplaceSin = 0.6;

	// 環境光
	float globalAmbientColor = -0.1;

	// 太陽光
	float sunColor = 0.0;


	//==============================
	// モデルの座標
	//==============================

	// ルームの位置
	const Vec3 roomPos{ 0, 0, 0 };

	// ドアの位置
	Vec3 doorPos{ -1.6, 0, 1.9 };	// 最初の部屋のドア
	Vec3 door2Pos{ 6.68, 0, 1.9 };	// 左下の部屋のドア
	Vec3 door3Pos{ -1.55, 1.2, -3.53 };	// 右上の部屋のドア

	// 錆びた鍵の座標
	Vec3 keyPos = { 3.7, 0.01, 3 };

	// 鉄製の鍵の座標
	Vec3 IronkeyPos = { -0.48, 0.465, -4.80 };

	// パンの座標
	Vec3 breadPos = { 0, 0.75, 7 };

	// 火かき棒の座標
	Vec3 pokerPos = { 20, 0.45, -6.2 };

	// 引き出し
	Vec3 drawerPos = { 16.3, 0, 1.6 };
	double debugDrawerY = 0;

	// 羊皮紙
	Vec3 parchmentPos = { 16, 0.75, -3 };

	// ベッド
	Vec3 bedPos = { 2.87, 0.5, 11.3 };
	Vec3 bed2Pos = { 2.87, 0.5, 8.3 };
	Vec3 bed3Pos = { -2.87, 0.5, 11.3 };
	Vec3 bed4Pos = { -2.87, 0.5, 8.3 };
	Vec3 oldBedPos = { 2.87, 0.5, 5.15 };

	// トイレ
	Vec3 toiletPos = { -3.4, 0.5, 3.1 };

	// トイレ２（左上の部屋）
	Vec3 toilet2Pos = { 5, 0.5, -4.5 };

	// 棚
	Vec3 shelfPos = { 0, 0.5, 11.5 };

	// 壁
	Vec3 wallPos = { 3.7, 0.25, 3 };

	// 暖炉
	Vec3 fireplacePos = { 16.3, 0.5, -6.5 };
	Vec3 fireplaceLightPos = { 16.3, 0.7, -6.5 };

	// ハンガー
	Vec3 hangerPos = { 0, 1.03, 11.8 };
	Vec3 hangerRot{ 0, 90_deg, 0 };

	// 樽
	Vec3 barrelPos = { -3.55, 0.965, 0.185 };
	Vec3 barrel2Pos = { -3.55, 0.965, -0.72 };
	Vec3 barrel3Pos = { 13.07, 0.965, -6.8 };

	// 教団の紋章
	Vec3 emblemPos = { 16.43, 2, -6.83 };

	// 汚れた布
	Vec3 dirtyClothPos = { 11.522, 0.458, 10.29 };

	// 手記
	Vec3 memoPos = { -3.6, 0, 10.6 };

	// オブジェクトクラス
	ObjectController breadController;
	ObjectController keyController;
	ObjectController ironkeyController;
	ObjectController pokerController;
	ObjectController doorController;
	ObjectController door2Controller;
	ObjectController door3Controller;
	ObjectController parchmentController;
	ObjectController bedController;
	ObjectController bed2Controller;
	ObjectController bed3Controller;
	ObjectController bed4Controller;
	ObjectController oldBedController;
	ObjectController toiletController;
	ObjectController shelfController;
	ObjectController wallController;
	ObjectController fireplaceController;
	ObjectController fireplaceStrongController;
	ObjectController hangerController;
	ObjectController barrelController;
	ObjectController dirtyClothController;
	ObjectController memoController;

	double GlobalAmbientColorR = 0.4;
	double GlobalAmbientColorG = 0.4;
	double GlobalAmbientColorB = 0.5;

	double toGlobalAmbientColorR = GlobalAmbientColorR;
	double toGlobalAmbientColorG = GlobalAmbientColorG;
	double toGlobalAmbientColorB = GlobalAmbientColorB;

	double mousePosX = 0.0f;
	double mousePosY = 0.0f;
	double toMousePosX = 0.0f;
	double toMousePosY = 0.0f;

	int mouseDirectionX = 1;
	int mouseDirectionY = 1;

	// コリジョンの有効フラグ
	bool bCollision = true;
	bool bCollisionDoor = true;

	// 60FPS対応
	const double targetDeltaTime = 1.0 / 60.0; // 約16.666ms

	// コリジョンリスト
	double collisionList[12][2] = {
	   { 0, 1 },
	   { 1, 3 },
	   { 3, 2 },
	   { 2, 0 },

	   { 0, 4 },
	   { 1, 5 },
	   { 2, 6 },
	   { 3, 7 },

	   { 4, 5 },
	   { 5, 7 },
	   { 7, 6 },
	   { 6, 4 },
	};

	// コリジョンリスト
	double collisionTriangle[12][3] = {
	   { 0, 1, 2 },
	   { 1, 2, 3 },

	   { 0, 2, 4 },
	   { 2, 4, 6 },

	   { 2, 3, 6 },
	   { 3, 6, 7 },

	   { 1, 3, 5 },
	   { 3, 5, 7 },

	   { 1, 3, 5 },
	   { 3, 5, 7 },

	   { 4, 5, 6 },
	   { 5, 6, 7 },
	};

	// デバッグ用のフラグ
	bool bDebugViewFrame = true;
	bool bDebugViewCollision = false;
	bool bDebugviewModel = true;
	bool bDebugFlashingLight = true;
	//	bool bDebugShader = true;

	// コリジョン無効エリア
	double collisionNone[DoorNum][4] =
	{
		// x_min, x_max, z_min, z_max
		{ -2.2, -1.1, 1.2, 2.6},	// 最初の部屋
		{ 6.1, 7.2, 1.2, 2.6},		// 左下の部屋
		{ 10, 12, -1.5, -0.3},		// 奥の部屋
		{ 6.3, 7.0, -4.4, -3.0},	// 左上の部屋
	};

	// ライトを切り替えるエリア
	//double collisionLight[5][4] =
	//{
	//	{ -3,  3,  3, 11},	// x_min, x_max, z_min, z_max
	//	{ -3, 11, -3,  1},	// 通路
	//	{ 12, 19, -2,  1},	// 奥の部屋１
	////	{ 13, 19, -6, -3},	// 奥の部屋２
	//	{  5, 11,-12, -4},	// 真ん中の部屋（北）
	//	{  5, 11,  3, 11},	// 真ん中の部屋（南）
	//};

	// ライトの位置
	//Vec3 lightPosList[5] =
	//{
	//	{ 0, 2.084, 7 },	// x_min, x_max, z_min, z_max
	//	{ 4.26, 2.60, -0.93},	// 通路
	//	{ 16.05, 2.60, -0.92},	// 奥の部屋１
	////	{ 16.05, 2.60, -4.27},	// 奥の部屋２
	//	{ 8.29, 2.08, -8.8},	// 真ん中の部屋（北）
	//	{ 8.29, 2.08, 7.0},	// 真ん中の部屋（南）
	//};

	// 自分自身のコリジョンの半径
	double myRadius = 0.3;

	// ウィンドウの中心座標
	const Vec2 center = Scene::Center();
	const int32 height = Scene::Height();

	// 仮想マウス座標
	Vec2 virtualCursorPos = Vec2{ 0, 0 };

	// スプライト
	const Texture inventorySprite{ U"assets/sprites/BG_Inventory01.png" };
	const Texture breadMiniSprite{ U"assets/sprites/bread_mini.png" };
	const Texture breadBigSprite{ U"assets/sprites/bread_big.png" };
	const Texture memoMiniSprite{ U"assets/sprites/memo_mini.png" };
	const Texture memoBigSprite{ U"assets/sprites/memo_big.png" };
	const Texture keyMiniSprite{ U"assets/sprites/key_mini.png" };
	const Texture keyBigSprite{ U"assets/sprites/key_big.png" };
	const Texture pokerMiniSprite{ U"assets/sprites/poker_mini.png" };
	const Texture pokerBigSprite{ U"assets/sprites/poker_big.png" };
	const Texture parchmentMiniSprite{ U"assets/sprites/Parchment_mini.png" };
	const Texture parchmentBigSprite{ U"assets/sprites/Parchment_big.png" };
	const Texture wireMiniSprite{ U"assets/sprites/hanger_mini.png" };
	const Texture wireBigSprite{ U"assets/sprites/hanger_big.png" };
	const Texture wireKeyMiniSprite{ U"assets/sprites/UI_WireKey_mini.png" };
	const Texture wireKeyBigSprite{ U"assets/sprites/UI_WireKey_big.png" };
	const Texture toastedParchmentMiniSprite{ U"assets/sprites/ToastedParchment_mini.png" };
	const Texture toastedParchmentBigSprite{ U"assets/sprites/ToastedParchment_big.png" };
	const Texture dirtyClothMiniSprite{ U"assets/sprites/DirtyCloth_mini.png" };
	const Texture dirtyClothBigSprite{ U"assets/sprites/DirtyCloth_big.png" };
	const Texture clothMiniSprite{ U"assets/sprites/Cloth_mini.png" };
	const Texture clothBigSprite{ U"assets/sprites/Cloth_big.png" };
	const Texture ironKeyMiniSprite{ U"assets/sprites/iron_key_mini.png" };
	const Texture ironKeyBigSprite{ U"assets/sprites/iron_key_big.png" };

	// ビルボード用
	const Texture uvChecker{ U"assets/sprites/ExclamationMark.png", TextureDesc::MippedSRGB };

	// ビルボード表示する板
	const Mesh billboard{ MeshData::Billboard() };

	// インベントリの表示
	bool bInventory = false;

	// インベントリの大きさ
	const int inventoryWidth = 512;
	const int inventoryHeight = 512;

	// インベントリのON／OFF
	void inventoryOnOff();

	// アイテムアイコンの大きさ
	const int itemMiniWidth = 60;
	const int itemMiniHeight = 60;
	const int itemBigWidth = 140;
	const int itemBigHeight = 140;

	// 選択中のアイテム番号
	int itemIndex = 0;

	// ビックリマーク
	Vec3 markPosition{ 0.0, 0.0, 0.0 };
	float markSize = 0.3;
	float markHigh = 0.2;

	// 取得しているアイテム一覧
	Array<int> items;

	// メッセージ番号
	int message = 0;
	int messagePattern = 0;
	double messagePatternCount = 0.0;
	int itemMessage = -1;

	// ライト関係の変数
	//int lightArea = 0;
	//int lastLightArea = 0;
	//double lightTime = 0;

	int debugItemNameX = 805;
	int debugItemNameY = 236;

	// シナリオ番号
	int scenario = 0;

	// 錆びた鍵を持っている
	bool bKeyHave = false;

	// パンを持っている
	bool bBreadHave = false;

	// 火かき棒を持っている
	bool bPokerHave = false;

	// 羊皮紙を持っている
	bool bParchmentHave = false;

	// 手記を読んでいる
	bool bMemo = false;

	// 壁を調べた
	bool bWall = false;

	// 針金を持っている
	bool bWire = false;

	// 針金製の鍵を持っている
	bool bWireKey = false;

	// 鉄製の鍵を持っている
	bool bIronKeyHave = false;

	// 暖炉の火が強くなった
	bool bFireplaceStrong = false;

	// 炙った羊皮紙を読んでいる
	bool bToastedParchmentRead = false;

	// ハンガーを見つけた
	bool bHangerFind = false;

	// ハンガーを持っている
	bool bHangerHave = false;

	// 汚れた布を持っている
	bool bDirtyClothHave = false;

	// 手記を持っている
	bool bMemoHave = false;

	// 布を持っている
	bool bClothHave = false;

	// 布を読んでいる
	bool bClothRead = false;

	// 樽３の近くにいる
	//bool bBarrel3Lockon = false;

	// トイレ２の近くにいる
	bool bToilet2Lockon = false;

	// 火の弱い暖炉の近くにいる
	bool bFireplaceWeakLockon = false;

	// 火の強い暖炉の近くにいる（オミット）
//	bool bFireplaceStrongLockon = false;

	// 鉄製の鍵の前にいる
	bool bIronKeyLockon = false;

	// 最初の部屋のドアの前にいる
	bool bDoorLockon = false;

	const Font& boldFont = FontAsset(U"Bold");

	// テキストを事前に読み込むための関数
	void dummyTextView(Array<String> text);
	void dummyTextView(Array<Array<String>> text);

	// メッセージのパターン数
	const int MessagePatternMax = 3;

	Array<int> messageRead;

	// 画面下のテキスト
	Array<String> Text =
	{
		// シナリオ系

		// 0 最初
		U"　　　　　　　　　　腹が減った。まずは食事をしなければ…。　　　　　　　　　　",	// 0
		U"",	// 0
		U"",	// 0

		// 1 パンを食べた後
		//U"パンの中に手記が入っていた。",	// 1
		U"腹の足しになったな…。さて、ここから脱出する方法はないだろうか…。",	// 1
		U"",	// 1
		U"",	// 1
		
		// 2 手記を読んだ後
		//U"ベッド脇の壁を調べてみよう。",	// 2
		U"この手記は何かのヒントだろうか…。",	// 2
		U"",	// 2
		U"",	// 2

		// 3 錆びた鍵を取った後
		//U"この鍵は錆びて使えないな…。",	// 3
		U"",	// 3
		U"",	// 3
		U"",	// 3

		// 4 棚の上のハンガー（これは使わない）
		//U"棚の中にハンガーがあった。これは何かに使えるかも？",	// 4
		U"",	// 4
		U"",	// 4
		U"",	// 4

		// 5 針金製の鍵を手に入れた後
		//U"この針金製の鍵で扉を開けてみよう。",	// 5
		U"",	// 5
		U"",	// 5
		U"",	// 5

		// 6 扉を開けた後
		U"",	// 6
		U"",	// 6
		U"",	// 6

		U"水槽がある。",	// 7
		U"",	// 7
		U"",	// 7

		U"羊皮紙の汚れを落としたい。",	// 8
		U"",	// 8
		U"",	// 8

		U"この引き出しは何だろう。",	// 9
		U"",	// 9
		U"",	// 9

		// 10 鍵がかかっている鉄格子の扉
		U"厚みのある鉄格子が道を塞いでいる。しっかりと鍵がかかっているようだ。",
		U"鉄の扉が頑丈そうに閉じている。無理に開けるのは現実的じゃない。",
		U"鍵穴が見える。何かで開けるしかなさそうだ。",

		// 11 自分のベッド
		U"汚れたマットレス。ここで眠っていたのか……？",
		U"臭いが染みついている……寝返りを打つのも苦痛だっただろう",	// 11
		U"何の夢も見なかった。いや、夢を見ていたのかも……思い出せない",	// 11

		// 12 トイレ
		U"錆びたバケツが床に置かれている。どうやらこれがトイレらしい……最悪だ",
		U"悪臭が鼻を突く。中身は……見ないほうがいい。",
		U"トイレとは名ばかりの容器がそこにある。長居したくない。",

		// 13 棚
		U"手作りのような粗末な棚だ。",	
		U"",
		U"",

		// 14 ベッド
		U"薄汚れたマットレスが載っている。何かの染みが点々とついている……。",
		U"横になる気も失せるほど、マットレスがへたっている。カビ臭さが鼻を刺す。",
		U"汚れと湿気が染みついた寝床。数日は誰かがここで過ごした形跡がある。",

		// 15  金属フレームのみのベッド
		U"マットレスすらない、剥き出しの金属フレーム。横になれば骨が痛みそうだ",
		U"錆びついたフレームだけが残っている。ここで寝るのは拷問に近い。",
		U"寝具の欠片も見当たらない。ただの鉄の枠が虚しく横たわっている。",

		U"ん？この壁は…。",	// 16
		U"",	// 16
		U"",	// 16

		// 棚（ハンガーを見つける前）
		U"何かあるぞ。",	// 17
		U"",	// 17
		U"",	// 17

		// 18 暖炉
		U"炉の中で火が小さく揺れている。",
		U"弱々しい炎がくすぶっている。何かを炙るには、ぎりぎり使えそうだ。",
		U"燃え残った薪がゆっくりと火を灯している。消えるのは時間の問題かもしれない。",

		// パンを取った後のシナリオテキスト
		U"（「I」キーを押して、持ち物からパンを食べよう。）",	// 19
		U"",	// 19
		U"",	// 19

		U"扉だ。この針金製の鍵で開けられるかもしれない。",	// 20
		U"",	// 20
		U"",	// 20

		U"火が強くなった。",	// 21
		U"",	// 21
		U"",	// 21

		U"ハンガーがある。これは何かに使えるかも？",	// 22
		U"",	// 22
		U"",	// 22

		// 23 教団の紋章
		U"奇妙な模様が描かれた布が掛けられている。何かの紋章だろうか……。",
		U"見慣れない図形が布に刻まれている。宗教的な意味を持っていそうだ",
		U"円と棘が交わるようなマーク……教団の象徴かもしれない。",

		U"針金を見つけた。この針金で錆びた鍵の形を模すことはできるだろうか…。",	// 24
		U"",	// 24
		U"",	// 24

		// 25 樽
		U"古びた木製の樽。中からは液体の音がしない。空っぽのようだ。",	// 25
		U"",	// 25
		U"",	// 25

		// 26 汚れた布
		U"布だ。",	// 26
		U"",	// 26
		U"",	// 26

		// 27 樽（中に水が入っている）
		U"古びた木製の樽。中に液体が入っている。",
		U"しかし、この樽を開けることは無理そうだ…。",
		U"",

		// 28 手記がある
		U"古い手記が落ちている。",	// 28
		U"",	// 28
		U"",	// 28

		// 29 ドア２（開けることができないとき）
		//U"手持ちの鍵で開けることができないようだ。別の鍵があるのだろうか。",	// 29
		U"",	// 29
		U"",	// 29
		U"",	// 29

		// 30 閉まっている木製の扉（左下の部屋のドア）
		U"古びた木の扉。鍵がかかっているようだ",	// 30
		U"頑丈な造りだ。そう簡単には開かないだろう",	// 30
		U"",	// 34

		// 31 鉄製の鍵
		U"格子の向こうに鍵が落ちている……届かない",	// 31
		U"手を伸ばしてみたが、あと少し届かない",	// 31
		U"",	// 31

		// 32 トイレ２（左上の部屋）
		U"臭い……ここがトイレなのか？",	// 32
		U"見るに耐えない……だが、使うしかなかったのだろう",	// 32
		U"水が少し溜まっている。使えるかもしれないが……気が進まない",	// 32

		// 33 閉まっている鉄製の扉（右上の部屋のドア）
		U"重たそうな鉄扉だ。びくともしない……",	// 33
		U"頑丈な造りだ。そう簡単には開かないだろう",	// 33
		U"",	// 33

		// 34 引き出しギミック
		U"奇妙な紋章が彫られている……",	// 34
		U"この模様…何を意味しているのだろうか？",	// 34
		U"",	// 34

		// プロローグ用
		U"　　　　　　　　　　壁は冷たい石。窓もない。　　　　　　　　　　",	// 35
		U"",	// 35
		U"",	// 35

		// プロローグ用
		U"　　　　　　　　　　床には薄汚れたマットレス、隅にはバケツのようなものが置かれている。　　　　　　　　　　",	// 36
		U"",	// 36
		U"",	// 36

		// プロローグ用
		U"　　　　　　　　　　ここは……牢屋だろうか。　　　　　　　　　　",	// 37
		U"",	// 37
		U"",	// 37

		U"",	// 38
		U"",	// 38
		U"",	// 38

		U"",	// 39
		U"",	// 39
		U"",	// 39

		U"",	// 40
		U"",	// 40
		U"",	// 40

		U"",	// 41
		U"",	// 41
		U"",	// 41

		U"",	// 42
		U"",	// 42
		U"",	// 42

		U"",	// 43
		U"",	// 43
		U"",	// 43

		U"",	// 44
		U"",	// 44
		U"",	// 44

		U"",	// 45
		U"",	// 45
		U"",	// 45

		U"",	// 46
		U"",	// 46
		U"",	// 46

		U"",	// 47
		U"",	// 47
		U"",	// 47

		U"",	// 48
		U"",	// 48
		U"",	// 48

		U"",	// 49
		U"",	// 49
		U"",	// 49

		// アイテム系
		U"黒ずんだ硬いパンがある。これは食べられそうだ。",	// 50 パン
		U"",	// 50
		U"",	// 50

		U"",	// 51 手記
		U"",	// 51
		U"",	// 51

		U"鍵だ！",	// 52 鍵
		U"",	// 52
		U"",	// 52

		U"何の棒だろう。",	// 53 火かき棒
		U"",	// 53
		U"",	// 53

		U"紙？",	// 54 羊皮紙
		U"",	// 54
		U"",	// 54

		U"ただの針金だ。",	// 55 針金
		U"",	// 55
		U"",	// 55

		U"ひどく汚れていて、何かが書かれているようだが判別できない。洗えば読めるかもしれない。",	// 
		U"",	// 56
		U"",	// 56

		U"文字がはっきりと見える。何かの順番を示しているようだ。",	// 57
		U"",	// 57
		U"",	// 57

		U"表面に新たな文字が浮かび上がった。これは隠された何かのヒントだろうか？",	// 58
		U"",	// 58
		U"",	// 58

	};

	// インベントリ用のテキスト
	Array<Array<String>> itemText =
	{
		// 0 パン
		{
			U"黒ずんだ硬いパン。　　　　　　　　",
			U"日にちの感覚も失いかけた者が、　　",
			U"最後に噛みしめたのか。　　　　　　",
		},

		// 1 手記
		{
			U"誰かが書いた手記だ。　　　　　　　",
			U"読んでみようか…。　　　　　　　　",
			U"",
		},

		// 2 鍵
		{
			U"長い間放置されていたのか、　　　　",
			U"すっかり錆びついている。　　　　　",
			U"力を加えると折れてしまいそうだ。　",
		},

		// 3 火かき棒
		{
			U"鉄製の長い棒。　　　　　　　　　　",
			U"炉の中をかき混ぜたり、熱いものを　",
			U"引き寄せたりするのに使えそうだ。　",
		},

		// 4 羊皮紙
		{
			U"無地の羊皮紙。　　　　　　　　　　",
			U"触るとわずかにざらつきがある。　　",
			U"　　　　　　　　　　　　　　　　　",
		},

		// 5 針金（ハンガー）
		{
			U"ただのハンガーだが適度に柔らかく　",
			U"曲げやすい。何かの形を真似ること　",
			U"ができるかもしれない。　　　　　　",
		},

		// 6 汚れた布
		{
			U"ひどく汚れていて、何かが書かれて　",
			U"いるようだが判別できない。　　　　",
			U"洗えば読めるかもしれない。　　　　",
		},

		// 7 布
		{
			U"文字がはっきりと見える。　　　　　",
			U"何かの順番を示しているようだ。　　",
			U"　　　　　　　　　　　　　　　　　",
		},

		// 8 炙った羊皮紙
		{
			U"表面に新たな文字が浮かび上がった。",
			U"これは隠された何かの　　　　　　　",
			U"ヒントだろうか？　　　　　　　　　",
		},

		// 9 針金製の鍵
		{
			U"ハンガーの針金で作った鍵。　　　　",
			U"これで扉を開けることができるかも　",
			U"しれない。　　　　　　　　　　　　",
		},

		// 10 鉄製の鍵
		{
			U"鉄製の鍵だ。　　　　　　　　　　　",
			U"　　　　　　　　　　　　　　　　　",
			U"　　　　　　　　　　　　　　　　　",
		},
	};

	// インベントリ用のテキスト（アイテム名）
	Array<String> itemNameText =
	{
		// アイテム系
		U"パン",	// 0 
		U"手記",	// 1 
		U"錆びた鍵",	// 2 
		U"火かき棒",	// 3 
		U"羊皮紙",	// 4 
		U"ハンガー",	// 5 針金→ハンガー
		U"汚れた布",	// 6
		U"布",	// 7
		U"炙った羊皮紙",	// 8
		U"針金製の鍵",	// 9
		U"鉄製の鍵",	// 10

	};

	// 手記
	Array<String> memoText =
	{
		U"【囚人の手記】\n",
		U"看守が巡回中に、腰の鍵を落とした\n",
		U"奇跡だ。…いや、呪いかもしれない\n",
		U"やつは気づかずに通り過ぎた。俺は急いで拾い、音を立てぬように隠した\n",
		U"\n",
		U"隠すには、見えぬ場所がよかった\n",
		U"見えるけど、見ようとしなければ通り過ぎてしまうような…そんな場所\n",
		U"いつも眠りに落ちるとき、顔を向ける方角。\n",
		U"眠っている間、壁の向こうでこいつも眠っている——\n",
		U"\n",
		U"お前がこれを見つけたなら、まだ希望はある\n",
		U"見つけてくれ、この“目に見えぬ出口”を\n",
		U"\n",
		U"……俺には、もう時間がなかった。",
	};

	// 羊皮紙
	Array<String> toastedParchmentText =
	{
		U"ちぎれた輪が、\n",
		U"空の底へと沈む。\n",
		U"白い欠片が、\n",
		U"風に踊り、やがて落ちる。\n",
		U"絡みつくものが、\n",
		U"静かに喉元へ這う。\n",
		U"枯れた夢が、\n",
		U"黒ずんだ土に崩れ落ちる。\n",
		U"見開かれたそれが、\n",
		U"すべてを呪うように、夜を睨む。\n",
	};

	// 布
	Array<String> clothText =
	{
		U"ちぎれた輪が、\n",
		U"空の底へと沈む。\n",
		U"白い欠片が、\n",
		U"風に踊り、やがて落ちる。\n",
		U"絡みつくものが、\n",
		U"静かに喉元へ這う。\n",
		U"枯れた夢が、\n",
		U"黒ずんだ土に崩れ落ちる。\n",
		U"見開かれたそれが、\n",
		U"すべてを呪うように、夜を睨む。\n",
	};

	// プロローグ
	Array<String> prologueText =
	{
		U"　　目が覚めると、そこは見知らぬ部屋だった。　　",
		U"　　鉄の匂いと、湿った空気。　　",
		U"　　確か――あの夜、帰り道で何かに襲われて…　　",
		U"　　思い出せない。だが、確かに――“連中”に捕まった。　　",
		U"　　…ここは、どこだ？　　",
		U"　　そして――なぜ、私を？　　",
		U"　　　　　　　　　　……　　　　　　　　　　",
	};

	float prologueCount = 0;
	float messageCount = 0;
	int   prologueLength = 0;
	bool  bPrologueMessageEnd = false;
//	bool  bPrologue2End = false;
//	bool bPrologueBGM = false;
	bool bStartPlaying = false;

	
	const float prologueTiming = 30;
	const float prologueMessageSpeed = 5;
	const float MessageSpeed = 10;
//	const float prologueMessageWait = 1;

	// デバッグ用
	double debugHeight = 0;

};
