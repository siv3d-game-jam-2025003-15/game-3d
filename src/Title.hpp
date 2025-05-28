# pragma once
# include "Common.hpp"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:

	//RoundRect m_startButton{ Arg::center(400, 300), 300, 60, 8 };
	RoundRect m_cameraTestButton{ Arg::center(200, 800), 300, 60, 8 };
	//RoundRect m_rankingButton{ Arg::center(400, 400), 300, 60, 8 };
	RoundRect m_exitButton{ Arg::center(200, 900), 300, 60, 8 };
    //RoundRect m_modelViewButton{ Arg::center(400, 400), 300, 60, 8 };
    //RoundRect m_stageButton{ Arg::center(800, 300), 300, 60, 8 };

	Transition m_startTransition{ 0.4s, 0.2s };
	Transition m_rankingTransition{ 0.4s, 0.2s };
	Transition m_exitTransition{ 0.4s, 0.2s };
	Transition m_cameraTestTransition{ 0.4s, 0.2s };
    Transition m_modelViewTransition{ 0.4s, 0.2s };
    Transition m_stageTransition{ 0.4s, 0.2s };

	// 画像ファイルからテクスチャを作成する
//	const Texture BG{ U"assets/models/Room/T_EV_Wall01_D.png" };
	
// タイトル画面
	const Texture BG{ U"assets/sprites/BG_Title01.png" };

};
