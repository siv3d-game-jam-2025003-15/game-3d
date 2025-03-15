# pragma once
# include "Common.hpp"

// ランキングシーン
class ModelView : public App::Scene
{
public:

    ModelView(const InitData& init);

    void update() override;

    void draw() const override;

private:
    
    // モデルの読み込み（マテリアル参照を無視）
    const Model model{ U"assets/models/Room/EV_Room01.obj" };
        
    const Texture floorTexture{ U"assets/models/Room/T_EV_Floor01_D.png", TextureDesc::Mipped };
    const Texture wallTexture{ U"assets/models/Room/T_EV_Wall01_D.png", TextureDesc::Mipped };
    
    
    // 背景色 (3D 用の色は .removeSRGBCurve() で sRGB カーブを除去）
    const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
    // 3D シーンを描く、マルチサンプリング対応レンダーテクスチャ
    const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
    // カメラの設定
    DebugCamera3D camera{ renderTexture.size(), 30_deg, Vec3{ 10, 10, 10 } };
    
    // スケーリングの倍率
    const Vec3 scale{ 0.01, 0.01, 0.01 };
};
