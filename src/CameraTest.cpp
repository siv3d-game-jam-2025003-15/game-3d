# include "CameraTest.hpp"

CameraTest::CameraTest(const InitData& init)
	: IScene{ init }
{
}

void CameraTest::update()
{
	if (MouseL.down())
	{

	}
}

void CameraTest::draw() const
{
	Scene::SetBackground(ColorF{ 0.4, 0.6, 0.9 });
	const Font& boldFont = FontAsset(U"Bold");

	boldFont(U"Camera Test").drawAt(400, 60);


}