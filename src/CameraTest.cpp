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

	Stopwatch stopwatch{ StartImmediately::Yes };
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
		// ライトの点滅あり
		bDebugFlashingLight == true ? bDebugFlashingLight = false : bDebugFlashingLight = true;
	}
	if (Key8.down())
	{
		// シェーダーあり
		bDebugShader == true ? bDebugShader = false : bDebugShader = true;
	}
	if (Key9.down())
	{
		TextID += 1;
	}
	if (Key0.down())
	{
		TextID -= 1;
	}
	if (KeyO.pressed())
	{

	}
	if (KeyP.pressed())
	{

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

	if (bDebugFlashingLight)
	{
		Print << U"[7]ライトの点滅：あり";
	}
	else
	{
		Print << U"[7]ライトの点滅：なし";
	}

	if (bDebugShader)
	{
		Print << U"[8]シェーダー：あり";
	}
	else
	{
		Print << U"[8]シェーダー：なし";
	}

	Print << U"[R][F]上下移動";

#ifdef _DEBUG

	Print << U"CameraX=" << toCameraPos.x;
	Print << U"CameraZ=" << toCameraPos.z;

	//Print << U"phiController.getPhi()=" << phiController.getPhi();

	//Print << U"lightZ=" << lightZ;

	//Print << U"breadX=" << breadX;
	//Print << U"breadY=" << breadY;
	//Print << U"breadZ=" << breadZ;
	//
	//Print << U"debugDrawerY=" << debugDrawerY;

	//Print << U"tmpItemX=" << tmpItemX;
	//Print << U"tmpItemY=" << tmpItemY;

	//Print << U"itemIndex=" << itemIndex;

	//Print << U"drawerX=" << drawerX;
	//Print << U"drawerZ=" << drawerZ;

	Print << U"TextID=" << TextID;
	
#endif
}

// マウスポインタのRay
Ray CameraTest::getMouseRay() const
{
	return camera.screenToRay(Cursor::PosF());
}

// ベクトルの外積（2D）
double CameraTest::cross(const Vec2& a, const Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

// 点Cが線分ABのどちら側にあるか
double CameraTest::direction(const Vec2& a, const Vec2& b, const Vec2& c)
{
	return cross({ b.x - a.x, b.y - a.y }, { c.x - a.x, c.y - a.y });
}

// 線分同士が交差しているかを判定
bool CameraTest::isIntersecting(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	double d1 = direction(a, b, c);
	double d2 = direction(a, b, d);
	double d3 = direction(c, d, a);
	double d4 = direction(c, d, b);

	return (d1 * d2 < 0) && (d3 * d4 < 0);
}

// 2Dの線分同士の交差判定
bool CameraTest::isCollision(const Vec2& a, const Vec2& b, double* collisionList)
{
	Vec2 A{ a.x, a.y }, B{ b.x, b.y };
	for (int i = 0; i < 1; i++)	// TODO
	{
		if (isIntersecting(A, B, Vec2{ collisionList[0], collisionList[1] }, Vec2{ collisionList[2], collisionList[1] })) { return true; }
		if (isIntersecting(A, B, Vec2{ collisionList[2], collisionList[1] }, Vec2{ collisionList[2], collisionList[3] })) { return true; }
		if (isIntersecting(A, B, Vec2{ collisionList[2], collisionList[3] }, Vec2{ collisionList[0], collisionList[3] })) { return true; }
		if (isIntersecting(A, B, Vec2{ collisionList[0], collisionList[3] }, Vec2{ collisionList[0], collisionList[1] })) { return true; }
	}
	return false;
}

// 3D空間上の線分と三角形が交差しているかどうかを判定する関数
bool CameraTest::IntersectSegmentTriangle(const Segment& segment, const Triangle_& tri, HitResult& outResult) {
	const double EPSILON = 1e-8f;

	Vec3_ dir = segment.end - segment.start;  // 線分の方向ベクトル
	Vec3_ v0 = tri[0];
	Vec3_ v1 = tri[1];
	Vec3_ v2 = tri[2];

	Vec3_ edge1 = v1 - v0;
	Vec3_ edge2 = v2 - v0;

	Vec3_ h = Vec3_::cross(dir, edge2);
	double a = Vec3_::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) {
		return false; // 平行
	}

	double f = 1.0f / a;
	Vec3_ s = segment.start - v0;
	double u = f * Vec3_::dot(s, h);

	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	Vec3_ q = Vec3_::cross(s, edge1);
	double v = f * Vec3_::dot(dir, q);

	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	double t = f * Vec3_::dot(edge2, q);

	// 線分の範囲内か？（0 <= t <= 1）
	if (t >= 0.0f && t <= 1.0f) {
		outResult.t = t;
		outResult.u = u;
		outResult.v = v;
		outResult.point = segment.start + dir * t;
		return true;
	}

	return false;
}

