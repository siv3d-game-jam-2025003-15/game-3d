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
	Print << ray.direction;	// 角度
	Print << ray.direction.getX();	// 角度
	Print << ray.direction.getY();	// 角度
	Print << ray.direction.getZ();	// 角度
	Print << ray.origin;	// 座標
	Print << ray.origin.getX();	// 座標
	Print << ray.origin.getY();	// 座標
	Print << ray.origin.getZ();	// 座標


	camera.update(2.0);
	Graphics3D::SetCameraTransform(camera);

	// 床を描画
	Plane{ 16 }.draw(ColorF{ 0.3 });

	const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

	blacksmithModel.draw(Vec3{ 8, 0, 4 });

	// [RenderTexture を 2D シーンに描画]
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
}