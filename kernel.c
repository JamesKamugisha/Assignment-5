#include "loader.h"
#include <stdlib.h>
#include <string.h>

/** Returns p1 with each channel multiplied by scalar. */
struct pixel mul(struct pixel p1, float scalar) {
    return (struct pixel){r: p1.r * scalar, g: p1.g * scalar, b: p1.b * scalar};
}
/** Returns the channel-wise sum of p1 and p2. */
struct pixel add(struct pixel p1, struct pixel p2) {
    return (struct pixel){r: p1.r + p2.r, g: p1.g + p2.g, b: p1.b + p2.b};
}

/**
 * Applies a square kernel to an image (cross-correlation).
 *
 * Produces a new image where each output pixel is the weighted sum of
 * the ksize x ksize neighborhood centered on the corresponding input
 * pixel, multiplied by normalize. The kernel is applied as-is (not
 * flipped), so this is technically cross-correlation; the result is
 * identical to convolution for symmetric kernels.
 *
 * The input img is padded so that kernel operations that fall outside of the 
 * original image are multiplied by a black pixel (zero padding).
 *
 * img        Source image. Not modified.
 * kernel     Kernel weights in row-major order, containing ksize * ksize elements.
 * ksize      Width and height of the kernel. Should be odd
 * normalize  Scale factor applied to each weighted sum
 *                       (e.g., 1.0f / 9 for a 3x3 box blur).
 *
 * Returns a pointer to a newly allocated image with the same dimensions as img.
 *
 */
struct image* apply_kernel(struct image* img, int* kernel, int ksize, float normalize) {

    struct image* output= malloc(sizeof(struct image));
    if(output==NULL){
        return NULL;
    }

    output->width=img->width;
    output->height=img->height;
    output->pixels=malloc(sizeof(struct pixel) * output->width*output->height);

    if(output->pixels == NULL){
        free(output);
        return NULL;
    }

    int radius=ksize/2;
    //visiting every image pixel
    for(int y=0; y<img->height; y++){
        for(int x=0; x<img->width; x++){
            struct pixel sum={0,0,0};
      
   
    //walking through the kernel
            for(int ky=0; ky<ksize; ky++){
                for(int kx=0; kx<ksize; kx++){
                    //find a match neighbor
                    int image_x= x+kx-radius;
                    int image_y= y+ky-radius;

                    if(image_x >=0 && image_x<img->width && image_y>=0 && image_y <img->height){
                        struct pixel p= img->pixels[image_y * img->width + image_x];

                        int weight=kernel[
                            ky*ksize + kx
                        ];

                        sum=add(sum, mul(p,weight));
            }

        }
    }
    
                 
    output->pixels[
        y*output->width+x
    ] = mul(sum, normalize);
      }
        
    }
    return output;
 }


