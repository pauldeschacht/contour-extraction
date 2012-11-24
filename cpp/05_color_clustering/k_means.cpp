#include <common.h>

#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <sstream>

/**
   1. get BOOST in the CMake file (and remove atoi and co)
 **/

/*
img_struct<unsigned char> read_centroids_txt(const std::string& filename) {
  std::vector<unsigned char> temp;
  std::string line;
  std::string entry;
  std::ifstream f;


  f.open(filename.c_str(), std::ifstream::in);
  while(std::getline(f,line)) {
    std::cout << line << std::endl;
    std::istringstream linestream(line);
    while(std::getline(linestream, entry, ',')) {
      temp.push_back((unsigned char)atoi(entry.c_str()));
    }
  }

  size_t width=temp.size()/3;
  size_t height=1;
  size_t depth=3;
  img_struct<unsigned char>* centroids = new img_struct<unsigned char>
  unsigned char* data = centroids.allocate_data();

  for(unsigned int i=0;i<temp.size();i++) {
      data[i++]=temp[i];
  }

  return centroids;
  
}

*/

/*
//
//already moved to image_to_rgb
//
img_struct<unsigned char> read_image(const std::string& filename) {
  PixelWand **pixels;
  MagickPixelPacket pixel;  

  MagickWandGenesis();
  MagickWand* image_wand=NewMagickWand();
  if(image_wand==NULL) {
    std::cerr << "Unable to allocate wand" << std::endl;
    throw 2;
  }
  MagickBooleanType status=MagickReadImage(image_wand,filename.c_str());
  if (status == MagickFalse) {
    std::cerr << "Reading image failed " << std::endl;
    throw 1;
  }

  img_struct<unsigned char> rgb;
  rgb.width = (unsigned int)MagickGetImageWidth(image_wand);
  rgb.height = (unsigned int)MagickGetImageHeight(image_wand);
  rgb.depth = 3;
  unsigned char* data = rgb.allocate_data();

  size_t width;
  unsigned int index = 0;
  PixelIterator* iterator = NewPixelIterator(image_wand);
  for (long y=0; y < (long)MagickGetImageHeight(image_wand); y++) {
    pixels=PixelGetNextIteratorRow(iterator,&width);
    for (long x=0; x < (long)MagickGetImageWidth(image_wand); x++) {
      PixelGetMagickColor(pixels[x],&pixel);
        data[index++] = (unsigned char)pixel.red;
        data[index++] = (unsigned char)pixel.green;
        data[index++] = (unsigned char)pixel.blue;
      }
  }
  DestroyPixelIterator(iterator);
  DestroyMagickWand(image_wand);
  MagickWandTerminus();

  return rgb;
}


template<typename T> bool save_file(const img_struct<T>& src, const std::string& filename) {

  MagickWandGenesis();
  MagickWand* image_wand = NewMagickWand();
  MagickBooleanType status = MagickConstituteImage(image_wand, src.width, src.height,"RGB",CharPixel,src.data);
  if(status==MagickTrue) {
    status = MagickWriteImage(image_wand, filename.c_str());
  }
  MagickWandTerminus();
  return status == MagickTrue;
}

*/
template<typename T1, typename T2> 
void convert_img_struct(const img_struct<T1>& src, const float* m, img_struct<T2>& dst) {

  T1* src_data = src.data;
  unsigned short src_depth = src.depth;
  unsigned int src_num_pixels = src.num_pixels;
  T1 s1, s2, s3;
  T2 d1, d2, d3;
  
  T2* dst_data = dst.data;
  unsigned short dst_depth = dst.depth;
  
  for(unsigned int i=0; i<src_num_pixels; i++) {
    s1 = src_data[(i*src_depth)+0];
    s2 = src_data[(i*src_depth)+1];
    s3 = src_data[(i*src_depth)+2];
    
    d1 = s1 *  m[0] + s2 * m[1] + s3 * m[2];
    d2 = s1 *  m[3] + s2 * m[4] + s3 * m[5];
    d3 = s1 *  m[6] + s2 * m[7] + s3 * m[8];
    
    dst_data[(i*dst_depth)+0] = d1;
    dst_data[(i*dst_depth)+1] = d2;
    dst_data[(i*dst_depth)+2] = d3;
  }
};


