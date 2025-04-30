# pragma once
#include "Common.hpp"
#include "PhiController.hpp"
#include "ObjectController.hpp"

struct Vec3_ {	// TODO Vec3を継承してみる
	double x, y, z;

	Vec3_ operator-(const Vec3_& v) const { return { x - v.x, y - v.y, z - v.z }; }
	Vec3_ operator+(const Vec3_& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vec3_ operator*(double s) const { return { x * s, y * s, z * s }; }

	static Vec3_ cross(const Vec3_& a, const Vec3_& b) {
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	static double dot(const Vec3_& a, const Vec3_& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	double dot(const Vec3_& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

	double lengthSq() const { return x * x + y * y + z * z; }
};

// レイ（三角形との交差を調べる）
struct Ray_ {	// TODO Rayを継承してみる
	Vec3_ origin;
	Vec3_ direction;
};

// 交差点の情報
struct HitResult {
	double t;       // レイのパラメータ（距離）
	double u, v;    // バリセントリック座標
	Vec3_ point;    // 交差点
};

// 三角形（頂点3つ）
using Triangle_ = std::array<Vec3_, 3>;

struct Segment {
	Vec3_ start;
	Vec3_ end;
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

	// デバッグ表示
	void debug();

	// マウスポインタのRay
	Ray getMouseRay() const;

	// ベクトルの外積（2D）
	double cross(const Vec2& a, const Vec2& b);

	// 点Cが線分ABのどちら側にあるか
	double direction(const Vec2& a, const Vec2& b, const Vec2& c);
	
	// 線分同士が交差しているかを判定
	bool isIntersecting(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
	
	// 2Dの線分同士の交差判定
	bool isCollision(const Vec2& a, const Vec2& b, double* collisionList);

	// 3D空間上の線分と三角形が交差しているかどうかを判定する関数
	bool IntersectSegmentTriangle(const Segment& segment, const Triangle_& tri, HitResult& outResult);

	// 最近点を三角形上に求める（Möllerのアルゴリズム系）
	Vec3_ ClosestPointOnTriangle(const Vec3_& p, const Vec3_& a, const Vec3_& b, const Vec3_& c);

	// 球と三角形の交差判定
	bool IsSphereIntersectingTriangle(const Vec3_& sphereCenter, double radius,
		const Vec3_& a, const Vec3_& b, const Vec3_& c);

	// カメラ
	BasicCamera3D camera;

	// カメラスピード
	const double cameraSpeed = 2.0f;

	Size SceneSize{ 256, 192 };

	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R16G16B16A16_Float, HasDepth::Yes };

	// 地面
	Texture groundTexture;
	Mesh groundPlane;

	ColorF backgroundColor;

	double m_fastMove = 1.0;

	bool m_padPressed = false;

	// カメラの視点
	Vec3 m_eyePosition = Vec3{ 2.5, 1.5, 10 };

	// カメラの視点（1フレーム前）
	Vec3 last_eyePosition = Vec3{ 2.5, 1.5, 10 };

	// カメラの注視点（カメラが「いま見ている」場所のこと）
	Vec3 m_focusPosition = Vec3{ 0, 0, -1 };

	Vec3 m_upDirection = Vec3{ 0, 1, 0 };

	Size m_sceneSize = Scene::Size();

	// 視野角
	double m_verticalFOV = 55_deg;

	double m_nearClip = 0.2;

	// 開始時の目の角度
	double m_focusY = 0;

	// カメラの角度
	PhiController phiController = PhiController(m_eyePosition, m_focusPosition);

	// モデルの読み込み
	const Model model{ U"assets/models/Room/EV_Room01.obj" };
	const Model modelDoor{ U"assets/models/Room/Gimmick_FenceDoor01.obj" };
	const Model modelKey{ U"assets/models/Key/key.obj" };
	const Model modelBread{ U"assets/models/Bread/bread.obj" };
	const Model modelPoker{ U"assets/models/Poker/Gimmick_Poker01.obj" };

	// 各オブジェクトの位置
	const Vec3 roomPos{ 0, 0, 0 };

	// ドアの位置
	const Vec3 doorPos{ -1.6, 0, 2 };

	// スケーリングの倍率
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };

	bool bKeyHave = false;
	bool bBreadHave = false;
	bool bPokerHave = false;
	bool bClear = false;

	double bgmStopCount = 0.0f;

	const PixelShader psBright = HLSL{
		U"example/shader/hlsl/extract_bright_linear.hlsl", 
		U"PS" 
	}
	| GLSL{
		U"example/shader/glsl/extract_bright_linear.frag",
		{
			{U"PSConstants2D", 0}
		}
	};

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
			{ U"PSPerMaterial", 3 }
		}
	};

	const RenderTexture gaussianA4{ renderTexture.size() / 4 }, gaussianB4{ renderTexture.size() / 4 };
	const RenderTexture gaussianA8{ renderTexture.size() / 8 }, gaussianB8{ renderTexture.size() / 8 };
	const RenderTexture gaussianA16{ renderTexture.size() / 16 }, gaussianB16{ renderTexture.size() / 16 };

	bool isGlowEffect = true;
	int glowEffectType = 0;

	Vec3 toCameraPos = m_eyePosition;

	Stopwatch stopwatch{ StartImmediately::Yes };

	double to_m_focusY = m_focusY;
	double s = 0;
	double c = 0;

	// 動きをスムーズにする値
	double smooth = 0.1f;

	// プレイヤーインデックス (0 - 3)
	size_t playerIndex = 0;

	// m_focusY
	double focusY_max = 2.0;

	// ライトの設定
	double lightY = 2.084;
	double lightZ = 7;
	double lightSize = 0.073;
	double emission = 1.0;
	double toEmission = 1.0;

	double zoom = 0.0;
	double to_zoom = 0.0;
	double zoom_smooth = 2;

	// 鍵の座標
	double keyX = 0;
	double keyY = 0.75;
	double keyZ = 8;

	// パンの座標
	double breadX = 0;
	double breadY = 0.75;
	double breadZ = 7;

	// 火かき棒の座標
	double pokerX = 0;
	double pokerY = 0.75;
	double pokerZ = 6;

	// 鍵をフォーカスする時の座標
	double keyFocusX = keyX - 0.5;
	double keyFocusY = keyY + 0.25;
	double keyFocusZ = keyZ + 0.0;
	double keyFocusCameraY = -0.5;
	double keyFocusPhi = 0;

	bool isFocus = false;
	double focusSmooth = 1;
	bool isFocusSmooth = false;

	Vec3 lastToCameraPos = {0.0, 0.0, 0.0};
	double last_to_m_focusY = 0;
	double last_m_phi = 0;

	double GlobalAmbientColorR = 0.4;
	double GlobalAmbientColorG = 0.4;
	double GlobalAmbientColorB = 0.5;

	double toGlobalAmbientColorR = GlobalAmbientColorR;
	double toGlobalAmbientColorG = GlobalAmbientColorG;
	double toGlobalAmbientColorB = GlobalAmbientColorB;

	// ドアの座標
	double doorX = -1.6;
	double doorY = 1.0;
	double doorZ = -4.9;

	// ドアをフォーカスする時の座標
	double doorFocusX = -1.6 + 0.0;
	double doorFocusY =  1.0 + 0.25;
	double doorFocusZ = -4.9 + 0.5;
	double doorFocusCameraY = -0.5;
	double doorFocusPhi = -1.5;

	double mousePosX = 0.0f;
	double mousePosY = 0.0f;
	double toMousePosX = 0.0f;
	double toMousePosY = 0.0f;

//	double focusWait = 0.0f;

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

	bool bDebugViewFrame = true;
    bool bDebugViewCollision = false;
	bool bDebugviewModel = true;
	bool bDebugFlashingLight = true;
	bool bDebugShader = true;


	double collisionNone[4][4] =
	{
		{ -2.2, -1.1, 1.2, 2.6},	// x_min, x_max, z_min, z_max
		{ 6.1, 7.2, 1.2, 2.6},
		{ 10, 12, -1.5, -0.3},
		{ 6.3, 7.0, -4.4, -3.0},
	};

	// 自分自身のコリジョンの半径
	double myRadius = 0.3;

	// ウィンドウの中心座標
	const Vec2 center = Scene::Center();

	// 仮想マウス座標
	Vec2 virtualCursorPos = Vec2{ 0, 0 };

	// インベントリ
	const Texture inventoryTexture{ U"assets/sprites/inventory.png" };

	// インベントリの表示
	bool bInventory = false;

	// オブジェクトを取るクラス
	ObjectController breadController;
	ObjectController keyController;

};