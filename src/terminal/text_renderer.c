#include <common.h>
#include <text_renderer.h>
#include <renderer.h>
#include <texture_tint_2d_vertex.h>
#include <texture_tint_2d_pixel.h>
#include <stdio.h>



#define UTF8_ENCODE_CHARACTER(a) (unsigned int)(a<128?a:0xc080|((a&0x7c0)<<2)|(a&0x3f))
#define UTF8_DECODE_CHARACTER(a,b) (uint32_t)(((a&0x1f)<<6)|(b&0x3f))
#define UTF8_DECODE_CHARACTER_3BYTE(a,b,c) (uint32_t)(((a&0xf)<<10)|((b&0x3f)<<6)|(c&0x3f))
#define UTF8_DECODE_CHARACTER_4BYTE(a,b,c,d) (uint32_t)(((a&0x7)<<18)|((b&0x3f)<<12)|((c&0x3f)<<6)|(d&0x3f))
// #define ANSI_ENCODE_CHARACTER(a) (uint32_t)(a<0x10000?a:0xd800dc00|((a-0x10000)&0x7ff03ff))
#define ANSI_ENCODE_CHARACTER(a) (uint32_t)(a<0x10000?a:((((a-0x10000)>>10)|0xd800)<<16)|(((a-0x10000)&0x3ff)|0xdc00))



