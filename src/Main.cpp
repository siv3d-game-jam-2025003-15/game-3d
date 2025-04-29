# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"
# include "Ranking.hpp"
# include "CameraTest.hpp"
# include "ModelView.hpp"
# include "Result.hpp"
# include "Stage.hpp"

void Main()
{
    // ウインドウとシーンを 1280x720 にリサイズする
    Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    
	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");
	FontAsset(U"TitleFont").setBufferThickness(4);

	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, Typeface::Bold);
    AudioAsset::Register(U"足音45秒のループ", U"assets/sound/se/足音45秒のループ.wav", true);
    AudioAsset::Register(U"牢屋の扉を閉める", U"assets/sound/se/牢屋の扉を閉める.wav");
    AudioAsset::Register(U"牢屋の扉を開ける", U"assets/sound/se/牢屋の扉を開ける.wav");
    AudioAsset::Register(U"GET", U"assets/sound/bgm/GET.wav");
    AudioAsset::Register(U"BGM", U"assets/sound/bgm/BGM.mp3", Loop::Yes);

    App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);
	manager.add<Ranking>(State::Ranking);
	manager.add<CameraTest>(State::CameraTest);
    manager.add<ModelView>(State::ModelView);
    manager.add<Result>(State::Result);
    manager.add<Stage>(State::Stage);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