// 最近点を三角形上に求める（Möllerのアルゴリズム系）
Vec3_ CameraTest::ClosestPointOnTriangle(const Vec3_& p, const Vec3_& a, const Vec3_& b, const Vec3_& c)
{
	Vec3_ ab = b - a;
	Vec3_ ac = c - a;
	Vec3_ ap = p - a;

	double d1 = ab.dot(ap);
	double d2 = ac.dot(ap);
	if (d1 <= 0 && d2 <= 0) return a;

	Vec3_ bp = p - b;
	double d3 = ab.dot(bp);
	double d4 = ac.dot(bp);
	if (d3 >= 0 && d4 <= d3) return b;

	double vc = d1 * d4 - d3 * d2;
	if (vc <= 0 && d1 >= 0 && d3 <= 0) {
		double v = d1 / (d1 - d3);
		return a + ab * v;
	}

	Vec3_ cp = p - c;
	double d5 = ab.dot(cp);
	double d6 = ac.dot(cp);
	if (d6 >= 0 && d5 <= d6) return c;

	double vb = d5 * d2 - d1 * d6;
	if (vb <= 0 && d2 >= 0 && d6 <= 0) {
		double w = d2 / (d2 - d6);
		return a + ac * w;
	}

	double va = d3 * d6 - d5 * d4;
	if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0) {
		double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + (c - b) * w;
	}

	// 内部領域
	double denom = 1.0f / (va + vb + vc);
	double v = vb * denom;
	double w = vc * denom;
	return a + ab * v + ac * w;
}

// 球と三角形の交差判定
bool CameraTest::IsSphereIntersectingTriangle(const Vec3_& sphereCenter, double radius,
	const Vec3_& a, const Vec3_& b, const Vec3_& c)
{
	Vec3_ closest = ClosestPointOnTriangle(sphereCenter, a, b, c);
	Vec3_ diff = closest - sphereCenter;
	return diff.lengthSq() <= radius * radius;
}

