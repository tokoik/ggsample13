/*
** タイル
*/
#include <cmath>

#include "GgTileShader.h"

/*
** 一様乱数発生 (Xorshift 法)
*/
static GLfloat xor128()
{ 
  static unsigned int x = 123456789;
  static unsigned int y = 362436069;
  static unsigned int z = 521288629;
  static unsigned int w = 88675123; 
  unsigned int t = x ^ x << 11;

  x = y;
  y = z;
  z = w;

  return (GLfloat)(w ^= w >> 19 ^ t ^ t >> 8); 
}

/*
** 正規乱数発生 (Box Muller 法)
*/
static void boxmuller(GLfloat *r)
{
  float a = sqrt(-2.0f * log(((float)xor128() + 1.0f) / 4294967296.0f));
  float b = 6.2831853f * ((float)xor128() + 1.0f) / 4294967296.0f;

  r[0] = a * sin(b);
  r[1] = a * cos(b);
}

gg::GgTileShader::GgTileShader(const char *vert, const char *frag,
  const char *geom, int nvarying, const char **varyings)
  : GgSimpleShader(vert, frag, geom, nvarying, varyings)
{
  // プログラム名
  GLuint program = get();

  // テクスチャユニットの uniform 変数の場所
  colorLoc = glGetUniformLocation(program, "color");
  depthLoc = glGetUniformLocation(program, "depth");

  // シャドウマッピング用の uniform 変数の場所
  msLoc = glGetUniformLocation(program, "ms");

  // ビューポートの uniform 変数の場所
  vpLoc = glGetUniformLocation(program, "vp");

  // 乱数の uniform 変数の場所
  rnLoc = glGetUniformLocation(program, "rn");
  for (unsigned int i = 0; i < sizeof rn / sizeof rn[0]; i += 2) boxmuller(rn + i);
}

void gg::GgTileShader::use() const
{
  // 基底クラスのシェーダの設定を呼び出す
  GgSimpleShader::use();

  // 乱数
  glUniform2fv(rnLoc, sizeof rn / sizeof rn[0] / 2, rn);

  // ビューポート
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glUniform2f(vpLoc, static_cast<GLfloat>(vp[2]), static_cast<GLfloat>(vp[3]));

  // テクスチャユニット
  glUniform1i(colorLoc, 0);
  glUniform1i(depthLoc, 1);
}
