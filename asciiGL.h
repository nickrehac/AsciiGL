#include <vector>
#include <array>
#include <memory>
#include <ncurses.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

using std::shared_ptr;
using namespace glm;

struct screenPosition {
  int x;
  int y;
  double z;
};

class VertexInformation {
  vec3 position;
public:
  virtual VertexInformation interpolate(VertexInformation dest, float amount);
};

class FragmentShaderSettings {
  //light positions and such
};

class FragmentShader {
  shared_ptr<FragmentShaderSettings> settings;
public:
  FragmentShader();
  virtual char compute(shared_ptr<VertexInformation> input);
  void setSettings(shared_ptr<FragmentShaderSettings> s);
};

class VertexShaderSettings {
  //transforms and the like
};

class VertexShader {
  shared_ptr<VertexShaderSettings> settings;
public:
  VertexShader();
  virtual shared_ptr<VertexInformation> compute(shared_ptr<VertexInformation> v);
  void setSettings(shared_ptr<VertexShaderSettings> s);
};

struct Pixel {
  char character;
  int color;

  Pixel(char ch, int col);

  Pixel(char ch);
};

template<typename T>
class Buffer2D {
  std::vector<std::vector<T>> buffer;
public:
  Buffer2D();
  Buffer2D(int w, int h, T fill);
  T& at(int x, int y);
};

typedef std::array<shared_ptr<VertexInformation>, 3> Triangle;

class Renderer {
  std::vector<Triangle> triangles;

  Buffer2D<Pixel> frameBuffer;
  Buffer2D<float> depthBuffer;
  int width;
  int height;

  shared_ptr<VertexShader> vertexShader;

  shared_ptr<FragmentShader> fragmentShader;

  std::vector<shared_ptr<VertexInformation>> rasterize(Triangle tri);

  void screenSizeChanged(int);
public:
  Renderer();
  Renderer(shared_ptr<VertexShader> vs, shared_ptr<FragmentShader> fs);
  Renderer(int width, int height, shared_ptr<VertexShader> vs, shared_ptr<FragmentShader> fs);
  void setVertexShader(VertexShader s);
  void setFragmentShader(FragmentShader s);
  void setTriangles(std::vector<std::array<shared_ptr<VertexInformation>, 3>> tris);
  void render();
  void pushFrame();//after successive render calls
};
