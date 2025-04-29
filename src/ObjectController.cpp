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
		Print << U"�I�u�W�F�N�g����ʂ̒��S�ɂ���";
#endif
		Print << U"���N���b�N�Ŏ��";
		Print << U"�G���^�[�L�[�Ŏ��";

		// �}�E�X�̓����蔻��̕`��
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
