# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"
# include "Ranking.hpp"
# include "CameraTest.hpp"
# include "Result.hpp"

void Main()
{
    // ウインドウとシーンを 1280x720 にリサイズする
    Window::Resize(1280, 720);
    
    
	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");
	FontAsset(U"TitleFont").setBufferThickness(4);

	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, Typeface::Bold);

	App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);
	manager.add<Ranking>(State::Ranking);
    manager.add<CameraTest>(State::CameraTest);
    manager.add<Result>(State::Result);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
