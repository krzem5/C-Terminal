#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <common.h>



#define TEXTURE_X0(m,t) (m[t].x0)
#define TEXTURE_Y0(m,t) (m[t].y0)
#define TEXTURE_X1(m,t) (m[t].x1)
#define TEXTURE_Y1(m,t) (m[t].y1)



typedef struct _RAW_TEXTURE RawTexture;
typedef ID3D11ShaderResourceView* Texture;
typedef struct _TEXTURE_MAP_DATA TextureMapData;



struct _RAW_TEXTURE{
	uint32_t f;
	uint32_t w;
	uint32_t h;
	uint32_t p;
	uint32_t sp;
	const uint8_t* dt;
};



struct _TEXTURE_MAP_DATA{
	float x0;
	float y0;
	float x1;
	float y1;
};



Texture create_texture(RawTexture r);



#endif
