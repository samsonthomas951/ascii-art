#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *gscale1 = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

const char *gscale2 = "@%#*+=-:. ";


float getAverageL(unsigned char *image, int width, int height) {
    long total = 0;

    for (int i = 0; i < width * height; i++) {
        total += image[i];
    }

    return (float)total / (width * height);
}


char **convertImageToAscii(unsigned char *image, int img_width, int img_height, int cols, float scale, int moreLevels) {
    int w = img_width / cols;
    int h = w / scale;
    int rows = img_height / h;

    printf("cols: %d, rows: %d\n", cols, rows);
    printf("tile dims: %d x %d\n", w, h);

    if (cols > img_width || rows > img_height) {
        printf("Image too small for specified cols!\n");
        exit(0);
    }

    char **aimg = (char **)malloc(rows * sizeof(char *));
    

    for (int j = 0; j < rows; j++) {
        int y1 = j * h;
        int y2 = (j + 1) * h;
        if (j == rows - 1) y2 = img_height;

        aimg[j] = (char *)malloc(cols + 1);
        memset(aimg[j], 0, cols + 1);

        for (int i = 0; i < cols; i++) {
            int x1 = i * w;
            int x2 = (i + 1) * w;
            if (i == cols - 1) x2 = img_width;

            unsigned char *tile = (unsigned char *)malloc((x2 - x1) * (y2 - y1));
            int tile_index = 0;

            for (int ty = y1; ty < y2; ty++) {
                for (int tx = x1; tx < x2; tx++) {
                    tile[tile_index++] = image[ty * img_width + tx];
                }
            }

            float avg = getAverageL(tile, x2 - x1, y2 - y1);
            free(tile);

            if (moreLevels) {
                aimg[j][i] = gscale1[(int)((avg * 69) / 255)];
            } else {
                aimg[j][i] = gscale2[(int)((avg * 9) / 255)];
            }
        }
    }

    return aimg;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s --file <image file> [--scale <scale>] [--out <output file>] [--cols <cols>] [--morelevels]\n", argv[0]);
        return 1;
    }

    char *imgFile = NULL;
    char *outFile = "out.txt";
    float scale = 0.43;
    int cols = 80;
    int moreLevels = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            imgFile = argv[++i];
        } else if (strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            scale = atof(argv[++i]);
        } else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc) {
            outFile = argv[++i];
        } else if (strcmp(argv[i], "--cols") == 0 && i + 1 < argc) {
            cols = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--morelevels") == 0) {
            moreLevels = 1;
        }
    }

    if (!imgFile) {
        printf("Image file not specified!\n");
        return 1;
    }


    int img_width, img_height, img_channels;
    unsigned char *image = stbi_load(imgFile, &img_width, &img_height, &img_channels, 1);
    if (!image) {
        printf("Error loading image: %s\n", imgFile);
        return 1;
    }

    printf("input image dims: %d x %d\n", img_width, img_height);


    printf("generating ASCII art...\n");
    char **aimg = convertImageToAscii(image, img_width, img_height, cols, scale, moreLevels);

    FILE *f = fopen(outFile, "w");
    if (!f) {
        printf("Error opening output file!\n");
        return 1;
    }

    for (int i = 0; i < img_height / (img_width / cols / scale); i++) {
        fprintf(f, "%s\n", aimg[i]);
        free(aimg[i]);
    }

    fclose(f);
    printf("ASCII art written to %s\n", outFile);


    free(aimg);
    stbi_image_free(image);

    return 0;
}
