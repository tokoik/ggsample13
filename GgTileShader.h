#pragma once

/*
** タイル
*/
#include "gg.h"

namespace gg
{

  class GgTileShader
    : public GgSimpleShader
  {
    // 映り込みのサンプル数
    static const int samples = 16;

    // テクスチャユニット
    GLuint color;             // カラーマップのテクスチャユニット
    GLint colorLoc;           // カラーマップのテクスチャユニットの uniform 変数の場所
    GLuint depth;             // デプスマップのテクスチャユニット
    GLint depthLoc;           // デプスマップのテクスチャユニットの uniform 変数の場所

    // シャドウマッピング用の変換行列
    GLint msLoc;              // シャドウマッピング用の変換行列の uniform 変数の場所

    // ビュポート
    GLint vpLoc;              // ビューポートの uniform 変数の場所

    // 乱数
    GLfloat rn[samples * 2];  // 乱数テーブル
    GLint rnLoc;              // 乱数テーブルの uniform 変数の場所

  public:

    // デストラクタ
    virtual ~GgTileShader() {}

    // コンストラクタ
    GgTileShader() {}
    GgTileShader(const char *vert, const char *frag = 0,
      const char *geom = 0, int nvarying = 0, const char **varyings = 0);

    // シェーダの使用開始
    virtual void use() const;

    // シャドウマッピング用の変換行列
    void loadShadowMatrix(const GLfloat *m) { glUniformMatrix4fv(msLoc, 1, GL_FALSE, m); }
    void loadShadowMatrix(const GgMatrix &m) { loadShadowMatrix(m.get()); }
  };
}
