#include "ObjectController.hpp"

ObjectController::ObjectController()
{
	m_isHave = false;
}

std::tuple<bool, bool, int> ObjectController::update(
	const Vec3& objPos,
	const BasicCamera3D& camera,
	const Vec3& eyePos,
	Ray ray,
	Vec3& MarkPosition,
	int bgmNo,
	bool bHave	// 取ることができるかどうか
)
{
	bool isLockon = false;
	int bgmStopCount = 4.0;

	if (m_isHave)
	{
		return { m_isHave,isLockon, bgmStopCount };
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

		isLockon = true;

		MarkPosition = objPos;

		// マウスの当たり判定の描画
		Box box = Box{ objPos, 0.3 }.drawFrame(ColorF{ 1, 1, 1, 1 });

		if (KeyEnter.pressed() && bHave
		|| (
		//	box.intersects(ray) &&	// Rayで取れるようにするのをやめてみる
			MouseL.down()
			&& bHave
			)
		)
		{
			m_isHave = true;
			switch (bgmNo)
			{
			case 0:
				AudioAsset(U"GET").play();
				bgmStopCount = 4.00;
				break;
			case 1:
				AudioAsset(U"牢屋の扉を開ける").play();
				bgmStopCount = 3.0;
				break;
			}
			AudioAsset(U"BGM").stop();
		}
	}

	return { m_isHave, isLockon, bgmStopCount };
}
