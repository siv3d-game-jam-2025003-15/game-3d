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

	BasicCamera3D camera;
	//DebugCamera3D camera;

	Size SceneSize{ 256, 192 };

	Model blacksmithModel{ U"" };

	MSRenderTexture renderTexture;
	
	// �n��
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

	// �J�n���̖ڂ̊p�x
	double m_focusY = 0;

	double m_phi = std::atan2((m_focusPosition.z - m_eyePosition.z),
		(m_focusPosition.x - m_eyePosition.x));



	/// ���f���z�u
	// ���f���̓ǂݍ��݁i�}�e���A���Q�Ƃ𖳎��j
	const Model model{ U"assets/models/Room/EV_Room01.obj" };

	const Texture floorTexture{ U"assets/models/Room/T_EV_Floor01_D.png", TextureDesc::Mipped };
	const Texture wallTexture{ U"assets/models/Room/T_EV_Wall01_D.png", TextureDesc::Mipped };

	// �e�I�u�W�F�N�g�̈ʒu
	const Vec3 roomPos{ 0, 0, 0 };
	// �X�P�[�����O�̔{��
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };
};