#define MAX_CHAR 0x10ffff
#define PACKER_MARGIN 5
#define PACKER_SIZE_TO_WIDTH 8
#define PACKER_N_OF_CHARS_TO_WIDTH 32



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
	HFONT f=CreateFontA(sz,0,0,0,FW_NORMAL,false,false,false,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,nm);
	assert(f!=NULL);
	HGDIOBJ fo=SelectObject(dc,f);
	unsigned int otm_sz=GetOutlineTextMetrics(dc,0,NULL);
	assert(otm_sz!=0);
	OUTLINETEXTMETRICW* otm=malloc(otm_sz);
	GetOutlineTextMetricsW(dc,otm_sz,otm);
	o->a=((float)otm->otmTextMetrics.tmAscent)/sz;
	o->d=((float)otm->otmTextMetrics.tmDescent)/sz;
	o->lg=((float)otm->otmLineGap)/sz;
	free(otm);
	struct _GLYPH_INFO* gl=calloc(sizeof(struct _GLYPH_INFO),MAX_CHAR);
	/******************************************************************************************************************/
	#define N_OF_CHARS (255-32+1)
	for (size_t i=32;i<=255;i++){
		(gl+i)->dt=SIZE_MAX;
	}
	/******************************************************************************************************************/
	uint8_t* g_dt=NULL;
	size_t g_dt_sz=0;
	o->c_m=malloc(MAX_CHAR*sizeof(uint32_t));
	for (uint32_t i=0;true;i++){
		*(o->c_m+i)=UINT32_MAX;
		if (i==MAX_CHAR-1){
			break;
		}
	}
	o->c_dt=calloc(sizeof(struct _FONT_CHAR_INFO),N_OF_CHARS);
	uint32_t* scl=calloc(sizeof(struct _FONT_CHAR_INFO),N_OF_CHARS);
	uint32_t scl_l=0;
	uint32_t j=0;
	for (uint32_t i=0;true;i++){
		if ((gl+i)->dt==SIZE_MAX){
			*(o->c_m+i)=j;
			GLYPHMETRICS gm={0};
			uint32_t r=GetGlyphOutlineW(dc,ANSI_ENCODE_CHARACTER(i),GGO_METRICS,&gm,0,NULL,&tm);
			if (r!=GDI_ERROR){
				(gl+i)->dt=g_dt_sz+1;
				(o->c_dt+j)->ox=(float)gm.gmptGlyphOrigin.x/sz;
				(o->c_dt+j)->oy=o->a-(float)gm.gmptGlyphOrigin.y/sz;
				(o->c_dt+j)->sx=(float)gm.gmBlackBoxX/sz;
				(o->c_dt+j)->sy=(float)gm.gmBlackBoxY/sz;
				(o->c_dt+j)->a=(float)gm.gmCellIncX/sz;
				(o->c_dt+j)->ks=UINT32_MAX;
				if (gm.gmBlackBoxX&&gm.gmBlackBoxY){
					r=GetGlyphOutlineW(dc,ANSI_ENCODE_CHARACTER(i),GGO_GRAY8_BITMAP,&gm,0,NULL,&tm);
					if (r>0&&r!=GDI_ERROR){
						g_dt=realloc(g_dt,g_dt_sz+r);
						(gl+i)->w=gm.gmBlackBoxX;
						(gl+i)->h=gm.gmBlackBoxY;
						scl_l++;
						for (uint32_t k=0;true;k++){
							if (k==scl_l-1){
								scl[k]=i;
								break;
							}
							if ((gl+scl[k])->h<(gl+i)->h){
								for (uint32_t l=scl_l-1;l>k;l--){
									scl[l]=scl[l-1];
								}
								scl[k]=i;
								break;
							}
							if (k==UINT32_MAX-1){
								assert(0);
							}
						}
						uint32_t gr=GetGlyphOutlineW(dc,ANSI_ENCODE_CHARACTER(i),GGO_GRAY8_BITMAP,&gm,r,g_dt+g_dt_sz,&tm);
						g_dt_sz+=r;
						if (gr==0||gr==GDI_ERROR){
							assert(0);
						}
					}
				}
			}
			j++;
		}
		if (i>=MAX_CHAR-1){
			break;
		}
	}
	o->kpl=GetKerningPairsA(dc,0,NULL);
	if (o->kpl>0){
		o->kp=malloc(o->kpl*sizeof(struct _FONT_KERNING_PAIR));
		KERNINGPAIR* bf=malloc(o->kpl*sizeof(KERNINGPAIR));
		assert(GetKerningPairsA(dc,o->kpl,bf)==o->kpl);
		uint32_t j=0;
		for (uint32_t i=0;i<o->kpl;i++){
			if ((float)(bf+i)->iKernAmount!=0){
				uint8_t f=0;
				if ((gl+(bf+i)->wFirst)->dt!=0){
					f++;
				}
				if ((gl+(bf+i)->wSecond)->dt!=0){
					f++;
				}
				if (f!=2){
					continue;
				}
				j++;
				for (uint32_t k=0;true;k++){
					if (k==j-1||(o->kp+k)->a>(bf+i)->wFirst){
						if (k!=j-1){
							for (uint32_t l=j;l>k;l--){
								(o->kp+l)->a=(o->kp+l-1)->a;
								(o->kp+l)->b=(o->kp+l-1)->b;
								(o->kp+l)->v=(o->kp+l-1)->v;
							}
						}
						(o->kp+k)->a=(bf+i)->wFirst;
						(o->kp+k)->b=(bf+i)->wSecond;
						(o->kp+k)->v=((float)(bf+i)->iKernAmount)/sz;
						break;
					}
					if ((o->kp+k)->a==(bf+i)->wFirst){
						if ((o->kp+k)->b<(bf+i)->wSecond){
							continue;
						}
						if ((o->kp+k)->b==(bf+i)->wSecond){
							j--;
							if ((o->kp+k)->v!=((float)(bf+i)->iKernAmount)/sz){
								printf("WARNING: Font '%s' Contains Multiple Diffrent Kernings for Character Pair %lu + %lu: %.2f%% and %.2f%%. The First One will be Used.\n",nm,(o->kp+k)->a,(o->kp+k)->b,(o->kp+k)->v*100,((float)(bf+i)->iKernAmount)/sz*100);
							}
							break;
						}
						for (uint32_t l=j;l>k;l--){
							(o->kp+l)->a=(o->kp+l-1)->a;
							(o->kp+l)->b=(o->kp+l-1)->b;
							(o->kp+l)->v=(o->kp+l-1)->v;
						}
						(o->kp+k)->a=(bf+i)->wFirst;
						(o->kp+k)->b=(bf+i)->wSecond;
						(o->kp+k)->v=((float)(bf+i)->iKernAmount)/sz;
						break;
					}
				}
			}
		}
		if (j!=o->kpl){
			o->kp=realloc(o->kp,j*sizeof(struct _FONT_KERNING_PAIR));
			uint32_t t=UINT32_MAX;
			for (uint32_t k=0;k<j;k++){
				if ((o->kp+k)->a!=t){
					t=(o->kp+k)->a;
					(o->c_dt+*(o->c_m+(o->kp+k)->a))->ks=k;
				}
			}
			o->kpl=j;
		}
		free(bf);
	}
	SelectObject(dc,fo);
	DeleteObject(f);
	DeleteDC(dc);
	o->tx_w=sz*PACKER_SIZE_TO_WIDTH+N_OF_CHARS/PACKER_N_OF_CHARS_TO_WIDTH*PACKER_SIZE_TO_WIDTH;
	o->tx_h=PACKER_MARGIN;
	uint32_t cx=PACKER_MARGIN;
	uint32_t cy=PACKER_MARGIN;
	for (uint32_t i=0;true;i++){
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
	for (uint32_t i=0;true;i++){
		uint32_t gr=((gl+scl[i])->w+3)/4*4;
		for (uint32_t j=0;j<(gl+scl[i])->h;j++){
			for (uint32_t k=0;k<(gl+scl[i])->w;k++){
				uint8_t v=*(g_dt+(gl+scl[i])->dt+j*gr+k-1);
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



RenderedText render_text(Font f,char* s,uint16_t sz,float x,float y){
	RenderedText o=malloc(sizeof(struct _RENDERED_TEXT));
	uint32_t* ns=NULL;
	uint32_t nsl=0;
	uint32_t cc=0;
	for (uint32_t i=0;true;i++){
		nsl++;
		ns=realloc(ns,nsl*sizeof(uint32_t));
		*(ns+nsl-1)=*(s+i);
		if ((*(s+i)&0xf8)==0xf0){
			*(ns+nsl-1)=UTF8_DECODE_CHARACTER_4BYTE(*(s+i),*(s+i+1),*(s+i+2),*(s+i+3));
			i+=3;
		}
		else if ((*(s+i)&0xf0)==0xe0){
			*(ns+nsl-1)=UTF8_DECODE_CHARACTER_3BYTE(*(s+i),*(s+i+1),*(s+i+2));
			i+=2;
		}
		else if ((*(s+i)&0xe0)==0xc0){
			*(ns+nsl-1)=UTF8_DECODE_CHARACTER(*(s+i),*(s+i+1));
			i++;
		}
		if (*(s+i)==0){
			break;
		}
		if (*(ns+nsl-1)!=' '){
			cc++;
		}
		if (*(f->c_m+*(ns+nsl-1))==UINT32_MAX){
			printf("WARNING: Ingoring Unmapped Character: %lu\n",*(ns+nsl-1));
			nsl--;
		}
		// CHECK ALL CHAR_CODE RANGES
	}
	uint32_t* bns=ns;
	float* vl=malloc(cc*28*sizeof(float));
	uint16_t* il=malloc(cc*6*sizeof(uint16_t));
	uint32_t i=0;
	while (*ns!=0){
		#define COLOR_R 0.85f
		#define COLOR_G 0.85f
		#define COLOR_B 0.85f
		struct _FONT_CHAR_INFO* c=f->c_dt+*(f->c_m+*ns);
		if (c->x0!=c->x1){
			*(vl+i*28)=x+c->ox*sz;
			*(vl+i*28+1)=y+c->oy*sz;
			*(vl+i*28+2)=c->x0;
			*(vl+i*28+3)=c->y0;
			*(vl+i*28+4)=COLOR_R;
			*(vl+i*28+5)=COLOR_G;
			*(vl+i*28+6)=COLOR_B;
			*(vl+i*28+7)=x+c->ox*sz;
			*(vl+i*28+8)=y+c->oy*sz+c->sy*sz;
			*(vl+i*28+9)=c->x0;
			*(vl+i*28+10)=c->y1;
			*(vl+i*28+11)=COLOR_R;
			*(vl+i*28+12)=COLOR_G;
			*(vl+i*28+13)=COLOR_B;
			*(vl+i*28+14)=x+c->ox*sz+c->sx*sz;
			*(vl+i*28+15)=y+c->oy*sz+c->sy*sz;
			*(vl+i*28+16)=c->x1;
			*(vl+i*28+17)=c->y1;
			*(vl+i*28+18)=COLOR_R;
			*(vl+i*28+19)=COLOR_G;
			*(vl+i*28+20)=COLOR_B;
			*(vl+i*28+21)=x+c->ox*sz+c->sx*sz;
			*(vl+i*28+22)=y+c->oy*sz;
			*(vl+i*28+23)=c->x1;
			*(vl+i*28+24)=c->y0;
			*(vl+i*28+25)=COLOR_R;
			*(vl+i*28+26)=COLOR_G;
			*(vl+i*28+27)=COLOR_B;
			// if (0){*(vl+i*28)=0;*(vl+i*28+1)=0;*(vl+i*28+2)=0;*(vl+i*28+3)=0;*(vl+i*28+4)=COLOR_R;*(vl+i*28+5)=COLOR_G;*(vl+i*28+6)=COLOR_B;*(vl+i*28+7)=0;*(vl+i*28+8)=(float)f->tx_h;*(vl+i*28+9)=0;*(vl+i*28+10)=1;*(vl+i*28+11)=COLOR_R;*(vl+i*28+12)=COLOR_G;*(vl+i*28+13)=COLOR_B;*(vl+i*28+14)=(float)f->tx_w;*(vl+i*28+15)=(float)f->tx_h;*(vl+i*28+16)=1;*(vl+i*28+17)=1;*(vl+i*28+18)=COLOR_R;*(vl+i*28+19)=COLOR_G;*(vl+i*28+20)=COLOR_B;*(vl+i*28+21)=(float)f->tx_w;*(vl+i*28+22)=0;*(vl+i*28+23)=1;*(vl+i*28+24)=0;*(vl+i*28+25)=COLOR_R;*(vl+i*28+26)=COLOR_G;*(vl+i*28+27)=COLOR_B;}
			*(il+i*6)=i*4+2;
			*(il+i*6+1)=i*4+1;
			*(il+i*6+2)=i*4;
			*(il+i*6+3)=i*4+3;
			*(il+i*6+4)=i*4+2;
			*(il+i*6+5)=i*4;
			i++;
		}
		x+=c->a*sz;
		if (c->ks!=UINT32_MAX&&*(ns+1)!=0){
			for (uint32_t j=c->ks;j<f->kpl&&(f->kp+j)->a==*ns;j++){
				if ((f->kp+j)->b==*(ns+1)){
					x+=(f->kp+j)->v*sz;
					break;
				}
				if ((f->kp+j)->b>*(ns+1)){
					break;
				}
			}
		}
		ns++;
	}
	free(bns);
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
