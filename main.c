#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <string.h>

int main(int argc, const char* argv[]){
    if (argc < 6)
        goto err_usage;
    
    char compact = 0;
    int output_name_idx, array_name_idx;
    {
        int check = 0;
        for (int i = 0; i<argc; i++){
            if (strncmp(argv[i], "-o", 2) == 0) {
                output_name_idx = i+1;
                check |= 1;
                i++;
            }
            else if (strncmp(argv[i], "-n", 2) == 0) {
                array_name_idx = i+1;
                check |= 2;
                i++;
            }
            else if (strncmp(argv[i], "-c", 2) == 0)
                compact = 1;
        }
        if (check != 3)
            goto err_usage;
    }
    
    int img_width, img_height, img_depth;
    stbi_uc* img_data = stbi_load(argv[argc-1], &img_width, &img_height, &img_depth, 0);
    if (!img_data)
        return fprintf(stderr, "Could not read input file!\n"), -1;
    
    unsigned long long img_size = img_width*img_height*img_depth;
    
    FILE* output_file = fopen(argv[output_name_idx], "w+");
    if (!output_file)
        return perror("File"), -1;
    
    if (compact) {
        fprintf(output_file, "/*Image generated with img2c*/"
                             "int %s_width=%d,%s_height=%d,%s_depth=%d;"
                             "unsigned char %s_data[%llu]={",
                             argv[array_name_idx], img_width,
                             argv[array_name_idx], img_height,
                             argv[array_name_idx], img_depth,
                             argv[array_name_idx], img_size);
        for (int i = 0; i<img_size; i++){
            fprintf(output_file, "%d,", img_data[i]);
        }
        fprintf(output_file, "};");
    }
    else {
        fprintf(output_file, "// Image generated with img2c\n"
                             "int %s_width = %d, %s_height = %d, %s_depth = %d;\n"
                             "unsigned char %s_data[%llu] = {",
                             argv[array_name_idx], img_width,
                             argv[array_name_idx], img_height,
                             argv[array_name_idx], img_depth,
                             argv[array_name_idx], img_size);
                             
        for (int i = 0; i<img_size; i++){
            if (i%img_depth == 0)
                fprintf(output_file, " ");
            if (i%(img_width*img_depth) == 0)
                fprintf(output_file, "\n\t");
            fprintf(output_file, "0x%.2X,", img_data[i]);
        }
        fprintf(output_file, "\n};");
    }
    
    fclose(output_file);
    
    return 0;
    
    err_usage:
    return fprintf(stderr,
            "Usage: %s [options] file\n\n"
            "Options:\n"
            "\t-o\toutput file destination (required)\n"
            "\t-n\tarray name (required)\n"
            "\t-c\tcompact output\n"
            , argv[0]
            ), -1;
}
