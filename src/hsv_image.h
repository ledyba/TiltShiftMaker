/*
 * hsv_image.h
 *
 *  Created on: 2009/04/18
 *      Author: PSI
 */

#ifndef HSV_IMAGE_H_
#define HSV_IMAGE_H_
#include <SDL/SDL.h>

//固定小数点の精度

typedef struct HSV_Color{
	float h;
	float s;
	float v;
}HSV_Color;

typedef struct HSV_Surface{
	unsigned int w;
	unsigned int h;
	HSV_Color* pix;
}HSV_Surface;
HSV_Surface* HSV_CreateSurface(int x,int y,int is_clear);
HSV_Surface* HSV_ConvertSurfaceToHSV(SDL_Surface *src);
SDL_Surface* HSV_ConvertSurfaceToRGB(HSV_Surface *src, Uint32 flags);
void HSV_FreeSurface(HSV_Surface *surf);
HSV_Color* HSV_getColor(HSV_Surface* surface,int x,int y);

#endif /* HSV_IMAGE_H_ */
