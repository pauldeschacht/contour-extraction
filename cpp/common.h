#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <fstream>
/*
todo: extract img_type from img_struct
class color_space {
 public:
  enum type { RGB, YUV, LAB, XYZ, HSV };
  std::string to_string() const;
  void from_string(const std::string&);
  int to_int() const;
  void from_int();
private:

}
*/

class img_struct {
public:
  enum img_type { RGB, YUV, LAB, XYZ, HSV };
  img_struct(size_t width, size_t height, img_type type );
  img_struct(const std::string& filename);
  
  virtual ~img_struct();
  void write_image(const std::string&);
  
  img_struct* convolution(const float*) const;
  void pixel_matrix_3(size_t row, size_t col, const float**) const;
  void pixel_matrix_5(size_t row, size_t col, const float**) const;
  float* data; //todo change to unique_ptr
  size_t width;
  size_t height;
  img_type type;
  unsigned short depth() const;
  unsigned int num_pixels; // num_pixels = wdith * depth; data length_pixels * depth
  
  //img_type related
  img_type string_to_type(const std::string&) const;
  std::string type_to_string(img_struct::img_type type) const;
 private:
  img_type char_to_type(const char) const;
  char type_to_char(img_type) const;
};

img_struct::img_struct(size_t width, size_t height, img_type type) {      
  this->width = width;
  this->height = height;                      
  this->type = type;
  this->num_pixels = this->width * this->height;
  
  data = new float[this->num_pixels * this->depth()];
};

img_struct::img_struct(const std::string& filename) {
  //read img_struct file (basically RAW data)
  std::ifstream f;
  f.open(filename.c_str(), std::ifstream::in);
  f.read((char*)&this->width, sizeof(this->width));
  f.read((char*)&this->height, sizeof(this->height));
  char temp = 0;
  f.read((char*)&temp, sizeof(char));
  this->type = char_to_type(temp);

  std::cout << "reading img struct from disk, type is " << temp << " --> " << this->type << std::endl;

  this->num_pixels = this->width * this->height;
  std::cout << "Reading img from file " << this->width << "," << this->height << "," << this->depth();
  data = new float[this->num_pixels * this->depth() ];
  size_t size = sizeof(float) * this->num_pixels * this->depth();
  std::cout << " pixels = " << this->num_pixels << ", total size = " << size << std::endl;
  f.read((char*)data,size);
  f.close();
};

img_struct::~img_struct() {
  delete[] this->data;
  this->data = NULL;
};

unsigned short img_struct::depth() const {
  return 3;
};

img_struct::img_type img_struct::string_to_type(const std::string& s) const {
  if (s=="RGB") {
    return img_struct::RGB;
  }
  else if(s=="YUV") {
    return img_struct::YUV;
  }
  else if(s=="XYZ") {
    return img_struct::XYZ;
  }
  else if(s=="LAB") {
    return img_struct::LAB;
  }
  else if(s=="HSV") {
    return HSV;
  }
  else {
    std::cerr << "Wrong color space code" << std::endl;
    throw 1;
  }
}

std::string img_struct::type_to_string(img_struct::img_type type) const {
  switch(type) {
  case RGB: return "RGB";
  case YUV: return "YUV";
  case XYZ: return "XYZ";
  case LAB: return "LAB";
  case HSV: return "HSV";
  }
}

char img_struct::type_to_char(img_struct::img_type type) const {
  switch (type) {
  case RGB: return 0;
  case YUV: return 1;
  case LAB: return 2;
  case XYZ: return 3;
  case HSV: return 4;
  default:std::cerr << "wrong value for image type " << (int)type<< std::endl; throw 1;
  };
}

img_struct::img_type img_struct::char_to_type(const char c) const {
  switch (c) {
  case 0: return RGB;
  case 1: return YUV;
  case 2: return LAB;
  case 3: return XYZ;
  case 4: return HSV;
  default: std::cerr << "wrong char for image type " << c << std::endl; throw 1;
  };
}

void img_struct::write_image(const std::string& filename) {

  std::ofstream f;
  f.open(filename.c_str(), std::ofstream::out);
  f.write((char*)&this->width, sizeof(this->width));
  f.write((char*)&this->height, sizeof(this->height));
  char temp = type_to_char(this->type);
  f.write((char*)&temp, sizeof(char));
  std::cout << "writing img " << this->width << "," << this->height << "," << this->depth() << std::endl;
  size_t size = sizeof(float) * this->num_pixels * this->depth();
  f.write((char*)data, size);
  f.close();
};

img_struct* img_struct::convolution(const float* m) const {

  unsigned short depth = this->depth();
  img_struct* dst = new img_struct(this->width, this->height, this->type);

  size_t index=0;
  size_t size = this->width * this->height * depth;
  for(size_t i=0; i<size; i+=depth) {
    const float* t = &(this->data[i]);
    
    dst->data[index++] = t[0]*m[0] + t[1]*m[1] + t[2]*m[2];
    dst->data[index++] = t[0]*m[3] + t[1]*m[4] + t[2]*m[5];
    dst->data[index++] = t[0]*m[6] + t[1]*m[7] + t[2]*m[8];
  }
  return dst;
};

void img_struct::pixel_matrix_3(size_t row, size_t col, const float** matrix) const {
  //pixels above center picture
  const size_t depth=this->depth();
  const size_t max=this->width*this->height*depth;
  size_t idx=0;
  const long lr = (long)row;
  const long lc = (long)col;
  for(int dr=-1; dr<=1; dr++) {
    for(int dc=-1; dc<=1; dc++) {
      long index = (lr+dr)*this->width*depth + (lc+dc)*depth;
      if(index<0 || index>max) {
        matrix[idx++] = NULL;
      }
      else {
        matrix[idx++] = &(this->data[index]);
      }
    }
  }
}

void img_struct::pixel_matrix_5(size_t row, size_t col, const float** matrix) const {
  //pixels above center picture
  const size_t depth=this->depth();
  const size_t max=this->width*this->height*depth;
  size_t idx=0;
  const long lr = (long)row;
  const long lc = (long)col;
  for(int dr=-2; dr<=2; dr++) {
    for(int dc=-2; dc<=2; dc++) {
      long index = (lr+dr)*this->width*depth + (lc+dc)*depth;
      if(index<0 || index>max) {
        matrix[idx++] = NULL;
      }
      else {
        matrix[idx++] = &(this->data[index]);
      }
    }
  }
}
#endif
