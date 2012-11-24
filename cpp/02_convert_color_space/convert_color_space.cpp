#include <../common.h>
#include <../cmdline.h>
#include <cmath>
#include <algorithm>

//http://www.cse.unr.edu/~quiroz/inc/colortransforms.py
//
// RGB
//
img_struct* rgb_to_yuv(const img_struct* s) {
  float m[] = { 0.299, 0.587, 0.114, -0.14713, -0.28886, 0.436, 0.615, -0.51499, -0.10001 };
  img_struct* d = s->convolution(m);
  d->type = img_struct::YUV;
  return d;
}

img_struct* rgb_to_xyz(const img_struct* s) {
  double r,g,b;

  img_struct* d = new img_struct(s->width, s->height, img_struct::XYZ);
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {
    r = double(s->data[i+0]) / 255.0;
    g = double(s->data[i+1]) / 255.0;
    b = double(s->data[i+2]) / 255.0;
    
    if(r > 0.04045) {
      r = pow(((r+0.055)/1.055),2.4);
    }
    else {
      r /= 12.92;
    }
    if (g > 0.04045){
      g = pow(((g+0.055)/1.055 ),2.4);
    }
    else {
      g /= 12.92;
    }
    if (b > 0.04045) {
      b = pow(((b+0.055)/1.055),2.4);
    }
    else {
      b /= 12.92;
    }
    
    r *= 100;
    g *= 100;
    b *= 100;
    
    double x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    double y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    double z = r * 0.0193 + g * 0.1192 + b * 0.9505;

    d->data[i+0] = (float)x;
    d->data[i+1] = (float)y;
    d->data[i+2] = (float)z;
  }
  return d;
}

img_struct* rgb_to_hsv(const img_struct* img) {
  double r,g,b, delta_r,delta_g,delta_b;
  double h,s,v;
  double min,max,delta;

  img_struct* d = new img_struct(img->width, img->height, img_struct::HSV);
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {

    r = double(img->data[i+0]) / 255.0;
    g = double(img->data[i+1]) / 255.0;
    b = double(img->data[i+2]) / 255.0;
    
    min = std::min<double>( r, std::min<double>(g,b));
    max = std::max<double>( r, std::max<double>(g,b));
    delta = max - min;

    v = max;
    
    if (delta == 0) {    //This is a gray, no chroma
      h = 0.0;
      s = 0.0;
    }
    else { //               # Chromatic data...
      s = delta / max;
      
      delta_r = (((max-r) / 6.0) + (delta / 2.0)) / delta;
      delta_g = (((max-g) / 6.0) + (delta / 2.0)) / delta;
      delta_b = (((max-b) / 6.0) + (delta / 2.0)) / delta;
      
      if (r  == max) {
        h = delta_b-delta_g;
      }
      else if (g == max) {
        h = (1.0/3.0) + delta_r - delta_b;
      }
      else if (b == max) {
        h = (2.0/3.0) + delta_g - delta_r;
      }
      if (h < 0)
        h += 1;
      if (h > 1)
        h -= 1;
      
      d->data[i+0] = (float)h;
      d->data[i+1] = (float)s;
      d->data[i+2] = (float)v;
    }
  }
  return d;
}
//
// YUV
//
img_struct* yuv_to_rgb(const img_struct* s) {
  float m[] = { 1, 0, 1.13983, 1, -0.39465, -0.58060, 1, 2.03211, 0 };
  img_struct* d = s->convolution(m);
  d->type = img_struct::RGB;
  return d;
}

//
// XYZ
//
img_struct* xyz_to_rgb(const img_struct* s) {

  double r,g,b;

  img_struct* d = new img_struct(s->width, s->height, img_struct::RGB);
  
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {
    double x = (double)s->data[i+0] / 100.0;
    double y = (double)s->data[i+1] / 100.0;
    double z = (double)s->data[i+2] / 100.0;
    
    r = x *  3.2406 + y * -1.5372 + z * -0.4986;
    g = x * -0.9689 + y *  1.8758 + z *  0.0415;
    b = x *  0.0557 + y * -0.2040 + z *  1.0570;
    
    if (r > 0.0031308)
      r = 1.055 * (pow(r,(1.0 /2.4))) - 0.055;
    else 
      r *= 12.92;
    
    if (g > 0.0031308)
      g = 1.055 * (pow(g,(1.0/2.4))) - 0.055;
    else
      g *= 12.92;
    
    if (b > 0.0031308)
      b = 1.055 * (pow(b,(1.0/2.4))) - 0.055;
    else
      b *= 12.92;
    
    d->data[i+0] = float(r * 255.0);
    d->data[i+1] = float(g * 255.0);
    d->data[i+2] = float(b * 255.0);
  }
  return d;
}

img_struct* xyz_to_lab(const img_struct* s) {
  double x,y,z;
  double l,a,b;

  img_struct* d = new img_struct(s->width, s->height, img_struct::LAB);
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {
    
    x = double(s->data[i+0]) / 95.047;
    y = double(s->data[i+1]) / 100.0;
    z = double(s->data[i+2]) / 108.883;
    
    if (x > 0.008856)
      x = pow(x, 1.0/3.0);
    else
      x = (7.787 * x) + (16.0 / 116.0);

    if (y > 0.008856)
      y = pow(y, 1.0/3.0);
    else
      y = (7.787 * y) + (16.0 / 116.0);

    if (z > 0.008856)
      z = pow(z, 1.0/3.0);
    else
      z = (7.787 * z) + (16.0 / 116.0);

    l = (116 * y) - 16;
    a = 500.0 * (x-y );
    b = 200.0 * (y-z);
    
    d->data[i+0] = float(l);
    d->data[i+1] = float(a);
    d->data[i+2] = float(b);
  }

  return d;
}

