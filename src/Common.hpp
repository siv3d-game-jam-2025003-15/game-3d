﻿# pragma once
# include <Siv3D.hpp>

// シーンのステート
enum class State
{
	Title,
	Game,
	Ranking,
	CameraTest,
    ModelView,
    Result,
    Stage,
};

// 共有するデータ
struct GameData
{
	// 直前のゲームのスコア
	int32 lastScore = 0;

	// ハイスコア
	Array<int32> highScores = { 10, 8, 6, 4, 2 };
};

using App = SceneManager<State, GameData>;

// Windows で Direct3D の代わりに OpenGL を使用するモードに切り替える
SIV3D_SET(EngineOption::Renderer::OpenGL);