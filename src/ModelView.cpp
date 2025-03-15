# include "ModelView.hpp"

ModelView::ModelView(const InitData& init)
    : IScene{ init }
{
    // モデルに付随するテクスチャをアセット管理に登録
    Model::RegisterDiffuseTextures(model, TextureDesc::MippedSRGB);
    
    // おまじない
    // 環境光を設定する
    Graphics3D::SetGlobalAmbientColor(ColorF{ 0.75, 0.75, 0.75 });
    // 太陽光を設定する
    Graphics3D::SetSunColor(ColorF{ 0.5, 0.5, 0.5 });
    // 太陽の方向を設定する
    Graphics3D::SetSunDirection(Vec3{ 0, 1, -0.3 }.normalized());
}

void ModelView::update()
{
    if (MouseL.down())
    {
        // タイトルシーンへ
        changeScene(State::Title);
    }
    
    // カメラの更新
    camera.update(5.0);
    
    
}

void ModelView::draw() const
{
    // renderTexture を背景色で塗りつぶし、3D 描画のレンダーターゲットに
    const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
    
    // 3D 描画
    {
        Graphics3D::SetCameraTransform(camera);
        // スケーリングを適用してモデルを描画
        Transformer3D t{ Mat4x4::Scale(scale) };
        // モデルを描画
        model.draw();
    }
    
    
    // 描画のおまじない
    {
        Graphics3D::Flush();
        renderTexture.resolve();
        Shader::LinearToScreen(renderTexture);
    }
}