//
// LAB
//
img_struct* lab_to_xyz(const img_struct* s) {

  double x,y,z;
  double l,a,b;

  img_struct* d = new img_struct(s->width, s->height, img_struct::XYZ);
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {

    l = double(s->data[i+0]);
    a = double(s->data[i+1]);
    b = double(s->data[i+2]);

    
    y = (l + 16.0 ) / 116.0;
    x = a / 500. + y;
    z = y - b / 200.0;

    if (pow(y,3) > 0.008856)
      y = pow(y,3.0);
    else 
      y = ( y - 16.0 / 116.0 ) / 7.787;
    
    if (pow(x,3) > 0.008856)
      x = pow(x,3.0);
    else
      x = ( x - 16.0 / 116.0 ) / 7.787;

    if (pow(z,3.0) > 0.008856)
      z = pow(z,3.0);
    else
      z = ( z - 16.0 / 116.0 ) / 7.787;

    d->data[i+0] = float(95.047*x);
    d->data[i+1] = float(100.0*y);
    d->data[i+2] = float(108.883*z);
  }
  return d;
}
//
// HSV
//
img_struct* hsv_to_rgb(const img_struct* img) {
  
  double h,s,v;
  double r,g,b;
  double vh,vi,v1,v2,v3;

  img_struct* d = new img_struct(img->width, img->height, img_struct::RGB);
  
  size_t size = d->width * d->height * 3;
  for(size_t i=0; i<size; i+=3) {
    double h = double(img->data[i+0]);
    double s = double(img->data[i+1]);
    double v = double(img->data[i+2]);

    if(s==0) {
      r = v * 255.0;
      g = v * 255.0;
      b = v * 255.0;
    }
    else {
      vh = h * 6.0;
      if (vh == 6) {
        vh = 0; //      # H must be < 1
      }
      vi = int(vh);
      v1 = v * (1.0 - s);
      v2 = v * (1.0 - s * (vh-vi));
      v3 = v * (1.0 - s * (1.0 - (vh - vi)));
      
      if (vi == 0) {
        r = v;
        g = v3;
        b = v1;
      }
      else if (vi==1) {
        r = v2;
        g = v;
        b = v1;
      }
      else if(vi==2) {
        r = v1;
        g = v;
        b = v3;
      }
      else if (vi==3) {
        r = v1;
        g = v2;
        b = v;
      }
      else if (vi==4) {
        r = v3;
        g = v1;
        b = v;
      }
      else {
        r = v;
        g = v1;
        b = v2;
      }
      
      d->data[i+0] = float(r * 255.0);
      d->data[i+1] = float(g * 255.0);
      d->data[i+2] = float(b * 255.0);
    }
  }
  return d;
};
//
// COMPOSITE
//
img_struct* rgb_to_lab(const img_struct* s) {
  img_struct* t = rgb_to_xyz(s);
  img_struct* r = xyz_to_lab(t);
  delete t;
  return r;
}

img_struct* lab_to_rgb(const img_struct* s) {
  img_struct* t = lab_to_xyz(s);
  img_struct* r = xyz_to_rgb(t);
  delete t;
  return r;
}

int main(int argc, char** argv) {

  cmdline::parser p;

  p.add<std::string>("infile", 'i', "input image filename", true, "");
  p.add<std::string>("outfile", 'o', "output image filename", true, "");
  p.add<std::string>("colorspace", 'c', "output color space", true, "RGB", cmdline::oneof<std::string>("RGB","YUV","LAB","XYZ","HSV"));

  p.parse_check(argc, argv);

  std::string infile = p.get<std::string>("infile");
  std::string outfile = p.get<std::string>("outfile");
  std::string colorspace = p.get<std::string>("colorspace");

  img_struct* s = new img_struct(infile);
  
  img_struct::img_type d_type = s->string_to_type(colorspace);
  
  if(s->type != d_type) {
    
    typedef img_struct* (*ConvertFunction)(const img_struct*);
    typedef std::pair<img_struct::img_type, img_struct::img_type> type_type;
    std::map< type_type, ConvertFunction> converters;
    
    converters[type_type(img_struct::RGB, img_struct::YUV)] = &rgb_to_yuv;
    converters[type_type(img_struct::RGB, img_struct::XYZ)] = &rgb_to_xyz;
    converters[type_type(img_struct::RGB, img_struct::LAB)] = &rgb_to_lab;
    converters[type_type(img_struct::RGB, img_struct::HSV)] = &rgb_to_hsv;

    converters[type_type(img_struct::YUV, img_struct::RGB)] = &yuv_to_rgb;

    converters[type_type(img_struct::XYZ, img_struct::RGB)] = &xyz_to_rgb;
    converters[type_type(img_struct::XYZ, img_struct::LAB)] = &xyz_to_lab;

    converters[type_type(img_struct::LAB, img_struct::RGB)] = &lab_to_rgb;
    converters[type_type(img_struct::LAB, img_struct::XYZ)] = &lab_to_xyz;

    converters[type_type(img_struct::HSV, img_struct::RGB)] = &hsv_to_rgb;

    std::cout << "[TEST] From type " << s->type_to_string(s->type) << " to " << s->type_to_string(d_type) << std::endl;

    type_type current(s->type, d_type);
    std::map<type_type, ConvertFunction>::iterator it = converters.find(current);
    if(it!=converters.end()) {
      ConvertFunction f = it->second;
      img_struct* d = (*f)(s);
    
      d->write_image(outfile);
    
      delete d;
    }
    else  {
      std::cerr << "Conversion from " << s->type_to_string(s->type) << " to " << s->type_to_string(d_type) << " is not implemented " << std::endl;
      return -1;
    }
  }
  else {
    // same type
    s->write_image(outfile);
  }
  
  delete s;
  
  return 0;
}
