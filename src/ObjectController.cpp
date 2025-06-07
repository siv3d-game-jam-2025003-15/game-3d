#include "ObjectController.hpp"

ObjectController::ObjectController()
{
	m_isHave = false;
}

std::tuple<bool, bool, int, bool> ObjectController::update(
	const Vec3& objPos,
	const BasicCamera3D& camera,
	const Vec3& eyePos,
//	Ray ray,
	Vec3& MarkPosition,
	int bgmNo,
	bool bHave	// 取ることができるかどうか
)
{
	bool isLockon = false;
	bool isClick = false;
	int bgmStopCount = 4.0;

	if (m_isHave)
	{
		return { m_isHave,isLockon, bgmStopCount, isClick };
	}

	Vec3 screenPos = camera.worldToScreenPoint(objPos);
	//double distance = eyePos.distanceFrom(objPos);

	Vec3 focus = camera.getFocusPosition();
	double focusDistance = focus.distanceFrom(objPos);

	//Print << U"focusDistance=" << focusDistance;

	if (
		screenPos.x >= (WINDOW_WIDTH / 2 - 200)
		&& screenPos.x <= (WINDOW_WIDTH / 2 + 200)
		&& screenPos.y >= (WINDOW_HEIGHT / 2 - 200)
		&& screenPos.y <= (WINDOW_HEIGHT / 2 + 200)
		//&& distance < 3.5
		&& focusDistance < 1.5
	)
	{
#ifdef _DEBUG
		Print << U"オブジェクトが画面の中心にある";
		Print << U"左クリックで取る";
		Print << U"エンターキーで取る";
#endif
		isLockon = true;

		MarkPosition = objPos;

		// マウスの当たり判定の描画
		Box box = Box{ objPos, 0.3 }.drawFrame(ColorF{ 1, 1, 1, 1 });

		if (KeyEnter.pressed() && bHave
		|| (
		//	box.intersects(ray) &&	// Rayで取れるようにするのをやめてみる
			MouseL.down()
			)
		)
		{
			// 取れるオブジェクトかどうか
			if (bHave)
			{
				// 取る処理
				m_isHave = true;
				switch (bgmNo)
				{
				case 0:
					AudioAsset(U"GET").play();
					bgmStopCount = 4.00;
					break;
				case 1:
					AudioAsset(U"牢屋の扉を開ける").play();
					//	bgmStopCount = 999999.0;
					bgmStopCount = 4.00;
					break;
				}
				AudioAsset(U"BGM").stop();
			}
			else 
			{
				// クリックしたかどうか
				isClick = true;
			}
		}
	}

	return { m_isHave, isLockon, bgmStopCount, isClick };
}
