#ifndef __TEXT_RENDERER_H__
#define __TEXT_RENDERER_H__
#include <common.h>



typedef struct _RENDERED_TEXT* RenderedText;
typedef struct _FONT* Font;



struct _FONT_CHAR_INFO{
	float x0;
	float y0;
	float x1;
	float y1;
	float ox;
	float oy;
	float sx;
	float sy;
	float a;
};



struct _FONT{
	uint16_t sz;
	uint32_t tx_w;
	uint32_t tx_h;
	uint16_t* c_m;
	struct _FONT_CHAR_INFO* c_dt;
	ID3D11ShaderResourceView* sr;
	ID3D11SamplerState* ss;
};



struct _RENDERED_TEXT{
	ID3D11Buffer* ib;
	ID3D11Buffer* vb;
	uint32_t ill;
};



extern ID3D11VertexShader* txt_vs;
extern ID3D11PixelShader* txt_ps;
extern ID3D11InputLayout* txt_vl;



Font create_font(char* nm,uint16_t sz);



RenderedText render_text(Font f,char* s,float x,float y);



void draw_text(Font f,RenderedText t);



void free_text(RenderedText t);



#endif
