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
	// マウスでサイズ変更可能にする
	Window::SetStyle(WindowStyle::Sizable);

	// シーンサイズ：ゲーム中はキープしておく（変更するとズレてしまうため）
	Scene::SetResizeMode(ResizeMode::Keep);

    // ウインドウとシーンをリサイズする
    Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Scene::Resize(SCENE_WIDTH, SCENE_HEIGHT);

	// フルスクリーンモード
//	Window::SetFullscreen(true);

//	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");
	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"assets/font/SoukouMincho-Font/SoukouMincho.ttf");
	FontAsset(U"TitleFont").setBufferThickness(4);

//	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, Typeface::Bold);
	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, U"assets/font/SoukouMincho-Font/SoukouMincho.ttf");
	AudioAsset::Register(U"足音45秒のループ", U"assets/sound/se/足音45秒のループ.wav", true);
    AudioAsset::Register(U"牢屋の扉を閉める", U"assets/sound/se/牢屋の扉を閉める.wav");
    AudioAsset::Register(U"牢屋の扉を開ける", U"assets/sound/se/牢屋の扉を開ける.wav");
	AudioAsset::Register(U"GET", U"assets/sound/se/GET.wav");
	AudioAsset::Register(U"Cancellation", U"assets/sound/se/SE_Cancellation01.mp3");
	AudioAsset::Register(U"Item", U"assets/sound/se/SE_Item01.mp3");
	AudioAsset::Register(U"Paper", U"assets/sound/se/SE_Paper01.mp3");
	AudioAsset::Register(U"Title", U"assets/sound/se/SE_Title01.mp3");
	AudioAsset::Register(U"Water", U"assets/sound/se/SE_Water01.mp3");
//	AudioAsset::Register(U"BGM", U"assets/sound/bgm/BGM.mp3", Loop::Yes);
	AudioAsset::Register(U"BGM", U"assets/sound/bgm/BGM_Map01.mp3", Loop::Yes);

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
