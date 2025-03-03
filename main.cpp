#include <ncurses.h>
#include <string>
#include <signal.h>
#include "asciiGL.hpp"


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"

struct MyVInfo : asciiGL::VertexInformation {
  MyVInfo(glm::vec3 pos) {
    this->position = pos;
  }
};

class MyVShader : public asciiGL::VertexShader {
public:
  float rotx;
  float roty;
  float rotz;
  MyVShader() {
    rotx = 0.0f;
    roty = 0.0f;
    rotz = 0.0f;
  }
  glm::mat4 perspectiveMat;
  asciiGL::VertexInformation * compute(asciiGL::VertexInformation * v) {
    glm::mat4 idMat(1.0f);
    glm::mat4 rMatX = glm::rotate(idMat, rotx, glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 rMatY = glm::rotate(idMat, roty, glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 rMatZ = glm::rotate(idMat, rotz, glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 modelMat = rMatX * rMatY * rMatZ;
    return new MyVInfo(glm::vec3(perspectiveMat * modelMat * glm::vec4(v->position,1.0)));
  }
};

class MyFShader : public asciiGL::FragmentShader {
  public:
  asciiGL::Pixel compute(asciiGL::VertexInformation * v) {
    return asciiGL::Pixel(fromFloat(v->position.z), COLOR_WHITE);
    //return asciiGL::Pixel('X');
  }
  char fromFloat(float f) {
    const char * greyscale = ".-,=+*%#&@$";
    int index = f * 10;
    if(index > 9) index = 9;
    if(index < 0) index = 0;
    return greyscale[index];
  }
};

std::vector<asciiGL::Triangle> readTriangles(const char * filename) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  std::string err;
  std::string warn;
  std::vector<asciiGL::Triangle> retval;
  bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);

  if (!warn.empty()) {
  }

  if (!err.empty()) {
  }

  if (!res) exit(10);

  for(tinygltf::Mesh &mesh : model.meshes) {
    for(tinygltf::Primitive &primitive : mesh.primitives){
      tinygltf::Accessor &indicesAccessor = model.accessors[primitive.indices];
      tinygltf::Accessor *positionAccessor = NULL;
      for(auto &attribute : primitive.attributes) {
        if(attribute.first.compare("POSITION") == 0) {
          positionAccessor = &model.accessors[attribute.second];
        }
      }
      if(positionAccessor == NULL) exit(20);

      tinygltf::BufferView &indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
      tinygltf::BufferView &positionBufferView = model.bufferViews[positionAccessor->bufferView];

      int indicesStride = indicesAccessor.ByteStride(indicesBufferView);
      int indicesOffset = indicesBufferView.byteOffset;
      int indicesLength = indicesBufferView.byteLength;

      int positionStride = positionAccessor->ByteStride(positionBufferView);
      int positionOffset = positionBufferView.byteOffset;
      int positionLength = positionBufferView.byteLength;

      //triangle interpretation
      
      void * indicesData = indicesOffset + &model.buffers[indicesBufferView.buffer].data.at(0);
      void * positionData = positionOffset + &model.buffers[positionBufferView.buffer].data.at(0);
      int indicesIndex = 0;
      while(indicesIndex < indicesLength) {
        asciiGL::Triangle tri;

        short index1 = *(short*)(indicesData+indicesIndex);
        indicesIndex += indicesStride;
        short index2 = *(short*)(indicesData+indicesIndex);
        indicesIndex += indicesStride;
        short index3 = *(short*)(indicesData+indicesIndex);
        indicesIndex += indicesStride;

        glm::vec3 pos1 = *(glm::vec3*)(positionData+index1*positionStride);
        glm::vec3 pos2 = *(glm::vec3*)(positionData+index2*positionStride);
        glm::vec3 pos3 = *(glm::vec3*)(positionData+index3*positionStride);
        
        tri[0] = new MyVInfo(pos1);
        tri[1] = new MyVInfo(pos2);
        tri[2] = new MyVInfo(pos3);

        retval.push_back(tri);
      }

    }
  }
  return retval;
}

int main(int argc, char ** argv) {
  if(argc == 1) return 1;

  std::vector<asciiGL::Triangle> loadedFile = readTriangles(argv[1]);
  
  initscr();
  nodelay(stdscr, true);

  MyVShader vertShader;
  vertShader.perspectiveMat = glm::perspective(40.0, 1.5, 0.0, 10.0);
  //asciiGL::VertexShader vertShader(vertSettings);

  //asciiGL::FragmentShader fragShader(fragSettings);
  MyFShader fragShader;

  asciiGL::Renderer r(vertShader, fragShader);

  std::vector<asciiGL::Triangle> tris;
  tris.push_back({
    new MyVInfo(glm::vec3(-1.0, 0.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 0.0, -0.9)),
    new MyVInfo(glm::vec3(0.0, -1.0, 0.0))
  });
  tris.push_back({
    new MyVInfo(glm::vec3(1.0, 0.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 1.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 0.0, -0.9))
  });
  tris.push_back({
    new MyVInfo(glm::vec3(-1.0, 0.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 1.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 0.0, -0.9))
  });
  tris.push_back({
    new MyVInfo(glm::vec3(1.0, 0.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, -1.0, 0.0)),
    new MyVInfo(glm::vec3(0.0, 0.0, -0.9))
  });

  while(true) {
    int key_pressed = getch();
    if(key_pressed == 'q') break;
    if(key_pressed == KEY_RESIZE) r.resize();
    vertShader.roty += 0.001;
    r.render(loadedFile);
    r.pushFrame();
  }

  //define geometry
  //link shaders
  //get input
  //main loop
  //???
  //profit
  endwin();
}

