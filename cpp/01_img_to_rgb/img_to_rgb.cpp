#include <../common.h>
#include <../cmdline.h>
#include <wand/MagickWand.h>

img_struct* image_to_rgb(const std::string& jpg) {

  PixelWand **pixels;
  MagickPixelPacket pixel;  
  /*
    Read an image.
  */
  MagickWandGenesis();
  MagickWand* image_wand=NewMagickWand();
  if(image_wand==NULL) {
    std::cerr << "Unable to allocate wand" << std::endl;
    return NULL;
  }
  MagickBooleanType status=MagickReadImage(image_wand,jpg.c_str());
  if (status == MagickFalse) {
    std::cerr << "Reading image failed " << std::endl;
    return NULL;
  }

  size_t width = (size_t)MagickGetImageWidth(image_wand);
  size_t height = (size_t)MagickGetImageHeight(image_wand);

  img_struct* rgb = new img_struct(width, height, img_struct::RGB);

  unsigned int index = 0;
  PixelIterator* iterator = NewPixelIterator(image_wand);
  for (long y=0; y < (long)MagickGetImageHeight(image_wand); y++) {
    pixels=PixelGetNextIteratorRow(iterator,&width);
    for (long x=0; x < (long)MagickGetImageWidth(image_wand); x++) {
      PixelGetMagickColor(pixels[x],&pixel);
        rgb->data[index++] = (float)pixel.red;
        rgb->data[index++] = (float)pixel.green;
        rgb->data[index++] = (float)pixel.blue;
      }
  }
  DestroyPixelIterator(iterator);
  DestroyMagickWand(image_wand);
  MagickWandTerminus();

  return rgb;
};

bool rgb_to_image(const img_struct* rgb, const std::string& filename) {

  //MagickWand expects the float pixel data to be normalized --> this is not the case. Convert from float to unsigned char
  size_t size = rgb->width * rgb->height * 3;
  unsigned char* temp = new unsigned char[size]; //input is float RGB, hence depth is 3
  for(size_t i=0; i<size; i++) {
    temp[i] = (unsigned char)rgb->data[i];
  }
  MagickWandGenesis();
  MagickWand* image_wand = NewMagickWand();
  MagickBooleanType status = MagickConstituteImage(image_wand, rgb->width, rgb->height,"RGB",CharPixel, temp);
  if(status==MagickTrue) {
    status = MagickWriteImage(image_wand, filename.c_str());
  }
  else {
    std::cerr << "Error writing magick image " << std::endl;
  }
  MagickWandTerminus();
  return status == MagickTrue;
};


int main(int argc, char** argv) {
  cmdline::parser p;

  p.add<std::string>("img", 'i', "image filename", true, "");
  p.add<std::string>("rgb", 'r', "rgb filename", true, "");
  p.add<bool>("reverse", '!', "rgb to image", false, false);

  p.parse_check(argc,argv);

  std::string img_filename = p.get<std::string>("img");
  std::string rgb_filename = p.get<std::string>("rgb");
  bool reverse = p.get<bool>("reverse");

  std::cout << "options are " << img_filename << " <--> " << rgb_filename << " [" << reverse << "]" << std::endl;

  if(reverse==false) {
    img_struct* rgb = image_to_rgb(img_filename);
    if(rgb!=NULL) {
      rgb->write_image(rgb_filename);
      delete rgb;
      return 0;
    }
  }
  else {
    img_struct* rgb = new img_struct(rgb_filename);
    if(rgb != NULL ){
      rgb_to_image(rgb, img_filename);
      delete rgb;
      return 0;
    }
    else {
      std::cerr << "Error reading the file " << rgb_filename << std::endl;
    }
  }
  return -1;
}
