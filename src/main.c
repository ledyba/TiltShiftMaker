/*
 * main.c
 *
 *  Created on: 2009/08/24
 *      Author: PSI
 */


#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a > b ? b : a)

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "hsv_image.h"
#include <math.h>

void SDL_getColor(SDL_Surface* surf,Uint32 x,Uint32 y,Uint8* r,Uint8* g,Uint8* b){
	int bpp = surf->format->BytesPerPixel;
	Uint8 *pixels = (Uint8*)surf->pixels;
	Uint16 pitch = surf->pitch;
	SDL_GetRGB(*(Uint32*)&pixels[y*pitch + x*bpp], surf->format, r, g, b);
}

void SDL_setColor(SDL_Surface* surf,Uint32 x,Uint32 y,Uint8 r,Uint8 g,Uint8 b){
	int bpp = surf->format->BytesPerPixel;
	Uint8 *pixels = (Uint8*)surf->pixels;
	Uint16 pitch = surf->pitch;
	*(Uint32*)&pixels[y*pitch + x*bpp] = SDL_MapRGB(surf->format, r, g, b);
}

SDL_Surface* convertSurface(SDL_Surface* surf){
	HSV_Surface* hsv = HSV_ConvertSurfaceToHSV(surf);
	SDL_FreeSurface(surf);
	int x,y;
	for(y=0;y<hsv->h;y++){
		for(x=0;x<hsv->w;x++){
			HSV_Color* col = HSV_getColor(hsv,x,y);
			col->s = col->s*2.5;
			if(col->s > 1.0f){
				col->s = 1.0f;
			}
		}
	}
	surf = HSV_ConvertSurfaceToRGB(hsv,SDL_SWSURFACE);
	SDL_Surface* ret = SDL_CreateRGBSurface(surf->flags,surf->w,surf->h,surf->format->BitsPerPixel,
			surf->format->Rmask,surf->format->Gmask,surf->format->Bmask,surf->format->Amask);
	//ループ内で使う変数の指定
	Uint8 r,g,b;
	SDL_LockSurface(surf);
	SDL_LockSurface(ret);
	for(y=0;y<surf->h;y++){
		for(x=0;x<surf->w;x++){
			float zure = MAX((float)abs(y-(surf->h/2))/surf->h,(float)abs((int)x-(surf->w/2))/surf->w)*2;
			float bokasi = zure*sqrt(zure*zure*zure);
			float bokasi2 = bokasi*bokasi;
			float bokasi3 = bokasi2*bokasi;
			float bokasi4 = bokasi3*bokasi;
			int xx=0,yy=0;
			float sumr=0,sumg=0,sumb=0;
			float cnt=0;
			for(yy=MAX(0,y-4);yy<MIN(surf->h,y+4);yy++){
				for(xx=MAX(0,x-4);xx<MIN(surf->w,x+4);xx++){
					SDL_getColor(surf,xx,yy,&r,&g,&b);
					if(xx==x && yy==y){
						sumr += r;
						sumg += g;
						sumb += b;
						cnt += 1.0f;
					}else if(abs(xx-x) <= 1 && abs(yy-y) <= 1){//周り１マス
						sumr += r*bokasi;
						sumg += g*bokasi;
						sumb += b*bokasi;
						cnt += bokasi;
					}else if(abs(xx-x) <= 2 && abs(yy-y) <= 2){//周り２マス
						sumr += r*bokasi2;
						sumg += g*bokasi2;
						sumb += b*bokasi2;
						cnt += bokasi2;
					}else if(abs(xx-x) <= 3 && abs(yy-y) <= 3){//周り３マス
						sumr += r*bokasi3;
						sumg += g*bokasi3;
						sumb += b*bokasi3;
						cnt += bokasi3;
					}else{//外周
						sumr += r*bokasi4;
						sumg += g*bokasi4;
						sumb += b*bokasi4;
						cnt += bokasi4;
					}
				}
			}
			r = (Uint8)(sumr / cnt);
			g = (Uint8)(sumg / cnt);
			b = (Uint8)(sumb / cnt);
			SDL_setColor(ret,x,y,r,g,b);
		}
	}
	SDL_UnlockSurface(ret);
	SDL_UnlockSurface(surf);
	return ret;
}

int main(int argc,char* argv[]){
	int i;
	for(i=1;i<argc;i++){
		printf("Loading: %s\n",argv[i]);
		fflush(stdout);
		SDL_Surface* surf = IMG_Load(argv[i]);
		if(!surf){
			fprintf(stderr,"File not found: %s\n",argv[i]);
			fflush(stderr);
			continue;
		}
		printf("Converting...\n");
		fflush(stdout);
		surf = convertSurface(surf);
		printf("Saving...\n");
		fflush(stdout);
		SDL_SaveBMP(surf,"test.bmp");
		SDL_FreeSurface(surf);
	}
	return 0;
}
