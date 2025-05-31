#include "CameraTest.hpp"

// コンストラクタ
CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
	// 背景の色
	backgroundColor = ColorF{ 0.05, 0.08, 0.1 }.removeSRGBCurve();
	
	// カメラ
	camera = BasicCamera3D{ renderTexture.size(), m_verticalFOV, Vec3{ 10, 10, -10 } };

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDoor, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelIronKey, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelBread, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelPoker, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerChain, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerEye, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerFeather, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerFlower, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerNon, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDrawerSnake, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelShelf, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelExclamationMark, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelParchment, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelHanger, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelDirtyCloth, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(modelMemo, TextureDesc::MippedSRGB);

	// BGMの再生
	AudioAsset(U"BGM").play();

	// 最初にカーソルを中央に
	Cursor::SetPos(center.x, center.y);

	// マウスの初期化
	mousePosX = Cursor::PosF().x;
	mousePosY = Cursor::PosF().y;
	toMousePosX = mousePosX;
	toMousePosY = mousePosY;
	virtualCursorPos.x = center.x;
	virtualCursorPos.y = center.y;

#ifdef _DEBUG
	bDebugViewCollision = true;
#endif

#ifndef _DEBUG
	// テキストメッセージを先に読み込んでおく
	dummyTextView(Text);
	dummyTextView(itemText);
	dummyTextView(itemNameText);
	dummyTextView(memoText);
	dummyTextView(toastedParchmentText);
#endif

	// メッセージを読んだかどうかのフラグをリセット
	for (int i = 0; i < Text.size() / MessagePatternMax; i++)
	{
		messageRead.push_back(-1);
	}

	// ドアの回転
	//toDoorRotY = doorRot.y;
	
	// ドアの移動（横に開ける）
	toDoorPosX = doorPos.x;
	toDoor2PosX = door2Pos.x;

	// ドアが開いているかどうか
	bDoorOpen[0] = false;	// 最初の部屋
	bDoorOpen[1] = false;	// 左下の部屋
	bDoorOpen[2] = true;	// 奥の部屋
	bDoorOpen[3] = true;	// 左上の部屋

	// プロローグの文字列の長さ
	for (int i = 0; i < prologueText.size(); ++i)
	{
		prologueLength += prologueText[i].size();
	}

	// ストップウォッチ（なるべく最後に実行する）
	Stopwatch stopwatch{ StartImmediately::Yes };
}

// テキストメッセージを先に読み込んでおく
void CameraTest::dummyTextView(Array<String> text)
{
	for (int i = 0; i < text.size(); ++i)
	{
		boldFont(text[i]).drawAt(
			28,
			{ center.x, center.y },
			ColorF{ 1, 1, 1, 1 }
		);
	}
}

void CameraTest::dummyTextView(Array<Array<String>> text)
{
	for (int i = 0; i < text.size(); ++i)
	{
		dummyTextView(text[i]);
	}
}

// デバッグ機能
void CameraTest::debug()
{
	// デバッグ表示のクリア
	ClearPrint();

	Print << Profiler::FPS() << U" FPS";

	if (Key1.down())
	{
		mouseDirectionX == 1 ? mouseDirectionX = -1 : mouseDirectionX = 1;
	}
	if (Key2.down())
	{
		mouseDirectionY == 1 ? mouseDirectionY = -1 : mouseDirectionY = 1;
	}
	if (Key3.down())
	{
		// コリジョンワイヤーフレーム
		bDebugViewFrame == true ? bDebugViewFrame = false : bDebugViewFrame = true;
	}
	if (Key4.down())
	{
		// コリジョンを表示
		bDebugViewCollision == true ? bDebugViewCollision = false : bDebugViewCollision = true;
	}
	if (Key5.down())
	{
		// コリジョンを有効
		bCollision == true ? bCollision = false : bCollision = true;
	}
	if (Key6.down())
	{
		// モデルを表示
		bDebugviewModel == true ? bDebugviewModel = false : bDebugviewModel = true;
	}
	if (Key7.down())
	{
		// キャラライトの明るさ
		lightStrong += 0.1;
	}
	if (Key8.down())
	{
		// キャラライトの明るさ
		lightStrong -= 0.1;
	}
	if (Key9.down())
	{
		// 暖炉の明るさ
		fireplaceStrong += 0.1;
	}
	if (Key0.down())
	{
		// 暖炉の明るさ
		fireplaceStrong -= 0.1;
	}
	if (KeyT.down())
	{
		// 環境光の明るさ
		globalAmbientColor += 0.01;
	}
	if (KeyY.down())
	{
		// 環境光の明るさ
		globalAmbientColor -= 0.01;
	}
	if (KeyO.down())
	{
		// 太陽光の明るさ
		sunColor += 0.01;
	}
	if (KeyP.down())
	{
		// 太陽光の明るさ
		sunColor -= 0.01;
	}
	if (KeyK.down())
	{
		// 暖炉の明るさ（揺らぎ）
		fireplaceSin += 0.1;
	}
	if (KeyL.down())
	{
		// 暖炉の明るさ（揺らぎ）
		fireplaceSin -= 0.1;
	}

	//if (Key7.down())
	//{
	//	// ライトの点滅あり
	//	bDebugFlashingLight == true ? bDebugFlashingLight = false : bDebugFlashingLight = true;
	//}
	//if (Key8.down())
	//{
	//	// シェーダーあり
	//	bDebugShader == true ? bDebugShader = false : bDebugShader = true;
	//}
	//if (Key9.pressed())
	//{
	//	lightPosList[lightArea].x += 0.001;
	//}
	//if (Key0.pressed())
	//{
	//	lightPosList[lightArea].x -= 0.001;
	//}

	if (KeyN.pressed())
	{
		debugHeight += 1;
	}
	if (KeyM.pressed())
	{
		debugHeight -= 1;
	}
	if (KeyV.pressed())
	{
		door3Pos.y += 0.001;
	}
	if (KeyB.pressed())
	{
		door3Pos.y -= 0.001;
	}
	if (KeyX.pressed())
	{
		door3Pos.z += 0.001;
	}
	if (KeyC.pressed())
	{
		door3Pos.z -= 0.001;
	}

	if (mouseDirectionX == 1)
	{
		Print << U"[1]カメラ横回転：順";
	}
	else
	{
		Print << U"[1]カメラ横回転：逆";
	}

	if (mouseDirectionY == 1)
	{
		Print << U"[2]カメラ縦回転：順";
	}
	else
	{
		Print << U"[2]カメラ縦回転：逆";
	}
	
	if (bDebugViewFrame)
	{
		Print << U"[3]コリジョン：ワイヤーフレーム";
	}
	else
	{
		Print << U"[3]コリジョン：塗りつぶし";
	}

	if (bDebugViewCollision)
	{
		Print << U"[4]コリジョン表示：表示";
	}
	else
	{
		Print << U"[4]コリジョン表示：非表示";
	}

	if (bCollision)
	{
		Print << U"[5]コリジョン：有効";
	}
	else
	{
		Print << U"[5]コリジョン：無効";
	}

	if (bDebugviewModel)
	{
		Print << U"[6]モデル：表示";
	}
	else
	{
		Print << U"[6]モデル：非表示";
	}

	Print << U"[7][8]キャラライトの明るさ：" << lightStrong;
	Print << U"[9][0]暖炉の明るさ（最小値）：" << fireplaceStrong;
	Print << U"[K][L]暖炉の明るさ（最大値）：" << fireplaceStrong+fireplaceSin;
	Print << U"[T][Y]環境光の明るさ：" << globalAmbientColor;
	Print << U"[O][P]太陽光の明るさ：" << sunColor;

	// 環境光
	float globalAmbientColor = 0.01;

	// 太陽光
	float sunColor = 0.0;

	//if (bDebugFlashingLight)
	//{
	//	Print << U"[7]ライトの点滅：あり";
	//}
	//else
	//{
	//	Print << U"[7]ライトの点滅：なし";
	//}

	//if (bDebugShader)
	//{
	//	Print << U"[8]シェーダー：あり";
	//}
	//else
	//{
	//	Print << U"[8]シェーダー：なし";
	//}

	Print << U"[R][F]上下移動";

#ifdef _DEBUG

	Print << U"CameraX=" << toCameraPos.x;
	Print << U"CameraZ=" << toCameraPos.z;

	Print << U"debugHeight=" << debugHeight;

	Print << U"messagePattern=" << messagePattern;
	Print << U"messagePatternCount=" << messagePatternCount;

#endif
}

