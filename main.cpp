#include <ncurses.h>
#include "asciiGL.h"

int main() {
  
  RenderTarget target(40, 40);

  VertexShaderSettings vertSettings;
  VertexShader vertShader;
  vertShader.setSettings(shared_ptr<VertexShaderSettings>(&vertSettings));

  FragmentShaderSettings fragSettings;
  FragmentShader fragShader;
  fragShader.setSettings(shared_ptr<FragmentShaderSettings>(&fragSettings));


  //r->setTriangles(...);

  //define geometry
  //link shaders
  //get input
  //main loop
  //???
  //profit
}
