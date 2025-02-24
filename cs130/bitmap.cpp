//bitmap.cpp
//COSC130
//Ethan Maness
//2-26-21
//Creates an inverted and flipped image

#include <cstdio>
#include <cstdint>

using namespace std;

#pragma pack(push, 2)

//fileheader info
struct BitmapFileHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

//infoheader info
struct BitmapInfoHeader {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitcount;
    uint32_t compression;
    uint32_t imagesize;
    uint32_t x_pixels_per_meter;
    uint32_t y_pixels_per_meter;
    uint32_t color_used;
    uint32_t color_important;
};

//each pixel stores these
struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#pragma pack(pop)

int main(int argc, char *argv[]) {
    
    //make vars
    BitmapFileHeader fheader;
    BitmapInfoHeader iheader;
    FILE *ifile;
    FILE *ofile;

    if(argc != 3){
        printf("Incorrect arguments");
        return -1;
    }

    //open files from argv
    ifile = fopen(argv[1],"rb");
    ofile = fopen(argv[2],"wb");

    if(nullptr == ifile){
        perror("ifile");
        return -1;
    }
    if(nullptr == ofile){
        perror("ofile");
        return -1;
    }

    //read fheader
    fseek(ifile, 0, SEEK_SET);
    fread(&fheader, 1, sizeof(BitmapFileHeader), ifile);

    //read iheader
    fread(&iheader, 1, sizeof(BitmapInfoHeader), ifile);

    //make big pixel array on heap
    Pixel *pixels = new Pixel[iheader.width * iheader.height];

    //calc padding value
    int padding = 4 - ((iheader.width * sizeof(Pixel)) % 4);
    if(padding >= 4) padding %= 4;

    //go to start of pixels
    fseek(ifile, fheader.offset, SEEK_SET);

    //read in all pixels
    for(uint32_t r = 0; r < iheader.height; r++){

        for(uint32_t c = 0; c < iheader.width; c++){

            fread(&pixels[r*iheader.width + c], 1, sizeof(Pixel), ifile);

        }

        //skip padding bytes
        fseek(ifile, padding, SEEK_CUR);

    }

    //set headers of output file
    fseek(ofile, 0, SEEK_SET);
    fwrite(&fheader, 1, sizeof(BitmapFileHeader), ofile);
    fwrite(&iheader, 1, sizeof(BitmapInfoHeader), ofile);

    int zero = 0;
    
    //write all pixels
    for(uint32_t r = 0; r < iheader.height; r++){

        for(uint32_t c = 0; c < iheader.width; c++){

            //reverse order at which pixels are added
            uint32_t c_inv = iheader.width - c - 1;

            //invert pixel colors
            pixels[(r*iheader.width + c_inv)].r = 255 - pixels[(r*iheader.width + c_inv)].r;
            pixels[(r*iheader.width + c_inv)].g = 255 - pixels[(r*iheader.width + c_inv)].g;
            pixels[(r*iheader.width + c_inv)].b = 255 - pixels[(r*iheader.width + c_inv)].b;
            
            fwrite(&pixels[(r*iheader.width + c_inv)], 1, sizeof(Pixel), ofile);

        }

        //add padding bytes
        fwrite(&zero, 1, padding, ofile);

    }

    //clean up memory
    delete[] pixels;
    fclose(ifile);
    fclose(ofile);

    return 0;

}