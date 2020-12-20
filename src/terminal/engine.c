#include <common.h>
#include <engine.h>
#include <renderer.h>
#include <texture.h>
#include <consolas_font.h>
#include <texture_tint_2d_vertex.h>
#include <texture_tint_2d_pixel.h>
#include <stdio.h>



struct CBufferLayout{
	RawMatrix pm;
};



ID3D11Buffer* cb=NULL;
ID3D11VertexShader* tx_vs=NULL;
ID3D11PixelShader* tx_ps=NULL;
ID3D11InputLayout* tx_vl=NULL;
ID3D11SamplerState* tx_ss=NULL;
ID3D11ShaderResourceView* tx_sr=NULL;
ID3D11Buffer* TEMP_ib=NULL;
ID3D11Buffer* TEMP_vb=NULL;



void init_engine(void){
	cb=create_constant_buffer(sizeof(CBufferLayout));
	#define _near (0.1f)
	#define _far (1000.0f)
	CBufferLayout cb_dt={
		raw_matrix(2.0f/renderer_ww,0,0,0,0,-2.0f/renderer_wh,0,0,0,0,1/(_far-_near),_near/(_near-_far),-1,1,0,1)
	};
	update_constant_buffer(cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
}



void update_engine(double dt){
	if (renderer_wf==false){
		return;
	}
	static float t=0;
	t+=(float)(dt*1e-6);
	if (renderer_w==NULL){
		return;
	}
	if (tx_vs==NULL||tx_vl==NULL||tx_ps==NULL||tx_sr==NULL||tx_ss==NULL){
		assert(tx_vs==NULL);
		assert(tx_vl==NULL);
		assert(tx_ps==NULL);
		assert(tx_sr==NULL);
		assert(tx_ss==NULL);
		D3D11_INPUT_ELEMENT_DESC tx_il[]={
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
		tx_vs=load_vertex_shader(g_texture_tint_2d_vs,sizeof(g_texture_tint_2d_vs),tx_il,sizeof(tx_il)/sizeof(D3D11_INPUT_ELEMENT_DESC),&tx_vl);
		tx_ps=load_pixel_shader(g_texture_tint_2d_ps,sizeof(g_texture_tint_2d_ps));
		D3D11_TEXTURE2D_DESC d={
			CONSOLAS_FONT_TEX.w,
			CONSOLAS_FONT_TEX.h,
			1,
			1,
			CONSOLAS_FONT_TEX.f,
			{
				1,
				0
			},
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0
		};
		D3D11_SUBRESOURCE_DATA sd={
			CONSOLAS_FONT_TEX.dt,
			CONSOLAS_FONT_TEX.p,
			CONSOLAS_FONT_TEX.sp
		};
		ID3D11Texture2D* t=NULL;
		assert(ID3D11Device_CreateTexture2D(renderer_d3_d,&d,&sd,&t)==S_OK);
		D3D11_SHADER_RESOURCE_VIEW_DESC rvd={
			CONSOLAS_FONT_TEX.f,
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
		assert(ID3D11Device_CreateShaderResourceView(renderer_d3_d,tr,&rvd,&tx_sr)==S_OK);
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
		assert(ID3D11Device_CreateSamplerState(renderer_d3_d,&ss_d,&tx_ss)==S_OK);
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		/*****/
		float* TEMP_vl=malloc(4*7*sizeof(float));
		*(TEMP_vl+0)=0;
		*(TEMP_vl+1)=0;
		*(TEMP_vl+2)=0;
		*(TEMP_vl+3)=0;
		*(TEMP_vl+4)=0;
		*(TEMP_vl+5)=1;
		*(TEMP_vl+6)=0;

		*(TEMP_vl+7)=0;
		*(TEMP_vl+8)=100;
		*(TEMP_vl+9)=0;
		*(TEMP_vl+10)=1;
		*(TEMP_vl+11)=0;
		*(TEMP_vl+12)=1;
		*(TEMP_vl+13)=0;

		*(TEMP_vl+14)=100;
		*(TEMP_vl+15)=100;
		*(TEMP_vl+16)=1;
		*(TEMP_vl+17)=1;
		*(TEMP_vl+18)=0;
		*(TEMP_vl+19)=1;
		*(TEMP_vl+20)=0;

		*(TEMP_vl+21)=100;
		*(TEMP_vl+22)=0;
		*(TEMP_vl+23)=1;
		*(TEMP_vl+24)=0;
		*(TEMP_vl+25)=0;
		*(TEMP_vl+26)=1;
		*(TEMP_vl+27)=0;
		uint16_t* TEMP_il=malloc(6*sizeof(uint16_t));
		*(TEMP_il+0)=2;
		*(TEMP_il+1)=1;
		*(TEMP_il+2)=0;
		*(TEMP_il+3)=3;
		*(TEMP_il+4)=2;
		*(TEMP_il+5)=0;
		/*****/
		D3D11_BUFFER_DESC bd={
			(uint32_t)(6*sizeof(uint16_t)),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_INDEX_BUFFER,
			0,
			0,
			0
		};
		D3D11_SUBRESOURCE_DATA dt={
			TEMP_il,
			0,
			0
		};
		HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&TEMP_ib);
		free(TEMP_il);
		bd.ByteWidth=(uint32_t)(4*7*sizeof(float));
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		dt.pSysMem=TEMP_vl;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&TEMP_vb);
		free(TEMP_vl);
		/*****/
	}
	float bf[]={0,0,0,0};
	unsigned int st=7*sizeof(float);
	unsigned int off=0;
	ID3D11DeviceContext_OMSetBlendState(renderer_d3_dc,renderer_d3_bse,bf,0xffffffff);
	ID3D11DeviceContext_OMSetDepthStencilState(renderer_d3_dc,renderer_d3_ddss,1);
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,tx_vl);
	ID3D11DeviceContext_PSSetSamplers(renderer_d3_dc,0,1,&tx_ss);
	ID3D11DeviceContext_PSSetShaderResources(renderer_d3_dc,0,1,&tx_sr);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,tx_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,tx_ps,NULL,0);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&TEMP_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,TEMP_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,6,0,0);
}
