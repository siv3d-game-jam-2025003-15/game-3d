# include "CameraTest.hpp"


CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
	backgroundColor = ColorF{ 0.05, 0.08, 0.1 }.removeSRGBCurve();

	camera = BasicCamera3D{ renderTexture.size(), m_verticalFOV, Vec3{ 10, 10, -10 } };

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);

	AudioAsset(U"BGM").play();

	mousePosX = Cursor::PosF().x;
	mousePosY = Cursor::PosF().y;
	toMousePosX = mousePosX;
	toMousePosY = mousePosY;
}

void CameraTest::update()
{
	ClearPrint();

    Print << Profiler::FPS() << U" FPS";

	if (Key1.down())
	{
		mouseDirectionX = 1;
	}
	if (Key2.down())
	{
		mouseDirectionX = -1;
	}
	if (Key3.down())
	{
		mouseDirectionY = 1;
	}
	if (Key4.down())
	{
		mouseDirectionY = -1;
	}

	if (mouseDirectionX == 1)
	{
		Print << U"[1][2]カメラ横回転：順";
	}
	else
	{
		Print << U"[1][2]カメラ横回転：逆";
	}
	if (mouseDirectionY == 1)
	{
		Print << U"[3][4]カメラ縦回転：順";
	}
	else
	{
		Print << U"[3][4]カメラ縦回転：逆";
	}

	// 指定したプレイヤーインデックスの XInput コントローラを取得
	auto controller = XInput(playerIndex);

	// それぞれデフォルト値を設定
	controller.setLeftTriggerDeadZone();
	controller.setRightTriggerDeadZone();
	controller.setLeftThumbDeadZone();
	controller.setRightThumbDeadZone();

	Ray ray = getMouseRay();

	float speed = 2.0f;

	const double deltaTime = Scene::DeltaTime();
	const double scaledSpeed =
		speed * ((KeyControl | KeyCommand).pressed() ? 20.0 : KeyShift.pressed() ? 5.0 : 1.0)
		* deltaTime;

	toMousePosX = Cursor::PosF().x;
	toMousePosY = Cursor::PosF().y;

	float diffMousePosX = 0.0f;
	float diffMousePosY = 0.0f;

	if (MouseL.pressed())
	{
		diffMousePosX =  (toMousePosX - mousePosX) / 10 * mouseDirectionX;
		diffMousePosY = -(toMousePosY - mousePosY) / 10 * mouseDirectionY;
	}

	mousePosX = toMousePosX;
	mousePosY = toMousePosY;

	if (KeyLeft.pressed() && isFocusSmooth == false)
	{
		m_phi += (60_deg * deltaTime);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (controller.rightThumbX < -0.1 && isFocusSmooth == false)
	{
		m_phi += (60_deg * deltaTime * -controller.rightThumbX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (diffMousePosX < -0.1 && isFocusSmooth == false)
	{
		m_phi += (60_deg * deltaTime * -diffMousePosX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (KeyRight.pressed() && isFocusSmooth == false)
	{
		m_phi -= (60_deg * deltaTime);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	if (controller.rightThumbX > 0.1 && isFocusSmooth == false)
	{
		m_phi -= (60_deg * deltaTime * controller.rightThumbX);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	if (diffMousePosX > 0.1 && isFocusSmooth == false)
	{
		m_phi -= (60_deg * deltaTime * diffMousePosX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi -= 360_deg;
		}
	}

	const double to_s = (Math::Cos(m_phi));
	const double to_c = (Math::Sin(m_phi));

	s = Math::Lerp(s, to_s, smooth / focusSmooth); // 回転もスムーズに
	c = Math::Lerp(c, to_c, smooth / focusSmooth); // 回転もスムーズに

	{
		const double xr = (scaledSpeed * s);
		const double zr = (scaledSpeed * c);

		bool isWalk = false;
		if (KeyW.pressed() && isFocusSmooth == false)
		{
			toCameraPos.x += xr;
			toCameraPos.z += zr;
			isWalk = true;
		}
		if (controller.leftThumbY > 0.1 && isFocusSmooth == false)
		{
			toCameraPos.x += (xr * controller.leftThumbY);
			toCameraPos.z += (zr * controller.leftThumbY);
			isWalk = true;
		}

		if (KeyS.pressed() && isFocusSmooth == false)
		{
			toCameraPos.x -= xr;
			toCameraPos.z -= zr;
			isWalk = true;
		}
		if (controller.leftThumbY < -0.1 && isFocusSmooth == false)
		{
			toCameraPos.x -= (xr * -controller.leftThumbY);
			toCameraPos.z -= (zr * -controller.leftThumbY);
			isWalk = true;
		}

		if (KeyA.pressed() && isFocusSmooth == false)
		{
			toCameraPos.x -= zr;
			toCameraPos.z += xr;
			isWalk = true;
		}
		if (controller.leftThumbX < -0.1 && isFocusSmooth == false)
		{
			toCameraPos.x -= (zr * -controller.leftThumbX);
			toCameraPos.z += (xr * -controller.leftThumbX);
			isWalk = true;
		}

		if (KeyD.pressed() && isFocusSmooth == false)
		{
			toCameraPos.x += zr;
			toCameraPos.z -= xr;
			isWalk = true;
		}
		if (controller.leftThumbX > 0.1 && isFocusSmooth == false)
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

			stopwatch.restart();
		}
	}

	{
		const double yDelta = deltaTime;

		if (KeyUp.pressed() && isFocusSmooth == false)
		{
			to_m_focusY += yDelta;
		}

		if (controller.rightThumbY > 0.1 && isFocusSmooth == false)
		{
			to_m_focusY += (yDelta * controller.rightThumbY);
		}

		if (diffMousePosY > 0.1 && isFocusSmooth == false)
		{
			to_m_focusY += (yDelta * diffMousePosY);
		}


		if (KeyDown.pressed() && isFocusSmooth == false)
		{
			to_m_focusY -= yDelta;
		}

		if (controller.rightThumbY < -0.1 && isFocusSmooth == false)
		{
			to_m_focusY -= (yDelta * -controller.rightThumbY);
		}

		if (diffMousePosY < -0.1 && isFocusSmooth == false)
		{
			to_m_focusY -= (yDelta * -diffMousePosY);
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

	// ゆっくり移動
	m_eyePosition = m_eyePosition.lerp(toCameraPos, smooth / focusSmooth);

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
	//smooth += Mouse::Wheel() * 0.01;
	//smooth = Clamp(smooth, 0.01, 1.0); // 範囲を制限
	//Print << U"スムーズの値：" << smooth;
#endif

	m_focusY = Math::Lerp(m_focusY, to_m_focusY, smooth / focusSmooth); // 回転もスムーズに

	// カメラ
	Vec3 focusVector { s, m_focusY, c };

	camera.setProjection(
		Graphics3D::GetRenderTargetSize(),
		m_verticalFOV - zoom,
		m_nearClip
	);

	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);

	Graphics3D::SetCameraTransform(camera);

	if (controller.rightTrigger > 0.1
	 && isFocus == false
	)
	{
		// Rトリガーでズーム
		to_zoom = controller.rightTrigger * (Math::Pi / 180) * 25 * deltaTime * 60;
	}
	else if (KeyZ.pressed()
	      && isFocus == false
	)
	{
		// Zキーでズーム
		to_zoom = 1 * (Math::Pi / 180) * 25 * deltaTime * 60;
	}
	//else if (MouseM.pressed()
	//	&& isFocus == false
	//	)
	//{
	//	// 中クリックでズーム
	//	to_zoom = 1 * (Math::Pi / 180) * 25 * deltaTime * 60;
	//}
	else
	{
		to_zoom = 0;
	}
	zoom = Math::Lerp(zoom, to_zoom, smooth / zoom_smooth / focusSmooth * deltaTime * 60); // ズームをスムーズに


	Vec3 keyObjectPostion = Vec3{ keyX, keyY, keyZ };

	// 対象のオブジェクトが画面の中心にあるかどうかの判定
	Vec3 worldScreenPoint = camera.worldToScreenPoint(keyObjectPostion);

	double keyDistance = m_eyePosition.distanceFrom(keyObjectPostion);

	focusWait -= deltaTime;

#ifdef _DEBUG
	//Print << U"focusWait" << focusWait;
#endif

	if (
		worldScreenPoint.x >= (1280 / 2 - 200)
	 && worldScreenPoint.x <= (1280 / 2 + 200)
	 && worldScreenPoint.y >= (720 / 2 - 200)
	 && worldScreenPoint.y <= (720 / 2 + 200)
	 && keyDistance < 3.5
	 && isFocusSmooth == false
	 && focusWait < 0.0f
	)
	{
		// オブジェクトが画面の中心にある
#ifdef _DEBUG
		Print << U"オブジェクトが画面の中心にある";
#endif
		Print << U"左クリックで鍵を取る";
		Print << U"右クリックでズーム解除";


		// ズームしていたらフォーカスする
		//if (controller.rightTrigger > 0.5
		// || KeyZ.pressed()
		// || isFocus	// フォーカス中はカメラをフォーカスしたままにしたいため
		//)
		//{
			// 前の位置を記憶
			if (isFocus == false)
			{
				lastToCameraPos = toCameraPos;
				last_to_m_focusY = to_m_focusY;
				last_m_phi = m_phi;
			}

			// カメラの移動
			toCameraPos = Vec3{ keyFocusX ,keyFocusY, keyFocusZ };

			// カメラの向き
			to_m_focusY = keyFocusCameraY;
			m_phi = keyFocusPhi;

			// フォーカスの速度を遅くする
			focusSmooth = 2;

			isFocus = true;

			if (controller.buttonA.pressed()
			 || KeyEnter.pressed()
			 //|| MouseL.down()
			 //|| MouseR.down()
			)
			{
				isKeyHave = true;
				AudioAsset(U"GET").play();
				AudioAsset(U"BGM").stop();
			}
		//}
	}

	Vec3 doorObjectPostion = Vec3{ doorX, doorY, doorZ };

	worldScreenPoint = camera.worldToScreenPoint(doorObjectPostion);

	double doorDistance = m_eyePosition.distanceFrom(doorObjectPostion);

	//Print << U"doorObjectPostion=" << doorObjectPostion;
	//Print << U"worldScreenPoint2=" << worldScreenPoint;
	//Print << U"doorDistance=" << doorDistance;

	// TODO 共通化する
	if (
		worldScreenPoint.x >= (1280 / 2 - 200)
		&& worldScreenPoint.x <= (1280 / 2 + 200)
		&& worldScreenPoint.y >= (720 / 2 - 200)
		&& worldScreenPoint.y <= (720 / 2 + 200)
		&& doorDistance < 2.0
		&& isFocusSmooth == false
		&& focusWait < 0.0f
	)
	{
		// オブジェクトが画面の中心にある
#ifdef _DEBUG
		Print << U"オブジェクトが画面の中心にある";
#endif
		Print << U"左クリックで扉を開ける";
		Print << U"右クリックでズーム解除";

		// ズームしていたらフォーカスする
		//if (
		//	controller.rightTrigger > 0.5
		//	|| KeyZ.pressed()
		//	|| isFocus	// フォーカス中はカメラをフォーカスしたままにしたいため
		//	)
		//{
			// 前の位置を記憶
			if (isFocus == false)
			{
				lastToCameraPos = toCameraPos;
				last_to_m_focusY = to_m_focusY;
				last_m_phi = m_phi;
			}

			// カメラの移動
			toCameraPos = Vec3{ doorFocusX ,doorFocusY, doorFocusZ };

			// カメラの向き
			to_m_focusY = doorFocusCameraY;
			m_phi = doorFocusPhi;

			// フォーカスの速度を遅くする
			focusSmooth = 2;

			isFocus = true;

			if (isClear == false && isKeyHave == true)
			{
				if (controller.buttonA.pressed()
				|| KeyEnter.pressed()
				//|| MouseL.down()
				//|| MouseR.down()
					)
				{
					// クリア
					AudioAsset(U"BGM").stop();
					AudioAsset(U"牢屋の扉を開ける").play();
					isClear = true;
					changeScene(State::Title);
				}
			}
		//}

	}

	//Print << U"isFocusSmooth" << isFocusSmooth;

	if (KeyBackspace.pressed()
	 || controller.buttonB.pressed()
//	 || MouseL.down()
	 || MouseR.down()
	)
	{
		// フォーカスのキャンセル
		if (isFocus)
		{
			toCameraPos = lastToCameraPos;
			to_m_focusY = last_to_m_focusY;
			m_phi = last_m_phi;

			//focusSmooth = 1;	// 終わってから
			isFocusSmooth = true;

			isFocus = false;

			focusWait = 5.0f;	// ５秒間フォーカスできない
		}
	}

	if (isFocusSmooth)
	{
		double distance = toCameraPos.distanceFrom(m_eyePosition);

	//	Print << U"差分：" << distance;

		if (distance < 0.01)
		{
			focusSmooth = 1;
			isFocusSmooth = false;
		}
	}

	// ライトの点滅
	if (Random(0, 100) == 0)
	{
		glowEffectType++;
		if (glowEffectType >= 12)
		{
			glowEffectType = 0;
		}
	}

	if (glowEffectType == 9
	 || glowEffectType == 11
	)
	{
		// 点滅
		if (Random(0, 5) == 0)
		{
			isGlowEffect = isGlowEffect ? false : true;
		}
	}
	else if (glowEffectType == 10 )
	{
		// 暗闇
		isGlowEffect = false;
	}
	else 
	{
		// 点灯
		isGlowEffect = true;
	}

	if (isGlowEffect)
	{
		toEmission = 1.0;
	}
	else
	{
		toEmission = 0.0;
	}

	// 光源の設定
	if (isGlowEffect)
	{
		// 点灯
		toGlobalAmbientColorR = 0.8;
		toGlobalAmbientColorG = 0.8;
		toGlobalAmbientColorB = 1.0;
	}
	else
	{
		// 点灯していない
		toGlobalAmbientColorR = 0.1;
		toGlobalAmbientColorG = 0.1;
		toGlobalAmbientColorB = 0.125;
	}

	GlobalAmbientColorR = Math::Lerp(GlobalAmbientColorR, toGlobalAmbientColorR, smooth);
	GlobalAmbientColorG = Math::Lerp(GlobalAmbientColorG, toGlobalAmbientColorG, smooth);
	GlobalAmbientColorB = Math::Lerp(GlobalAmbientColorB, toGlobalAmbientColorB, smooth);

	Graphics3D::SetGlobalAmbientColor(ColorF{ GlobalAmbientColorR, GlobalAmbientColorG, GlobalAmbientColorB });
	Graphics3D::SetSunColor(ColorF{ GlobalAmbientColorR, GlobalAmbientColorG, GlobalAmbientColorB });



#ifdef _DEBUG
	if (KeyE.pressed()) { lightY += 0.0001; }
	if (KeyX.pressed()) { lightY -= 0.0001; }
	if (KeyR.pressed()) { lightSize += 0.0001; }
	if (KeyC.pressed()) { lightSize -= 0.0001; }
	if (KeyT.pressed()) { emission += 0.1; }
	if (KeyV.pressed()) { emission -= 0.1; }

	//Print << lightY;
	//Print << lightSize;
	//Print << emission;
#endif

	// モデルを描画
	//{
	//	Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };

	//	model.draw();
	//}



	// スポットライト
	{
		const ScopedCustomShader3D shader{ vs3D, ps3D };
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

		// モデルを描画
		{
			Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };

			model.draw();
		}

		// 鍵の描画（シェーダーを適用するために、ここで描画しています）
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
			// マウスの当たり判定の描画
			Box box = Box{
				Vec3{ keyX, keyY, keyZ }, 
				0.3
			}.drawFrame(ColorF{ 1, 1, 1, 1 });

			// フォーカス中
			if (isFocus)
			{
				if (box.intersects(ray))
				{
					// マウスが当たっている
					//Print << U"HIT";

					if (
					   MouseL.down()
				//	|| MouseR.down()
					)
					{
						isKeyHave = true;
						AudioAsset(U"GET").play();
						AudioAsset(U"BGM").stop();
					}
				}
			}
		}

		// TODO 共通化する
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
				Box box = Box{ 
					Vec3{doorX,doorY,doorZ}, 
					0.2
				}.drawFrame(ColorF{ 1, 1, 1, 1 });

				if (isFocus)
				{
					if (box.intersects(ray))
					{
						// マウスが当たっている
						//Print << U"HIT";

						if (
							MouseL.down()
							//	|| MouseR.down()
							)
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
		}


	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}

	emission = Math::Lerp(emission, toEmission, smooth);

	if (emission >= 0.2)
	{ 
		const ScopedCustomShader2D shader{ psBright };
		const ScopedRenderTarget2D target{ gaussianA4.clear(ColorF{0.0}) };
		renderTexture.scaled(1).draw();

		// ライト
		PhongMaterial phong;
		phong.ambientColor = ColorF{ 1.0 };
		phong.diffuseColor = ColorF{ 0.0 };
		phong.emissionColor = ColorF{ 1.0, 1.0, 1.0 }.removeSRGBCurve() * (emission);
		Sphere{ {0, lightY, 0}, lightSize }.draw(phong);
		
		//const auto& materials = model.materials();
		//for (const auto& object : model.objects())
		//{
		//	if (object.name == U"FixRoom EV_Light01")
		//	{
		//		object.draw(materials);
		//	}
		//	Print << object.name;
		//}
		
		// 高輝度部分のぼかしテクスチャの生成
		//Shader::GaussianBlur(gaussianA4, gaussianB4, gaussianA4);
		//Shader::Downsample(gaussianA4, gaussianA8);
		//Shader::GaussianBlur(gaussianA8, gaussianB8, gaussianA8);
		//Shader::GaussianBlur(gaussianA8, gaussianB8, gaussianA8);
		//Shader::Downsample(gaussianA8, gaussianA16);
		//Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
		//Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
		//Shader::GaussianBlur(gaussianA16, gaussianB16, gaussianA16);
		
		// Glow エフェクト
		//const ScopedRenderStates2D blend{ BlendState::AdditiveRGB };
		//{
		//	const ScopedRenderTarget2D target{ gaussianA8 };
		//	gaussianA16.scaled(2.0).draw(ColorF{ 3.0 });
		//}

		//{
		//	const ScopedRenderTarget2D target{ gaussianA4 };
		//	gaussianA8.scaled(2.0).draw(ColorF{ 1.0 });
		//}

		//gaussianA4.resized(Scene::Size()).draw(ColorF{ 1.0 });
	}
}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0, 0, 0 });
	
	// 現在のフレームレートを出力
	//Print << Profiler::FPS() << U" FPS";
}