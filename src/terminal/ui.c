#include <renderer.h>
#include <ui.h>
#include <color_2d_pixel.h>
#include <color_2d_vertex.h>
#include <stdio.h>



#define UI_SCALE(v) (((float)(v))*renderer_wsf)
#define TOPBAR_HEIGHT UI_SCALE(30)
#define TOPBAR_R 0.05f
#define TOPBAR_G 0.05f
#define TOPBAR_B 0.05f
#define TOPBAR_WINDOW_OP_WIDTH UI_SCALE(50)
#define TOPBAR_WINDOW_OP_HEIGHT UI_SCALE(30)
#define TOPBAR_WINDOW_OP_R 0.14f
#define TOPBAR_WINDOW_OP_G 0.14f
#define TOPBAR_WINDOW_OP_B 0.14f
#define TOPBAR_WINDOW_CLOSE_R 0.784f
#define TOPBAR_WINDOW_CLOSE_G 0.078f
#define TOPBAR_WINDOW_CLOSE_B 0.118f
#define TOPBAR_WINDOW_OP_STROKE_SZ UI_SCALE(9)
#define TOPBAR_WINDOW_OP_STROKE_R 0.8f
#define TOPBAR_WINDOW_OP_STROKE_G 0.8f
#define TOPBAR_WINDOW_OP_STROKE_B 0.8f
#define TOPBAR_WINDOW_OP_STROKE_LERP_R 0.76f
#define TOPBAR_WINDOW_OP_STROKE_LERP_G 0.76f
#define TOPBAR_WINDOW_OP_STROKE_LERP_B 0.76f
#define TOPBAR_MAXIMIZE_WINDOW_BUFFER (TOPBAR_HEIGHT/2)
#define C_NONE C_APPSTARTING
#define C_APPSTARTING 32650
#define C_ARROW 32512
#define C_CROSS 32515
#define C_HAND 32649
#define C_HELP 32651
#define C_IBEAM 32513
#define C_ICON 32641
#define C_NO 32648
#define C_SIZE 32640
#define C_SIZEALL 32646
#define C_SIZENESW 32643
#define C_SIZENS 32645
#define C_SIZENWSE 32642
#define C_SIZEWE 32644
#define C_UPARROW 32516
#define C_WAIT 32514



bool ui_enabled=true;
ID3D11VertexShader* ui_vs=NULL;
ID3D11PixelShader* ui_ps=NULL;
ID3D11InputLayout* ui_vl=NULL;
float* tb_vl=NULL;
float* tb_lvl=NULL;
ID3D11Buffer* tb_ib=NULL;
ID3D11Buffer* tb_vb=NULL;
ID3D11Buffer* tb_lib=NULL;
ID3D11Buffer* tb_lvb=NULL;
ID3D11Buffer* tb_mxlib=NULL;
float tb_op_a[]={0,0,0};
ID3D11Buffer* ui_cb=NULL;
uint16_t _lc=C_NONE;
uint16_t nc=C_NONE;
bool mx=false;
uint8_t d_tb=0;
uint16_t d_tb_ox=0;
uint16_t d_tb_oy=0;
uint16_t ow=0;
uint16_t oh=0;



struct CBufferLayout{
	RawMatrix pm;
};



