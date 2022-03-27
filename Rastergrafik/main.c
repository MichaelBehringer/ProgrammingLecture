/* 
 * File:   main.c
 * Author: Michael Behringer
 *         Baden-Wuerttemberg Cooperative State University
 *
 * Created on 12. März 2022, 16:24
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MYFILENAME "testimage.ppm" // Ihr Filename
#define X_PIXEL 1000 // Ihre Bildbreite
#define Y_PIXEL 1000 // Ihre Bildhöhe 

int image[1000][1000] = {0};

int main(int argc, char** argv) {
    printf("Anzahl Schnittpunkte eingeben: ");
    int numberPoints;
    scanf("%d", &numberPoints);
    
    if(numberPoints%2==1) {
        drawLinesToPoint(500, 500);
        numberPoints--;
    }
    
    for(int i=1; i<=(numberPoints)/2; i++) {
        drawLinesToPoint(500 + calcWidth(i), 500);
        drawLinesToPoint(500 - calcWidth(i), 500);
    }

    FILE *p_file = fopen(MYFILENAME,"w");
    if (NULL != p_file)
    {
    fprintf (p_file, "P3\n %d %d\n 255\n", X_PIXEL, Y_PIXEL);
    
    for(int y=0; y<Y_PIXEL; y++){
        for(int x=0; x<X_PIXEL; x++) {
            if(image[x][y]==0) {
                fprintf(p_file, "%d %d %d ", 255, 255, 255);
            } else {
                fprintf(p_file, "%d %d %d ", 0, 0, 0);
            }
        }
        fprintf(p_file, "\n");
    }
    
    
    fwrite(image, sizeof(image), 1, p_file);

    fclose (p_file);
    }
    
    return (EXIT_SUCCESS);
}


void drawLinesToPoint(int x, int y) {
    drawLine(400, 50, x, y);
    drawLine(600, 50, x, y);
    drawLine(400, 950, x, y);
    drawLine(600, 950, x, y);
}

int calcWidth(int num) {
    return (int) (-1.0 * pow(10.0, -0.002*(100*num-1350.0)) + 500.0); //-10^(-0.002*(100x-1350))+500
}

void setPixel(int y, int x) {
    image[y][x] = 1;
}

void drawLine(int x0, int y0, int x1, int y1)
{
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */

  while (1) {
    setPixel(x0,y0);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}