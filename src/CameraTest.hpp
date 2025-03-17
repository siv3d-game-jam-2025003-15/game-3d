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
		return camera.screenToRay(Cursor::PosF());
	}

private:

	BasicCamera3D camera;
//	DebugCamera3D camera;

	Size SceneSize{ 256, 192 };

	Model blacksmithModel{ U"" };

//	MSRenderTexture renderTexture;
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R16G16B16A16_Float, HasDepth::Yes };
	
	// �n��
	Texture groundTexture;
	Mesh groundPlane;

	ColorF backgroundColor;

	Audio audio;

	double m_fastMove = 1.0;

	bool m_padPressed = false;

	// �J�����̏����ʒu
	Vec3 m_eyePosition = Vec3{ 0, 1.5, 4 };
	Vec3 last_eyePosition = Vec3{ 0, 1.5, 4 };

	// �J�����̌���
	Vec3 m_focusPosition = Vec3{ 0, 0, -1 };

	Vec3 m_upDirection = Vec3{ 0, 1, 0 };

	Size m_sceneSize = Scene::Size();

	// ����p
	double m_verticalFOV = 55_deg;

	double m_nearClip = 0.2;

	// �J�n���̖ڂ̊p�x
	double m_focusY = 0;

	double m_phi = std::atan2((m_focusPosition.z - m_eyePosition.z),
		(m_focusPosition.x - m_eyePosition.x));

	/// ���f���z�u
	// ���f���̓ǂݍ��݁i�}�e���A���Q�Ƃ𖳎��j
	const Model model{ U"assets/models/Room/EV_Room01.obj" };
	const Model modelKey{ U"assets/models/Key/key.obj" };

//	const Texture floorTexture{ U"assets/models/Room/T_EV_Floor01_D.png", TextureDesc::Mipped };
//	const Texture wallTexture{ U"assets/models/Room/T_EV_Wall01_D.png", TextureDesc::Mipped };

	// �e�I�u�W�F�N�g�̈ʒu
	const Vec3 roomPos{ 0, 0, 0 };

	// �X�P�[�����O�̔{��
	const Vec3 roomScale{ 0.01, 0.01, 0.01 };

	bool isKeyHave = false;
	bool isClear = false;

	float bgmStopCount = 0.0f;

	//const PixelShader psBright = HLSL{ U"example/shader/hlsl/extract_bright_linear.hlsl", U"PS" }
	//| GLSL{ U"example/shader/glsl/extract_bright_linear.frag", {{U"PSConstants2D", 0}} };

	//const RenderTexture gaussianA4{ renderTexture.size() / 4 }, gaussianB4{ renderTexture.size() / 4 };
	//const RenderTexture gaussianA8{ renderTexture.size() / 8 }, gaussianB8{ renderTexture.size() / 8 };
	//const RenderTexture gaussianA16{ renderTexture.size() / 16 }, gaussianB16{ renderTexture.size() / 16 };

	bool isGlowEffect;

	Vec3 toCameraPos;

	Stopwatch stopwatch{ StartImmediately::Yes };

	//Vec3 focusVector;
	//Vec3 toFocusVector;

	double to_m_focusY;
	double s = 0;
	double c = 0;

};