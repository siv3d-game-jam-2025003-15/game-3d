﻿# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{

}

void Title::update()
{
	// デバッグ
//	changeScene(State::ToBeContinued);

	// ボタンの更新
	{
		// ゲーム終了ボタン
		m_exitTransition.update(m_exitButton.mouseOver());

		// ゲーム開始ボタン
		m_cameraTestTransition.update(m_cameraTestButton.mouseOver());

		if (m_cameraTestButton.mouseOver()
		 || m_exitButton.mouseOver()
		)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	// ボタンのクリック処理
	if (m_exitButton.leftClicked()) // 終了
	{
		System::Exit();
	}
	else if (m_cameraTestButton.leftClicked()) // ゲーム開始
	{
	//	AudioAsset(U"牢屋の扉を閉める").play();
		AudioAsset(U"Title").play();
		changeScene(State::CameraTest);
	}

	// マウスオーバー
	gamestartColor = mouseOver(gamestartColor, U"GAME START", m_cameraTestButton.center());
	exitColor      = mouseOver(exitColor     , U"EXIT"      , m_exitButton.center());
}

void Title::draw() const
{
	Scene::SetBackground(ColorF{ 0.2, 0.8, 0.4 });

	// 背景
	BG.draw(0, 0);

	// タイトル描画
	//FontAsset(U"TitleFont")(U"Siv3D脱出ゲーム")
	//	.drawAt(TextStyle::OutlineShadow(0.2, ColorF{ 0.2, 0.6, 0.2 }, Vec2{ 3, 3 }, ColorF{ 0.0, 0.5 }), 100, Vec2{ 640, 100 });

	// ボタン描画
	{
		// フォント
		const Font& boldFont = FontAsset(U"Bold");

		// ゲーム開始ボタン（マウスオーバーの領域を描画）
	//	m_cameraTestButton.draw(ColorF{ 1.0, m_cameraTestTransition.value() }).drawFrame(2);

		// ゲーム終了ボタン（マウスオーバーの領域を描画）
	//	m_exitButton.draw(ColorF{ 1.0, m_exitTransition.value() }).drawFrame(2);

		boldFont(U"GAME START").drawAt(fontSize, m_cameraTestButton.center(), ColorF{ gamestartColor });

		boldFont(U"EXIT").drawAt(fontSize, m_exitButton.center(), ColorF{ exitColor });
	}

	// 操作説明
	{
		//const Font& boldFont = FontAsset(U"Bold");
		//boldFont(U"操作説明").drawAt(36, { 800, 300 }, ColorF{ 1, 1, 1, 1 });
		//boldFont(U"AWSD：移動").drawAt(36, { 800, 400 }, ColorF{ 1, 1, 1, 1 });
		//boldFont(U"↑↓←→：視点").drawAt(36, { 800, 450 }, ColorF{ 1, 1, 1, 1 });
		//boldFont(U"ドラッグ：視点").drawAt(36, { 800, 500 }, ColorF{ 1, 1, 1, 1 });
		//boldFont(U"左クリック：アクション").drawAt(36, { 800, 550 }, ColorF{ 1, 1, 1, 1 });
		//boldFont(U"右クリック：キャンセル").drawAt(36, { 800, 600 }, ColorF{ 1, 1, 1, 1 });
	}

}

// マウスオーバー処理
float Title::mouseOver(float color, String text, RoundRect::position_type button)
{
	float result;
	
	// テキストの矩形領域を取得
	RectF textRect = boldFont(text).regionAt(button);

	// マウスオーバー判定
	bool isMouseOver = textRect.mouseOver();

	if (isMouseOver)
	{
		result = Math::Lerp(color, maxColor, 0.1);
	}
	else
	{
		result = Math::Lerp(color, minColor, 0.1);
	}
	
	return result;
}