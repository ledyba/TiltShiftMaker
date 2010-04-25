/*
 * hsv_image.c
 *
 *  Created on: 2009/04/18
 *      Author: PSI
 */

#include <SDL/SDL.h>
#include "hsv_image.h"

HSV_Surface* HSV_CreateSurface(int x,int y,int is_clear){
	HSV_Surface* surf = malloc(sizeof(HSV_Surface));
	if(!surf){
		fprintf(stderr,"[HSV_CreateSurface] Error has occured in allocating a HSV Surface.\n");
		exit(0);
	}
	int pix_size = sizeof(HSV_Color)*x*y;
	HSV_Color* color = malloc(pix_size);
	if(!color){
		fprintf(stderr,"[HSV_CreateSurface] Error has occured in allocating a HSV Surface Color Pixels.\n");
		exit(0);
	}
	surf->h = y;
	surf->w = x;
	surf->pix = color;
	if(is_clear){
		memset(color,0,pix_size);
	}
	return surf;
}

HSV_Surface* HSV_ConvertSurfaceToHSV(SDL_Surface *src){
	SDL_Surface* tmp = 0;
	if(!src->format->palette){
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 24, 0x0000ff, 0x00ff00, 0xff0000, 0x00);
		if(!tmp){
			fprintf(stderr,"[HSV_ConvertSurfaceToHSV] Error has occured in allocating a RGB Surface.\n");
			exit(0);
		}
		SDL_Rect tmp_rect = {0,0,src->w,src->h};
		SDL_BlitSurface(src,&tmp_rect, tmp, &tmp_rect);
		src = tmp;//若干トリッキー
	}
	//HSV構造体の作成
	HSV_Surface* dst = HSV_CreateSurface(src->w,src->h,0);
	//操作のためにロック
	SDL_LockSurface(src);
	//ループのための変数指定
	int x,y;
	int max_w = src->w;
	int max_h = src->h;
	int bpp = src->format->BytesPerPixel;
	Uint8 *pixels = (Uint8*)src->pixels;
	Uint16 pitch = src->pitch;

	HSV_Color *hsv;
	//ループ内で使う変数の指定
	Uint8 r,g,b;
	int max,min;

	//ループ
	for(y=0;y<max_h;y++){
		for(x=0;x<max_w;x++){
			SDL_GetRGB(*(Uint32*)&pixels[y*pitch + x*bpp], src->format, &r, &g, &b);
			hsv = &dst->pix[y*max_w+x];
			//最大・最小
			max = r > g ? r : g;
			max = max > b ? max : b;
			min = r < g ? r : g;
			min = min < b ? min : b;
			if(max > min){
				if(max==r){
					hsv->h = 60.0f*((int)g-(int)b)/(max-min);
					if(hsv->h < 0) {
						hsv->h += 360.0f;
					}
				}else if(max == g){
					hsv->h = 60.0f*((int)b-(int)r)/(max-min) + 120.0f;
				}else if(max == b){
					hsv->h = 60.0f*((int)r-(int)g)/(max-min) + 240.0f;
				}
			}else{
				hsv->h = 0;
			}
			if(max > 0){
				hsv->s = (float)(max - min)/max;
			}else{
				hsv->s = 0;
			}
			hsv->v = (float)max / 256;
			//printf("hsv(%f,%f,%f)\n",hsv->h,hsv->s,hsv->v);
		}
	}
	SDL_UnlockSurface(src);
	if(tmp){
		SDL_FreeSurface(tmp);
	}
	return dst;
}
SDL_Surface* HSV_ConvertSurfaceToRGB(HSV_Surface *src, Uint32 flags){
	SDL_Surface* dst = SDL_CreateRGBSurface(flags, src->w, src->h, 24, 0x0000ff, 0x00ff00, 0xff0000, 0x00);
	if(!dst){
		fprintf(stderr,"[HSV_ConvertSurfaceToRGB] Error has occured in allocating a RGB Surface.\n");
		exit(0);
	}
	//操作のためにロック
	SDL_LockSurface(dst);
	//ループのための変数指定
	int x,y;
	int max_w = dst->w;
	int max_h = dst->h;
	int bpp = dst->format->BytesPerPixel;
	Uint8 *pixels = (Uint8*)dst->pixels;
	Uint16 pitch = dst->pitch;

	HSV_Color *hsv;
	//ループ内で使う変数の指定
	Uint8 r,g,b;
	int i;
	float f,v,p,q,t;
	float tmp;
	//ループ
	for(y=0;y<max_h;y++){
		for(x=0;x<max_w;x++){
			hsv = &src->pix[y*max_w+x];
			tmp = hsv->h/60.0f;
			i = (int)(tmp) % 6;
			f = tmp-i;
			v = hsv->v;
			p = v*(1-hsv->s);
			q = v*(1-f*hsv->s);
			t = v*(1-(1-f)*hsv->s);
			switch(i){
			case 0:
				r = v*256;
				g = t*256;
				b = p*256;
				break;
			case 1:
				r = q*256;
				g = v*256;
				b = t*256;
				break;
			case 2:
				r = p*256;
				g = v*256;
				b = t*256;
				break;
			case 3:
				r = p*256;
				g = q*256;
				b = v*256;
				break;
			case 4:
				r = t*256;
				g = p*256;
				b = v*256;
				break;
			case 5:
				r = v*256;
				g = p*256;
				b = q*256;
				break;
			default:
				r = g = b = 0;
				break;
			}
			//printf("out rgb:(%d,%d,%d)\n",r,g,b);
			*(Uint32*)&pixels[y*pitch + x*bpp] = SDL_MapRGB(dst->format, r, g, b);
		}
	}
	SDL_UnlockSurface(dst);
	return dst;
}
void HSV_FreeSurface(HSV_Surface *surf){
	free(surf->pix);
	free(surf);
}

HSV_Color* HSV_getColor(HSV_Surface* surface,int x,int y){
	if(x < 0 || x >= surface->w || y < 0 || y >= surface->h){
		return 0;
	}
	return &surface->pix[y*surface->w+x];
}