// マウスポインタのRay
Ray CameraTest::getMouseRay() const
{
	return camera.screenToRay(Cursor::PosF());
}

void CameraTest::drawMiniItem(
	int itemId,
	int x,
	int y
) const
{
	switch (itemId)
	{
	case Bread:
		// パン
		breadMiniSprite.draw(x, y);
		break;
	case Memo:
		// 手記
		memoMiniSprite.draw(x, y);
		break;
	case Key:
		// 錆びた鍵
		keyMiniSprite.draw(x, y);
		break;
	case Poker:
		// 火かき棒
		pokerMiniSprite.draw(x, y);
		break;
	case Parchment:
		// 羊皮紙
		parchmentMiniSprite.draw(x, y);
		break;
	case Wire:
		// 針金
		wireMiniSprite.draw(x, y);
		break;
	case DirtyCloth:
		// 汚れた布
		dirtyClothMiniSprite.draw(x, y);
		break;
	case Cloth:
		// 布
		clothMiniSprite.draw(x, y);
		break;
	case ToastedParchment:
		// 炙った羊皮紙
		toastedParchmentMiniSprite.draw(x, y);
		break;
	case WireKey:
		// 針金の鍵
		wireKeyMiniSprite.draw(x, y);
		break;
	case IronKey:
		// 鉄製の鍵
		ironKeyMiniSprite.draw(x, y);
		break;
	}
}

void CameraTest::drawBigItem(
	int itemId,
	int x,
	int y
) const
{
	switch (itemId)
	{
	case Bread:
		// パン
		breadBigSprite.draw(x, y);
		break;
	case Memo:
		// 手記
		memoBigSprite.draw(x, y);
		break;
	case Key:
		// 錆びた鍵
		keyBigSprite.draw(x, y);
		break;
	case Poker:
		// 火かき棒
		pokerBigSprite.draw(x, y);
		break;
	case Parchment:
		// 羊皮紙
		parchmentBigSprite.draw(x, y);
		break;
	case Wire:
		// 針金
		wireBigSprite.draw(x, y);
		break;
	case DirtyCloth:
		// 汚れた布
		dirtyClothBigSprite.draw(x, y);
		break;
	case Cloth:
		// 布
		clothBigSprite.draw(x, y);
		break;
	case ToastedParchment:
		// 炙った羊皮紙
		toastedParchmentBigSprite.draw(x, y);
		break;
	case WireKey:
		// 針金の鍵
		wireKeyBigSprite.draw(x, y);
		break;
	case IronKey:
		// 鉄製の鍵
		ironKeyBigSprite.draw(x, y);
		break;
	}
}

void CameraTest::inventoryOnOff()
{
	// インベントリフラグの切り替え
	bInventory = bInventory ? false : true;

	// いったんカーソルを強制的に中央に戻す
	Cursor::SetPos(center.x, center.y);

	// フラグはオフにする TODO 増えたら困る
	bMemo = false;
	bToastedParchmentRead = false;
	bClothRead = false;
}

