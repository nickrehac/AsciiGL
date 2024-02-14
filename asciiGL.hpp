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

namespace asciiGL {
  struct Pixel {
    char character;
    int color;
  
    Pixel(char ch, int col);
  
    Pixel(char ch);
  };

  struct VertexInformation {
    glm::vec3 position;
    virtual VertexInformation * interpolate(VertexInformation * to, float amount);
    bool inBounds();
  };
  
  class FragmentShaderSettings {
    //light positions and such
  };
  
  class FragmentShader {
    FragmentShaderSettings& settings;
  public:
    FragmentShader(FragmentShaderSettings& s);
    virtual ~FragmentShader();
    virtual Pixel compute(VertexInformation* input);
  };
  
  class VertexShaderSettings {
    //transforms and the like
  };
  
  class VertexShader {
    VertexShaderSettings& settings;
  public:
    VertexShader(VertexShaderSettings & s);
    virtual ~VertexShader();
    virtual VertexInformation* compute(VertexInformation* v);
  };
  
  
  template<typename T>
  struct Buffer2D {
    std::vector<std::vector<T>> buffer;
    Buffer2D();
    Buffer2D(int w, int h, T fill);
    T& at(int x, int y);
    void clear();
    void fill(T element);
  };
  
  typedef std::array<VertexInformation*, 3> Triangle;
  
  class Renderer {
    std::vector<Triangle> triangles;
  
    Buffer2D<Pixel> frameBuffer;
    Buffer2D<float> depthBuffer;
    int width;
    int height;
  
    VertexShader& vertexShader;
  
    FragmentShader& fragmentShader;
  
    std::vector<VertexInformation*> rasterize(Triangle tri);

    glm::vec3 toScreenPos(glm::vec3 p);
    glm::vec2 toScreenPos(glm::vec2 p);

    bool depthTest(VertexInformation*);
    void rasterFlatTopTri(VertexInformation * point, VertexInformation * left, VertexInformation * right);
    void rasterFlatBottomTri(VertexInformation * point, VertexInformation * left, VertexInformation * right);
  public:
    Renderer();
    Renderer(VertexShader& vs, FragmentShader& fs);
    Renderer(int width, int height, VertexShader& vs, FragmentShader& fs);
    void setTriangles(std::vector<Triangle> tris);
    void render();
    void pushFrame();//after successive render calls
  };
};
