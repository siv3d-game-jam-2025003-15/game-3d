# include "CameraTest.hpp"

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
	Model::RegisterDiffuseTextures(modelKey, TextureDesc::MippedSRGB);

	// BGMの再生
	AudioAsset(U"BGM").play();

	// マウスの初期化
	mousePosX = Cursor::PosF().x;
	mousePosY = Cursor::PosF().y;
	toMousePosX = mousePosX;
	toMousePosY = mousePosY;

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
	if (Key5.down())
	{
		// コリジョンワイヤーフレーム
		bDebugViewFrame = true;
	}
	if (Key6.down())
	{
		// コリジョン塗りつぶし
		bDebugViewFrame = false;
	}
	if (Key7.down())
	{
		// コリジョンを表示
		bDebugViewCollision = true;
	}
	if (Key8.down())
	{
		// コリジョンを非表示
		bDebugViewCollision = false;
	}
	if (Key9.down())
	{
		// コリジョンを有効
		bCollision = true;
	}
	if (Key0.down())
	{
		// コリジョンを無効
		bCollision = false;
	}
	if (KeyZ.down())
	{
		// モデルを表示
		bDebugviewModel = true;
	}
	if (KeyX.down())
	{
		// モデルを非表示
		bDebugviewModel = false;
	}
	if (KeyC.down())
	{
		// ライトの点滅あり
		bDebugFlashingLight = true;
	}
	if (KeyV.down())
	{
		// ライトの点滅なし
		bDebugFlashingLight = false;
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
	
	if (bDebugViewFrame)
	{
		Print << U"[5][6]コリジョン：ワイヤーフレーム";
	}
	else
	{
		Print << U"[5][6]コリジョン：塗りつぶし";
	}

	if (bDebugViewCollision)
	{
		Print << U"[7][8]コリジョン表示：表示";
	}
	else
	{
		Print << U"[7][8]コリジョン表示：非表示";
	}

	if (bCollision)
	{
		Print << U"[9][0]コリジョン：有効";
	}
	else
	{
		Print << U"[9][0]コリジョン：無効";
	}

	if (bDebugviewModel)
	{
		Print << U"[Z][X]モデル：表示";
	}
	else
	{
		Print << U"[Z][X]モデル：非表示";
	}

	if (bDebugFlashingLight)
	{
		Print << U"[C][V]ライトの点滅：あり";
	}
	else
	{
		Print << U"[C][V]ライトの点滅：なし";
	}

	Print << U"[R][F]上下移動";
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

bool CameraTest::isCollision(const Vec2& a, const Vec2& b, double* collisionList)
{
	// 2Dの線分同士の交差判定
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

struct Vec3_ {
	double x, y, z;

	Vec3_ operator-(const Vec3_& v) const { return { x - v.x, y - v.y, z - v.z }; }
	Vec3_ operator+(const Vec3_& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vec3_ operator*(double s) const { return { x * s, y * s, z * s }; }

	static Vec3_ cross(const Vec3_& a, const Vec3_& b) {
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	static double dot(const Vec3_& a, const Vec3_& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	double dot(const Vec3_& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

	double lengthSq() const { return x * x + y * y + z * z; }
};

// レイ（三角形との交差を調べる）
struct Ray_ {
	Vec3_ origin;
	Vec3_ direction;
};

// 交差点の情報
struct HitResult {
	double t;       // レイのパラメータ（距離）
	double u, v;    // バリセントリック座標
	Vec3_ point;    // 交差点
};

// 三角形（頂点3つ）
using Triangle_ = std::array<Vec3_, 3>;

struct Segment {
	Vec3_ start;
	Vec3_ end;
};

bool IntersectSegmentTriangle(const Segment& segment, const Triangle_& tri, HitResult& outResult) {
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
Vec3_ ClosestPointOnTriangle(const Vec3_& p, const Vec3_& a, const Vec3_& b, const Vec3_& c)
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
bool IsSphereIntersectingTriangle(const Vec3_& sphereCenter, double radius,
	const Vec3_& a, const Vec3_& b, const Vec3_& c)
{
	Vec3_ closest = ClosestPointOnTriangle(sphereCenter, a, b, c);
	Vec3_ diff = closest - sphereCenter;
	return diff.lengthSq() <= radius * radius;
}

void CameraTest::update()
{
	debug();

	// 指定したプレイヤーインデックスの XInput コントローラを取得
	auto controller = XInput(playerIndex);

	// それぞれデフォルト値を設定
	controller.setLeftTriggerDeadZone();
	controller.setRightTriggerDeadZone();
	controller.setLeftThumbDeadZone();
	controller.setRightThumbDeadZone();

	Ray ray = getMouseRay();

	double speed = 2.0f;

	const double deltaTime = Scene::DeltaTime();
	const double scaledSpeed =
		speed * ((KeyControl | KeyCommand).pressed() ? 20.0 : KeyShift.pressed() ? 5.0 : 1.0)
		* deltaTime;

	toMousePosX = Cursor::PosF().x;
	toMousePosY = Cursor::PosF().y;

	double diffMousePosX = 0.0f;
	double diffMousePosY = 0.0f;

	if (MouseL.pressed())
	{
		diffMousePosX =  (toMousePosX - mousePosX) / 10 * mouseDirectionX;
		diffMousePosY = -(toMousePosY - mousePosY) / 10 * mouseDirectionY;
	}

	mousePosX = toMousePosX;
	mousePosY = toMousePosY;

	if (KeyLeft.pressed()
//	&& isFocusSmooth == false
	)
	{
		m_phi += (60_deg * deltaTime);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (controller.rightThumbX < -0.1 
//	&& isFocusSmooth == false
	)
	{
		m_phi += (60_deg * deltaTime * -controller.rightThumbX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (diffMousePosX < -0.1 
//	&& isFocusSmooth == false
	)
	{
		m_phi += (60_deg * deltaTime * -diffMousePosX);

		// TODO 共通化したい
		if (m_phi < 0_deg)
		{
			m_phi += 360_deg;
		}
	}

	if (KeyRight.pressed()
//	&& isFocusSmooth == false
	)
	{
		m_phi -= (60_deg * deltaTime);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	if (controller.rightThumbX > 0.1
//	&& isFocusSmooth == false
	)
	{
		m_phi -= (60_deg * deltaTime * controller.rightThumbX);

		// TODO 共通化したい
		if (360_deg < m_phi)
		{
			m_phi -= 360_deg;
		}
	}

	if (diffMousePosX > 0.1 
//	&& isFocusSmooth == false
	)
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
		if (KeyW.pressed()
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x += xr;
			toCameraPos.z += zr;
			isWalk = true;
		}
		if (controller.leftThumbY > 0.1
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x += (xr * controller.leftThumbY);
			toCameraPos.z += (zr * controller.leftThumbY);
			isWalk = true;
		}

		if (KeyS.pressed()
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x -= xr;
			toCameraPos.z -= zr;
			isWalk = true;
		}
		if (controller.leftThumbY < -0.1
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x -= (xr * -controller.leftThumbY);
			toCameraPos.z -= (zr * -controller.leftThumbY);
			isWalk = true;
		}

		if (KeyA.pressed() 
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x -= zr;
			toCameraPos.z += xr;
			isWalk = true;
		}
		if (controller.leftThumbX < -0.1
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x -= (zr * -controller.leftThumbX);
			toCameraPos.z += (xr * -controller.leftThumbX);
			isWalk = true;
		}

		if (KeyD.pressed() 
	//	&& isFocusSmooth == false
		)
		{
			toCameraPos.x += zr;
			toCameraPos.z -= xr;
			isWalk = true;
		}
		if (controller.leftThumbX > 0.1
	//	&& isFocusSmooth == false
		)
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

		if (KeyUp.pressed()
	//	&& isFocusSmooth == false
		)
		{
			to_m_focusY += yDelta;
		}

		if (controller.rightThumbY > 0.1 
	//	&& isFocusSmooth == false
		)
		{
			to_m_focusY += (yDelta * controller.rightThumbY);
		}

		if (diffMousePosY > 0.1 
	//	&& isFocusSmooth == false
		)
		{
			to_m_focusY += (yDelta * diffMousePosY);
		}


		if (KeyDown.pressed() 
	//	&& isFocusSmooth == false
		)
		{
			to_m_focusY -= yDelta;
		}

		if (controller.rightThumbY < -0.1
	//	&& isFocusSmooth == false
		)
		{
			to_m_focusY -= (yDelta * -controller.rightThumbY);
		}

		if (diffMousePosY < -0.1
	//	&& isFocusSmooth == false
		)
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


	// コリジョンを無効にするエリア
	Print << U"x=" << toCameraPos.x;
	Print << U"z=" << toCameraPos.z;

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
		/*
		Vec2 A{ 
			last_eyePosition.x,
			last_eyePosition.z
		};

		// 向かっている位置より少し前にコリジョンを持たせる TODO これじゃなくて、距離にしたい
		Vec2 cameraNormal = Vec2(
			toCameraPos.x - last_eyePosition.x,
			toCameraPos.z - last_eyePosition.z
		).normalized();

		Vec2 B{
			toCameraPos.x + cameraNormal.x / 2,
			toCameraPos.z + cameraNormal.y / 2,
		};
		*/

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
		// TODO これじゃなくて、距離にしたい
		Vec3 B{
			toCameraPos.x + cameraNormal.x / 2,
			toCameraPos.y + cameraNormal.y / 2,
			toCameraPos.z + cameraNormal.z / 2,
		};

		Segment segment = {
			{
				last_eyePosition.x,
				//last_eyePosition.y,
				0.1,
				last_eyePosition.z
			}
			, 
			{
				toCameraPos.x + cameraNormal.x / 2,
				//toCameraPos.y,
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
				/*
				if (
					isIntersecting (
						A,
						B,
						Vec2{ cube[collisionList[i][0]].x / 100, cube[collisionList[i][0]].z / 100 },
						Vec2{ cube[collisionList[i][1]].x / 100, cube[collisionList[i][1]].z / 100 }
					)
				)
				*/


				Triangle_ tri = {
					Vec3_{cube[collisionTriangle[i][0]].x / 100, cube[collisionTriangle[i][0]].y / 100, cube[collisionTriangle[i][0]].z / 100},
					Vec3_{cube[collisionTriangle[i][1]].x / 100, cube[collisionTriangle[i][1]].y / 100, cube[collisionTriangle[i][1]].z / 100},
					Vec3_{cube[collisionTriangle[i][2]].x / 100, cube[collisionTriangle[i][2]].y / 100, cube[collisionTriangle[i][2]].z / 100}
				};

				//Print << U"camera_x=" << toCameraPos.x << U" camera_y=" << toCameraPos.y << U" camera_z=" << toCameraPos.z;
				//Print
				//	<< U" ray_x=" << toCameraPos.x - last_eyePosition.x
				//	<< U" ray_y=" << toCameraPos.y - last_eyePosition.y 
				//	<< U" ray_z=" << toCameraPos.z - last_eyePosition.z;
				//Print << U"cube_x=" << cube[collisionTriangle[i][0]].x / 100 << U" cube_y=" << cube[collisionTriangle[i][0]].y / 100 << U" cube_z=" << cube[collisionTriangle[i][0]].z / 100;
				//Print << U"cube_x=" << cube[collisionTriangle[i][1]].x / 100 << U" cube_y=" << cube[collisionTriangle[i][1]].y / 100 << U" cube_z=" << cube[collisionTriangle[i][1]].z / 100;
				//Print << U"cube_x=" << cube[collisionTriangle[i][2]].x / 100 << U" cube_y=" << cube[collisionTriangle[i][2]].y / 100 << U" cube_z=" << cube[collisionTriangle[i][2]].z / 100;

				// デバッグ
				//checkCollision = true;

				//Segment segment = { {0, 0, 0}, {0, 0, 5} };

				HitResult hit;
				//if (IntersectSegmentTriangle(segment, tri, hit))

				Vec3_ triA = { cube[collisionTriangle[i][0]].x / 100, cube[collisionTriangle[i][0]].y / 100, cube[collisionTriangle[i][0]].z / 100 };
				Vec3_ triB = { cube[collisionTriangle[i][1]].x / 100, cube[collisionTriangle[i][1]].y / 100, cube[collisionTriangle[i][1]].z / 100 };
				Vec3_ triC = { cube[collisionTriangle[i][2]].x / 100, cube[collisionTriangle[i][2]].y / 100, cube[collisionTriangle[i][2]].z / 100 };

				if (IsSphereIntersectingTriangle(myPosition, myRadius, triA, triB, triC))
				{
					//Print << U"交差しました！ t=" << hit.t << U" x=" << hit.point.x << U" y=" << hit.point.y << U" z=" << hit.point.z;
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

    /* ズームはいったんクローズ
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
	else
	{
		to_zoom = 0;
	}
	zoom = Math::Lerp(zoom, to_zoom, smooth / zoom_smooth / focusSmooth * deltaTime * 60); // ズームをスムーズに
	*/


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
//	 && isFocusSmooth == false
//	 && focusWait < 0.0f
	)
	{
		// オブジェクトが画面の中心にある
#ifdef _DEBUG
		Print << U"オブジェクトが画面の中心にある";
#endif
		Print << U"左クリックで鍵を取る";
		Print << U"エンターキーで鍵を取る";
		//	Print << U"右クリックでズーム解除";


		// ズームしていたらフォーカスする
		//if (controller.rightTrigger > 0.5
		// || KeyZ.pressed()
		// || isFocus	// フォーカス中はカメラをフォーカスしたままにしたいため
		//)
		//{
		
        /* ズームの処理はバグっているので、いったんコメントアウト
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
		*/
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

	// 止まっているBGMを再度鳴らす TODO 汎用的な仕組みではないので、修正する
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
	}


	Vec3 doorObjectPostion = Vec3{ doorX, doorY, doorZ };

	worldScreenPoint = camera.worldToScreenPoint(doorObjectPostion);

	double doorDistance = m_eyePosition.distanceFrom(doorObjectPostion);

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
		if (bDebugviewModel)
		{
			Transformer3D t{ Mat4x4::RotateY(0_deg).scaled(roomScale).translated(roomPos) };
			model.draw();
		}

		/*
		Mat4x4 mat = Mat4x4::Translate(1, 0, 1);
		for (const auto& object : model.objects())
		{
			Mat4x4 m = Mat4x4::Identity();

			Print << U"オブジェクト：" << object.name;

			if (object.name == U"FixRoom EV_Partition01")
			{
				m *= Mat4x4::Rotate(
					Vec3{ 0,0,-1 },
					(Scene::Time() * -120_deg),
					Vec3{ 0, 0, 0 }
				);
			}

			const Transformer3D t{ (m * mat) };

			//Model::Draw(object, materials);
			model.draw();
		}
		*/
		


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
		/* 扉の処理はいったんクローズ
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
		*/


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

						//break;

						/*
						//
						Line3D{ cube[0], cube[1] }.draw(color);
						Line3D{ cube[1], cube[2] }.draw(color);
						Line3D{ cube[2], cube[0] }.draw(color);

						Line3D{ cube[1], cube[2] }.draw(color);
						Line3D{ cube[2], cube[3] }.draw(color);
						Line3D{ cube[3], cube[1] }.draw(color);

						//
						Line3D{ cube[0], cube[2] }.draw(color);
						Line3D{ cube[2], cube[4] }.draw(color);
						Line3D{ cube[4], cube[0] }.draw(color);

						Line3D{ cube[2], cube[4] }.draw(color);
						Line3D{ cube[4], cube[6] }.draw(color);
						Line3D{ cube[6], cube[2] }.draw(color);

						//
						Line3D{ cube[2], cube[3] }.draw(color);
						Line3D{ cube[3], cube[6] }.draw(color);
						Line3D{ cube[6], cube[2] }.draw(color);

						Line3D{ cube[3], cube[6] }.draw(color);
						Line3D{ cube[6], cube[7] }.draw(color);
						Line3D{ cube[7], cube[3] }.draw(color);

						//
						Line3D{ cube[1], cube[3] }.draw(color);
						Line3D{ cube[3], cube[5] }.draw(color);
						Line3D{ cube[5], cube[1] }.draw(color);

						Line3D{ cube[3], cube[5] }.draw(color);
						Line3D{ cube[5], cube[7] }.draw(color);
						Line3D{ cube[7], cube[3] }.draw(color);

						//
						Line3D{ cube[1], cube[3] }.draw(color);
						Line3D{ cube[3], cube[5] }.draw(color);
						Line3D{ cube[5], cube[1] }.draw(color);

						Line3D{ cube[3], cube[5] }.draw(color);
						Line3D{ cube[5], cube[7] }.draw(color);
						Line3D{ cube[7], cube[3] }.draw(color);

						//
						Line3D{ cube[4], cube[5] }.draw(color);
						Line3D{ cube[5], cube[6] }.draw(color);
						Line3D{ cube[6], cube[4] }.draw(color);

						Line3D{ cube[5], cube[6] }.draw(color);
						Line3D{ cube[6], cube[7] }.draw(color);
						Line3D{ cube[7], cube[5] }.draw(color);
						*/
					}
					else
					{
						// 塗りつぶし
						object.boundingBox.draw(ColorF{ 1, 1, 1, 1 });
					}
				}
			}

			// 自分のコリジョン（たぶん見えない）
			/*
			Sphere{
				myPosition.x,
				myRadius+0.1,
				myPosition.z,
				myRadius
			}.draw(ColorF{ 0.4, 0.8, 0.6 }.removeSRGBCurve());
			*/

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
	Scene::SetBackground(ColorF{ 0, 0, 0 });
}