void CameraTest::update()
{
	const double deltaTime = Scene::DeltaTime();

	// デバッグ表示
	debug();

	// シナリオの進捗によってメッセージを変える
	message = scenario;

	// 指定したプレイヤーインデックスの XInput コントローラを取得
	auto controller = XInput(playerIndex);
	controller.setLeftTriggerDeadZone();
	controller.setRightTriggerDeadZone();
	controller.setLeftThumbDeadZone();
	controller.setRightThumbDeadZone();

	// マウスのRayを取得
	Ray ray = getMouseRay();

	// 移動速度
	double addSpeed = 1.0;
	if ((KeyControl | KeyCommand).pressed())
	{
		// コントロールキーを押しているときの速度
	//	addSpeed = 20.0;
		addSpeed = 5.0;
	}
	else if (KeyShift.pressed())
	{
		// シフトキーを押しているときの速度
	//	addSpeed = 5.0;
		addSpeed = 2.5;
	}
	const double scaledSpeed = cameraSpeed * addSpeed * deltaTime;

	// マウスの座標を取得
	double diffMousePosX = 0.0f;
	double diffMousePosY = 0.0f;

	// アイテムのロックオンフラグ
	bool bLockon = false;

	// インベントリを表示しているかどうか
	if (!bInventory)
	{
		// インベントリを表示していない（通常時）

		// このアプリケーションがアクティブな時だけ
		if (Window::GetState().focused)
		{
			// マウスカーソルを非表示
			Cursor::RequestStyle(CursorStyle::Hidden);

			// 現在のマウス座標
			Vec2 currentCursorPos = Cursor::PosF();

			// 中心からの差分を取る
			Vec2 delta = currentCursorPos - center;

			// 仮想座標に加算
			virtualCursorPos += delta;

			toMousePosX = virtualCursorPos.x;
			toMousePosY = virtualCursorPos.y;

			diffMousePosX = (toMousePosX - mousePosX) / 10 * mouseDirectionX;
			diffMousePosY = -(toMousePosY - mousePosY) / 10 * mouseDirectionY;

			mousePosX = toMousePosX;
			mousePosY = toMousePosY;

			// 毎フレーム、カーソルを強制的に中央に戻す
			Cursor::SetPos(center.x, center.y);
		}



		// プレイヤーの移動
		if (KeyLeft.pressed())
		{
			phiController.rotate(60_deg, deltaTime, 1.0);
		}

		if (controller.rightThumbX < -0.1)
		{
			phiController.rotate(60_deg, deltaTime, -controller.rightThumbX);
		}

		if (diffMousePosX < -0.1)
		{
			phiController.rotate(60_deg, deltaTime, -diffMousePosX);
		}

		if (KeyRight.pressed())
		{
			phiController.rotate(-60_deg, deltaTime, 1.0);
		}

		if (controller.rightThumbX > 0.1)
		{
			phiController.rotate(-60_deg, deltaTime, controller.rightThumbX);
		}

		if (diffMousePosX > 0.1)
		{
			phiController.rotate(-60_deg, deltaTime, diffMousePosX);
		}

		{
			const double xr = (scaledSpeed * sss);
			const double zr = (scaledSpeed * ccc);

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

			if (KeyR.pressed())
			{
				toCameraPos.y += scaledSpeed;
			}
			if (KeyF.pressed())
			{
				toCameraPos.y -= scaledSpeed;
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

			if (KeyUp.pressed())
			{
				to_m_focusY += yDelta;
			}

			if (controller.rightThumbY > 0.1)
			{
				to_m_focusY += (yDelta * controller.rightThumbY);
			}

			if (diffMousePosY > 0.1)
			{
				to_m_focusY += (yDelta * diffMousePosY);
			}


			if (KeyDown.pressed())
			{
				to_m_focusY -= yDelta;
			}

			if (controller.rightThumbY < -0.1)
			{
				to_m_focusY -= (yDelta * -controller.rightThumbY);
			}

			if (diffMousePosY < -0.1)
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

		// オブジェクトを取ることができるか
		if (!bLockon)
		{
			auto [a, b, c] = breadController.update(breadPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true && bBreadHave == false)
			{
				// アイテムを取った
				items << Bread;
				bBreadHave = a;
				bgmStopCount = c;

				scenario = 19;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 50;
			}

		}

		{
			// アイテム１は手記
		}

		// 錆びた鍵
		if (!bLockon && bKeyHave == false)
		{
			auto [a, b, c] = keyController.update(keyPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true)
			{
				// アイテムを取った
				items << Key;
				bKeyHave = true;
				bgmStopCount = c;
				scenario = 3;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 52;
			}
		}

		// 鉄製の鍵
		if (!bLockon && bIronKeyHave == false)
		{
			auto [a, b, c] = ironkeyController.update(IronkeyPos, camera, m_eyePosition, ray, markPosition, 0, bPokerHave);
			if (a == true)
			{
				// アイテムを取った
				items << IronKey;
				bIronKeyHave = true;
				bgmStopCount = c;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 31;
			}
		}

		// 火かき棒
		if (!bLockon)
		{
			auto [a, b, c] = pokerController.update(pokerPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true && bPokerHave == false)
			{
				// アイテムを取った
				items << Poker;
				bPokerHave = a;
				bgmStopCount = c;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 53;
			}
		}

		// ドア
		if (!bLockon)
		{
			// 座標の調整
			Vec3 temp = doorPos;
			//temp.x += 0.70;	// 原点が端っこの時
			temp.x += 0.0;
			temp.y += 1.2;
			temp.z += 0.2;

			auto [a, b, c] = doorController.update(temp, camera, m_eyePosition, ray, markPosition, 1, bWireKey);
			// TODO インベントリから開ける
			if (a == true && bDoorOpen[0] == false && bWireKey)
			{
				// ドアを開いた
				bDoorOpen[0] = true;
			//	toDoorRotY = 270_deg;	// 回転で開ける
			//	toDoorPosX = -0.11;	// 移動で開ける
				toDoorPosX = doorPos.x + 1.49;	// 移動で開ける
				bgmStopCount = c;

				scenario = 6;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				if (bWireKey)
				{
					message = 20;
				}
				else
				{
					message = 10;
				}
			}
		}

		// ドア２
		if (!bLockon)
		{
			// 座標の調整
			Vec3 temp = door2Pos;
			temp.x += 0.0;
			temp.y += 1.2;
			temp.z -= 0.2;

			auto [a, b, c] = door2Controller.update(temp, camera, m_eyePosition, ray, markPosition, 1, bIronKeyHave);	
			// TODO インベントリから開ける
			if (a == true && bDoorOpen[1] == false)
			{
				// ドアを開いた
				bDoorOpen[1] = true;
				toDoor2PosX = door2Pos.x + 1.49;	// 移動で開ける
				bgmStopCount = c;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 30;
			}
		}

		// ドア３（右上のドア）
		if (!bLockon)
		{
			auto [a, b, c] = door3Controller.update(door3Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 33;
			}
		}

		// 羊皮紙
		if (!bLockon)
		{
			auto [a, b, c] = parchmentController.update(parchmentPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true && bParchmentHave == false)
			{
				// アイテムを取った
				items << Parchment;
				bParchmentHave = a;
				bgmStopCount = c;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 54;
			}
		}

		// 自分のベッド
		if (!bLockon)
		{
			auto [a, b, c] = bedController.update(bedPos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 11;
			}
		}

		// 他人のベッド2
		if (!bLockon)
		{
			auto [a, b, c] = bed2Controller.update(bed2Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 14;
			}
		}

		// 他人のベッド3
		if (!bLockon)
		{
			auto [a, b, c] = bed3Controller.update(bed3Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 14;
			}
		}

		// 他人のベッド4
		if (!bLockon)
		{
			auto [a, b, c] = bed4Controller.update(bed4Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 14;
			}
		}

		// 古いベッド
		if (!bLockon)
		{
			auto [a, b, c] = oldBedController.update(oldBedPos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 15;
			}
		}

		// トイレ
		if (!bLockon)
		{
			auto [a, b, c] = toiletController.update(toiletPos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 12;
			}
		}

		// トイレ２（左上の部屋）
		if (!bLockon)
		{
			auto [a, b, c] = toiletController.update(toilet2Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 32;
				bToilet2Lockon = true;
			}
			else
			{
				bToilet2Lockon = false;
			}
		}
		else
		{
			bToilet2Lockon = false;
		}

		// 棚
		if (!bLockon)
		{
			auto [a, b, c] = shelfController.update(shelfPos, camera, m_eyePosition, ray, markPosition, -1, false);
			//if (a == true && bWire == false)
			//{
			//	// 針金が取れる
			////	items << Shelf;
			//	bWire = true;
			//	bgmStopCount = c;
			//	scenario = 4;
			//	bHangerFind = true;
			//}
			if (b)
			{
				// 見ている
				bLockon = b;
				//if (bKeyHave)
				//{
				//	// 鍵を持っている
				//	message = 17;
				//}
				//else
				//{
					// 鍵を持っていない
					message = 13;
				//}
			}
		}

		// 壁
		//if (!bLockon && !bWall && scenario == 2)
		//{
		//	auto [a, b, c] = wallController.update(wallPos, camera, m_eyePosition, ray, markPosition, 0, true);
		//	if (a == true && bParchmentHave == false)
		//	{
		//		// 鍵が現れる
		//		bWall = true;
		//		bgmStopCount = c;
		//	}
		//	if (b)
		//	{
		//		// 見ている
		//		bLockon = b;
		//		message = 16;
		//	}
		//}

		// 暖炉（火が弱い）
		if (!bLockon && bFireplaceStrong == false)
		{
		//	auto [a, b, c] = fireplaceController.update(fireplacePos, camera, m_eyePosition, ray, MarkPosition, 0, bPokerHave);
			auto [a, b, c] = fireplaceController.update(fireplacePos, camera, m_eyePosition, ray, markPosition, 0, false);
			//if (a == true)
			//{
			//	// 火が強くなった
			//	bFireplaceStrong = true;
			//}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 18;
				bFireplaceWeakLockon = true;
			}
			else
			{
				bFireplaceWeakLockon = false;
			}
		}
		else
		{
			bFireplaceWeakLockon = false;
		}

		// 暖炉（火が強い）
		if (!bLockon && bFireplaceStrong)
		{
			auto [a, b, c] = fireplaceStrongController.update(fireplacePos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 21;
				bFireplaceStrongLockon = true;
			}
			else
			{
				bFireplaceStrongLockon = false;
			}
		}
		else
		{
			bFireplaceStrongLockon = false;
		}

		// ハンガー
		if (!bLockon && bHangerHave == false)
		{
			auto [a, b, c] = hangerController.update(hangerPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true)
			{
				// ハンガーを取得
				bHangerHave = true;
				items << Wire;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 22;
			}
		}

		// 樽
		if (!bLockon)
		{
			auto [a, b, c] = barrelController.update(barrelPos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 25;
			}
		}

		// 樽2
		if (!bLockon)
		{
			auto [a, b, c] = barrelController.update(barrel2Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 25;
			}
		}

		// 樽3
		if (!bLockon)
		{
			auto [a, b, c] = barrelController.update(barrel3Pos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 27;

			//	bBarrel3Lockon = true;
			}
			else
			{
			//	bBarrel3Lockon = false;
			}
		}
		else
		{
		//	bBarrel3Lockon = false;
		}

		// 教団の紋章
		if (!bLockon)
		{
			auto [a, b, c] = barrelController.update(emblemPos, camera, m_eyePosition, ray, markPosition, -1, false);
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 23;
			}
		}

		// 汚れた布
		if (!bLockon && bDirtyClothHave == false)
		{
			auto [a, b, c] = dirtyClothController.update(dirtyClothPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true)
			{
				// 汚れた布を取得
				bDirtyClothHave = true;
				items << DirtyCloth;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 26;
			}
		}

		// 手記
		if (!bLockon && bMemoHave == false)
		{
			auto [a, b, c] = memoController.update(memoPos, camera, m_eyePosition, ray, markPosition, 0, true);
			if (a == true)
			{
				// 手記を取得
				bMemoHave = true;
				items << Memo;
			}
			if (b)
			{
				// 見ている
				bLockon = b;
				message = 28;
			}
		}



		// メッセージパターンの自動変更
		messagePatternCount += deltaTime;

		// メッセージを切り替える
		if (bLockon == false)
		{
			if (messagePatternCount > 5.0)
			{
				messagePattern++;
				messagePattern %= MessagePatternMax;
				messagePatternCount = 0;
			}
		}
		else
		{
			if (MouseL.down())
			{
				messagePattern++;
				messagePattern %= 3;
			}

			if (messagePattern > messageRead[message])
			{
				messageRead[message]++;
				messagePattern = messageRead[message];
			}
		}

		int m = message * MessagePatternMax + messagePattern;
		if (Text[m].isEmpty())
		{
			// メッセージが空の場合、０に戻す
			messagePattern = 0;
		}



		//if (message == 14)
		//{
		//	if (MouseL.down())
		//	{
		//		messagePattern++;
		//		messagePattern %= 3;
		//	}

		//	// ベッド
		//	switch (messagePattern)
		//	{
		//	case 0:
		//		message = 23;
		//		break;
		//	case 1:
		//		message = 24;
		//		break;
		//	case 2:
		//		message = 25;
		//		break;
		//	}
		//}

	}
	else
	{
		// インベントリを表示している

		// 足音の削除
		if (AudioAsset(U"足音45秒のループ").isPlaying()) {
			AudioAsset(U"足音45秒のループ").stop();
		}

		if (KeyUp.down())
		{
			itemIndex -= 4;

			if (itemIndex < 0)
			{
				itemIndex += 12;
			}
		}

		if (KeyDown.down())
		{
			itemIndex += 4;

			if (itemIndex >= 12)
			{
				itemIndex -= 12;
			}
		}

		if (KeyLeft.down())
		{
			itemIndex -= 1;
			if (itemIndex < 0)
			{
				itemIndex += 12;
			}
		}

		if (KeyRight.down())
		{
			itemIndex += 1;
			if (itemIndex >= 15)
			{
				itemIndex -= 12;
			}
		}



	}

	// インベントリの表示・非表示
	if (KeyI.down())
	{
		inventoryOnOff();
	}

	// コリジョンを無効にするエリア
	for (int i = 0; i < DoorNum; i++)
	{
		if (bDoorOpen[i] == false)
		{
			continue;
		}

		if ( collisionNone[i][0] < toCameraPos.x
		  && toCameraPos.x < collisionNone[i][1]
		  && collisionNone[i][2] < toCameraPos.z
		  && toCameraPos.z < collisionNone[i][3]
		)
		{
			// コリジョンを無効にするエリア
			bCollisionDoor = false;
			break;
		}
		else
		{
			bCollisionDoor = true;
		}
	}

	// ライトエリア
	//int tmpLightArea = 0;
	//for (auto& row : collisionLight)
	//{
	//	if (row[0] < toCameraPos.x
	//		&& toCameraPos.x < row[1]
	//		&& row[2] < toCameraPos.z
	//		&& toCameraPos.z < row[3]
	//		)
	//	{
	//		// エリアの変更
	//		if (lightArea == lastLightArea)
	//		{
	//		//	lightTime = 2;
	//			lightTime = 1;
	//		//	lightTime = 0.5;
	//		}
	//		lightArea = tmpLightArea;
	//		break;
	//	}
	//	tmpLightArea++;
	//}

	Vec3_ myPosition = {
		toCameraPos.x,
		myRadius+0.1,
		toCameraPos.z
	};

	// 線分交差で判定する
	if (bCollision && bCollisionDoor)
	{
		Vec3 A{
			last_eyePosition.x,
			last_eyePosition.y,
			last_eyePosition.z
		};

		Vec3 cameraNormal = Vec3(
			toCameraPos.x - last_eyePosition.x,
			toCameraPos.y - last_eyePosition.y,
			toCameraPos.z - last_eyePosition.z
		).normalized();

		// 向かっている位置より少し前にコリジョンを持たせる
		Vec3 B{
			toCameraPos.x + cameraNormal.x / 2,
			toCameraPos.y + cameraNormal.y / 2,
			toCameraPos.z + cameraNormal.z / 2,
		};

		Segment segment = {
			{
				last_eyePosition.x,
				0.1,
				last_eyePosition.z
			}
			, 
			{
				toCameraPos.x + cameraNormal.x / 2,
				0.1,
				toCameraPos.z + cameraNormal.z / 2,
			}
		};

		const ColorF LineColor = ColorF{ 0.3, 0.2, 0.0 }.removeSRGBCurve();

		Line3D{ Vec3{ last_eyePosition.x, last_eyePosition.y, last_eyePosition.z }, Vec3{ toCameraPos.x, toCameraPos.y, toCameraPos.z } }.draw(LineColor);

		// モデルデータと判定する
		bool checkCollision = false;
		for (const auto& object : model.objects())
		{
			const std::array<Vec3, 8> cube = object.boundingBox.getCorners();

			for (int i = 0; i < 12; i++)
			{
				Triangle_ tri = {
					Vec3_{cube[collisionTriangle[i][0]].x / 100, cube[collisionTriangle[i][0]].y / 100, cube[collisionTriangle[i][0]].z / 100},
					Vec3_{cube[collisionTriangle[i][1]].x / 100, cube[collisionTriangle[i][1]].y / 100, cube[collisionTriangle[i][1]].z / 100},
					Vec3_{cube[collisionTriangle[i][2]].x / 100, cube[collisionTriangle[i][2]].y / 100, cube[collisionTriangle[i][2]].z / 100}
				};

				HitResult hit;

				Vec3_ triA = { cube[collisionTriangle[i][0]].x / 100, cube[collisionTriangle[i][0]].y / 100, cube[collisionTriangle[i][0]].z / 100 };
				Vec3_ triB = { cube[collisionTriangle[i][1]].x / 100, cube[collisionTriangle[i][1]].y / 100, cube[collisionTriangle[i][1]].z / 100 };
				Vec3_ triC = { cube[collisionTriangle[i][2]].x / 100, cube[collisionTriangle[i][2]].y / 100, cube[collisionTriangle[i][2]].z / 100 };

				if (IsSphereIntersectingTriangle(myPosition, myRadius, triA, triB, triC))
				{
					Print << U"交差しました！";

					// 交差している（ぶつかった）
					checkCollision = true;

					// いったん止める
					toCameraPos = last_eyePosition;

					break;





					// プレイヤーの移動速度
					Vec2 velocity = Vec2(
						toCameraPos.x - last_eyePosition.x,
						toCameraPos.z - last_eyePosition.z
					);
					double length = velocity.length();

					Vec2 velocityNormal = Vec2(
						toCameraPos.x - last_eyePosition.x,
						toCameraPos.z - last_eyePosition.z
					).normalized();

					// 当たり判定の法線
					Vec2 wallNormal = Vec2(
						cube[collisionList[i][0]].x - cube[collisionList[i][1]].x,
						cube[collisionList[i][0]].z - cube[collisionList[i][1]].z
					).normalized();

					// 壁の法線に対する接線方向を計算
					//Vec2 tangent = Vec2(wallNormal.x, wallNormal.y); // 法線に直交するベクトル

					// 今の速度を壁に沿ってスライド（内積で投影）
					//double dot = (velocityNormal.dot(wallNormal));

					// 壁の法線に対する接線方向を計算
					Vec2 tangent = Vec2(wallNormal.y, -wallNormal.x); // 法線に直交するベクトル

					// 今の速度を壁に沿ってスライド（内積で投影）
					Vec2 resultVelocity = tangent * (velocityNormal.dot(tangent));

					Vec2 resultVelocity2 = Vec2(
						resultVelocity.y,
						resultVelocity.x
					);

					double dot = (velocityNormal.dot(resultVelocity2));

					//if (dot > 0)
					//{
					//	// 前に進む
					//}
					//else if (dot < 0)
					//{
					//	// 後ろに進む
					//	// 進行方向を反転
					//	resultVelocity2.x *= -1;
					//	resultVelocity2.y *= -1;
					//}

					Print << U"dot " << dot;

					// いったん前の状態に戻す
					toCameraPos = last_eyePosition;

					// 壁に沿ってスライド
					toCameraPos.x += (resultVelocity2.x * length * dot);
					toCameraPos.z += (resultVelocity2.y * length * dot);


					// TODO ここから先、関数化する
					
					// 進んだ先にコリジョンがないかどうか
					Vec2 AA{
						last_eyePosition.x,
						last_eyePosition.z
					};

					// 向かっている位置より少し前にコリジョンを持たせる TODO これじゃなくて、距離にしたい
					Vec2 cameraNormal2 = Vec2(
						toCameraPos.x - last_eyePosition.x,
						toCameraPos.z - last_eyePosition.z
					).normalized();

					Vec2 BB{
						toCameraPos.x + cameraNormal2.x / 2,
						toCameraPos.z + cameraNormal2.y / 2,
					};

					for (const auto& object2 : model.objects())
					{
						const std::array<Vec3, 8> cube2 = object2.boundingBox.getCorners();

						for (int i2 = 0; i2 < 12; i2++)
						{
							if (
								isIntersecting(
									AA,
									BB,
									Vec2{ cube2[collisionList[i2][0]].x / 100, cube2[collisionList[i2][0]].z / 100 },
									Vec2{ cube2[collisionList[i2][1]].x / 100, cube2[collisionList[i2][1]].z / 100 }
								)
							)
							{
								// 交差している（ぶつかった）失敗
								checkCollision = false;
								toCameraPos = last_eyePosition;
								break;
						
							}
						}
						if (checkCollision == false)
						{
							// 進んだ先にコリジョンがある 失敗
							break;
						}
					}


					break;
				}
			}
			if (checkCollision)
			{
				// 正しいコリジョン
				break;
			}
		}
	}

	// カメラ関係
	const double to_s = (Math::Cos(phiController.getPhi()));
	const double to_c = (Math::Sin(phiController.getPhi()));

	sss = Math::Lerp(sss, to_s, smooth); // 回転もスムーズに
	ccc = Math::Lerp(ccc, to_c, smooth); // 回転もスムーズに

	// ゆっくり移動
	m_eyePosition = m_eyePosition.lerp(toCameraPos, smooth);

	last_eyePosition = toCameraPos;

	// 回転もスムーズに
	m_focusY = Math::Lerp(m_focusY, to_m_focusY, smooth); 

	// カメラ
	Vec3 focusVector { sss, m_focusY, ccc };

	camera.setProjection(
		Graphics3D::GetRenderTargetSize(),
		//m_verticalFOV - zoom,
		m_verticalFOV,
		m_nearClip
	);

	camera.setView(m_eyePosition, (m_eyePosition + focusVector), m_upDirection);

	Graphics3D::SetCameraTransform(camera);


	// ドアの回転
//	doorRot.y = Math::Lerp(doorRot.y, toDoorRotY, smooth);
//	doorRot.y = Math::Lerp(doorRot.y, toDoorRotY, smooth/10);	// ドアはゆっくり開ける

	// ドアの移動
	doorPos.x = Math::Lerp(doorPos.x, toDoorPosX, smooth / 10);	// ドアはゆっくり開ける
	door2Pos.x = Math::Lerp(door2Pos.x, toDoor2PosX, smooth / 10);	// ドアはゆっくり開ける


	// 止まっているBGMを再度鳴らす
	{
		// BGMの再開
		if (bgmStopCount <= 0.0f)
		{
			if (!AudioAsset(U"BGM").isPlaying())
			{
				AudioAsset(U"BGM").play();
			}
		}
		else {
			bgmStopCount -= deltaTime;

			// 演出
			//if (scenario == 6 && lightArea == 1 && bgmStopCount > 4.1)
			//{
			//	bgmStopCount = 1.0;
			//}
		}
	}


	//if (lightArea != lastLightArea)
	//{
	//	// ライトの位置が変わった

	//	toGlobalAmbientColorR = 0.1;
	//	toGlobalAmbientColorG = 0.1;
	//	toGlobalAmbientColorB = 0.125;

	//	lightTime -= deltaTime;

	//	if (lightTime < 0)
	//	{
	//		lastLightArea = lightArea;

	//		isGlowEffect = true;
	//	}
	//}
	//else
	//{
		// ライト位置
		lightPos = m_eyePosition;
		//lightPos = lightPosList[lightArea];
		//switch (lightArea)
		//{
		//case 0:
		//	lightPos.x = 0;
		//	lightPos.y = 2.084;
		//	lightPos.z = 7;
		//	break;
		//case 1:
		//	lightPos.x = 4.26;
		//	lightPos.y = 2.60;
		//	lightPos.z = -0.93;
		//	break;
		//case 2:
		//	lightPos.x = 16.05;
		//	lightPos.y = 2.60;
		//	lightPos.z = -0.92;
		//	break;
		//case 3:
		//	lightPos.x = 16.05;
		//	lightPos.y = 2.60;
		//	lightPos.z = -4.27;
		//	break;
		//}

		// ライトの点滅 TODO いつか使うかも？
		//if (Random(0, 100) == 0)
		//{
		//	glowEffectType++;
		//	if (glowEffectType >= 12)
		//	{
		//		glowEffectType = 0;
		//	}
		//}

		//if (glowEffectType == 9
		//	|| glowEffectType == 11
		//	)
		//{
		//	// 点滅
		//	if (Random(0, 5) == 0)
		//	{
		//		isGlowEffect = isGlowEffect ? false : true;
		//	}
		//}
		//else if (glowEffectType == 10)
		//{
		//	// 暗闇
		//	isGlowEffect = false;
		//}
		//else
		//{
		//	// 点灯
		//	isGlowEffect = true;
		//}

		// エミッシブ
		//if (isGlowEffect)
		//{
		//	toEmission = 1.0;
		//}
		//else
		//{
		//	toEmission = 0.0;
		//}

		//if (!bDebugFlashingLight)
		//{
		//	// 点滅しない場合は常に点灯
		//	isGlowEffect = true;
		//}

		// 光源の設定
		//if (isGlowEffect)
		//{
		//	// 点灯
		//	toGlobalAmbientColorR = 0.8;
		//	toGlobalAmbientColorG = 0.8;
		//	toGlobalAmbientColorB = 1.0;
		//}
		//else
		//{
		//	// 点灯していない
		//	toGlobalAmbientColorR = 0.1;
		//	toGlobalAmbientColorG = 0.1;
		//	toGlobalAmbientColorB = 0.125;
		//}
	//}

	//GlobalAmbientColorR = Math::Lerp(GlobalAmbientColorR, toGlobalAmbientColorR, lightSmooth);
	//GlobalAmbientColorG = Math::Lerp(GlobalAmbientColorG, toGlobalAmbientColorG, lightSmooth);
	//GlobalAmbientColorB = Math::Lerp(GlobalAmbientColorB, toGlobalAmbientColorB, lightSmooth);

	//Graphics3D::SetGlobalAmbientColor(
	//	ColorF{
	//		GlobalAmbientColorR,
	//		GlobalAmbientColorG,
	//		GlobalAmbientColorB
	//	}
	//);
	//Graphics3D::SetSunColor(
	//	ColorF{
	//		GlobalAmbientColorR,
	//		GlobalAmbientColorG,
	//		GlobalAmbientColorB
	//	}
	//);

	// 環境光を小さくする
	Graphics3D::SetGlobalAmbientColor(ColorF{ globalAmbientColor });
	
	// 太陽光をオフにする
	Graphics3D::SetSunColor(ColorF{ sunColor });

	// スポットライト
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

		// ライトの位置（ポイントライトVer.1）
		//ConstantBuffer<Light> cb;
		//cb->g_pointLightPos = lightPos;
		//cb->g_pointLightStrong = lightStrong;
		//Graphics3D::SetConstantBuffer(ShaderStage::Pixel, 2, cb);

		// 点光源を設定する
		constantBuffer->setPointLight(0, lightPos, ColorF{ 1.0, 1.0, 1.0 }, lightStrong);
		constantBuffer->setPointLight(1, fireplaceLightPos, ColorF{ 1.0, 0.2, 0.0 }, Periodic::Sine0_1(2s)* fireplaceSin + fireplaceStrong);

		const ScopedCustomShader3D shader(vs3D, ps3D);

		// ピクセルシェーダに定数バッファを渡す
		Graphics3D::SetPSConstantBuffer(4, constantBuffer);

		constantBuffer->drawPointLightAsEmissiveSphere(0, 0);
		constantBuffer->drawPointLightAsEmissiveSphere(1, 0.1);

		// モデルを描画
		if (bDebugviewModel)
		{
			Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
			model.draw();
		}

		// ドア
		{
			Transformer3D t{
				Mat4x4::RotateY(doorRot.y).scaled(roomScale).translated(doorPos)
			};
			modelDoor.draw();
		}

		// ドア２
		{
			Transformer3D t{
				Mat4x4::RotateY(door2Rot.y).scaled(roomScale).translated(door2Pos)
			};
			modelDoor.draw();
		}

		// パンの描画
		if (bBreadHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(breadPos)
			};

			modelBread.draw();
		}

		// 錆びた鍵の描画
		if (bKeyHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateZ(0_deg).scaled(0.015).translated(keyPos)
			};
			modelKey.draw();
		}

		// 鉄製の鍵の描画
		if (bIronKeyHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateZ(0_deg).scaled(0.015).translated(IronkeyPos)
			};
			modelIronKey.draw();
		}

		// 火かき棒の描画
		if (bPokerHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateZ(75_deg).scaled(0.01).translated(pokerPos)
			};

			modelPoker.draw();
		}

		// 引き出し（１段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y + 0.06 + 0.16*5, drawerPos.z})
			};

			modelDrawerNon.draw();
		}

		// 引き出し（２段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y + 0.06 + 0.16*4, drawerPos.z})
			};

			modelDrawerFlower.draw();
		}

		// 引き出し（３段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y + 0.06 + 0.16*3, drawerPos.z})
			};

			modelDrawerChain.draw();
		}

		// 引き出し（４段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y + 0.06 + 0.16*2, drawerPos.z})
			};

			modelDrawerFeather.draw();

		}

		// 引き出し（５段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y + 0.06+0.16, drawerPos.z})
			};

			modelDrawerSnake.draw();
		}

		// 引き出し（６段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerPos.x, drawerPos.y+ 0.06, drawerPos.z})
			};

			modelDrawerEye.draw();
		}

		// 引き出し
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(drawerPos)
			};

			modelShelf.draw();
		}

		// ビックリマーク
		if(bLockon)
		{
			// 少し上にする
			markPosition.y += markHigh;

			// 3Dのビックリマーク
			//Transformer3D t{
			//	Mat4x4::RotateY(0_deg).scaled(1).translated(MarkPosition)
			//};
			//modelExclamationMark.draw();

			// ビルボード
			const Mat4x4 billboardMat = camera.getInvView();
			billboard.draw(camera.billboard(markPosition, markSize), uvChecker);
		}

		// 羊皮紙の描画
		if (bParchmentHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(parchmentPos)
			};

			modelParchment.draw();
		}

		// ハンガーの描画
		if (bHangerHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(hangerRot.y).scaled(0.01).translated(hangerPos)
			};

			modelHanger.draw();
		}

		// 汚れた布の描画
		if (bDirtyClothHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(dirtyClothPos)
			};

			modelDirtyCloth.draw();
		}

		// 汚れた布の描画
		if (bMemoHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.1).translated(memoPos)
			};

			modelMemo.draw();
		}

		// デバッグ表示
		if (bDebugViewCollision)
		{
			// モデルのワイヤーフレーム表示
			{
				Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
				for (const auto& object : model.objects())
				{
					if (bDebugViewFrame)
					{
						// ワイヤーフレーム
						//object.boundingBox.drawFrame(ColorF{ 1, 1, 1, 1 });

						// 三角形
						const std::array<Vec3, 8> cube = object.boundingBox.getCorners();

						ColorF color{ 1, 1, 1, 1 };

						for (int i = 0; i < 12; i++)
						{
							Line3D{ cube[collisionTriangle[i][0]], cube[collisionTriangle[i][1]] }.draw(color);
							Line3D{ cube[collisionTriangle[i][1]], cube[collisionTriangle[i][2]] }.draw(color);
							Line3D{ cube[collisionTriangle[i][2]], cube[collisionTriangle[i][0]] }.draw(color);
						}
					}
					else
					{
						// 塗りつぶし
						object.boundingBox.draw(ColorF{ 1, 1, 1, 1 });
					}
				}
			}

			// コリジョンなし
			for (int i = 0; i < DoorNum; i++)
			{
				if (bDoorOpen[i] == false)
				{
					continue;
				}

				ColorF color{ 0.0, 0.0, 1, 1 };

				// 縦
				Line3D{ Vec3{collisionNone[i][0], 0, collisionNone[i][2]}, Vec3{collisionNone[i][0], 3, collisionNone[i][2]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][1], 0, collisionNone[i][2]}, Vec3{collisionNone[i][1], 3, collisionNone[i][2]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][0], 0, collisionNone[i][3]}, Vec3{collisionNone[i][0], 3, collisionNone[i][3]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][1], 0, collisionNone[i][3]}, Vec3{collisionNone[i][1], 3, collisionNone[i][3]} }.draw(color);

				// 下
				Line3D{ Vec3{collisionNone[i][0], 0, collisionNone[i][2]}, Vec3{collisionNone[i][1], 0, collisionNone[i][2]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][0], 0, collisionNone[i][3]}, Vec3{collisionNone[i][1], 0, collisionNone[i][3]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][0], 0, collisionNone[i][2]}, Vec3{collisionNone[i][0], 0, collisionNone[i][3]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][1], 0, collisionNone[i][2]}, Vec3{collisionNone[i][1], 0, collisionNone[i][3]} }.draw(color);

				// 上
				Line3D{ Vec3{collisionNone[i][0], 3, collisionNone[i][2]}, Vec3{collisionNone[i][1], 3, collisionNone[i][2]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][0], 3, collisionNone[i][3]}, Vec3{collisionNone[i][1], 3, collisionNone[i][3]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][0], 3, collisionNone[i][2]}, Vec3{collisionNone[i][0], 3, collisionNone[i][3]} }.draw(color);
				Line3D{ Vec3{collisionNone[i][1], 3, collisionNone[i][2]}, Vec3{collisionNone[i][1], 3, collisionNone[i][3]} }.draw(color);
			}

			// ライトの判定
			//for (auto& row : collisionLight)
			//{
			//	ColorF color{ 1.0, 1.0, 0.0, 1 };

			//	// 縦
			//	Line3D{ Vec3{row[0], 0, row[2]}, Vec3{row[0], 3, row[2]} }.draw(color);
			//	Line3D{ Vec3{row[1], 0, row[2]}, Vec3{row[1], 3, row[2]} }.draw(color);
			//	Line3D{ Vec3{row[0], 0, row[3]}, Vec3{row[0], 3, row[3]} }.draw(color);
			//	Line3D{ Vec3{row[1], 0, row[3]}, Vec3{row[1], 3, row[3]} }.draw(color);

			//	// 下
			//	Line3D{ Vec3{row[0], 0, row[2]}, Vec3{row[1], 0, row[2]} }.draw(color);
			//	Line3D{ Vec3{row[0], 0, row[3]}, Vec3{row[1], 0, row[3]} }.draw(color);
			//	Line3D{ Vec3{row[0], 0, row[2]}, Vec3{row[0], 0, row[3]} }.draw(color);
			//	Line3D{ Vec3{row[1], 0, row[2]}, Vec3{row[1], 0, row[3]} }.draw(color);

			//	// 上
			//	Line3D{ Vec3{row[0], 3, row[2]}, Vec3{row[1], 3, row[2]} }.draw(color);
			//	Line3D{ Vec3{row[0], 3, row[3]}, Vec3{row[1], 3, row[3]} }.draw(color);
			//	Line3D{ Vec3{row[0], 3, row[2]}, Vec3{row[0], 3, row[3]} }.draw(color);
			//	Line3D{ Vec3{row[1], 3, row[2]}, Vec3{row[1], 3, row[3]} }.draw(color);
			//}

		}