template<typename T1, typename T2, int depth> 
img_struct<T2> convert_img(const img_struct<T1>& src, const float* matrix) {
  
  img_struct<T2> dst;
  dst.height = src.height;
  dst.width  = src.width;
  dst.depth  = depth;
  dst.allocate_data();
  
  convert_img_struct<T1, T2>(src, matrix, dst);
  
  return dst;
};


template<typename T>
double distance_square(const T* v1, const T* v2, unsigned short depth) {
  double sum = 0;
  for(unsigned short k=0; k<depth; k++) {
    sum += (v1[k]-v2[k])*(v1[k]-v2[k]);
  }
  return sum;
};

template<typename T>
double distance_square(const T s1, const T s2, const T s3, const T d1, const T d2, const T d3) {
  T diff1 = s1-d1;
  T diff2 = s2-d2;
  T diff3 = s3-d3;
  return ( (diff1*diff1) + (diff2*diff2) + (diff3*diff3) );
}

template<typename T>
img_struct<T> k_means(img_struct<T>& src, img_struct<T>& centroids, unsigned int max_loops) {
  
  const T* s_data = src.data;
  unsigned short s_depth = src.depth;
  
  const T* c_data = centroids.data;
  unsigned short c_depth = centroids.depth;

  //	ASSERT(s_depth==c_depth);
  
  for(unsigned int i=0; i<centroids.num_pixels; i++) {
    std::cout << "centroid " << i << " : ";
    std::cout << centroids.data[(i*c_depth)+0] << " ";
    std::cout << centroids.data[(i*c_depth)+1] << " ";
    std::cout << centroids.data[(i*c_depth)+2] << std::endl;
  }

  //init the clusters
  unsigned int cluster_index = 0;
  std::vector<unsigned int>* clusters = new std::vector<unsigned int>[centroids.num_pixels];
  for(unsigned int i=0; i<centroids.num_pixels; i++) {
    clusters[i] = std::vector<unsigned int>();
  }

  for(unsigned int loop=0; loop<max_loops; loop++) {
    //clear clusters
    for(unsigned int i=0; i<centroids.num_pixels; i++) {
      clusters[i].clear();
    }
    
    //assign pixel to cluster
    for(unsigned int i=0; i<src.num_pixels; i++) {
      
      double min_distance = std::numeric_limits<T>::max();
      double distance;
      for(unsigned int j=0; j<centroids.num_pixels; j++) {
        
        //        distance = distance_square<T>(s_data+(i*s_depth), c_data+(j*c_depth), s_depth);
        distance = distance_square<T>(s_data[(i*s_depth)+0], s_data[(i*s_depth)+1], s_data[(i*s_depth)+2],
                                      c_data[(j*c_depth)+0], c_data[(j*c_depth)+1], c_data[(j*c_depth)+2]);
                                      
        if(distance<min_distance) {
          min_distance = distance;
          cluster_index=j;
        }
      }
      //assign src_data[i] to cluster_index
      clusters[cluster_index].push_back(i);
    }
    
    //calculate the averages 
    
    double* total = new double[s_depth];
    for(unsigned int i=0; i<centroids.num_pixels; i++) {
      for(unsigned short k=0; k<s_depth; k++) {
        total[k] = 0.0;
      }
      for(unsigned int j=0; j<clusters[i].size(); j++) {
        unsigned int offset = clusters[i][j]; // for each pixel in the current cluster
        for(unsigned short k=0;k<s_depth;k++) {
          total[k] += s_data[(offset*s_depth)+k];
        }
      }
      for(unsigned short k=0;k<s_depth;k++) {
        total[k] /= clusters[i].size();
      }
      std::cout << " cluster " << i << " with size " << clusters[i].size() << " : "; 
      for(unsigned short k=0;k<s_depth;k++) {
        centroids.data[(i*c_depth)+k] = (T)total[k];
        std::cout << (T)total[k] << " ";
      }
      std::cout << std::endl;
    }
    
  }
  
  img_struct<T> dst;
  dst.width = src.width;
  dst.height = src.height;
  dst.depth = src.depth;
  T* dst_data = dst.allocate_data();
  //assign the same color to all the pixels in the same cluster
  for(unsigned int i=0; i<centroids.num_pixels; i++) {	// for each centroid == for each cluster
    for(unsigned int j=0; j<clusters[i].size(); j++) { //for each pixel/offset in the cluster
      unsigned int offset = clusters[i][j];
      for(unsigned short k=0; k<c_depth; k++) {
        dst_data[(offset*s_depth)+k] = centroids.data[(i*c_depth)+k];
      }
    }
  }
  
  return dst;
};

