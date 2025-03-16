# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
	backgroundColor = ColorF{ 0.05, 0.08, 0.1 }.removeSRGBCurve();

	renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

//	camera = DebugCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	camera = BasicCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);

	// おまじない
	// 牢屋のような薄暗い雰囲気の設定
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.2, 0.2, 0.25 }); // ほぼ暗闇
	Graphics3D::SetSunColor(ColorF{ 0.2, 0.2, 0.25 }); // 光源を弱める
	Graphics3D::SetSunDirection(Vec3{ 0, -1, -0.5 }.normalized()); // 影を強調

	AudioAsset(U"BGM").play();
}

void CameraTest::update()
{
	ClearPrint();

	Ray ray = getMouseRay();

	Print << ray;
	Print << ray.direction;	// 角度
	Print << ray.direction.getX();	// 角度
	Print << ray.direction.getY();	// 角度
	Print << ray.direction.getZ();	// 角度
	Print << ray.origin;	// 座標
	Print << ray.origin.getX();	// 座標
	Print << ray.origin.getY();	// 座標
	Print << ray.origin.getZ();	// 座標


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
			if (!AudioAsset(U"足音45秒のループ").isPlaying()) {
				AudioAsset(U"足音45秒のループ").play();
			}
		}
		else {
			if (AudioAsset(U"足音45秒のループ").isPlaying()) {
				AudioAsset(U"足音45秒のループ").stop();
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

	// コリジョン
	if (m_eyePosition.x < -3.5
	|| m_eyePosition.x > 3.5
	|| m_eyePosition.z < -4.5
	|| m_eyePosition.z > 4.5
	|| m_eyePosition.y < 0.5	// 高さ
	|| m_eyePosition.y > 5.0	// 高さ
	)
	{
		// 進めない
		m_eyePosition = last_eyePosition;
	}
	last_eyePosition = m_eyePosition;



	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);

	Graphics3D::SetCameraTransform(camera);




	// 背景の描画
	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };


	// モデルを描画
	{
		Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
		
		model.draw();

		/* オブジェクトを回転させる実験
		const auto& materials = model.materials();

		Mat4x4 mat = Mat4x4::Translate(0, 0, 0);

		for (const auto& object : model.objects())
		{
			Mat4x4 m = Mat4x4::Identity();

			// 扉を開く
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

	// 鍵の描画
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


	// マウスの当たり判定
	Box box = Box{ Vec3{0, 0.9, 0}, 0.3 }.drawFrame(ColorF{ 1.0, 1.0, 1.0, 1.0 });

	if (box.intersects(ray))
	{
		// マウスが当たっている
		Print << U"HIT";
	}


}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });
	
	// 現在のフレームレートを出力
	Print << Profiler::FPS() << U" FPS";

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
}