void CameraTest::update()
{
	const double deltaTime = Scene::DeltaTime();

	// デバッグ表示
	debug();

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
		addSpeed = 20.0;
	}
	else if (KeyShift.pressed())
	{
		// シフトキーを押しているときの速度
		addSpeed = 5.0;
	}
	const double scaledSpeed = cameraSpeed * addSpeed * deltaTime;

	// マウスの座標を取得
	double diffMousePosX = 0.0f;
	double diffMousePosY = 0.0f;

	// インベントリを表示しているかどうか
	if (!bInventory)
	{
		// インベントリを表示していない（通常時）

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

	}
	else
	{
		// インベントリを表示している

		if (KeyUp.down())
		{
			itemIndex -= 4;

			if (itemIndex < 0)
			{
				itemIndex += 16;
				if (itemIndex == 15)
				{
					itemIndex = 11;
				}
			}
		}

		if (KeyDown.down())
		{
			itemIndex += 4;

			if (itemIndex >= 15)
			{
				itemIndex += 12;
				itemIndex %= 4;
			}
		}

		if (KeyLeft.down())
		{
			itemIndex -= 1;
			if (itemIndex < 0)
			{
				itemIndex += 15;
			}
		}

		if (KeyRight.down())
		{
			itemIndex += 1;
			if (itemIndex >= 15)
			{
				itemIndex -= 15;
			}
		}



	}

	// インベントリの表示・非表示
	if (KeyI.down())
	{
		// インベントリフラグの切り替え
		bInventory = bInventory ? false : true;

		// いったんカーソルを強制的に中央に戻す
		Cursor::SetPos(center.x, center.y);
	}



	const double to_s = (Math::Cos(phiController.getPhi()));
	const double to_c = (Math::Sin(phiController.getPhi()));

	s = Math::Lerp(s, to_s, smooth / focusSmooth); // 回転もスムーズに
	c = Math::Lerp(c, to_c, smooth / focusSmooth); // 回転もスムーズに



	// ゆっくり移動
	m_eyePosition = m_eyePosition.lerp(toCameraPos, smooth / focusSmooth);

	// コリジョンを無効にするエリア
	for (int i = 0; i < 4; i++)
	{
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

	last_eyePosition = toCameraPos;

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


	// 対象物の座標
	//Vec3 keyObjectPosition = Vec3{ keyX, keyY, keyZ };

	// 対象のオブジェクトが画面の中心にあるかどうかの判定
	//Vec3 worldScreenPoint = camera.worldToScreenPoint(keyObjectPostion);

	// 対象オブジェクトとの距離
	//double keyDistance = m_eyePosition.distanceFrom(keyObjectPostion);

	// 毎フレーム更新
//	focusWait -= deltaTime;



	// オブジェクトを取ることができるか
	bool bLockon = false;

	if (!bLockon)
	{
		auto [a, b] = breadController.update(Vec3{ breadX, breadY, breadZ }, camera, m_eyePosition, ray, MarkPosition);
		bBreadHave = a;
		if (b)
		{
			bLockon = b;
		}
	}
	
	if(!bLockon)
	{
		auto [a, b] = keyController.update(Vec3{ keyX, keyY, keyZ }, camera, m_eyePosition, ray, MarkPosition);
		bKeyHave = a;
		if (b)
		{
			bLockon = b;
		}
	}



//	if (
//		worldScreenPoint.x >= (1280 / 2 - 200)
//	 && worldScreenPoint.x <= (1280 / 2 + 200)
//	 && worldScreenPoint.y >= (720 / 2 - 200)
//	 && worldScreenPoint.y <= (720 / 2 + 200)
//	 && keyDistance < 3.5
//	)
//	{
//		// オブジェクトが画面の中心にある
//#ifdef _DEBUG
//		Print << U"オブジェクトが画面の中心にある";
//#endif
//		Print << U"左クリックで取る";
//		Print << U"エンターキーで取る";
//
//		isFocus = true;
//
//		if (controller.buttonA.pressed()
//			|| KeyEnter.pressed()
//		)
//		{
//			isKeyHave = true;
//			AudioAsset(U"GET").play();
//			AudioAsset(U"BGM").stop();
//		}
//	}

	// 止まっているBGMを再度鳴らす TODO 汎用的な仕組みではないので、修正する
	if (bKeyHave == true
	&& bClear == false
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
	}


	//Vec3 doorObjectPostion = Vec3{ doorX, doorY, doorZ };

	//worldScreenPoint = camera.worldToScreenPoint(doorObjectPostion);

	//double doorDistance = m_eyePosition.distanceFrom(doorObjectPostion);

	//Print << U"doorObjectPostion=" << doorObjectPostion;
	//Print << U"worldScreenPoint2=" << worldScreenPoint;
	//Print << U"doorDistance=" << doorDistance;

	// TODO 共通化する
	/* 扉のフォーカスはいったんクローズ
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
	*/

	//Print << U"isFocusSmooth" << isFocusSmooth;

    /* フォーカスのキャンセルをいったんクローズ
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
	*/

	/* いったんクローズ
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
	*/

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

	if (!bDebugFlashingLight)
	{
		// 点滅しない場合は常に点灯
		isGlowEffect = true;
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

	// スポットライト
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

		if (bDebugShader)
		{
			const ScopedCustomShader3D shader(vs3D, ps3D);

			// モデルを描画
			if (bDebugviewModel)
			{
				Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
				model.draw();
			}
		}
		else
		{
			// モデルを描画
			if (bDebugviewModel)
			{
				Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
				model.draw();
			}
		}

		// ドア
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(roomScale).translated(doorPos)
			};
			modelDoor.draw();
		}

		// パンの描画
		if (bBreadHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(1.0).translated(Vec3{breadX, breadY, breadZ})
			};

			modelBread.draw();
		}

		// 鍵の描画
		if (bKeyHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateZ(0_deg).scaled(0.01).translated(Vec3{keyX, keyY, keyZ})
			};
			modelKey.draw();
		}

		// 火かき棒の描画
		if (bPokerHave == false)
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{pokerX, pokerY, pokerZ})
			};

			modelPoker.draw();
		}

		// 引き出し（１段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY + 0.06 + 0.16*5, drawerZ})
			};

			modelDrawerNon.draw();
		}

		// 引き出し（２段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY + 0.06 + 0.16*4, drawerZ})
			};

			modelDrawerFlower.draw();
		}

		// 引き出し（３段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY + 0.06 + 0.16*3, drawerZ})
			};

			modelDrawerChain.draw();
		}

		// 引き出し（４段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY + 0.06 + 0.16*2, drawerZ})
			};

			modelDrawerFeather.draw();

		}

		// 引き出し（５段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY + 0.06+0.16, drawerZ})
			};

			modelDrawerSnake.draw();
		}

		// 引き出し（６段目）
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY+ 0.06, drawerZ})
			};

			modelDrawerEye.draw();
		}

		// 引き出し
		{
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(0.01).translated(Vec3{drawerX, drawerY, drawerZ})
			};

			modelShelf.draw();
		}

		// ビックリマーク
		if(bLockon)
		{
			MarkPosition.y += 0.2;
			Transformer3D t{
				Mat4x4::RotateY(0_deg).scaled(1).translated(MarkPosition)
			};

			modelExclamationMark.draw();
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
			for (int i = 0; i < 4; i++)
			{
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
		Sphere{ {0, lightY, lightZ}, lightSize }.draw(phong);
		
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

	// UI
	if (bInventory)
	{
		// インベントリ
		int itemX = center.x - 512 / 2;
		int itemY = center.y - 512 / 2;
		inventorySprite.draw(itemX, itemY);

		// アイテム
		for (int i = 0; i < 15; i++)	// TODO 数
		{
			int itemMiniX = center.x - 60 / 2 + (i % 4 * 80) - 224;
			int itemMiniY = center.y - 60 / 2 + (i / 4 * 80) - 110;

			// TODO アイテムIDで管理する
			breadSprite.draw(itemMiniX, itemMiniY);
		//	memoSprite.draw(itemMiniX, itemMiniY);
		//	keySprite.draw(itemMiniX, itemMiniY);

			// アイテムの選択
		//	itemMiniX = center.x - 60 / 2 + (i % 4 * 80) - 224;
		//	itemMiniY = center.y - 60 / 2 + (i / 4 * 80) - 110;

			Rect rect(itemMiniX, itemMiniY, 60, 60);

			if (rect.mouseOver())
			{
				// 枠線
				rect.drawFrame(
					2,		// 枠線の太さ
					Palette::Skyblue	// 空色で描画
				);
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

		// 現在選択中のアイテム
		{
			int itemBigX = center.x - 160 / 2 + 164;
			int itemBigY = center.y - 160 / 2 - 53;

			// TODO アイテムIDで管理する
			memoBigSprite.draw(itemBigX, itemBigY);
		}
		
	}

	std::vector<String> Text =
	{
		// シナリオ系
		U"腹が減った。まずは食事をしなければ…。",
		U"パンの中に手記が入っていた。",
		U"ベッド脇の壁を調べてみよう。",
		U"この鍵は錆びて使えないな…。",
		U"この針金で錆びた鍵の形を模すことはできるだろうか…。",
		U"扉をこの針金で開けてみよう。",
		U"暖炉がある。",
		U"水槽がある。",
		U"羊皮紙の汚れを落としたい。",
		U"この引き出しは何だろう。",

		// アイテム系
		U"黒ずんだ硬いパン。日にちの感覚も失いかけた者が、最後に噛みしめたのか。",
		U"長い間放置されていたのか、すっかり錆びついている。力を加えると折れてしまいそうだ。",
		U"ただの針金だが、適度に柔らかく曲げやすい。何かの形を真似ることができるかもしれない。",
		U"ひどく汚れていて、何かが書かれているようだが判別できない。洗えば読めるかもしれない。",
		U"文字がはっきりと見える。何かの順番を示しているようだ。",
		U"無地の羊皮紙。触るとわずかにざらつきがある。",
		U"表面に新たな文字が浮かび上がった。これは隠された何かのヒントだろうか？",
		U"鉄製の長い棒。炉の中をかき混ぜたり、熱いものを引き寄せたりするのに使えそうだ。",
	};

	Print << U"size=" << Text.size();

	if (0 <= TextID && TextID < Text.size())
	{
		// セリフ表示（仮）
		const Font& boldFont = FontAsset(U"Bold");
		boldFont(Text[TextID]).drawAt(
			28,
			{ center.x, 700 },
			ColorF{ 1, 1, 1, 1 }
		);
	}
}