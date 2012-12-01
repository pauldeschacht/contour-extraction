#include <../common.h>
#include <../cmdline.h>
#include <math.h>

double euclidean_distance(const float* v1, const float* v2) {

  double d1 = v1[0]-v2[0];
  double d2 = v1[1]-v2[1];
  double d3 = v1[2]-v2[2];
  return sqrt( (d1*d1) + (d2*d2) + (d3*d3) );
}
/*
m[0] = pointer to first color component of the first pixel
m[1] = pointer to second color component of the second pixel
m[2] = ...
*/
double get_average_distance_5(const float** m) {
  float central_color[5];
  float surround_color[5];

  int nb = 0;
  double distance = 0;
  for(size_t i=0;i<25;i++) {
    if(i!=12) {
      if(m[i] != NULL) {
        distance += euclidean_distance(m[12], m[i]); 
        nb++;
      }
    }
    else {
    }
  }
  
  return distance / nb;
}

void apply_gaussian_blur(const img_struct* img, size_t row, size_t col, img_struct* d) {
  //float gaussianFilter[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  float gaussianFilter[25] = {1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6, 4, 16, 24, 16, 4, 1, 4, 6, 4, 1};
  const float** kernel = new const float*[25];

  double divisor = 0;

  double c1 = 0;
  double c2 = 0;
  double c3 = 0;

  img->pixel_matrix_5(row,col,kernel);
  for(size_t i=0; i<25; i++) {
    if (kernel[i] != NULL) {

      divisor += (double)gaussianFilter[i];
      c1 += (gaussianFilter[i] * kernel[i][0]); //color component 1 of surrounding pixel i
      c2 += (gaussianFilter[i] * kernel[i][1]);
      c3 += (gaussianFilter[i] * kernel[i][2]);

      /*
      c1 += kernel[i][0];
      c2 += kernel[i][1];
      c3 += kernel[i][2];
      divisor++;
      */

    }
  }

  c1 /= divisor;
  c2 /= divisor;
  c3 /= divisor;

  size_t index = row*img->width*img->depth() + col*img->depth();

  d->data[index+0] = (float)c1;
  d->data[index+1] = (float)c2;
  d->data[index+2] = (float)c3;
  
  /*
  d->data[index+0] = (float)kernel[12][0];
  d->data[index+1] = (float)kernel[12][1];
  d->data[index+2] = (float)kernel[12][2];
  */
  
}

img_struct* selective_gaussian_blur(const img_struct* img, double threshold) {

  size_t index=0;
  size_t blurred=0;
  const float** surrounding_pixels = new const float*[25];
  img_struct* d = new img_struct(img->width, img->height, img->type);
  memset((void*)d->data,'\0',img->width*img->height*img->depth()*sizeof(float));

  // don't blur the borders
  //TODO: deal with the borders, at the moment, the destination image is smaller (width and height don't match the image)
  for(size_t h=0; h<img->height; h++) {
    for(size_t w=0; w<img->width; w++) {
      img->pixel_matrix_5(h,w,surrounding_pixels);
      double avg = get_average_distance_5(surrounding_pixels);
      if(avg < threshold) {
        blurred++;
        apply_gaussian_blur(img,h,w,d);
        index += 3;
      }
      else {
        d->data[index++] = surrounding_pixels[12][0];//color component 1
        d->data[index++] = surrounding_pixels[12][1];//color component 2
        d->data[index++] = surrounding_pixels[12][2];//color component 3
      }
    }
  }
  std::cerr << "number of blurred pixels" << blurred << std::endl;
  return d;
}

int main(int argc, char** argv) {

  cmdline::parser p;

  p.add<std::string>("infile", 'i', "input image filename", true, "");
  p.add<std::string>("outfile", 'o', "output image filename", true, "");
  p.add<double>("threshold", 't', "threshold for the selective blur", true, 100.0);

  p.parse_check(argc, argv);

  std::string infile = p.get<std::string>("infile");
  std::string outfile = p.get<std::string>("outfile");
  float threshold = p.get<double>("threshold");

  std::cout << "threshold = " << threshold << std::endl;

  img_struct* s = new img_struct(infile);

  img_struct* d = selective_gaussian_blur(s,threshold);

  d->write_image(outfile);

  return 0;
}
