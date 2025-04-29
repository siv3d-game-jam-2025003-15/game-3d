#include "ObjectController.hpp"

ObjectController::ObjectController()
{
	m_isHave = false;
}

bool ObjectController::update(
	const Vec3& objPos,
	const BasicCamera3D& camera,
	const Vec3& eyePos,
	Ray ray
)
{
	if (m_isHave)
	{
		return m_isHave;
	}

	Vec3 screenPos = camera.worldToScreenPoint(objPos);
	double distance = eyePos.distanceFrom(objPos);

	if (screenPos.x >= (WINDOW_WIDTH / 2 - 200)
		&& screenPos.x <= (WINDOW_WIDTH / 2 + 200)
		&& screenPos.y >= (WINDOW_HEIGHT / 2 - 200)
		&& screenPos.y <= (WINDOW_HEIGHT / 2 + 200)
		&& distance < 3.5)
	{
#ifdef _DEBUG
		Print << U"オブジェクトが画面の中心にある";
#endif
		Print << U"左クリックで取る";
		Print << U"エンターキーで取る";

		// マウスの当たり判定の描画
		Box box = Box{ objPos, 0.3 }.drawFrame(ColorF{ 1, 1, 1, 1 });

		if (KeyEnter.pressed()
		|| (box.intersects(ray) && MouseL.down())
		)
		{
			m_isHave = true;
			AudioAsset(U"GET").play();
			AudioAsset(U"BGM").stop();
		}
	}

	return m_isHave;
}
