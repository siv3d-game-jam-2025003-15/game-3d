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
	//camera = DebugCamera3D{ renderTexture.size(), 45_deg, Vec3{ 10, 10, -10 } };
	//camera = DebugCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	//camera = DebugCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	camera = BasicCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	//	DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 10, 16, -32 } };
//	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 10, 16, -32 } };

	//Audio audio(U"close.wav");

	//audio.setVolume(0.5);

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);

	// おまじない
	// 牢屋のような薄暗い雰囲気の設定
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.2, 0.2, 0.25 }); // ほぼ暗闇
	Graphics3D::SetSunColor(ColorF{ 0.2, 0.2, 0.25 }); // 光源を弱める
	Graphics3D::SetSunDirection(Vec3{ 0, -1, -0.5 }.normalized()); // 影を強調

	AudioAsset(U"BGM").play();

	//m_focusY = 0.0f;
}

void CameraTest::update()
{
	ClearPrint();

	//if (MouseL.down())
	//{
	//	count++;
	//}
	//if (KeyUp.pressed())
	//{
	//	count++;
	//}
	//if (KeySpace.down())
	//{
	//	audio.play();
	//}


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

	Vec3 fixedPosition{ 0, 5, -10 };
	Vec3 fixedForward{ 0, 0, 1 };

	if (ray.origin.getX() < -2.0)
	{

	}
	else
	{
		// 移動できる
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

	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);


	Graphics3D::SetCameraTransform(camera);

	last_eyePosition = m_eyePosition;







	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };


	{
		Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
		// モデルを描画
		model.draw();
	}


	//blacksmithModel.draw(Vec3{ 8, 0, 4 });


	// 地面の描画
	//groundPlane.draw(groundTexture);

	// マウスの位置を判定
	//Box box = Box{ Vec3{0, 0, 0}, 1.0 }.draw(ColorF{ 1.0, 1.0, 1.0, 1.0 });
	Box box = Box{ Vec3{0, 0, 0}, 1 }.drawFrame(ColorF{ 1.0, 1.0, 1.0, 1.0 });	// TODO 半透明にできない
	//const Box box = Box::FromPoints(Vec3{ 1, 4, 4 }, Vec3{ 0, 4, 4 });

	if (box.intersects(ray))
	{
		Print << U"HIT";
	}

	// コリジョン確認用
	// Box{ Vec3{0, 0, 5.5}, 10 }.drawFrame(ColorF{ 1.0, 1.0, 1.0, 1.0 });	// TODO 半透明にできない


}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });
	const Font& boldFont = FontAsset(U"Bold");

	boldFont(U"Camera Test").drawAt(400, 60);



	// 現在のフレームレートを出力
	//ClearPrint();
	Print << Profiler::FPS() << U" FPS";
	Print << count;


	// 視点を球面座標系で計算する
	//m_eyePosition = Spherical{ 24, m_theta, (270_deg - m_phi) };

	// カメラを更新する
	//m_camera.setView(m_eyePosition, m_focusPosition);

	// シーンにカメラを設定する
	//Graphics3D::SetCameraTransform(m_camera);

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
}