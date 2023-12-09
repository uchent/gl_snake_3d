#ifndef SNAKE_IMAGE_H_
#define SNAKE_IMAGE_H_

class Image {
 public:
  Image(char* ps, int w, int h);
  ~Image();

  char* pixels;
  int width;
  int height;
};

#endif // SNAKE_IMAGE_H_
