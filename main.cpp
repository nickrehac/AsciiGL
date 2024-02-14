#include <ncurses.h>
#include "asciiGL.hpp"

struct MyVInfo : asciiGL::VertexInformation {
  MyVInfo(glm::vec3 pos) {
    this->position = pos;
  }
};

class MyVShader : public asciiGL::VertexShader {
public:
  MyVShader(asciiGL::VertexShaderSettings &vertSettings) : asciiGL::VertexShader{vertSettings} {}
  static float rot;
  asciiGL::VertexInformation * compute(asciiGL::VertexInformation * v) {
    rot += 0.001;
    glm::mat4 idMat(1.0f);
    return new MyVInfo(glm::vec3(glm::rotate(idMat, rot, glm::vec3(0.0f,0.0f,1.0f)) * glm::vec4(v->position,1.0)));
  }
};
float MyVShader::rot = 0.5f;

class MyFShader : public asciiGL::FragmentShader {
  public:
  MyFShader(asciiGL::FragmentShaderSettings &f) : asciiGL::FragmentShader{f} {}
  asciiGL::Pixel compute(asciiGL::VertexInformation * v) {
    return asciiGL::Pixel(fromFloat(v->position.z*-0.99f));
    //return asciiGL::Pixel('X');
  }
  char fromFloat(float f) {
    if(f > 1.0f) f = 1.0f;
    if(f < 0.0f) f = 0.0f;
    const char * greyscale = ".:-=+*#%@";
    return greyscale[(int)((f)* 8)];
  }
};

int main() {
  
  initscr();

  asciiGL::VertexShaderSettings vertSettings;
  MyVShader vertShader(vertSettings);
  //asciiGL::VertexShader vertShader(vertSettings);

  asciiGL::FragmentShaderSettings fragSettings;
  //asciiGL::FragmentShader fragShader(fragSettings);
  MyFShader fragShader(fragSettings);

  asciiGL::Renderer r(vertShader, fragShader);



  std::vector<asciiGL::Triangle> tris;
  tris.push_back({
    new MyVInfo(glm::vec3(-1.0, 0.0, -0.0)),
    new MyVInfo(glm::vec3(0.0, 0.0, -0.9)),
    new MyVInfo(glm::vec3(0.0, -1.0, -0.0))
  });
  r.setTriangles(tris);

  while(true) {
    r.render();
    r.pushFrame();
  }

  //define geometry
  //link shaders
  //get input
  //main loop
  //???
  //profit
  getch();

  endwin();
}

