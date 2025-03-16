# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
	backgroundColor = ColorF{ 0.05, 0.08, 0.1 }.removeSRGBCurve();

	renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

//	camera = DebugCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	camera = BasicCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };

	// ���f���ɕt������e�N�X�`�����A�Z�b�g�Ǘ��ɓo�^
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);

	// ���܂��Ȃ�
	// �S���̂悤�Ȕ��Â����͋C�̐ݒ�
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.2, 0.2, 0.25 }); // �قڈÈ�
	Graphics3D::SetSunColor(ColorF{ 0.2, 0.2, 0.25 }); // ��������߂�
	Graphics3D::SetSunDirection(Vec3{ 0, -1, -0.5 }.normalized()); // �e������

	AudioAsset(U"BGM").play();
}

void CameraTest::update()
{
	ClearPrint();

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

		bool isWalk = false;
		if (KeyW.pressed())
		{
			m_eyePosition.x += xr;
			m_eyePosition.z += zr;
			isWalk = true;
		}

		if (KeyS.pressed())
		{
			m_eyePosition.x -= xr;
			m_eyePosition.z -= zr;
			isWalk = true;
		}

		if (KeyA.pressed())
		{
			m_eyePosition.x -= zr;
			m_eyePosition.z += xr;
			isWalk = true;
		}

		if (KeyD.pressed())
		{
			m_eyePosition.x += zr;
			m_eyePosition.z -= xr;
			isWalk = true;
		}
		
		if (isWalk)
		{
			if (!AudioAsset(U"����45�b�̃��[�v").isPlaying()) {
				AudioAsset(U"����45�b�̃��[�v").play();
			}
		}
		else {
			if (AudioAsset(U"����45�b�̃��[�v").isPlaying()) {
				AudioAsset(U"����45�b�̃��[�v").stop();
			}
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
	if (m_eyePosition.x < -3.5
	|| m_eyePosition.x > 3.5
	|| m_eyePosition.z < -4.5
	|| m_eyePosition.z > 4.5
	|| m_eyePosition.y < 0.5	// ����
	|| m_eyePosition.y > 5.0	// ����
	)
	{
		// �i�߂Ȃ�
		m_eyePosition = last_eyePosition;
	}
	last_eyePosition = m_eyePosition;



	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);

	Graphics3D::SetCameraTransform(camera);




	// �w�i�̕`��
	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };


	// ���f����`��
	{
		Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
		
		model.draw();

		/* �I�u�W�F�N�g����]���������
		const auto& materials = model.materials();

		Mat4x4 mat = Mat4x4::Translate(0, 0, 0);

		for (const auto& object : model.objects())
		{
			Mat4x4 m = Mat4x4::Identity();

			// �����J��
			if (object.name == U"FixRoom EV_Floor")
			{
				m *= Mat4x4::Rotate(Vec3{ 0,1,0 }, (Scene::Time() * -120_deg), Vec3{ 0, 0, 0 });
			}

			Print << object.name;

			const Transformer3D transform{ (m * mat) };

			object.draw(materials);
		}
		*/
	}

	// ���̕`��
	{
		Mat4x4 mat = Mat4x4::Translate(0, 0.9, 0);
		Mat4x4 m = Mat4x4::Identity();
		m *= Mat4x4::Rotate(
			Vec3{ 1,1,1 },
			45_deg,
			Vec3{ 0, 0.9, 0 }
		);
		const Transformer3D transform{ mat * m };
		modelKey.draw();
	}


	// �}�E�X�̓����蔻��
	Box box = Box{ Vec3{0, 0.9, 0}, 0.3 }.drawFrame(ColorF{ 1.0, 1.0, 1.0, 1.0 });

	if (box.intersects(ray))
	{
		// �}�E�X���������Ă���
		Print << U"HIT";
	}


}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });
	
	// ���݂̃t���[�����[�g���o��
	Print << Profiler::FPS() << U" FPS";

	// [RenderTexture �� 2D �V�[���ɕ`��]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
}