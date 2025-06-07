# pragma once
#include "Common.hpp"

class ObjectController
{
private:
	bool m_isHave;

public:
	ObjectController();

	std::tuple<bool, bool, int> update(
		const Vec3& objPos,
		const BasicCamera3D& camera,
		const Vec3& eyePos,
	//	Ray ray,
		Vec3& MarkPosition,
		int bgmNo,
		bool bHave
	);

};
