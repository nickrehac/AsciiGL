#include "asciiGL.hpp"
#include <algorithm>
#include <signal.h>

using namespace asciiGL;

VertexInformation * VertexInformation::interpolate(VertexInformation* to, float amount) {
  if(amount > 1.0f) amount = 1.0f;
  if(amount < 0.0f) amount = 0.0f;
  VertexInformation * retval = new VertexInformation;
  retval->position = (1.0f - amount)*this->position + amount*to->position;
  return retval;
}


bool VertexInformation::inBounds() {
  glm::vec3 pos = this->position;
  return pos.x > -1.0 && pos.x < 1.0 && pos.y > -1.0 && pos.y < 1.0 && pos.z < 0.0 && pos.z > -1.0;
}

VertexShader::VertexShader() {}
VertexShader::~VertexShader() {}
VertexInformation * VertexShader::compute(VertexInformation * v) {
  VertexInformation * retval = new VertexInformation;
  retval->position = v->position;
  return retval;
}
FragmentShader::FragmentShader() {}
FragmentShader::~FragmentShader() {}
Pixel FragmentShader::compute(VertexInformation * v) {
  return Pixel('X');
}

template<typename T>
Buffer2D<T>::Buffer2D(int w, int h, T fill) {
  std::vector<T> row;
  row.resize(w, fill);
  this->buffer.resize(h, row);
}
template<typename T>
Buffer2D<T>::Buffer2D() {}
template<typename T>
T& Buffer2D<T>::at(int x, int y) {
  return this->buffer[y][x];
}
template<typename T>
void Buffer2D<T>::fill(T fill) {
  for(std::vector<T> &row : buffer) {
    for(T &element : row) {
      element = fill;
    }
  }
}

template<typename T>
void Buffer2D<T>::clear() {
  for(std::vector<T> &row : buffer) {
    for(T &element : row) {
      element = T();
    }
  }
}


Pixel::Pixel(char ch, int col) {
  this->character = ch;
  this->color = col;
}

Pixel::Pixel(char ch) {
  this->character = ch;
  this->color = 0;
}


Renderer::Renderer(VertexShader & vs, FragmentShader & fs) : vertexShader{vs}, fragmentShader{fs} {
  initscr();

  if(has_colors()) {
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  }

  getmaxyx(stdscr, this->height, this->width);
  this->depthBuffer = Buffer2D<float>(width, height, -1.0f);
  this->frameBuffer = Buffer2D<Pixel>(width, height, Pixel(' '));
}
Renderer::~Renderer() {
  endwin();
}

void Renderer::resize() {
  getmaxyx(stdscr, this->height, this->width);
  this->depthBuffer = Buffer2D<float>(width, height, -1.0f);
  this->frameBuffer = Buffer2D<Pixel>(width, height, Pixel(' '));
}

