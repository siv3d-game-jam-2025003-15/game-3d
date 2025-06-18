# include "ToBeContinued.hpp"

ToBeContinued::ToBeContinued(const InitData& init)
    : IScene{ init }
{

}

void ToBeContinued::update()
{
    // ボタンのクリック処理
    if (MouseL.down())
    {
        changeScene(State::Title);
    }
}

void ToBeContinued::draw() const
{
    Scene::SetBackground(ColorF{ 0, 0, 0 });

    const Font& boldFont = FontAsset(U"Bold");
    boldFont(U"To Be Continued...").drawAt(36, SCENE_WIDTH - 260, SCENE_HEIGHT - 100, ColorF{ 0.7 });

}
