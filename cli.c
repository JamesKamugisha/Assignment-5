#include "kernel.h"
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int generate_pagefault() {
    int fd = open("fault.bin", O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        return -1;
    }
    if (ftruncate(fd, 4096) == -1) {
        close(fd);
        return -1;
    }
    char* mapping=mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(mapping==MAP_FAILED){
        close(fd);
        return -1;
    }
    mapping[0] = 'A';
    msync(mapping, 4096, MS_SYNC);
    munmap(mapping, 4096);
    close(fd);

    return 0;

}

int main(int argc, char** argv){
    // TODO: parse the arguments in argv. 
    // You can expect argv[1] to be the mode
    // You can expect argv[2] to be the filepath
    // You can expect argv[3] to be the integer width
    // You can expect argv[4] to be the integer height
    // You can expect argv[5] to be the output filepath.

    if(argc != 6) {
        printf("Incorrect number of arguments. Expected: ./build/image_calc <MODE=kernel|mmap|convert|uconvert|fault> <input_image> <width> <height> <output_image_path>\n");
        return -1;
    }

   
    char* mode=argv[1];
    char* filepath=argv[2];

    int width=atoi(argv[3]);
    int height=atoi(argv[4]);
    char* output_filepath=argv[5];

     // TODO: call correct function based on mode
     if(strcmp(mode, "kernel")==0){
        struct image* image=malloc(sizeof(struct image));
        if(image==NULL){
            return -1;
        }
            image->width=width;
            image->height=height;

            if(loadimage(filepath, image) !=0){
                free(image);
                return -1;
            }
        
     

    // TODO: allocate the space needed for one image and load the image

    int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};

    // TODO: call apply kernel with 1/9 (as a float) as the normalization value
    struct image* output=apply_kernel(image, (int*) kernel, 3, 1.0f/9.0f);
    if(output == NULL){
        free(image->pixels);
        free(image);
        return -1;
    }

    if(saveimage(output_filepath, output) !=0){
        free(output->pixels);
        free(output);

        free(image->pixels);
        free(image);

        return -1;
    }

    free(output->pixels);
    free(output);

    free(image->pixels);
    free(image);
    return 0;
    
    }
    if (strcmp(mode, "convert") == 0) {

        struct image* image = malloc(sizeof(struct image));

        if(image==NULL){
            return -1;
        }
        image->width=width;
        image->height=height;

        if(loadimage(filepath, image) !=0){
            free(image);
            return -1;
        }

        int result=saveimage_mmap(output_filepath, image);

        free(image->pixels);
        free(image);

        return result;
    }

    if (strcmp(mode, "uconvert") == 0) {

    struct image* image = malloc(sizeof(struct image));

    if (image == NULL) {
        return -1;
    }

    image->width = width;
    image->height = height;

    if (loadimage_mmap(filepath, image) != 0) {
        free(image);
        return -1;
    }

    int result = saveimage(output_filepath, image);

    size_t map_size =
        sizeof(struct image) +
        sizeof(struct pixel) *
        image->width *
        image->height;

    void* mapping =
        (char*)image->pixels - sizeof(struct image);

    munmap(mapping, map_size);

    free(image);

    return result;
}
    if (strcmp(mode, "mmap") == 0) {

    struct image* image = malloc(sizeof(struct image));

    if (image == NULL) {
        return -1;
    }

    image->width = width;
    image->height = height;

    if (loadimage_mmap(filepath, image) != 0) {
        free(image);
        return -1;
    }

    int kernel[3][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1}
    };

    struct image* output =
        apply_kernel(
            image,
            (int*) kernel,
            3,
            1.0f / 9.0f
        );
        if (output == NULL) {

        size_t map_size =
            sizeof(struct image) +
            sizeof(struct pixel) *
            image->width *
            image->height;

        void* mapping =
            (char*)image->pixels -
            sizeof(struct image);

        munmap(mapping, map_size);
        free(image);

        return -1;
    }
        if (saveimage(output_filepath, output) != 0) {

        free(output->pixels);
        free(output);

        size_t map_size =
            sizeof(struct image) +
            sizeof(struct pixel) *
            image->width *
            image->height;

        void* mapping =
            (char*)image->pixels -
            sizeof(struct image);

        munmap(mapping, map_size);
        free(image);

        return -1;
    }

        free(output->pixels);
        free(output);

        size_t map_size =
            sizeof(struct image) +
            sizeof(struct pixel) *
            image->width *
            image->height;

        void* mapping =
            (char*)image->pixels -
            sizeof(struct image);

        munmap(mapping, map_size);

        free(image);

        return 0;
}

    if (strcmp(mode, "fault") == 0) {
        return generate_pagefault();
    }
    printf("Unknown mode: %s\n", mode);
        return -1;
        
    }

