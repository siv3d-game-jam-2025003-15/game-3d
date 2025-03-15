# pragma once
# include "Common.hpp"

// カメラの実験
class CameraTest : public App::Scene
{
public:

	CameraTest(const InitData& init);

	void update() override;

	void draw() const override;

	Ray getMouseRay() const
	{
		//return m_camera.screenToRay(Cursor::PosF());
		return camera.screenToRay(Cursor::PosF());
	}

private:

	// 縦方向の視野角（ラジアン）
	//double m_verticalFOV = 25_deg;

	// カメラの視点の位置
	//Vec3 m_eyePosition{ 16, 16, -16 };

	// カメラの注視点の位置
	//Vec3 m_focusPosition{ 0, 0, 0 };

	//double m_phi = -20_deg;

	//double m_theta = 50_deg;

	BasicCamera3D camera;
	//DebugCamera3D camera;

	Size SceneSize{ 256, 192 };

	Model blacksmithModel{ U"" };

	MSRenderTexture renderTexture;
	
	// 地面
	Texture groundTexture;
	Mesh groundPlane;

	ColorF backgroundColor;

	int count = 0;

	Audio audio;

	double m_fastMove = 1.0;

	bool m_padPressed = false;

	Vec3 m_eyePosition = Vec3{ 0, 1.5, -4 };
	Vec3 last_eyePosition = Vec3{ 0, 1.5, -4 };

	Vec3 m_focusPosition = Vec3{ 0, 0, 0 };

	Vec3 m_upDirection = Vec3{ 0, 1, 0 };

	double DefaultVerticalFOV = 30_deg;

	double DefaultNearClip = 0.2;

	//
	// Proj
	//
	Size m_sceneSize = Scene::Size();

	double m_verticalFOV = DefaultVerticalFOV;

	double m_nearClip = DefaultNearClip;

	//double m_focusY = [this]
	//	{
	//		const auto focusVector = (m_focusPosition - m_eyePosition);
	//		return (focusVector.y / std::hypot(focusVector.x, focusVector.z));
	//	}();

	// 開始時の目の角度
	double m_focusY = 0;

	double m_phi = std::atan2((m_focusPosition.z - m_eyePosition.z),
		(m_focusPosition.x - m_eyePosition.x));



	/// モデル配置
	// モデルの読み込み（マテリアル参照を無視）
	const Model model{ U"assets/models/Room/EV_Room01.obj" };

	const Texture floorTexture{ U"assets/models/Room/T_EV_Floor01_D.png", TextureDesc::Mipped };
	const Texture wallTexture{ U"assets/models/Room/T_EV_Wall01_D.png", TextureDesc::Mipped };

	// 各オブジェクトの位置
	const Vec3 roomPos{ 0, 0, 0 };
	// スケーリングの倍率
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };
};