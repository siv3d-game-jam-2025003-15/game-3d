# pragma once
#include "Common.hpp"

class ObjectController
{
private:
	bool m_isHave;

public:
	ObjectController();

	bool update(
		const Vec3& objPos,
		const BasicCamera3D& camera,
		const Vec3& eyePos,
		Ray ray
	);

};