void Renderer::rasterFlatTopTri(VertexInformation * point, VertexInformation * left, VertexInformation * right) {
  float leftX = toScreenPos(left->position).x;
  float rightX = toScreenPos(right->position).x;
  int bottom = toScreenPos(left->position).y;
  int dest = toScreenPos(point->position).y;

  float aspectRatio = (float)width / height;
  float leftSlope = (left->position.x - point->position.x) / (point->position.y - left->position.y);
  float rightSlope = (right->position.x - point->position.x) / (point->position.y - right->position.y);
  leftSlope *= aspectRatio;
  rightSlope *= aspectRatio;

  for(int y = bottom; y < dest; y++) {
    if(y < 0 || y >= height) break;
    leftX += leftSlope;
    rightX += rightSlope;

    VertexInformation * leftInfo = left->interpolate(point, (y - bottom) / (float)(dest - bottom));
    VertexInformation * rightInfo = right->interpolate(point, (y - bottom) / (float)(dest - bottom));

    for(int x = leftX; x < rightX; x++) {
      if(x >= width || x < 0) break;
      VertexInformation * fragment = leftInfo->VertexInformation::interpolate(rightInfo, (float)(x-leftX) / (rightX - leftX));
      if(depthBuffer.at(x, y) > fragment->position.z) continue;
      depthBuffer.at(x,y) = fragment->position.z;
      frameBuffer.at(x,y) = fragmentShader.compute(fragment);
      delete fragment;
    }

    delete leftInfo;
    delete rightInfo;
    
  }
}
void Renderer::rasterFlatBottomTri(VertexInformation * point, VertexInformation * left, VertexInformation * right) {
  glm::vec2 pointScrPos(toScreenPos(point->position));
  glm::vec2 rightScrPos(toScreenPos(right->position));
  glm::vec2 leftScrPos(toScreenPos(left->position));

  float leftX = pointScrPos.x;
  float rightX = leftX;
  int rightLimit = std::max(rightScrPos.x, leftX);
  int leftLimit = std::min(leftScrPos.x, leftX);
  int bottom = pointScrPos.y;
  int dest = leftScrPos.y;

  float aspectRatio = (float)width / height;
  float leftSlope = (left->position.x - point->position.x) / (point->position.y - left->position.y);
  float rightSlope = (right->position.x - point->position.x) / (point->position.y - right->position.y);
  leftSlope *= aspectRatio;
  rightSlope *= aspectRatio;

  for(int y = bottom; y < dest; y++) {
    if(y < 0 || y >= height) break;
    leftX += leftSlope;
    rightX += rightSlope;
    int interpRange = rightX - leftX;
    if(rightX > rightLimit) rightX = rightLimit;
    if(leftX < leftLimit) leftX = leftLimit;

    VertexInformation * leftInfo = point->interpolate(left, (y - bottom) / (float)(dest - bottom));
    VertexInformation * rightInfo = point->interpolate(right, (y - bottom) / (float)(dest - bottom));

    for(int x = leftX; x < rightX; x++) {
      if(x >= width || x < 0) break;
      VertexInformation * fragment = leftInfo->VertexInformation::interpolate(rightInfo, (float)(x-leftX) / (interpRange));
      if(depthBuffer.at(x, y) > fragment->position.z) continue;
      depthBuffer.at(x,y) = fragment->position.z;
      frameBuffer.at(x,y) = fragmentShader.compute(fragment);
      delete fragment;
    }

    delete leftInfo;
    delete rightInfo;
    
  }
}

void Renderer::render(std::vector<Triangle> triangles) {
  //for each triangle :
    //apply vec shader
    //rasterize
    //if passed depth test :
      //write to buffer with frag shader from raster array
  for(Triangle tri : triangles) {
    VertexInformation* p1 = this->vertexShader.compute(tri[0]);
    VertexInformation* p2 = this->vertexShader.compute(tri[1]);
    VertexInformation* p3 = this->vertexShader.compute(tri[2]);
  
    //p1.y <= p2.y p3.y
    //p2.x <= p3.x
    //Important: Remember that Y coords get flipped across horizontal axis when converting to screen space
    if(p1->position.y > p3->position.y) std::swap(p1, p3);
    if(p1->position.y > p2->position.y) std::swap(p1, p2);
    if(p2->position.y > p3->position.y) std::swap(p2, p3);

    float p4Placement = (p2->position.y - p1->position.y) / (p3->position.y - p1->position.y);
    VertexInformation* p4 = p1->interpolate(p3, p4Placement);
    if(p4->position.x < p2->position.x) std::swap(p2, p4);
    
    rasterFlatBottomTri(p3, p2, p4);
    rasterFlatTopTri(p1, p2, p4);


    delete p1;
    delete p2;
    delete p3;
    delete p4;

      //if(fragment out of bounds) {
      //  delete fragment
      //  continue;
      //}
      //if(fragment passes depth test) {
      //  buffer.at(fragment location) = fragmentShader->compute(fragment)
      //}
      //delete fragment
  }
}



glm::vec3 Renderer::toScreenPos(glm::vec3 p) {
  p.x += 1.0;
  p.y *= -1.0;//screen space is y-inverted
  p.y += 1.0;
  p.x *= width / 2.0f;
  p.y *= height / 2.0f;
  return p;
}

glm::vec2 Renderer::toScreenPos(glm::vec2 p) {
  p.x += 1.0;
  p.y *= -1.0;//screen space is y-inverted
  p.y += 1.0;
  p.x *= width / 2.0f;
  p.y *= height / 2.0f;
  return p;
}

void Renderer::pushFrame() {
  erase();

  for(int y = 0; y < frameBuffer.buffer.size(); y++) {
    for(int x = 0; x < frameBuffer.buffer[y].size(); x++) {
      attron(COLOR_PAIR(frameBuffer.at(x,y).color));
      mvaddch(y, x, frameBuffer.at(x,y).character);
    }
  }

  refresh();
  frameBuffer.fill(Pixel(' '));
  depthBuffer.fill(-1.0f);
}