#if _DEBUG
		{
			Print << U"m_focusPosition" << camera.getFocusPosition();
			modelExclamationMark.draw(camera.getFocusPosition());
		}
#endif
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}

	// ライトの表示
#if _DEBUG
	//emission = Math::Lerp(emission, toEmission, smooth);

	//if (emission >= 0.2)
	//{ 
	//	const ScopedCustomShader2D shader{ psBright };
	//	const ScopedRenderTarget2D target{ gaussianA4.clear(ColorF{0.0}) };
	//	renderTexture.scaled(1).draw();

	//	PhongMaterial phong;
	//	phong.ambientColor = ColorF{ 1.0 };
	//	phong.diffuseColor = ColorF{ 0.0 };
	//	phong.emissionColor = ColorF{ 1.0, 1.0, 1.0 }.removeSRGBCurve() * (emission);
	//	Sphere{ lightPos, lightSize }.draw(phong);

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
	//}
#endif

	// UI
	if (bInventory)
	{
		// インベントリ
		int itemX = center.x - inventoryWidth / 2;
		int itemY = center.y - inventoryHeight / 2;
		inventorySprite.scaled(0.5).draw(itemX, itemY);

		int selectItem = -1;

		// 初期化
		itemMessage = -1;

		// インベントリ全体
		{
			Rect rect(itemX, itemY, inventoryWidth, inventoryHeight);
			if (rect.mouseOver())
			{
				//Print << U"インベントリ内";
			}
			else
			{
				//Print << U"インベントリ外";
				if (MouseL.down())
				{
					// インベントリを非表示にする
					inventoryOnOff();
				}
			}
		}

		// アイテム
		for (int i = 0; i < items.size(); i++)
		{
			int itemMiniX = center.x - itemMiniWidth / 2 + (i % 4 * 82) - 194;
			int itemMiniY = center.y - itemMiniHeight / 2 + (i / 4 * 82) - 104;

			drawMiniItem(items[i], itemMiniX, itemMiniY);

			if (bMemo || bToastedParchmentRead || bClothRead)	// TODO 増えたら困る
			{
				// 選択できないようにする
				continue;
			}

			Rect rect(itemMiniX, itemMiniY, itemMiniWidth, itemMiniHeight);

			if (rect.mouseOver())
			{
				// 枠線
				rect.drawFrame(
					2,		// 枠線の太さ
					Palette::Skyblue	// 空色で描画
				);

				selectItem = i;

				// コメント
				itemMessage = items[i];
			}
			else
			{
				// 枠線
				rect.drawFrame(
					1,				// 枠線の太さ
					Palette::Black	// 黒で描画
				);
			}

		}

		//現在選択中のアイテム
		if (selectItem >= 0)
		{
			int itemBigX = center.x - itemBigWidth / 2 + 166;
			int itemBigY = center.y - itemBigHeight / 2 - 35;

			drawBigItem(items[selectItem], itemBigX, itemBigY);
		}

		if (MouseL.down())
		{
			// アイテムを使う
			if (bMemo || bToastedParchmentRead || bClothRead)	// TODO 増えたら困る
			{
				// メッセージが表示されていたら閉じる
				bMemo = false;
				bToastedParchmentRead = false;
				bClothRead = false;
			}
			else if (items.size() <= selectItem)
			{
				// 処理しない
			}
			else if (items[selectItem] == Bread)
			{
				// パンを食べる

				// 手記を手に入れる（←オミット）
				//items[selectItem] = Memo;
				items.remove_at(selectItem);

				// シナリオを進める
				scenario = 1;

				// SEを鳴らす
				AudioAsset(U"BGM").stop();
				AudioAsset(U"GET").play();
				bgmStopCount = 4.00;
			}
			else if (items[selectItem] == Memo)
			{
				// 手記を使った
				bMemo = true;

				if (scenario == 1)
				{
					scenario = 2;
				}
			}
			else if (items[selectItem] == Wire)
			{
				if (bKeyHave)
				{
					// 錆びた鍵を持っている状態で針金を使う
					items[selectItem] = WireKey;
					bWireKey = true;

					// SEを鳴らす
					AudioAsset(U"BGM").stop();
					AudioAsset(U"GET").play();
					bgmStopCount = 4.00;

					scenario = 5;
				}
			}
			else if (items[selectItem] == Poker)
			{
				// 火が弱い暖炉の近くで使う
				if (bFireplaceWeakLockon)
				{
					// 暖炉の火を強くする
					bFireplaceStrong = true;

					// SEを鳴らす
					AudioAsset(U"BGM").stop();
					AudioAsset(U"GET").play();
					bgmStopCount = 4.00;
				}
			}
			else if (items[selectItem] == Parchment)
			{
				// 火が強くなった暖炉の近くで使う
				if (bFireplaceStrongLockon)
				{
					// 暖炉で羊皮紙を使う 
					items[selectItem] = ToastedParchment;

					// SEを鳴らす
					AudioAsset(U"BGM").stop();
					AudioAsset(U"GET").play();
					bgmStopCount = 4.00;
				}
			}
			else if (items[selectItem] == ToastedParchment)
			{
				// 炙った羊皮紙を使った
				bToastedParchmentRead = true;
			}
			else if (items[selectItem] == DirtyCloth)
			{
				// 汚れた布

				// トイレの近くで使う
				if (bToilet2Lockon)
				{
					items[selectItem] = Cloth;

					// SEを鳴らす
					AudioAsset(U"BGM").stop();
					AudioAsset(U"GET").play();
					bgmStopCount = 4.00;
				}
			}
			else if (items[selectItem] == Cloth)
			{
				// 布を使った
				bClothRead = true;
			}
		}
	}
	else
	{
#if _DEBUG
		// フォーカスエリア
		Rect rect(
			WINDOW_WIDTH / 2 - 200,
			WINDOW_HEIGHT / 2 - 200,
			400,
			400
		);

		rect.drawFrame(2, Palette::Red);
#endif
	}

	prologueCount += deltaTime;


	// 経過時間を取得
	const double frameTime = stopwatch.sF();
	if (frameTime < targetDeltaTime)
	{
		// 残り時間だけスリープ（精度を高めたいなら Sleep せずにループで待機する方法もある）
		System::Sleep(targetDeltaTime - frameTime);
	}

	// タイマーをリセットして次のフレームへ
	stopwatch.restart();
}

