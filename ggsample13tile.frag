#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// 市松模様の色
const vec4 c1 = vec4(0.9, 0.9, 0.9, 1.0);
const vec4 c2 = vec4(0.6, 0.7, 0.8, 1.0);

// 映り込みのぼけ具合
const float sigma = 0.005;

// 材質
layout (std140) uniform Material
{
  vec4 kamb;                                          // 環境光の反射係数
  vec4 kdiff;                                         // 拡散反射係数
  vec4 kspec;                                         // 鏡面反射係数
  float kshi;                                         // 輝き係数
};

// テクスチャ座標のサンプラ
uniform sampler2D color;                              // カラーマップ用
uniform sampler2DShadow depth;                        // デプスマップ用

// シャドウマッピング用のデータ
uniform vec2 vp;                                      // ビューポートの大きさ
uniform vec2 rn[16];                                  // 二次元正規乱数

// ラスタライザから受け取る頂点属性の補間値
in vec4 iamb;                                         // 環境光の反射光強度
in vec4 idiff;                                        // 拡散反射光強度
in vec4 ispec;                                        // 鏡面反射光強度
in vec2 tc;                                           // カラーマップのテクスチャ座標

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                    // フラグメントの色

void main()
{
  // 市松模様
  vec4 a = mix(c1, c2, mod(floor(tc.x * 2.0) + floor(tc.y * 2.0), 2.0));
  vec2 p = gl_FragCoord.xy / vp;
  vec4 c = texture(color, p);

  fc = (iamb + idiff) * a + ispec + kspec * c;
}
