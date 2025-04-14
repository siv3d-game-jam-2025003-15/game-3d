# pragma once
# include "Common.hpp"

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
	Ray getMouseRay() const
	{
		return camera.screenToRay(Cursor::PosF());
	}

	// カメラ
	BasicCamera3D camera;

	Size SceneSize{ 256, 192 };

	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R16G16B16A16_Float, HasDepth::Yes };

	// 地面
	Texture groundTexture;
	Mesh groundPlane;

	ColorF backgroundColor;

	Audio audio;

	double m_fastMove = 1.0;

	bool m_padPressed = false;

	// カメラの初期位置
	Vec3 m_eyePosition = Vec3{ 0, 1.5, 4 };
	Vec3 last_eyePosition = Vec3{ 0, 1.5, 4 };

	// カメラの向き
	Vec3 m_focusPosition = Vec3{ 0, 0, -1 };

	Vec3 m_upDirection = Vec3{ 0, 1, 0 };

	Size m_sceneSize = Scene::Size();

	// 視野角
	double m_verticalFOV = 55_deg;

	double m_nearClip = 0.2;

	// 開始時の目の角度
	double m_focusY = 0;

	double m_phi = std::atan2(
		(m_focusPosition.z - m_eyePosition.z),
		(m_focusPosition.x - m_eyePosition.x)
	);

	// モデルの読み込み
	const Model model{ U"assets/models/Room/EV_Room01.obj" };
	const Model modelKey{ U"assets/models/Key/key.obj" };

	// 各オブジェクトの位置
	const Vec3 roomPos{ 0, 0, 0 };

	// スケーリングの倍率
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };

	bool isKeyHave = false;
	bool isClear = false;

	float bgmStopCount = 0.0f;

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

	//Vec3 focusVector;
	//Vec3 toFocusVector;

	double to_m_focusY = m_focusY;
	double s = 0;
	double c = 0;

	// 動きをスムーズにする値
	double smooth = 0.1f;

	// プレイヤーインデックス (0 - 3)
	size_t playerIndex = 0;

	// m_focusY
	double focusY_max = 2.0;

	double lightY = 2.084;
	double lightSize = 0.073;
	double emission = 1.0;
	double toEmission = 1.0;

	double zoom = 0.0;
	double to_zoom = 0.0;
	double zoom_smooth = 2;

	// 鍵の座標
	float keyX = 3.25;
	float keyY = 0.6;
	float keyZ = 3.3;

	// 鍵をフォーカスする時の座標
	float keyFocusX = keyX - 0.5;
	float keyFocusY = keyY + 0.25;
	float keyFocusZ = keyZ + 0.0;
	float keyFocusCameraY = -0.5;
	float keyFocusPhi = 0;

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
	float doorX = -1.6;
	float doorY = 1.0;
	float doorZ = -4.9;

	// ドアをフォーカスする時の座標
	float doorFocusX = -1.6 + 0.0;
	float doorFocusY =  1.0 + 0.25;
	float doorFocusZ = -4.9 + 0.5;
	float doorFocusCameraY = -0.5;
	float doorFocusPhi = -1.5;

	float mousePosX = 0.0f;
	float mousePosY = 0.0f;
	float toMousePosX = 0.0f;
	float toMousePosY = 0.0f;

	float focusWait = 0.0f;

	int mouseDirectionX = 1;
	int mouseDirectionY = 1;

	// コリジョンの有効フラグ
	bool bCollision = true;

	// 目標フレームレート
	const double targetFPS = 60.0;
	const double frameTime = 1.0 / targetFPS; // 約 0.01666 秒

	double startTime = 0.0;
};