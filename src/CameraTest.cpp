# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
	backgroundColor = ColorF{ 0.05, 0.08, 0.1 }.removeSRGBCurve();

//	renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

//	camera = DebugCamera3D{ renderTexture.size(), 55_deg, Vec3{ 10, 10, -10 } };
	camera = BasicCamera3D{ renderTexture.size(), m_verticalFOV, Vec3{ 10, 10, -10 } };

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);

	// おまじない
	// 牢屋のような薄暗い雰囲気の設定
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.2, 0.2, 0.25 }); // ほぼ暗闇
	Graphics3D::SetSunColor(ColorF{ 0.2, 0.2, 0.25 }); // 光源を弱める
	Graphics3D::SetSunDirection(Vec3{ 0, -1, -0.5 }.normalized()); // 影を強調

	AudioAsset(U"BGM").play();

	toCameraPos = m_eyePosition;

	to_m_focusY = m_focusY;
}

void CameraTest::update()
{
	ClearPrint();

	// 指定したプレイヤーインデックスの XInput コントローラを取得
	auto controller = XInput(playerIndex);

	// それぞれデフォルト値を設定
	controller.setLeftTriggerDeadZone();
	controller.setRightTriggerDeadZone();
	controller.setLeftThumbDeadZone();
	controller.setRightThumbDeadZone();

	//Print << controller.leftThumbX;
	//Print << controller.leftThumbY;
	//Print << controller.rightThumbX;
	//Print << controller.rightThumbY;

	Ray ray = getMouseRay();

	//Print << ray;
	//Print << ray.direction;	// 角度
	//Print << ray.direction.getX();	// 角度
	//Print << ray.direction.getY();	// 角度
	//Print << ray.direction.getZ();	// 角度
	//Print << ray.origin;	// 座標
	//Print << ray.origin.getX();	// 座標
	//Print << ray.origin.getY();	// 座標
	//Print << ray.origin.getZ();	// 座標

	//double t = Min(stopwatch.sF(), 1.0);

	//Print << t;

	// イージング関数を適用
	//const double e = EaseInOutExpo(t);

	//Print << e;

	float speed = 2.0f;
	//float speed = 2.0f * e;

	//Print << U"speed：" << speed;

	const double deltaTime = Scene::DeltaTime();
	const double scaledSpeed =
		speed * ((KeyControl | KeyCommand).pressed() ? 20.0 : KeyShift.pressed() ? 5.0 : 1.0)
		* deltaTime;

	if (KeyLeft.pressed())
	{
		m_phi += (60_deg * deltaTime);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (controller.rightThumbX < -0.1)
	{
		m_phi += (60_deg * deltaTime * -controller.rightThumbX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (KeyRight.pressed())
	{
		m_phi -= (60_deg * deltaTime);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	if (controller.rightThumbX > 0.1)
	{
		m_phi -= (60_deg * deltaTime * controller.rightThumbX);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	const double to_s = (Math::Cos(m_phi));
	const double to_c = (Math::Sin(m_phi));

	s = Math::Lerp(s, to_s, smooth); // 回転もスムーズに
	c = Math::Lerp(c, to_c, smooth); // 回転もスムーズに

	//Vec3 oldCameraPos = { 0.0, 2.0, -5.0 };
	//Quat oldRotation = Quat::Identity();
	//Quat targetRotation = oldRotation;

	{
		const double xr = (scaledSpeed * s);
		const double zr = (scaledSpeed * c);

		bool isWalk = false;
		if (KeyW.pressed())
		{
			toCameraPos.x += xr;
			toCameraPos.z += zr;
			isWalk = true;
		}
		if (controller.leftThumbY > 0.1)
		{
			toCameraPos.x += (xr * controller.leftThumbY);
			toCameraPos.z += (zr * controller.leftThumbY);
			isWalk = true;
		}

		if (KeyS.pressed())
		{
			toCameraPos.x -= xr;
			toCameraPos.z -= zr;
			isWalk = true;
		}
		if (controller.leftThumbY < -0.1)
		{
			toCameraPos.x -= (xr * -controller.leftThumbY);
			toCameraPos.z -= (zr * -controller.leftThumbY);
			isWalk = true;
		}

		if (KeyA.pressed())
		{
			toCameraPos.x -= zr;
			toCameraPos.z += xr;
			isWalk = true;
		}
		if (controller.leftThumbX < -0.1)
		{
			toCameraPos.x -= (zr * -controller.leftThumbX);
			toCameraPos.z += (xr * -controller.leftThumbX);
			isWalk = true;
		}

		if (KeyD.pressed())
		{
			toCameraPos.x += zr;
			toCameraPos.z -= xr;
			isWalk = true;
		}
		if (controller.leftThumbX > 0.1)
		{
			toCameraPos.x += (zr * controller.leftThumbX);
			toCameraPos.z -= (xr * controller.leftThumbX);
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

			// スタート位置を現在の位置に
			//fromCameraPos = m_eyePosition;

			// ゴール位置をマウスカーソルの位置に
			//toCameraPos = Cursor::Pos();

			stopwatch.restart();
		}
	}

	{
		const double yDelta = deltaTime;

		if (KeyUp.pressed())
		{
			to_m_focusY += yDelta;
		}
		if (controller.rightThumbY > 0.1)
		{
			to_m_focusY += (yDelta * controller.rightThumbY);
		}

		if (KeyDown.pressed())
		{
			to_m_focusY -= yDelta;
		}
		if (controller.rightThumbY < -0.1)
		{
			to_m_focusY -= (yDelta * -controller.rightThumbY);
		}

		// カメラ上下の可動域
		if (to_m_focusY < -focusY_max)
		{
			to_m_focusY = -focusY_max;
		}
		else if (to_m_focusY > focusY_max)
		{
			to_m_focusY = focusY_max;
		}
	}
	/*
	if (KeyE.pressed())
	{
		m_eyePosition.y += scaledSpeed;
	}

	if (KeyX.pressed())
	{
		m_eyePosition.y -= scaledSpeed;
	}
	*/

	// 移動の割合 0.0～1.0
	//const double t = Min(stopwatch.sF(), 1.0);

	// ゆっくり移動
	//m_eyePosition = m_eyePosition.lerp(toCameraPos, e);
	m_eyePosition = m_eyePosition.lerp(toCameraPos, smooth);

	//Print << m_eyePosition;
	//Print << fromCameraPos;
	//Print << toCameraPos;

	//Print << m_eyePosition;

	// コリジョン
	if (toCameraPos.x < -3.5
	|| toCameraPos.x > 3.5
	|| toCameraPos.z < -4.5
	|| toCameraPos.z > 4.5
	|| toCameraPos.y < 0.5	// 高さ
	|| toCameraPos.y > 5.0	// 高さ
	)
	{
		// 進めない
		toCameraPos = last_eyePosition;
	}
	last_eyePosition = toCameraPos;

#ifdef _DEBUG
	// マウスホイールの入力でFOVを変更
	//m_verticalFOV += Mouse::Wheel() * ( Math::Pi / 180);
	//m_verticalFOV = Clamp(m_verticalFOV, 1_deg, 180_deg); // FOVの範囲を制限
	//Print << U"カメラの視野角：" << m_verticalFOV / (Math::Pi / 180);

	// マウスホイールで、スムーズの値を調整
	smooth += Mouse::Wheel() * 0.01;
	smooth = Clamp(smooth, 0.01, 1.0); // 範囲を制限
	Print << U"スムーズの値：" << smooth;
#endif

	m_focusY = Math::Lerp(m_focusY, to_m_focusY, smooth); // 回転もスムーズに

	// カメラ
	Vec3 focusVector { s, m_focusY, c };

	camera.setProjection(
		Graphics3D::GetRenderTargetSize(),
		m_verticalFOV - zoom,
		m_nearClip
	);

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

	if (controller.rightTrigger > 0.1)
	{
		// Rトリガーでズーム
		to_zoom = controller.rightTrigger * (Math::Pi / 180) * 25;
	}
	else if (KeyZ.pressed())
	{
		// Rトリガーでズーム
		to_zoom = 1 * (Math::Pi / 180) * 25;
	}
	else
	{
		to_zoom = 0;
	}
	zoom = Math::Lerp(zoom, to_zoom, smooth/5); // ズームをスムーズに


	// 鍵の描画
	float keyX = 3.25;
	float keyY = 0.6;
	float keyZ = 3.3;
	if (isKeyHave == false)
	{
		{
			Mat4x4 mat = Mat4x4::Translate(keyX, keyY, keyZ);
			Mat4x4 m = Mat4x4::Identity();
			m *= Mat4x4::Rotate(
				Vec3{ 1,1,1 },
				45_deg,
				Vec3{ keyX, keyY, keyZ }
			);
			const Transformer3D transform{ mat * m };
			modelKey.draw();
		}

#ifndef _DEBUG
		Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(0).translated({100,100,100}) };	// 見えない位置へ
#endif
		// マウスの当たり判定
		Box box = Box{
			Vec3{ keyX, keyY, keyZ }, 0.3
		}.drawFrame(ColorF{ 1, 1, 1, 1 });

		if (box.intersects(ray))
		{
			// マウスが当たっている
			//Print << U"HIT";

			if (MouseL.down())
			{
				isKeyHave = true;
				AudioAsset(U"GET").play();
				AudioAsset(U"BGM").stop();
			}
		}
	}

	if (isKeyHave == true
	 && isClear == false
	)
	{
		// BGMの再開
		if (bgmStopCount > 4.0f)
		{
			if (!AudioAsset(U"BGM").isPlaying())
			{
				AudioAsset(U"BGM").play();
			}
		}
		else {
			bgmStopCount += deltaTime;
		}

		// 扉に入れるようにする
		{
#ifndef _DEBUG
			Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(0).translated({100,100,100}) };	// 見えない位置へ
#endif
			// マウスの当たり判定
			Box box = Box{ Vec3{-1.6, 1.0, -4.9}, 0.2 }.drawFrame(ColorF{ 1, 1, 1, 1 });

			if (box.intersects(ray))
			{
				// マウスが当たっている
				//Print << U"HIT";

				if (MouseL.down())
				{
					// クリア
					AudioAsset(U"BGM").stop();
					AudioAsset(U"牢屋の扉を開ける").play();
					isClear = true;
					changeScene(State::Title);
				}
			}
		}
	}


	// 点滅
	int value2 = Random(0, 1000);
	if (value2 == 0)
	{
		isGlowEffect2 = isGlowEffect2 ? false : true;
	}
	if (isGlowEffect2)
	{
		// 点灯
		isGlowEffect = true;
	}
	else
	{
		// ランダムで点灯
		int value = Random(0, 50);
		if (value == 0)
		{
			isGlowEffect = isGlowEffect ? false : true;
		}
	}

	if (KeyE.pressed()) { lightY += 0.0001; }
	if (KeyX.pressed()) { lightY -= 0.0001; }
	if (KeyR.pressed()) { lightSize += 0.0001; }
	if (KeyC.pressed()) { lightSize -= 0.0001; }
	if (KeyT.pressed()) { emission += 1; }
	if (KeyV.pressed()) { emission -= 1; }

	Print << lightY;
	Print << lightSize;
	Print << emission;

	// ライト
	if (isGlowEffect)
	{ 
		PhongMaterial phong;
		phong.ambientColor = ColorF{ 1.0 };
		phong.diffuseColor = ColorF{ 0.0 };
		phong.emissionColor = ColorF{ 1.0, 1.0, 1.0 }.removeSRGBCurve() * (emission);
		Sphere{ {0, lightY, 0}, lightSize }.draw(phong);
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}


	if (isGlowEffect)
	{
		// 高輝度部分を抽出
		{
			const ScopedCustomShader2D shader{ psBright };
			const ScopedRenderTarget2D target{ gaussianA4.clear(ColorF{0.0}) };
			renderTexture.scaled(0.25).draw();
		}

		// 高輝度部分のぼかしテクスチャの生成
		{
			Shader::GaussianBlur(gaussianA4, gaussianB4, gaussianA4);
			Shader::Downsample(gaussianA4, gaussianA8);
			Shader::GaussianBlur(gaussianA8, gaussianB8, gaussianA8);
			Shader::GaussianBlur(gaussianA8, gaussianB8, gaussianA8);
			Shader::Downsample(gaussianA8, gaussianA16);
			Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
			Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
			Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
		}

		// Glow エフェクト
		{
			const ScopedRenderStates2D blend{ BlendState::AdditiveRGB };

			{
				const ScopedRenderTarget2D target{ gaussianA8 };
				gaussianA16.scaled(2.0).draw(ColorF{ 3.0 });
			}

			{
				const ScopedRenderTarget2D target{ gaussianA4 };
				gaussianA8.scaled(2.0).draw(ColorF{ 1.0 });
			}

			gaussianA4.resized(Scene::Size()).draw(ColorF{ 1.0 });
		}
	}

}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0, 0, 0 });
	
	// 現在のフレームレートを出力
	//Print << Profiler::FPS() << U" FPS";





}