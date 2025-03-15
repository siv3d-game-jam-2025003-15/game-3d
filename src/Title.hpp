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

	RoundRect m_startButton{ Arg::center(640, 300), 300, 60, 8 };
	RoundRect m_rankingButton{ Arg::center(640, 400), 300, 60, 8 };
	RoundRect m_exitButton{ Arg::center(640, 500), 300, 60, 8 };
	RoundRect m_cameraTestButton{ Arg::center(640, 600), 300, 60, 8 };

	Transition m_startTransition{ 0.4s, 0.2s };
	Transition m_rankingTransition{ 0.4s, 0.2s };
	Transition m_exitTransition{ 0.4s, 0.2s };
	Transition m_cameraTestTransition{ 0.4s, 0.2s };
};
