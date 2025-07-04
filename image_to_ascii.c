#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int MAX_LENGTH = 100;
int MAX_ITER = 10;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} color;

void FisherYates(int *player, int n) { //implementation of Fisher
     int i, j, tmp; // create local variables to hold values for shuffle

     for (i = n - 1; i > 0; i--) { // for loop to shuffle
         j = rand() % (i + 1); //randomise j for shuffle with Fisher Yates
         tmp = player[j];
         player[j] = player[i];
         player[i] = tmp;
     }
}

double euclideanDistance(color centroid, color point) {
    double sum = 0;
    sum += pow(centroid.r - point.r, 2);
    sum += pow(centroid.g - point.g, 2);
    sum += pow(centroid.b - point.b, 2);
    return sum;
}

color kMeansClustering(int k, color *c, int numColors) {
    int indexes[numColors];
    for(int i = 0; i < numColors ; i++) {
        indexes[i] = i;
    }

    FisherYates(indexes, numColors);

    color initial[k]; // Forgy Initialization   
    for(int i = 0; i < k; i++) {
        initial[i] = c[indexes[i]];
    }
    int centroidAssignments[numColors];
    int iterations = 0;
    int finalCount[k];
    while(iterations < MAX_ITER) {
        for(int i = 0; i < numColors; i++) {
            double lowest = INFINITY;
            for(int j = 0; j < k; j++) {
                double distance = euclideanDistance(c[i], initial[j]);
                if(distance < lowest) {
                    lowest = distance;
                    centroidAssignments[i] = j;
                }
            }
        }
        long long rSum[k];
        long long gSum[k];
        long long bSum[k];
        int count[k];

        // Initialize to 0 because C99 Compiler
        for(int i = 0; i < k; i++) {
            rSum[i] = 0;
            gSum[i] = 0;
            bSum[i] = 0;
            count[i] = 0;
        }

        for(int i = 0; i < numColors; i++) {
            int currentIndex = centroidAssignments[i];
            rSum[currentIndex] += c[i].r;
            gSum[currentIndex] += c[i].g;
            bSum[currentIndex] += c[i].b;
            count[currentIndex]++;
        }

        for(int i = 0; i < k; i++) {
            if(count[i] > 0) {
               initial[i].r = rSum[i] / count[i];
               initial[i].g = gSum[i] / count[i];
               initial[i].b = bSum[i] / count[i];
               finalCount[i] = count[i];
            }
        }
        iterations++;
    }
    int max = 0;
    int colorMaxIndex = -1;
    for(int i = 0; i < k; i++) {
        if(finalCount[i] > max) {
            max = finalCount[i];
            colorMaxIndex = i;
        }
    }

    // IF somehow finalCount doesn't work just make it black
    if(colorMaxIndex == -1) {
        color black = {0, 0, 0};
        return black; 
    }

    return initial[colorMaxIndex];
}

char *getInput() {
    int length = 100;
    char *path = calloc(100, sizeof(char));
    printf("Input path (MAXIMUM LENGTH %d characters)\n", length);
    if(fgets(path, length, stdin)) {

        return path;
    }
    return NULL;
}

double srgb_to_linear(double color) {
    color /= 255;
    if(color <= 0.04045) {
        return color / 12.92;
    } else {
        return pow((color + 0.055)/1.055, 2.4);
    }
}

double calculateBrightness(color color) {
    double rLinear = srgb_to_linear(color.r);
    double gLinear = srgb_to_linear(color.g);
    double bLinear = srgb_to_linear(color.b);

    return (0.2126 * rLinear) + (0.7152 * gLinear) + (0.0722 * bLinear);
}

char getCharacterFromBrightness(double brightness, char *asciiBrightness) {
    int asciiIndex = round(brightness * (strlen(asciiBrightness) - 1));
    return asciiBrightness[asciiIndex];
}

char *getRGBValues(int xStart, int yStart, int size, int width, int channels, unsigned char *image, int type, int k, int colored) {
    char asciiBrightness[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
    if(type == 1) {
        // 1 is dark, no need to do anything since it's the default
    } else if(type == 2) {
        // 2 is bright
        strcpy(asciiBrightness, "`^\",:;Il!i~+_-?][}(1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao#MW&8%B@S");
    } else if(type == 3) {
        strcpy(asciiBrightness, " .:-=+*#%@");
    }

    double brightnessSum = 0;
    color array[(size * 2) * size];
    int i = 0;
    for(int y = yStart; y < yStart + (size * 2); y++) {
        for(int x = xStart; x < xStart + size; x++) {
            int index = (x + y * width) * channels;
            color color = {image[index], image[index+1], image[index+2]};
            array[i] = color;
            i++;
            brightnessSum += calculateBrightness(color);
        }
    }
    color c = kMeansClustering(k, array, i - 1);
    char character = getCharacterFromBrightness(brightnessSum / ((size * 2) * size), asciiBrightness);
    char *resultString = malloc(sizeof(char) * 30);
    if(colored == 0) {
        sprintf(resultString, "\x1b[38;2;%d;%d;%dm%c\x1b[0m", c.r, c.g, c.b, character);
    } else {
        sprintf(resultString, "%c", character);
    }
    return resultString;
}

int main(int argc, char *argv[]) {
    int opt;
    int outputWidth = 80; // default
    int type = 1;
    int k = 5;
    int colored = 0;
    char *path = calloc(MAX_LENGTH, sizeof(char));
    while((opt = getopt(argc, argv, ":f:w:t:k:c:")) != -1) {
        switch(opt) {
            case 'f':
                strcpy(path, optarg);
                break;
            case 'w':
                outputWidth = atoi(optarg);
                break;
            case 't':
                type = atoi(optarg);
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case 'c':
                colored = atoi(optarg);
                break;
        }
    }
    if(type < 1 || type > 3) {
        type = 1;
    }
    // If file path was not given in args, ask for a file input
    if(strlen(path) == 0) {
        char *tempPath = getInput();
        strcpy(path, tempPath);
        path[strcspn(path, "\r\n")] = '\0';
    }

    int width, height, channels;
    unsigned char *image = stbi_load(path, &width, &height, &channels, 0); // don't forget to free
                                                                    
    if(image == NULL) {
        printf("Image with path %s could not be loaded\n", path);
        free(path);
        return -1;
    }

    if(outputWidth < 1) {
        outputWidth = 80;
    }
    int size = round(width / outputWidth);

    for(int y = 0; y < (height - ((size * 2) - 1)); y += (size * 2)) {
        for(int x = 0; x < (width - (size - 1)); x += size) {
            printf("%s", getRGBValues(x, y, size, width, channels, image, type, k, colored));
        }
        printf("\n");
    }

    free(path);
    stbi_image_free(image);
    return 0;
}
