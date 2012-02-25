// �E�B���h�E�֘A�̏���
#include "Window.h"

// �^�C���̃V�F�[�_
#include "GgTileShader.h"

// �W�����C�u����
#include <cmath>
#include <memory>

// �A�j���[�V�����̎����i�b�j
const double cycle(10.0);

// �I�u�W�F�N�g�̐�
const int objects(6);

// ����
GgSimpleShader::Light light =
{
  { 0.2f, 0.2f, 0.2f, 1.0f }, // ��������
  { 1.0f, 1.0f, 1.0f, 0.0f }, // �g�U���ˌ�����
  { 1.0f, 1.0f, 1.0f, 0.0f }, // ���ʔ��ˌ�����
  { 0.0f, 0.0f, 1.0f, 1.0f }  // ���_���W�n�̌����ʒu
};

// ���[���h���W�n�̌����ʒu
const GLfloat lp[] = { 0.0f, 4.0f, 0.0f, 1.0f };

// ���[���h���W�n�̌����̖ڕW�ʒu
const GLfloat lt[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// �n�ʂ̍ގ�
const GgSimpleShader::Material tile =
{
  { 0.2f, 0.2f, 0.2f, 1.0f },
  { 0.6f, 0.6f, 0.6f, 0.0f },
  { 0.4f, 0.4f, 0.4f, 0.0f },
  30.0f
};

// �I�u�W�F�N�g�̕`��
//   shader: �I�u�W�F�N�g�̕`��ɗp���� GgSimpleShader �^�̃V�F�[�_
//   mp: �I�u�W�F�N�g��`�悷��ۂ� GgMatrix �^�̓��e�ϊ��s��
//   mv: �I�u�W�F�N�g��`�悷��ۂ� GgMatrix �^�̃r���[�ϊ��s��
//   object: �`�悷��I�u�W�F�N�g
//   count: �`�悷��I�u�W�F�N�g�̐�
//   t: [0, 1] �̒l (����)
void drawObjects(const GgSimpleShader &shader, const GgMatrix &mp, const GgMatrix &mv,
  const GgElements *object, int count, float t)
{
  // �V�F�[�_�v���O�����̎g�p�J�n (���� t �ɂ�ƂÂ���]�A�j���[�V����)
  for (int i = 1; i <= count; ++i)
  {
    // �A�j���[�V�����̕ϊ��s��
    const GLfloat h(fmod(36.0f * t, 2.0f) - 1.0f);
    const GLfloat x(0.0f), y(1.0f - h * h), z(1.5f);
    const GLfloat r(static_cast<GLfloat>(M_PI * (2.0 * i / GLfloat(count) - 4.0 * t)));
    const GgMatrix ma(ggRotateY(r).translate(x, y, z));

    // �I�u�W�F�N�g�̐F
    const GLfloat color[] =
    {
      (i & 1) * 0.4f + 0.4f,
      (i & 2) * 0.2f + 0.4f,
      (i & 4) * 0.1f + 0.4f,
      1.0f
    };

    // �}�`�̍ގ�
    shader.loadMaterialAmbient(color);
    shader.loadMaterialDiffuse(color);
    shader.loadMaterialSpecular(0.2f, 0.2f, 0.2f, 0.0f);
    shader.loadMaterialShininess(40.0f);

    // �}�`�̕`��
    shader.loadMatrix(mp, mv * ma);
    object->draw();
  }
}

//
// ���C���v���O����
//
int main(int argc, const char *argv[])
{
  // �E�B���h�E��쐬����
  Window window("ggsample13");

  // �w�i�F��w�肷��
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  // �B�ʏ�����L���ɂ���
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // �����p�̃v���O�����I�u�W�F�N�g
  GgSimpleShader simple("simple.vert", "simple.frag");

  // �n�ʗp�̃v���O�����I�u�W�F�N�g
  GgTileShader ground("tile.vert", "tile.frag");

  // OBJ �t�@�C���̓ǂݍ���
  const std::unique_ptr<const GgElements> object(ggElementsObj("bunny.obj"));

  // �n��
  const std::unique_ptr<const GgTriangles> rectangle(ggRectangle(4.0f, 4.0f));

  // �����̃r���[�ϊ��s��� mv �ɋ��߂�
  const GgMatrix mv(ggLookat(0.0f, 3.0f, 8.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // ���_���W�n�̌����ʒu��߂�
  mv.projection(light.position, lp);

  // �o�ߎ��Ԃ̃��Z�b�g
  glfwSetTime(0.0);

  // �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
  while (window.shouldClose() == GL_FALSE)
  {
    // �����̌v��
    const float t(static_cast<float>(fmod(glfwGetTime(), cycle) / cycle));

    // ���e�ϊ��s��
    const GgMatrix mp(ggPerspective(0.5f, window.getAspect(), 1.0f, 15.0f));
    
    // ��ʏ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �����p�̃V�F�[�_�̑I��
    simple.use();
    simple.loadLight(light);

    // �����̕`��
    drawObjects(simple, mp, mv, object.get(), objects, t);

    // ���ʗp�̃V�F�[�_�̑I��
    ground.use();
    ground.loadLight(light);
    
    // ���ʂ̕`��
    ground.loadMaterial(tile);
    ground.loadMatrix(mp, mv.rotateX(-1.5707963f));
    rectangle->draw();

    // �J���[�o�b�t�@�����ւ��ăC�x���g����o��
    window.swapBuffers();
  }

  return 0;
}
