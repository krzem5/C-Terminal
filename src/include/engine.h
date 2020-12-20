#ifndef __ENGINE_H__
#define __ENGINE_H__
#include <common.h>
#include <renderer.h>



typedef struct _CBUFFER_LAYOUT CBufferLayout;
typedef struct _CBUFFER_EXTRA_LAYOUT CBufferExtraLayout;



struct _CBUFFER_LAYOUT{
	RawMatrix pm;
};



struct _CBUFFER_EXTRA_LAYOUT{
	RawMatrix wm;
};



extern ID3D11VertexShader* cl_vs;
extern ID3D11PixelShader* cl_ps;
extern ID3D11InputLayout* cl_vl;
extern ID3D11VertexShader* tx_vs;
extern ID3D11PixelShader* tx_ps;
extern ID3D11InputLayout* tx_vl;



void init_engine(void);



void update_engine(double dt);



#endif
