#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

// include omp header file here
#include <omp.h>

#define RGB_COMPONENT_COLOR 255

struct PPMPixel {
    int red;
    int green;
    int blue;
};

typedef struct{
    int x, y, all;
    PPMPixel * data;
} PPMImage;

void readPPM(const char *filename, PPMImage& img){
    std::ifstream file (filename);
    if (file){
        std::string s;
        int rgb_comp_color;
        file >> s;
        if (s!="P3") {std::cout<< "error in format"<<std::endl; exit(9);}
        file >> img.x >>img.y;
        file >>rgb_comp_color;
        img.all = img.x*img.y;
        std::cout << s << std::endl;
        std::cout << "x=" << img.x << " y=" << img.y << " all=" <<img.all << std::endl;
        img.data = new PPMPixel[img.all];
        for (int i=0; i<img.all; i++){
            file >> img.data[i].red >>img.data[i].green >> img.data[i].blue;
        }

    }else{
        std::cout << "the file:" << filename << "was not found" << std::endl;
    }
    file.close();
}

void writePPM(const char *filename, PPMImage & img){
    std::ofstream file (filename, std::ofstream::out);
    file << "P3"<<std::endl;
    file << img.x << " " << img.y << " "<< std::endl;
    file << RGB_COMPONENT_COLOR << std::endl;

    for(int i=0; i<img.all; i++){
        file << img.data[i].red << " " << img.data[i].green << " " << img.data[i].blue << (((i+1)%img.x ==0)? "\n" : " ");
    }
    file.close();
}

//
//write the function for shifting
//

void shiftPPM(PPMImage &img){
    PPMImage shifted;
    shifted.data = new PPMPixel[img.all];
    #pragma omp parallel shared(img, shifted) num_threads(6)
    {
    #pragma omp for 
    for (int i =0;i<img.y;i++){
        for (int j=0;j<img.x; j++){
            if ((img.x*i+j+1)%img.x!=0){
            shifted.data[img.x*i+j+1] = img.data[img.x*i+j];
            }
            else{
                shifted.data[img.x*i] = img.data[img.x*i+j];
            }

        }

    }
    }
    img.data = shifted.data;

}



int main(int argc, char *argv[]){
    PPMImage image;
    
    
    //
    //write code here
    //
    double start, end;
    int shift = 50;
    start = omp_get_wtime();
    readPPM("car.ppm", image);

    int saveEvery = 10;

    for (int i=0;i< shift; i++){
        shiftPPM(image);
        if (i % saveEvery == 0) {
            writePPM("new_car.ppm", image);
        }
    }
    end = omp_get_wtime();
    std::cout << "Save every " << saveEvery << " frame: Time elapsed: " <<end-start << "seconds."<< std::endl;

    start = omp_get_wtime();
    readPPM("car.ppm", image);
    for (int i=0;i< shift; i++){
        shiftPPM(image);
        
    }
    writePPM("new_car.ppm", image);
    end = omp_get_wtime();
    std::cout << "Save only final frame: Time elapsed: " <<end-start << "seconds."<< std::endl;
    return 0;
}
