// ウィンドウ関連の処理
#include "GgApplication.h"

// タイルのシェーダ
#include "GgTileShader.h"

// 標準ライブラリ
#include <cmath>
#include <memory>

// アニメーションの周期（秒）
const double cycle(10.0);

// オブジェクトの数
const int objects(6);

// 光源
GgSimpleShader::Light lightProperty
{
  { 0.2f, 0.2f, 0.2f, 1.0f }, // 環境光成分
  { 1.0f, 1.0f, 1.0f, 0.0f }, // 拡散反射光成分
  { 1.0f, 1.0f, 1.0f, 0.0f }, // 鏡面反射光成分
  { 0.0f, 0.0f, 1.0f, 1.0f }  // 光源位置
};

// 物体の材質
const GgSimpleShader::Material objectMaterial
{
  { 0.8f, 0.8f, 0.8f, 1.0f }, // 環境光に対する反射係数
  { 0.8f, 0.8f, 0.8f, 0.0f }, // 拡散反射係数
  { 0.2f, 0.2f, 0.2f, 0.0f }, // 鏡面反射係数
  40.0f                       // 輝き係数
};

// 地面の材質
const GgSimpleShader::Material tileMaterial
{
  { 0.2f, 0.2f, 0.2f, 1.0f }, // 環境光に対する反射係数
  { 0.6f, 0.6f, 0.6f, 0.0f }, // 拡散反射係数
  { 0.4f, 0.4f, 0.4f, 0.0f }, // 鏡面反射係数
  30.0f                       // 輝き係数
};

// ワールド座標系の通常の光源位置
const GgVector position{ 0.0f, 4.0f, 0.0f, 1.0f };

// ワールド座標系の光源の目標位置
const GgVector target{ 0.0f, 0.0f, 0.0f, 1.0f };

// オブジェクトの描画
//   shader: オブジェクトの描画に用いる GgSimpleShader 型のシェーダ
//   mp: オブジェクトを描画する際の GgMatrix 型の投影変換行列
//   mv: オブジェクトを描画する際の GgMatrix 型のビュー変換行列
//   object: 描画するオブジェクト
//   count: 描画するオブジェクトの数
//   t: [0, 1] の値 (時刻)
void drawObjects(const GgSimpleShader &shader, const GgMatrix &mp, const GgMatrix &mv,
  const GgElements *object, const GgSimpleShader::MaterialBuffer *material, int count, float t)
{
  // 図形のデフォルトの材質
  shader.selectMaterial(material);

  // シェーダプログラムの使用開始 (時刻 t にもとづく回転アニメーション)
  for (int i = 1; i <= count; ++i)
  {
    // アニメーションの変換行列
    const GLfloat h(fmod(36.0f * t, 2.0f) - 1.0f);
    const GLfloat x(0.0f), y(1.0f - h * h), z(1.5f);
    const GLfloat r(static_cast<GLfloat>(M_PI * (2.0 * i / GLfloat(count) - 4.0 * t)));
    const GgMatrix ma(ggRotateY(r).translate(x, y, z));

    // オブジェクトの色
    const GLfloat color[] =
    {
      (i & 1) * 0.4f + 0.4f,
      (i & 2) * 0.2f + 0.4f,
      (i & 4) * 0.1f + 0.4f,
      1.0f
    };

    // 個々の図形の材質
    material->loadMaterialAmbientAndDiffuse(color);

    // 図形の描画
    shader.loadMatrix(mp, mv * ma);
    object->draw();
  }
}

//
// アプリケーションの実行
//
void GgApplication::run()
{
  // ウィンドウを作成する
  Window window("ggsample13");

  // 背景色を指定する
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // 隠面消去を有効にする
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // 正像用のプログラムオブジェクト
  GgSimpleShader simple("ggsample13.vert", "ggsample13.frag");

  // 地面用のプログラムオブジェクト
  GgTileShader floor("ggsample13tile.vert", "ggsample13tile.frag");

  // 地面の材質
  std::unique_ptr<GgTileShader::MaterialBuffer> tile(new GgTileShader::MaterialBuffer(tileMaterial));

  // OBJ ファイルの読み込み
  const std::unique_ptr<const GgElements> object(ggElementsObj("bunny.obj"));

  // デフォルトの材質
  const std::unique_ptr<GgSimpleShader::MaterialBuffer> material(new GgSimpleShader::MaterialBuffer(objectMaterial));

  // 地面
  const std::unique_ptr<const GgTriangles> rectangle(ggRectangle(4.0f, 4.0f));

  // 正像のビュー変換行列を mv に求める
  const GgMatrix mv(ggLookat(0.0f, 3.0f, 8.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 視点座標系の光源位置を求める
  const GgVector normal(mv * position);

  // 光源の材質
  const std::unique_ptr<GgSimpleShader::LightBuffer> light(new GgSimpleShader::LightBuffer(lightProperty));

  // 経過時間のリセット
  glfwSetTime(0.0);

  // ウィンドウが開いている間くり返し描画する
  while (window.shouldClose() == GL_FALSE)
  {
    // 時刻の計測
    const float t(static_cast<float>(fmod(glfwGetTime(), cycle) / cycle));

    // 投影変換行列
    const GgMatrix mp(ggPerspective(0.5f, window.getAspect(), 1.0f, 15.0f));
    
    // 画面消去
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 正像用の光源の位置
    light->loadLightPosition(normal);

    // 正像用のシェーダの選択
    simple.use();
    simple.selectLight(light.get());

    // 正像の描画
    drawObjects(simple, mp, mv, object.get(), material.get(), objects, t);

    // 床面用のシェーダの選択
    floor.use();
    floor.selectLight(light.get());
    
    // 床面の描画
    floor.selectMaterial(tile.get());
    floor.loadMatrix(mp, mv.rotateX(-1.5707963f));
    rectangle->draw();

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
