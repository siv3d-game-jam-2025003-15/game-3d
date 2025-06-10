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

	size_t playerIndex = 0;

	auto controller = XInput(playerIndex);
	controller.setLeftTriggerDeadZone();
	controller.setRightTriggerDeadZone();
	controller.setLeftThumbDeadZone();
	controller.setRightThumbDeadZone();

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
			|| controller.buttonA.down()
			)
		)
		{
			// 取れるオブジェクトかどうか
			if (bHave)
			{
				// 取る処理
				m_isHave = true;

				// BGMの停止
				if (AudioAsset(U"BGM").isPlaying())
				{
					AudioAsset(U"BGM").stop();
				}

				// SEの再生
				switch (bgmNo)
				{
				case 0:
					AudioAsset(U"Item").setVolume(1.0);
					AudioAsset(U"Item").play();
					bgmStopCount = 4.00;
					break;
				case 1:
					AudioAsset(U"Cancellation").setVolume(1.0);
					AudioAsset(U"Cancellation").play();
					bgmStopCount = 4.00;
					break;
				case 2:
					AudioAsset(U"Paper").setVolume(1.0);
					AudioAsset(U"Paper").play();
					bgmStopCount = 4.00;
					break;
				case 3:
					AudioAsset(U"Water").setVolume(1.0);
					AudioAsset(U"Water").play();
					bgmStopCount = 4.00;
					break;
				}

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
