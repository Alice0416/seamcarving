#include <stdio.h>
#include<string.h>
#include <math.h>
#include "seamcarving.h"
int power(int a){
    return a*a;
}
void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad, im->height, im->width);
    for (int y = 0; y < im->height; y++) {
        for (int x = 0; x < im->width; x++) {
            int red_diff_x = 0;
            int green_diff_x = 0;
            int blue_diff_x = 0;
            int red_diff_y = 0;
            int green_diff_y = 0;
            int blue_diff_y = 0;
            if (x == 0){
                red_diff_x = get_pixel(im, y, x+1, 0) - get_pixel(im, y, (im->width)-1, 0);
                green_diff_x =  get_pixel(im, y, x+1, 1) - get_pixel(im, y, (im->width)-1, 1);
                blue_diff_x =  get_pixel(im, y, x+1, 2) - get_pixel(im, y, (im->width)-1, 2);
            }
            else if (x == (im->width)-1){
                red_diff_x = get_pixel(im, y, 0, 0) - get_pixel(im, y, (im->width)-2, 0);
                green_diff_x =  get_pixel(im, y, 0, 1) - get_pixel(im, y, (im->width)-2, 1);
                blue_diff_x =  get_pixel(im, y, 0, 2) - get_pixel(im, y, (im->width)-2, 2);
            }
            else{
                red_diff_x = get_pixel(im, y, x+1, 0) - get_pixel(im, y, x-1, 0);
                green_diff_x =  get_pixel(im, y, x+1, 1) - get_pixel(im, y, x-1, 1);
                blue_diff_x =  get_pixel(im, y, x+1, 2) - get_pixel(im, y, x-1, 2);
            }
            if( y==0 ){
                red_diff_y = get_pixel(im, y+1, x, 0) - get_pixel(im, (im->height) - 1, x, 0);
                green_diff_y =  get_pixel(im, y+1, x, 1) - get_pixel(im, (im->height)-1, x, 1);
                blue_diff_y =  get_pixel(im, y+1, x, 2) - get_pixel(im, (im->height)-1, x, 2);
            }
            else if(y == (im->height)-1){
                red_diff_y = get_pixel(im, 0, x, 0) - get_pixel(im, y-1, x, 0);
                green_diff_y =  get_pixel(im, 0, x, 1) - get_pixel(im, y-1, x, 1);
                blue_diff_y =  get_pixel(im, 0, x, 2) - get_pixel(im, y-1, x, 2);
            }
            else{
                red_diff_y = get_pixel(im, y+1, x, 0) - get_pixel(im, y-1, x, 0);
                green_diff_y =  get_pixel(im, y+1, x, 1) - get_pixel(im, y-1, x, 1);
                blue_diff_y =  get_pixel(im, y+1, x, 2) - get_pixel(im, y-1, x, 2);
            }
        int delta_x = power(red_diff_x) + power(green_diff_x) + power(blue_diff_x);
        int delta_y = power(red_diff_y) + power(green_diff_y) + power(blue_diff_y);
        int energy = (int)(sqrt(delta_x+delta_y)/10.0);
        set_pixel(*grad, y, x, energy, energy, energy);
        }
    }
}
void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double*)malloc(sizeof(double)*(grad->height)*(grad->width));
    for (int q = 0; q < (grad->height)*(grad->width); q++){
        if(q < grad->width){
            (*best_arr)[q] = get_pixel(grad, 0, q, 0);
        }
        else{
        (*best_arr)[q] = 0.0;
        }
    }
    for(int i = 1; i < grad->height; i++){
        for(int j = 0; j< grad->width; j++){
            if(j == 0){
                for(int m = 0; m<2; m++){
                    if((*best_arr)[i*(grad->width)+j] == 0.0 ||(*best_arr)[i*(grad->width)+j] > (*best_arr)[grad->width*(i-1)+ j+m]+ (double)get_pixel(grad, i, j, 0) ){
                        (*best_arr)[i*(grad->width)+j] = (*best_arr)[grad->width*(i-1)+ j+m]+ (double)get_pixel(grad, i, j, 0);
                    }
                }
            }
            else if(j == grad->width-1){
                for (int m = 0; m<2; m++){
                    if((*best_arr)[i*(grad->width)+j] == 0.0||(*best_arr)[i*(grad->width)+j] > (*best_arr)[grad->width*(i-1)+ j-1+m]+ (double)get_pixel(grad, i, j, 0) ){
                        (*best_arr)[i*(grad->width)+j] = (*best_arr)[grad->width*(i-1)+ j-1+m]+ (double)get_pixel(grad, i, j, 0);
                    }
                }
            }
            else{
                for (int m = 0; m<3; m++){
                    if((*best_arr)[i*(grad->width)+j] == 0.0||(*best_arr)[i*(grad->width)+j] > (*best_arr)[grad->width*(i-1)+ j-1+m]+ (double)get_pixel(grad, i, j, 0) ){
                        (*best_arr)[i*(grad->width)+j] = (*best_arr)[grad->width*(i-1)+ j-1+m]+ (double)get_pixel(grad, i, j, 0);
                    }
                }
            }
        }
    }
}
void recover_path(double *best, int height, int width, int **path){
    *path = (int*)malloc(sizeof(int)*(height));
    int index = 0;
    //find the min value at the last row
    double min;
    min = best[(height - 1)*(width)];
    for (int i = (height - 1)*(width); i<(height * width); i++){
        //printf("the index 1 %d", index);
        if (best[i]< min){
            min = best[i];
            index = i-(height-1)*(width);
            //printf("the index 2 %d", index);\
        }
        *((*path)+height-1) = index;  // equveleng to (*path)[height-1] = index
    }
    // 循环height -1次
    for (int loop = height - 2; loop>= 0; loop--){
        if ((*path)[loop+1] == 0){
            double tool = best[(width)*loop + (*path)[loop+1]];//
            index = (*path)[loop+1];//
            for (int m = 0; m < 2; m++){
                if (best[(width)*(loop-1) + (*path)[loop+1]+m] < tool) {
                    tool = best[(width)*(loop) + (*path)[loop+1]+m];//
                    index = (*path)[loop+1]+m;
                }
            }
            (*path)[loop] = index;
        }
        else if((*path)[loop+1] == width-1){
            double tool = best[(width)*loop + (*path)[loop+1]-1];
            index = (*path)[loop+1]-1;
            for (int m = 0; m < 2; m++){
                if (best[(width)*(loop) + (*path)[loop+1]-1+m] < tool) {
                    tool = best[(width)*(loop) + (*path)[loop+1]-1+m];
                    index = (*path)[loop+1]-1+m;
                }
            }
            (*path)[loop] = index;
        }
        else{
            double tool = best[(width)*(loop) + (*path)[loop+1]-1];
            index = (*path)[loop+1]-1;
            for (int m = 0; m < 3; m++){
                if (best[(width)*(loop) + (*path)[loop+1]-1+m] < tool) {
                    tool = best[(width)*(loop) + (*path)[loop+1]-1+m];
                    index = (*path)[loop+1]-1+m;
                }
            }
            (*path)[loop] = index;
        }
    }
}
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    create_img(dest, src->height, src ->width - 1);
    int r;
    int g;
    int b;
    for(int cur_height = 0; cur_height < src->height; cur_height++){
        for (int cur_width = 0; cur_width < src->width-1; cur_width++){
            if (cur_width >= path[cur_height]){
                r = get_pixel(src, cur_height, cur_width+1, 0);
                g = get_pixel(src, cur_height, cur_width+1, 1);
                b = get_pixel(src, cur_height, cur_width+1, 2);
                set_pixel(*dest, cur_height, cur_width, r, g, b);
            }
            else{
                r = get_pixel(src, cur_height, cur_width, 0);
                g = get_pixel(src, cur_height, cur_width, 1);
                b = get_pixel(src, cur_height, cur_width, 2);
                set_pixel(*dest, cur_height, cur_width, r, g, b);
            }
        }
        
    }
}



