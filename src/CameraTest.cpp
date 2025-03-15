# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
	//, m_camera{ SceneSize, m_verticalFOV, m_eyePosition, m_focusPosition }
{
	backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	blacksmithModel = Model{ U"example/obj/blacksmith.obj" };
//	blacksmithModel = Model{ U"assets/models/Room/EV_Room01.obj" };

	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -32 } };
//	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 10, 16, -32 } };

	Audio audio(U"close.wav");

	audio.setVolume(0.5);
}

void CameraTest::update()
{
	ClearPrint();

	if (MouseL.down())
	{
		count++;
	}
	if (KeyUp.pressed())
	{
		count++;
	}
	if (KeySpace.down())
	{
		audio.play();
	}


	const Ray ray = getMouseRay();

	Print << ray;
	Print << ray.direction;	// �p�x
	Print << ray.direction.getX();	// �p�x
	Print << ray.direction.getY();	// �p�x
	Print << ray.direction.getZ();	// �p�x
	Print << ray.origin;	// ���W
	Print << ray.origin.getX();	// ���W
	Print << ray.origin.getY();	// ���W
	Print << ray.origin.getZ();	// ���W


	camera.update(2.0);
	Graphics3D::SetCameraTransform(camera);

	// ����`��
	Plane{ 16 }.draw(ColorF{ 0.3 });

	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

	blacksmithModel.draw(Vec3{ 8, 0, 4 });

	// [RenderTexture �� 2D �V�[���ɕ`��]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });
	const Font& boldFont = FontAsset(U"Bold");

	boldFont(U"Camera Test").drawAt(400, 60);



	// ���݂̃t���[�����[�g���o��
	//ClearPrint();
	Print << Profiler::FPS() << U" FPS";
	Print << count;


	// ���_�����ʍ��W�n�Ōv�Z����
	//m_eyePosition = Spherical{ 24, m_theta, (270_deg - m_phi) };

	// �J�������X�V����
	//m_camera.setView(m_eyePosition, m_focusPosition);

	// �V�[���ɃJ������ݒ肷��
	//Graphics3D::SetCameraTransform(m_camera);
}