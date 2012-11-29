#include <../common.h>
#include <../cmdline.h>
#include <complex>

double distance(const float* v1, const float* v2) {

  double d1 = v1[0]-v2[0];
  double d2 = v1[1]-v2[1];
  double d3 = v1[2]-v2[2];
  return sqrt<double>( (d1*d1) + (d2*d2) + (d3*d3) );
}
/*
m[0] = pointer to first color component of the first pixel
m[1] = pointer to second color component of the second pixel
m[2] = ...
*/
double get_average_distance_3(const float** m ) {
  float central_color[3];
  float surround_color[3];

  //TODO: take into account depth instead of hardcoding 3
  central_color[0] = m[4][0];
  central_color[1] = m[4][1];
  central_color[2] = m[4][2];

  double distance[9];
  for(size_t i=0;i<9;i++) {
    if(i!=4) {
      surround_color[0] = m[i][0];
      surround_color[1] = m[i][1];
      surround_color[2] = m[i][2];
      
      distance[i] = distance(central_color, surround_color); 
    }
    else {
      distance[i] = 0.0;
    }
  }
  
  double total = 0.0;
  for(size_t i=0;i<9;i++) {
    total += distance[i];
  }
  return total / 8.0;
}

void apply_gaussian_blur(const float** surrounding_pixels, float* blurred_pixels) {
  blurred_pixels[0] = surrounding_pixels[4][0];
  blurred_pixels[1] = surrounding_pixels[4][1];
  blurred_pixels[2] = surrounding_pixels[4][2];
}

img_struct* selective_gaussian_blur(const img_struct* img, double threshold) {

  size_t index=0;
  float** surrounding_pixels = new float*[9];
  img_struct d = new img_struct(img->width, img->height, img->type);

  // don't blur the borders
  //TODO: deal with the borders, at the moment, the destination image is smaller (width and height don't match the image)
  for(size_t h=2; h<img->height-2; h++) {
    for(size_t w=2; w<img->width-2; w++) {
      img->pixel_matrix_3(h,w, surrounding_pixels);
      double avg = get_average_distance_3(m);
      if(avg < threshold) {
        apply_gaussian_blur(surrounding_pixels, &(d->data[index]));
        index += 3;
      }
      else {
        d->data[index++] = surrounding_pixels[4][0];
        d->data[index++] = surrounding_pixels[4][1];
        d->data[index++] = surrounding_pixels[4][2];
      }
    }
  }
  return d;
}

int main(int argc, char** argv) {

  cmdline::parser p;

  p.add<std::string>("infile", 'i', "input image filename", true, "");
  p.add<std::string>("outfile", 'o', "output image filename", true, "");
  p.add<float>("threshold", 't', "threshold for the selective blur", true, 100.0);

  p.parse_check(argc, argv);

  std::string infile = p.get<std::string>("infile");
  std::string outfile = p.get<std::string>("outfile");
  float threshold = p.get<float>("threshold")

  img_struct* s = new img_struct(infile);
  

  return 0;
}