void test_read_write_image(const std::string& filename) {
  img_struct<unsigned char> rgb = read_image(filename);
  save_file<unsigned char>(rgb, "test_1.jpg");
}

void test_read_convert_write_image(const std::string& filename) {
  img_struct<unsigned char> rgb = read_image(filename);
  // RGB 2 YUV
  float rgb_2_yuv[] = { 0.299, 0.587, 0.114, -0.14713, -0.28886, 0.436, 0.615, -0.51499, -0.10001 };
  img_struct<float> yuv = convert_img<unsigned char, float, 3>(rgb, rgb_2_yuv);
  // YUV 2 RGB
  float yuv_2_rgb[] = { 1, 0, 1.13983, 1, -0.39465, -0.58060, 1, 2.03211, 0 };
  img_struct<unsigned char> rgb2 = convert_img<float, unsigned char, 3>(yuv, yuv_2_rgb);
  save_file<unsigned char>(rgb2, "test_2.jpg");
}

void test(const std::string& filename) {
  test_read_write_image(filename);
  test_read_convert_write_image(filename);
}

int main(int argc, char** args) {

  if(argc!=3) {
    std::cerr << "Usage cq image.jpg centroids.txt" << std::endl;
    return -1;
  }

  unsigned int max_loops = 5;
  
  std::string img_filename = args[1];
  std::cout << "Image file = " << img_filename << std::endl;

  test(img_filename);

  // read image
  img_struct<unsigned char> rgb = read_image(img_filename);
  // read centroids
  std::string centroids_filename = args[2];
  img_struct<unsigned char> rgb_centroids = read_centroids_txt(centroids_filename);
  // convert image and centroids to YUV
  float rgb_2_yuv[] = { 0.299, 0.587, 0.114, -0.14713, -0.28886, 0.436, 0.615, -0.51499, -0.10001 };
  img_struct<float> yuv = convert_img<unsigned char, float, 3>(rgb, rgb_2_yuv);
  img_struct<float> centroids_yuv = convert_img<unsigned char, float, 3>(rgb_centroids, rgb_2_yuv);
  std::cout << "Done converting to yuv " << std::endl;
  // cluster the YUV image
  std::cout << "Start k_means " << std::endl;
  img_struct<float> clustered_yuv = k_means(yuv, centroids_yuv, max_loops);
  std::cout << "End k_means " << std::endl;  
  // YUV 2 RGB (cluster image)
  float yuv_2_rgb[] = { 1, 0, 1.13983, 1, -0.39465, -0.58060, 1, 2.03211, 0 };
  img_struct<unsigned char> clustered_rgb = convert_img<float, unsigned char, 3>(clustered_yuv, yuv_2_rgb);
  std::string img_clustered_filename = "clustered_image.jpg";
  save_file<unsigned char>(clustered_rgb, img_clustered_filename);
  
  return 0;
};
