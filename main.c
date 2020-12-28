#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum {
    PREF_OUTPUT     = 1<<0,
    PREF_NAME       = 1<<1,
    PREF_COMPACT    = 1<<2,
};

int main(int argc, char* argv[]){
    if (argc < 2)
        goto err_usage;
    
    char pref = 0;
    int output_name_idx, array_name_idx;
    {
        int check = 0;
        for (int i = 0; i<argc; i++){
            if (strncmp(argv[i], "-o", 2) == 0) {
                output_name_idx = i+1;
                pref |= PREF_OUTPUT;
                i++;
            }
            else if (strncmp(argv[i], "-n", 2) == 0) {
                array_name_idx = i+1;
                pref |= PREF_NAME;
                i++;
            }
            else if (strncmp(argv[i], "-c", 2) == 0)
                pref |= PREF_COMPACT;
        }
    }
    
    int img_width, img_height, img_depth;
    stbi_uc* img_data = stbi_load(argv[argc-1], &img_width, &img_height, &img_depth, 0);
    if (!img_data)
        return fprintf(stderr, "Could not read input file!\n"), -1;
    
    unsigned long long img_size = img_width*img_height*img_depth;
    
    FILE* output_file;
    if (pref & PREF_OUTPUT)
        output_file = fopen(argv[output_name_idx], "w+");
    else output_file = stdout;
    if (!output_file)
        return perror("File"), -1;
    
    char* array_name = pref & PREF_NAME ? argv[array_name_idx] : "image";
    
    if (pref & PREF_COMPACT) {
        fprintf(output_file, "/*Image generated with img2c*/"
                             "int %s_width=%d,%s_height=%d,%s_depth=%d;"
                             "unsigned char %s_data[%llu]={",
                             array_name, img_width,
                             array_name, img_height,
                             array_name, img_depth,
                             array_name, img_size);
        for (int i = 0; i<img_size; i++){
            fprintf(output_file, "%d,", img_data[i]);
        }
        fprintf(output_file, "};");
    }
    else {
        fprintf(output_file, "// Image generated with img2c\n"
                             "int %s_width = %d, %s_height = %d, %s_depth = %d;\n"
                             "unsigned char %s_data[%llu] = {",
                             array_name, img_width,
                             array_name, img_height,
                             array_name, img_depth,
                             array_name, img_size);
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
            "\t-o\toutput file destination\n"
            "\t-n\tarray name\n"
            "\t-c\tcompact output\n"
            "\n", argv[0]
            ), -1;
}
