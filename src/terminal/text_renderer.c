#include <common.h>
#include <text_renderer.h>
#include <renderer.h>
#include <texture_tint_2d_vertex.h>
#include <texture_tint_2d_pixel.h>
#include <stdio.h>



#define MAX_CHAR 0x10000
#define PACKER_MARGIN 5
#define PACKER_SIZE_TO_WIDTH 8



const MAT2 tm={{0,1},{0,0},{0,0},{0,1}};



struct _GLYPH_INFO{
	size_t dt;
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
};



ID3D11VertexShader* txt_vs=NULL;
ID3D11PixelShader* txt_ps=NULL;
ID3D11InputLayout* txt_vl=NULL;



Font create_font(char* nm,uint16_t sz){
	Font o=malloc(sizeof(struct _FONT));
	o->sz=sz;
	HDC dc=CreateCompatibleDC(NULL);
	assert(dc!=NULL);
	float i_sz=1.0f/(float)sz;
	HFONT f=CreateFontA(sz,0,0,0,400,false,false,false,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,nm);
	assert(f!=NULL);
	HGDIOBJ fo=SelectObject(dc,f);
	unsigned int otm_sz=GetOutlineTextMetrics(dc,0,NULL);
	assert(otm_sz!=0);
	OUTLINETEXTMETRICA* otm=malloc(otm_sz);
	GetOutlineTextMetricsA(dc,otm_sz,otm);
	uint32_t TMP_ascent=otm->otmTextMetrics.tmAscent;
	uint32_t TMP_descent=otm->otmTextMetrics.tmDescent;
	float TMP_line_gap=otm->otmLineGap*i_sz;
	free(otm);
	struct _GLYPH_INFO* gl=calloc(sizeof(struct _GLYPH_INFO),MAX_CHAR);
	/***/
	#define N_OF_CHARS (127-32+1)
	for (size_t i=32;i<=127;i++){
		(gl+i)->dt=SIZE_MAX;
	}
	/***/
	uint8_t* g_dt=NULL;
	size_t g_dt_sz=0;
	o->c_m=calloc(sizeof(uint16_t),MAX_CHAR);
	o->c_dt=calloc(sizeof(struct _FONT_CHAR_INFO),N_OF_CHARS);
	uint16_t* scl=calloc(sizeof(struct _FONT_CHAR_INFO),N_OF_CHARS);
	uint32_t scl_l=0;
	uint16_t j=0;
	for (uint16_t i=0;true;i++){
		if ((gl+i)->dt==SIZE_MAX){
			*(o->c_m+i)=j;
			GLYPHMETRICS gm={0};
			uint32_t r=GetGlyphOutlineA(dc,(unsigned int)i,GGO_METRICS,&gm,0,NULL,&tm);
			if (r!=GDI_ERROR){
				(gl+i)->dt=g_dt_sz;
				(o->c_dt+j)->ox=(float)gm.gmptGlyphOrigin.x*i_sz;
				(o->c_dt+j)->oy=(float)(TMP_ascent-gm.gmptGlyphOrigin.y)*i_sz;
				(o->c_dt+j)->sx=(float)gm.gmBlackBoxX*i_sz;
				(o->c_dt+j)->sy=(float)gm.gmBlackBoxY*i_sz;
				(o->c_dt+j)->a=(float)gm.gmCellIncX*i_sz;
				if (gm.gmBlackBoxX&&gm.gmBlackBoxY){
					r=GetGlyphOutlineA(dc,(unsigned int)i,GGO_GRAY8_BITMAP,&gm,0,NULL,&tm);
					if (r>0&&r!=GDI_ERROR){
						g_dt=realloc(g_dt,g_dt_sz+r);
						(gl+i)->w=gm.gmBlackBoxX;
						(gl+i)->h=gm.gmBlackBoxY;
						scl_l++;
						for (uint16_t k=0;true;k++){
							if (k==scl_l-1){
								scl[k]=i;
								break;
							}
							if (gl[scl[k]].h<(gl+i)->h){
								for (uint32_t l=scl_l-1;l>k;l--){
									scl[l]=scl[l-1];
								}
								scl[k]=i;
								break;
							}
							if (k==UINT16_MAX-1){
								assert(0);
							}
						}
						uint32_t gr=GetGlyphOutlineA(dc,(unsigned int)i,GGO_GRAY8_BITMAP,&gm,r,g_dt+g_dt_sz,&tm);
						g_dt_sz+=r;
						if (gr==0||gr==GDI_ERROR){
							assert(0);
						}
					}
				}
			}
			j++;
		}
		if (i==MAX_CHAR-1){
			break;
		}
	}
	SelectObject(dc,fo);
	DeleteObject(f);
	DeleteDC(dc);
	o->tx_w=(sz*PACKER_SIZE_TO_WIDTH+3)/4*4;
	o->tx_h=PACKER_MARGIN;
	uint32_t cx=PACKER_MARGIN;
	uint32_t cy=PACKER_MARGIN;
	for (uint16_t i=0;true;i++){
		struct _GLYPH_INFO* g=gl+scl[i];
		assert(g->w+2*PACKER_MARGIN<=o->tx_w);
		if (cx+g->w+PACKER_MARGIN>o->tx_w){
			cx=PACKER_MARGIN;
			cy=o->tx_h;
		}
		g->x=cx;
		g->y=cy;
		if (cy+g->h+PACKER_MARGIN>o->tx_h){
			o->tx_h=cy+g->h+PACKER_MARGIN;
		}
		cx+=g->w+PACKER_MARGIN;
		if (i==scl_l-1){
			break;
		}
	}
	float itx_w=1;
	float itx_h=1;
	uint8_t* tx_dt=calloc(sizeof(uint8_t),o->tx_w*o->tx_h);
	for (uint16_t i=0;true;i++){
		uint32_t glyphDataRowPitch=((gl+scl[i])->w+3)/4*4;
		for (uint32_t j=0;j<(gl+scl[i])->h;j++){
			for (uint32_t k=0;k<(gl+scl[i])->w;k++){
				uint8_t v=*(g_dt+(gl+scl[i])->dt+j*glyphDataRowPitch+k);
				*(tx_dt+(gl+scl[i])->y*o->tx_w+(gl+scl[i])->x+j*o->tx_w+k)=(v>=64?255:v*4);
			}
		}
		struct _FONT_CHAR_INFO* c=o->c_dt+*(o->c_m+scl[i]);
		c->x0=((float)(gl+scl[i])->x)/o->tx_w;
		c->y0=((float)(gl+scl[i])->y)/o->tx_h;
		c->x1=((float)(gl+scl[i])->w)/o->tx_w+c->x0;
		c->y1=((float)(gl+scl[i])->h)/o->tx_h+c->y0;
		if (i==scl_l-1){
			break;
		}
	}
	free(scl);
	free(gl);
	D3D11_TEXTURE2D_DESC d={
		o->tx_w,
		o->tx_h,
		1,
		1,
		DXGI_FORMAT_R8_UNORM,
		{
			1,
			0
		},
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA sd={
		(void*)tx_dt,
		o->tx_w,
		0
	};
	ID3D11Texture2D* t=NULL;
	assert(ID3D11Device_CreateTexture2D(renderer_d3_d,&d,&sd,&t)==S_OK);
	free(tx_dt);
	D3D11_SHADER_RESOURCE_VIEW_DESC rvd={
		DXGI_FORMAT_R8_UNORM,
		D3D_SRV_DIMENSION_TEXTURE2D,
		{
			.Texture2D={
				0,
				1
			}
		}
	};
	ID3D11Resource* tr;
	ID3D11Texture2D_QueryInterface(t,&IID_ID3D11Resource,(void**)&tr);
	assert(ID3D11Device_CreateShaderResourceView(renderer_d3_d,tr,&rvd,&(o->sr))==S_OK);
	IUnknown_Release(tr);
	D3D11_SAMPLER_DESC ss_d={
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0,
		1,
		D3D11_COMPARISON_ALWAYS,
		{
			0,
			0,
			0,
			0
		},
		0,
		D3D11_FLOAT32_MAX
	};
	assert(ID3D11Device_CreateSamplerState(renderer_d3_d,&ss_d,&(o->ss))==S_OK);
	return o;
}



RenderedText render_text(Font f,char* s,float x,float y){
	RenderedText o=malloc(sizeof(struct _RENDERED_TEXT));
	uint32_t cc=0;
	for (uint32_t i=0;*(s+i)!=0;i++){
		if (*(s+i)!=' '){
			cc++;
		}
	}
	float* vl=malloc(cc*28*sizeof(float));
	uint16_t* il=malloc(cc*6*sizeof(uint16_t));
	uint32_t i=0;
	while (*s!=0){
		#define FONT_SIZE f->sz
		struct _FONT_CHAR_INFO* c=f->c_dt+*(f->c_m+*s);
		if (c->x0!=c->x1){
			*(vl+i*28)=x+c->ox*FONT_SIZE;
			*(vl+i*28+1)=y+c->oy*FONT_SIZE;
			*(vl+i*28+2)=c->x0;
			*(vl+i*28+3)=c->y0;
			*(vl+i*28+4)=0;
			*(vl+i*28+5)=1;
			*(vl+i*28+6)=0;
			*(vl+i*28+7)=x+c->ox*FONT_SIZE;
			*(vl+i*28+8)=y+c->oy*FONT_SIZE+c->sy*FONT_SIZE;
			*(vl+i*28+9)=c->x0;
			*(vl+i*28+10)=c->y1;
			*(vl+i*28+11)=0;
			*(vl+i*28+12)=1;
			*(vl+i*28+13)=0;
			*(vl+i*28+14)=x+c->ox*FONT_SIZE+c->sx*FONT_SIZE;
			*(vl+i*28+15)=y+c->oy*FONT_SIZE+c->sy*FONT_SIZE;
			*(vl+i*28+16)=c->x1;
			*(vl+i*28+17)=c->y1;
			*(vl+i*28+18)=0;
			*(vl+i*28+19)=1;
			*(vl+i*28+20)=0;
			*(vl+i*28+21)=x+c->ox*FONT_SIZE+c->sx*FONT_SIZE;
			*(vl+i*28+22)=y+c->oy*FONT_SIZE;
			*(vl+i*28+23)=c->x1;
			*(vl+i*28+24)=c->y0;
			*(vl+i*28+25)=0;
			*(vl+i*28+26)=1;
			*(vl+i*28+27)=0;
			*(il+i*6)=i*4+2;
			*(il+i*6+1)=i*4+1;
			*(il+i*6+2)=i*4;
			*(il+i*6+3)=i*4+3;
			*(il+i*6+4)=i*4+2;
			*(il+i*6+5)=i*4;
			i++;
		}
		s++;
		x+=c->a*FONT_SIZE;
	}
	D3D11_BUFFER_DESC bd={
		(uint32_t)(cc*6*sizeof(uint16_t)),
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_INDEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		il,
		0,
		0
	};
	HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->ib));
	free(il);
	bd.ByteWidth=(uint32_t)(cc*28*sizeof(float));
	bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	dt.pSysMem=vl;
	hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&(o->vb));
	free(vl);
	o->ill=cc*6;
	return o;
}



void draw_text(Font f,RenderedText t){
	if (txt_vs==NULL||txt_vl==NULL||txt_ps==NULL){
		assert(txt_vs==NULL);
		assert(txt_vl==NULL);
		assert(txt_ps==NULL);
		D3D11_INPUT_ELEMENT_DESC txt_il[]={
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TEXCOORD",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};
		txt_vs=load_vertex_shader(g_texture_tint_2d_vs,sizeof(g_texture_tint_2d_vs),txt_il,sizeof(txt_il)/sizeof(D3D11_INPUT_ELEMENT_DESC),&txt_vl);
		txt_ps=load_pixel_shader(g_texture_tint_2d_ps,sizeof(g_texture_tint_2d_ps));
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,txt_vl);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,txt_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,txt_ps,NULL,0);
	ID3D11DeviceContext_PSSetSamplers(renderer_d3_dc,0,1,&(f->ss));
	ID3D11DeviceContext_PSSetShaderResources(renderer_d3_dc,0,1,&(f->sr));
	unsigned int st=7*sizeof(float);
	unsigned int off=0;
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&(t->vb),&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,t->ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,t->ill,0,0);
}



void free_text(RenderedText t);
