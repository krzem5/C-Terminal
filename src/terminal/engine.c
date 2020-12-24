#include <common.h>
#include <engine.h>
#include <renderer.h>
#include <text_renderer.h>
#include <stdio.h>



struct CBufferLayout{
	RawMatrix pm;
};



ID3D11Buffer* cb=NULL;
Font tmp_f=NULL;
RenderedText tmp=NULL;



void init_engine(void){
	SetConsoleOutputCP(CP_UTF8);
	cb=create_constant_buffer(sizeof(CBufferLayout));
	tmp_f=create_font("Consolas",128);
	tmp=render_text(tmp_f,"The quick brown fox jumped over the lazy dog.",32,0,0);// 💤€𐐷
	update_size();
}



void update_engine(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	if (renderer_w==NULL){
		return;
	}
	float bf[]={0,0,0,0};
	ID3D11DeviceContext_OMSetBlendState(renderer_d3_dc,renderer_d3_bse,bf,0xffffffff);
	ID3D11DeviceContext_OMSetDepthStencilState(renderer_d3_dc,renderer_d3_ddss,1);
	draw_text(tmp_f,tmp);
	SetCursor(LoadCursorW(NULL,IDC_ARROW));
}



void update_size(void){
	#define _near (0.1f)
	#define _far (1000.0f)
	CBufferLayout cb_dt={
		raw_matrix(2.0f/renderer_ww,0,0,0,0,-2.0f/renderer_wh,0,0,0,0,1/(_far-_near),_near/(_near-_far),-1,1,0,1)
	};
	update_constant_buffer(cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
}