void recalc_topbar(void){
	uint8_t u=0;
	if (tb_vl==NULL){
		u|=0x01;
		tb_vl=realloc(tb_vl,96*sizeof(float));
		*(tb_vl)=0;
		*(tb_vl+1)=0;
		*(tb_vl+2)=TOPBAR_R;
		*(tb_vl+3)=TOPBAR_G;
		*(tb_vl+4)=TOPBAR_B;
		*(tb_vl+5)=1;
		*(tb_vl+6)=(float)renderer_ww;
		*(tb_vl+7)=0;
		*(tb_vl+8)=TOPBAR_R;
		*(tb_vl+9)=TOPBAR_G;
		*(tb_vl+10)=TOPBAR_B;
		*(tb_vl+11)=1;
		*(tb_vl+12)=(float)renderer_ww;
		*(tb_vl+13)=(float)TOPBAR_HEIGHT;
		*(tb_vl+14)=TOPBAR_R;
		*(tb_vl+15)=TOPBAR_G;
		*(tb_vl+16)=TOPBAR_B;
		*(tb_vl+17)=1;
		*(tb_vl+18)=0;
		*(tb_vl+19)=(float)TOPBAR_HEIGHT;
		*(tb_vl+20)=TOPBAR_R;
		*(tb_vl+21)=TOPBAR_G;
		*(tb_vl+22)=TOPBAR_B;
		*(tb_vl+23)=1;
		*(tb_vl+24)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*3;
		*(tb_vl+25)=0;
		*(tb_vl+26)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+27)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+28)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+29)=1;
		*(tb_vl+30)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2;
		*(tb_vl+31)=0;
		*(tb_vl+32)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+33)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+34)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+35)=1;
		*(tb_vl+36)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2;
		*(tb_vl+37)=(float)TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+38)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+39)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+40)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+41)=1;
		*(tb_vl+42)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*3;
		*(tb_vl+43)=(float)TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+44)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+45)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+46)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+47)=1;
		*(tb_vl+48)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2;
		*(tb_vl+49)=0;
		*(tb_vl+50)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+51)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+52)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+53)=1;
		*(tb_vl+54)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH;
		*(tb_vl+55)=0;
		*(tb_vl+56)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+57)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+58)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+59)=1;
		*(tb_vl+60)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH;
		*(tb_vl+61)=(float)TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+62)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+63)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+64)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+65)=1;
		*(tb_vl+66)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2;
		*(tb_vl+67)=(float)TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+68)=TOPBAR_WINDOW_OP_R;
		*(tb_vl+69)=TOPBAR_WINDOW_OP_G;
		*(tb_vl+70)=TOPBAR_WINDOW_OP_B;
		*(tb_vl+71)=1;
		*(tb_vl+72)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH;
		*(tb_vl+73)=0;
		*(tb_vl+74)=TOPBAR_WINDOW_CLOSE_R;
		*(tb_vl+75)=TOPBAR_WINDOW_CLOSE_G;
		*(tb_vl+76)=TOPBAR_WINDOW_CLOSE_B;
		*(tb_vl+77)=1;
		*(tb_vl+78)=(float)renderer_ww;
		*(tb_vl+79)=0;
		*(tb_vl+80)=TOPBAR_WINDOW_CLOSE_R;
		*(tb_vl+81)=TOPBAR_WINDOW_CLOSE_G;
		*(tb_vl+82)=TOPBAR_WINDOW_CLOSE_B;
		*(tb_vl+83)=1;
		*(tb_vl+84)=(float)renderer_ww;
		*(tb_vl+85)=TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+86)=TOPBAR_WINDOW_CLOSE_R;
		*(tb_vl+87)=TOPBAR_WINDOW_CLOSE_G;
		*(tb_vl+88)=TOPBAR_WINDOW_CLOSE_B;
		*(tb_vl+89)=1;
		*(tb_vl+90)=(float)renderer_ww-TOPBAR_WINDOW_OP_WIDTH;
		*(tb_vl+91)=TOPBAR_WINDOW_OP_HEIGHT;
		*(tb_vl+92)=TOPBAR_WINDOW_CLOSE_R;
		*(tb_vl+93)=TOPBAR_WINDOW_CLOSE_G;
		*(tb_vl+94)=TOPBAR_WINDOW_CLOSE_B;
		*(tb_vl+95)=1;
	}
	if (tb_lvl==NULL){
		u|=0x02;
		tb_lvl=realloc(tb_lvl,90*sizeof(float));
		*(tb_lvl)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+1)=TOPBAR_WINDOW_OP_HEIGHT*0.5f;
		*(tb_lvl+2)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+3)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+4)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+5)=1;
		*(tb_lvl+6)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*2.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+7)=TOPBAR_WINDOW_OP_HEIGHT*0.5f;
		*(tb_lvl+8)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+9)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+10)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+11)=1;
		*(tb_lvl+12)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+13)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+14)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+15)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+16)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+17)=1;
		*(tb_lvl+18)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+19)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+20)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+21)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+22)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+23)=1;
		*(tb_lvl+24)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+25)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+26)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+27)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+28)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+29)=1;
		*(tb_lvl+30)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+31)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+32)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+33)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+34)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+35)=1;
		*(tb_lvl+36)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+37)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+38)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+39)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+40)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+41)=1;
		*(tb_lvl+42)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+43)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+44)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+45)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+46)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+47)=1;
		*(tb_lvl+48)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+49)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+50)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+51)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+52)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+53)=1;
		*(tb_lvl+54)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+55)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+56)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+57)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+58)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+59)=1;
		*(tb_lvl+60)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.25f;
		*(tb_lvl+61)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+62)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+63)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+64)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+65)=1;
		*(tb_lvl+66)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.25f;
		*(tb_lvl+67)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.75f;
		*(tb_lvl+68)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+69)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+70)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+71)=1;
		*(tb_lvl+72)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.75f;
		*(tb_lvl+73)=TOPBAR_WINDOW_OP_HEIGHT*0.5f-TOPBAR_WINDOW_OP_STROKE_SZ*0.75f;
		*(tb_lvl+74)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+75)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+76)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+77)=1;
		*(tb_lvl+78)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.75f;
		*(tb_lvl+79)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.25f;
		*(tb_lvl+80)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+81)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+82)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+83)=1;
		*(tb_lvl+84)=renderer_ww-TOPBAR_WINDOW_OP_WIDTH*1.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.5f;
		*(tb_lvl+85)=TOPBAR_WINDOW_OP_HEIGHT*0.5f+TOPBAR_WINDOW_OP_STROKE_SZ*0.25f;
		*(tb_lvl+86)=TOPBAR_WINDOW_OP_STROKE_R;
		*(tb_lvl+87)=TOPBAR_WINDOW_OP_STROKE_G;
		*(tb_lvl+88)=TOPBAR_WINDOW_OP_STROKE_B;
		*(tb_lvl+89)=1;
	}
	for (uint8_t i=0;i<3;i++){
		if (ui_enabled==true&&0<=renderer_my&&renderer_my<TOPBAR_WINDOW_OP_HEIGHT&&renderer_ww-TOPBAR_WINDOW_OP_WIDTH*(3-i)<=renderer_mx&&renderer_mx<renderer_ww-TOPBAR_WINDOW_OP_WIDTH*(2-i)){
			// if (renderer_mf&M_LEFT){
			// 	d_tb=2;
			// 	ui_enabled=false;
			// 	if (i==0){
			// 		ShowWindow(renderer_w,SW_MINIMIZE);
			// 	}
			// 	else if (i==1){
			// 		if (mx==false){
			// 			ow=renderer_ww;
			// 			oh=renderer_wh;
			// 		}
			// 		ShowWindow(renderer_w,(mx==false?SW_MAXIMIZE:SW_RESTORE));
			// 		free(tb_vl);
			// 		free(tb_lvl);
			// 		tb_vl=NULL;
			// 		tb_lvl=NULL;
			// 		mx=!mx;
			// 		return;
			// 	}
			// 	else{
			// 		close_window();
			// 		return;
			// 	}
			// }
			// if (*(tb_op_a+i)<1){
			// 	*(tb_op_a+i)+=0.4f;
			// 	if (*(tb_op_a+i)>1){
			// 		*(tb_op_a+i)=1;
			// 	}
			// 	u|=0x01;
			// 	if (i!=1){
			// 		u|=0x02;
			// 	}
			// }
		}
		else if (*(tb_op_a+i)>0){
			*(tb_op_a+i)-=0.1f;
			if (*(tb_op_a+i)<0){
				*(tb_op_a+i)=0;
			}
			u|=0x01;
			if (i!=1){
				u|=0x02;
			}
		}
		*(tb_vl+(i+1)*24+5)=*(tb_op_a+i);
		*(tb_vl+(i+1)*24+11)=*(tb_op_a+i);
		*(tb_vl+(i+1)*24+17)=*(tb_op_a+i);
		*(tb_vl+(i+1)*24+23)=*(tb_op_a+i);
	}
	for (uint8_t i=0;i<6;i++){
		*(tb_lvl+i*6+2)=TOPBAR_WINDOW_OP_STROKE_R+(*(tb_op_a+(i<2?0:2)))*(TOPBAR_WINDOW_OP_STROKE_LERP_R-TOPBAR_WINDOW_OP_STROKE_R);
		*(tb_lvl+i*6+3)=TOPBAR_WINDOW_OP_STROKE_G+(*(tb_op_a+(i<2?0:2)))*(TOPBAR_WINDOW_OP_STROKE_LERP_R-TOPBAR_WINDOW_OP_STROKE_G);
		*(tb_lvl+i*6+4)=TOPBAR_WINDOW_OP_STROKE_B+(*(tb_op_a+(i<2?0:2)))*(TOPBAR_WINDOW_OP_STROKE_LERP_R-TOPBAR_WINDOW_OP_STROKE_B);
	}
	if (renderer_wf==true){
		if (renderer_my>=0&&renderer_my<TOPBAR_WINDOW_OP_HEIGHT){
			nc=C_ARROW;
			if ((renderer_mf&M_LEFT)!=0&&renderer_mx>=0&&renderer_mx<renderer_ww-TOPBAR_WINDOW_OP_WIDTH*3&&d_tb==0){
				if (mx==true){
					mx=false;
					ShowWindow(renderer_w,SW_RESTORE);
					free(tb_vl);
					free(tb_lvl);
					tb_vl=NULL;
					tb_lvl=NULL;
					POINT mp;
					GetCursorPos(&mp);
					MoveWindow(renderer_w,mp.x-ow/2,0,ow,oh,false);
					d_tb_ox=(uint16_t)ow/2;
					d_tb_oy=(uint16_t)mp.y;
					d_tb=1;
					return;
				}
				d_tb_ox=renderer_mx;
				d_tb_oy=renderer_my;
				d_tb=1;
			}
		}
		if ((renderer_mf&M_LEFT)==0&&d_tb!=0){
			d_tb=0;
			POINT mp;
			GetCursorPos(&mp);
			if (mp.y<TOPBAR_MAXIMIZE_WINDOW_BUFFER&&mx==false){
				d_tb=2;
				ow=renderer_ww;
				oh=renderer_wh;
				mx=true;
				ShowWindow(renderer_w,SW_MAXIMIZE);
				free(tb_vl);
				free(tb_lvl);
				tb_vl=NULL;
				tb_lvl=NULL;
				return;
			}
		}
		if (d_tb==1){
			POINT mp;
			GetCursorPos(&mp);
			MoveWindow(renderer_w,mp.x-d_tb_ox,mp.y-d_tb_oy,renderer_ww,renderer_wh,false);
		}
	}
	else{
		d_tb=((renderer_mf&M_LEFT)==0?0:2);
	}
	if (tb_ib==NULL){
		uint16_t il[]={
			0,
			1,
			2,
			0,
			2,
			3,
			4,
			5,
			6,
			4,
			6,
			7,
			8,
			9,
			10,
			8,
			10,
			11,
			12,
			13,
			14,
			12,
			14,
			15
		};
		uint16_t lil[]={
			0,
			1,
			2,
			3,
			4,
			5,
			6,
			7,
			7,
			8,
			8,
			9,
			9,
			6
		};
		uint16_t mxlil[]={
			10,
			11,
			11,
			12,
			12,
			13,
			13,
			14
		};
		D3D11_BUFFER_DESC bd={
			24*sizeof(uint16_t),
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
		HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&tb_ib);
		bd.ByteWidth=14*sizeof(uint16_t);
		dt.pSysMem=lil;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&tb_lib);
		bd.ByteWidth=8*sizeof(uint16_t);
		dt.pSysMem=mxlil;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&tb_mxlib);
		bd.Usage=D3D11_USAGE_DYNAMIC;
		bd.ByteWidth=96*sizeof(float);
		bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		dt.pSysMem=tb_vl;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&tb_vb);
		bd.ByteWidth=90*sizeof(float);
		dt.pSysMem=tb_lvl;
		hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&tb_lvb);
	}
	else{
		if (u&0x01){
			ID3D11Resource* vbr;
			ID3D11Buffer_QueryInterface(tb_vb,&IID_ID3D11Resource,(void**)&vbr);
			D3D11_MAPPED_SUBRESOURCE mr;
			HRESULT hr=ID3D11DeviceContext_Map(renderer_d3_dc,vbr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			float* nvl=(float*)(mr.pData);
			for (size_t i=0;i<96;i++){
				*(nvl+i)=*(tb_vl+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,vbr,0);
			ID3D11Resource_Release(vbr);
		}
		if (u&0x02){
			ID3D11Resource* lvbr;
			ID3D11Buffer_QueryInterface(tb_lvb,&IID_ID3D11Resource,(void**)&lvbr);
			D3D11_MAPPED_SUBRESOURCE mr;
			HRESULT hr=ID3D11DeviceContext_Map(renderer_d3_dc,lvbr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			float* nlvl=(float*)(mr.pData);
			for (size_t i=0;i<90;i++){
				*(nlvl+i)=*(tb_lvl+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,lvbr,0);
			ID3D11Resource_Release(lvbr);
		}
	}
}



void init_ui(void){
	D3D11_INPUT_ELEMENT_DESC il[]={
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA
		}
	};
	ui_vs=load_vertex_shader(g_color_2d_vs,sizeof(g_color_2d_vs),il,sizeof(il)/sizeof(D3D11_INPUT_ELEMENT_DESC),&ui_vl);
	ui_ps=load_pixel_shader(g_color_2d_ps,sizeof(g_color_2d_ps));
	ui_cb=create_constant_buffer(sizeof(struct CBufferLayout));
	recalc_topbar();
}



void update_ui(double dt){
	recalc_topbar();
	if (renderer_w==NULL){
		return;
	}
}



void draw_ui(void){
	ID3D11DeviceContext_IASetInputLayout(renderer_d3_dc,ui_vl);
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,ui_vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,ui_ps,NULL,0);
	if (nc!=_lc){
		_lc=nc;
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(nc)));
	}
	unsigned int off=0;
	unsigned int st=6*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&tb_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,tb_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,24,0,0);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&tb_lvb,&st,&off);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,tb_lib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,14,0,0);
	if (mx==true){
		ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,tb_mxlib,DXGI_FORMAT_R16_UINT,0);
		ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,8,0,0);
	}
}
