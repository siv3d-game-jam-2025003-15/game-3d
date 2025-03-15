# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
	//, m_camera{ SceneSize, m_verticalFOV, m_eyePosition, m_focusPosition }
{
	backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	blacksmithModel = Model{ U"example/obj/blacksmith.obj" };
//	blacksmithModel = Model{ U"assets/models/Room/EV_Room01.obj" };

	groundTexture = Texture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };
	groundPlane = Mesh{ MeshData::OneSidedPlane(2000, { 400, 400 }) };

	//camera = DebugCamera3D{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -32 } };
	camera = DebugCamera3D{ renderTexture.size(), 45_deg, Vec3{ 10, 10, -10 } };
	//camera = BasicCamera3D{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -32 } };
	//	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -32 } };
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


	Ray ray = getMouseRay();

	Print << ray;
	Print << ray.direction;	// �p�x
	Print << ray.direction.getX();	// �p�x
	Print << ray.direction.getY();	// �p�x
	Print << ray.direction.getZ();	// �p�x
	Print << ray.origin;	// ���W
	Print << ray.origin.getX();	// ���W
	Print << ray.origin.getY();	// ���W
	Print << ray.origin.getZ();	// ���W

	Vec3 fixedPosition{ 0, 5, -10 };
	Vec3 fixedForward{ 0, 0, 1 };

	if (ray.origin.getX() < -2.0)
	{

	}
	else
	{
		// �ړ��ł���
		//camera.update(2.0);
	}

	//camera.setView(fixedPosition, fixedPosition, fixedPosition);

	//Graphics3D::SetCameraTransform(camera);






	float speed = 2.0f;

	const double deltaTime = Scene::DeltaTime();
	const double scaledSpeed =
		speed * ((KeyControl | KeyCommand).pressed() ? 20.0 : KeyShift.pressed() ? 5.0 : 1.0)
		* deltaTime;

	if (KeyLeft.pressed())
	{
		m_phi += (60_deg * deltaTime);

		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (KeyRight.pressed())
	{
		m_phi -= (60_deg * deltaTime);

		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	const double s = (Math::Cos(m_phi));
	const double c = (Math::Sin(m_phi));

	{
		const double xr = (scaledSpeed * s);
		const double zr = (scaledSpeed * c);

		if (KeyW.pressed())
		{
			m_eyePosition.x += xr;
			m_eyePosition.z += zr;
		}

		if (KeyS.pressed())
		{
			m_eyePosition.x -= xr;
			m_eyePosition.z -= zr;
		}

		if (KeyA.pressed())
		{
			m_eyePosition.x -= zr;
			m_eyePosition.z += xr;
		}

		if (KeyD.pressed())
		{
			m_eyePosition.x += zr;
			m_eyePosition.z -= xr;
		}
	}

	{
		const double yDelta = deltaTime;

		if (KeyUp.pressed())
		{
			m_focusY += yDelta;
		}

		if (KeyDown.pressed())
		{
			m_focusY -= yDelta;
		}
	}

	if (KeyE.pressed())
	{
		m_eyePosition.y += scaledSpeed;
	}

	if (KeyX.pressed())
	{
		m_eyePosition.y -= scaledSpeed;
	}

	const Vec3 focusVector{ s, m_focusY, c };

	camera.setProjection(Graphics3D::GetRenderTargetSize(), m_verticalFOV, m_nearClip);


	Print << m_eyePosition;

	// �R���W����
	if (m_eyePosition.x < -5.0
	|| m_eyePosition.x > 10.0
	|| m_eyePosition.z < -5.0
	|| m_eyePosition.z > 10.0
	|| m_eyePosition.y < 0.5	// ����
	|| m_eyePosition.y > 5.0	// ����
	)
	{
		// �i�߂Ȃ�
		m_eyePosition = last_eyePosition;
	}

	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);


	Graphics3D::SetCameraTransform(camera);

	last_eyePosition = m_eyePosition;












	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

	blacksmithModel.draw(Vec3{ 8, 0, 4 });

	// �n�ʂ̕`��
	groundPlane.draw(groundTexture);

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