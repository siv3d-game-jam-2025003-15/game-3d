# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{

}

void Title::update()
{
	// ボタンの更新
	{
		//m_startTransition.update(m_startButton.mouseOver());
		//m_rankingTransition.update(m_rankingButton.mouseOver());
		m_exitTransition.update(m_exitButton.mouseOver());
		m_cameraTestTransition.update(m_cameraTestButton.mouseOver());
        //m_modelViewTransition.update(m_modelViewButton.mouseOver());
        //m_stageTransition.update(m_stageButton.mouseOver());

		if (//m_startButton.mouseOver() 
			m_cameraTestButton.mouseOver()
		//|| m_rankingButton.mouseOver()
		|| m_exitButton.mouseOver()
        //|| m_modelViewButton.mouseOver()
        //|| m_stageButton.mouseOver()
		)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	// ボタンのクリック処理
//	if (m_startButton.leftClicked()) // ゲームへ
//	{
//		changeScene(State::Game);
//	}
//	else if (m_rankingButton.leftClicked()) // ランキングへ
//	{
//		changeScene(State::Ranking);
//	}
	if (m_exitButton.leftClicked()) // 終了
	{
		System::Exit();
	}
	else if (m_cameraTestButton.leftClicked()) // カメラへ
	{
		AudioAsset(U"牢屋の扉を閉める").play();
		changeScene(State::CameraTest);
	}
    //else if (m_modelViewButton.leftClicked()) // modelviewへ
    //{
    //    changeScene(State::ModelView);
    //}
    //else if (m_stageButton.leftClicked())
    //{
    //    changeScene(State::Stage);
    //}
}

void Title::draw() const
{
	Scene::SetBackground(ColorF{ 0.2, 0.8, 0.4 });

	// タイトル描画
	FontAsset(U"TitleFont")(U"Siv3D脱出ゲーム")
		.drawAt(TextStyle::OutlineShadow(0.2, ColorF{ 0.2, 0.6, 0.2 }, Vec2{ 3, 3 }, ColorF{ 0.0, 0.5 }), 100, Vec2{ 640, 100 });

	// ボタン描画
	{
		//m_startButton.draw(ColorF{ 1.0, m_startTransition.value() }).drawFrame(2);
		m_cameraTestButton.draw(ColorF{ 1.0, m_cameraTestTransition.value() }).drawFrame(2);
		//m_rankingButton.draw(ColorF{ 1.0, m_rankingTransition.value() }).drawFrame(2);
		m_exitButton.draw(ColorF{ 1.0, m_exitTransition.value() }).drawFrame(2);
		//m_modelViewButton.draw(ColorF{ 1.0, m_cameraTestTransition.value() }).drawFrame(2);
        //m_stageButton.draw(ColorF{ 1.0, m_cameraTestTransition.value() }).drawFrame(2);
        
		const Font& boldFont = FontAsset(U"Bold");
		//boldFont(U"PLAY").drawAt(36, m_startButton.center(), ColorF{ 0.1 });
		boldFont(U"PLAY").drawAt(36, m_cameraTestButton.center(), ColorF{ 0.1 });
		//boldFont(U"RANKING").drawAt(36, m_rankingButton.center(), ColorF{ 0.1 });
		boldFont(U"EXIT").drawAt(36, m_exitButton.center(), ColorF{ 0.1 });
        //boldFont(U"MODEL VIEW").drawAt(36, m_modelViewButton.center(), ColorF{ 0.1 });
        //boldFont(U"STAGE").drawAt(36, m_stageButton.center(), ColorF{ 0.1 });
	}
}
