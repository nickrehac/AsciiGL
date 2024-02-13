#include "asciiGL.h"
#include <signal.h>

template<typename T>
Buffer2D<T>::Buffer2D(int w, int h, T fill) {
  std::vector<T> row;
  row.resize(w, fill);
  buffer.resize(h, row);
};


Pixel::Pixel(char ch, int col) {
  this->character = ch;
  this->color = col;
};

Pixel::Pixel(char ch) {
  this->character = ch;
  this->color = COLOR_WHITE;
};


Renderer::Renderer(shared_ptr<VertexShader> vs, shared_ptr<FragmentShader> fs) {
  this->vertexShader = vs;
  this->fragmentShader = fs;

  getmaxyx(stdscr, this->height, this->width);

  this->depthBuffer = Buffer2D<float>(width, height, -1.0f);
  this->frameBuffer = Buffer2D<Pixel>(width, height, Pixel(' '));
};

void Renderer::render() {
  //for each triangle :
    //apply vec shader
    //rasterize
    //if passed depth test :
      //write to buffer with frag shader from raster array
  for(Triangle tri : triangles) {
    Triangle transformedTri;
    transformedTri[0] = this->vertexShader->compute(tri[0]);
    transformedTri[1] = this->vertexShader->compute(tri[1]);
    transformedTri[2] = this->vertexShader->compute(tri[2]);

    std::vector<shared_ptr<VertexInformation>> raster_list = rasterize(transformedTri);
    for(shared_ptr<VertexInformation> fragment : raster_list) {
      //if(fragment out of bounds) continue;
      //if(fragment passes depth test) {
      //  buffer.at(fragment location) = fragmentShader->compute(fragment)
      //}
    }
  }
};

std::vector<shared_ptr<VertexInformation>> Renderer::rasterize(Triangle t) {
  shared_ptr<VertexInformation> v1 = t[0];
  shared_ptr<VertexInformation> v2 = t[1];
  shared_ptr<VertexInformation> v3 = t[2];
};

void Renderer::pushFrame() {
  //clear depthbuffer
};