void CameraTest::draw() const
{
	// 背景色
	Scene::SetBackground(ColorF{ 0, 0, 0 });

	// プロローグ
	{
		// 画面全体を黒で描画
		Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF{ 0.0, 1.0 });

		// TODO 共通化する
		double lineSpacing = 96; // 行間（フォントサイズより少し大きめ）

		// 今、表示するための文字数
		int prologueIndex = prologueCount*5;

		// テキストのアルファ値
		float a = prologueCount * 5 - (float)prologueIndex;

		for (int i = 0; i < prologueText.size(); ++i)
		{
			if (prologueIndex <= 0)
			{
				break;
			}

			double x = center.x;
			double y = center.y + lineSpacing * (i - (int)prologueText.size() / 2) + 38;	// TODO 38は高さ調整のため

			int num = 0;
			int sub = 0;

			if (prologueIndex >= prologueText[i].size())
			{
				// すべて表示
				num = prologueText[i].size();
				prologueIndex -= prologueText[i].size();
			}
			else
			{
				// 一部表示
				num = prologueIndex;
				sub = prologueText[i].size() - num;
				prologueIndex = 0;
			}

			String tmp = prologueText[i].substr(0, num);
			tmp.append(sub, U'　');	// 後ろをスペースで埋める

			boldFont(tmp).drawAt(
				28,
				{ x, y },
				ColorF{ 1, 1, 1, 1 }
			);

			// 半透明文字
			if (num+1 < prologueText[i].size())
			{
				String tmp2;
				tmp2.append(num, U'　');		// 後ろをスペースで埋める
				tmp2.append(1, prologueText[i][num]);
				tmp2.append(sub - 1, U'　');

				boldFont(tmp2).drawAt(
					28,
					{ x, y },
					ColorF{ 1, 1, 1, a }
				);
			}

		}
	}

	// セリフ表示
	if (0 <= message && message < Text.size() / MessagePatternMax)
	{
		// 画面下を黒い半透明で描画
		Rect{ 0, Scene::Height()-40, Scene::Width(), Scene::Height() }.draw(ColorF{ 0.0, 0.5 });

		int m = message * MessagePatternMax + messagePattern;

		// ここでのチェックは不要
		//if (Text[m].isEmpty())
		//{
		//	m = message * MessagePatternMax;
		//}

		boldFont(Text[m]).drawAt(
			28,
		//	{ center.x, 700 },
			{ center.x, height - 20 },
			ColorF{ 1, 1, 1, 1 }
		);
	}

	// インベントリのアイテム説明
	if (
		bInventory
		&& 0 <= itemMessage
		&& itemMessage < itemText.size()
	)
	{
		//boldFont(itemText[itemMessage]).drawAt(
		//	28,
		////	{ center.x, 546 },
		//	{ center.x, center.y + itemHeight },
		//	ColorF{ 1, 1, 1, 1 }
		//);

		// TODO 共通化する
		double lineSpacing = 40.0; // 行間（フォントサイズより少し大きめ）

		for (int i = 0; i < itemText[itemMessage].size(); ++i)
		{
			double x = center.x;
			double y = center.y + 345 + lineSpacing * (i - (int)itemText.size() / 2);

			boldFont(itemText[itemMessage][i]).drawAt(
				28,
				{ x, y },
				ColorF{ 1, 1, 1, 1 }
			);
		}


	}

	if (
		bInventory
		&& 0 <= itemMessage
		&& itemMessage < itemText.size()
		)
	{
		// セリフ表示（仮）
		boldFont(itemNameText[itemMessage]).drawAt(
			28,
		//	{ 805, 236 },
			{ center.x + 165, center.y -128 },
			ColorF{ 1, 1, 1, 1 }
		);
	}

	// 手記
	if (bInventory && bMemo)
	{
		// 半透明の黒い画像
		Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF{ 0.0, 0.5 });

		// TODO 共通化する
		double lineSpacing = 40.0; // 行間（フォントサイズより少し大きめ）

		for (int i = 0; i < memoText.size(); ++i)
		{
			double x = center.x;
			double y = center.y + lineSpacing * (i - (int)memoText.size()/2);

			boldFont(memoText[i]).drawAt(
				28,
				{ x, y },
				ColorF{ 1, 1, 1, 1 }
			);
		}

	}

	// 羊皮紙
	if (bInventory && bToastedParchmentRead)
	{
		// 半透明の黒い画像
		Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF{ 0.0, 0.5 });

		// TODO 共通化する
		double lineSpacing = 40.0; // 行間（フォントサイズより少し大きめ）

		for (int i = 0; i < toastedParchmentText.size(); ++i)
		{
			double x = center.x;
			double y = center.y + lineSpacing * (i - (int)toastedParchmentText.size() / 2);

			boldFont(toastedParchmentText[i]).drawAt(
				28,
				{ x, y },
				ColorF{ 1, 1, 1, 1 }
			);
		}
	}

	// 羊皮紙
	if (bInventory && bClothRead)
	{
		// 半透明の黒い画像
		Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF{ 0.0, 0.5 });

		// TODO 共通化する
		double lineSpacing = 40.0; // 行間（フォントサイズより少し大きめ）

		for (int i = 0; i < clothText.size(); ++i)
		{
			double x = center.x;
			double y = center.y + lineSpacing * (i - (int)clothText.size() / 2);

			boldFont(clothText[i]).drawAt(
				28,
				{ x, y },
				ColorF{ 1, 1, 1, 1 }
			);
		}
	}

}