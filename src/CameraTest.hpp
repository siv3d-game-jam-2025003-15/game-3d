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

	//BasicCamera3D m_camera;
	DebugCamera3D camera;

	Size SceneSize{ 256, 192 };

	Model blacksmithModel{ U"" };

	MSRenderTexture renderTexture;

	ColorF backgroundColor;

	int count = 0;

	Audio audio;
};