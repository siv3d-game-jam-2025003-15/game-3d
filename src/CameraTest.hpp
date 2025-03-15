# pragma once
# include "Common.hpp"

// �J�����̎���
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

	// �c�����̎���p�i���W�A���j
	//double m_verticalFOV = 25_deg;

	// �J�����̎��_�̈ʒu
	//Vec3 m_eyePosition{ 16, 16, -16 };

	// �J�����̒����_�̈ʒu
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