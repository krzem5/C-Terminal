#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	matrix pm;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float4 c:COLOR;
};



VS_OUT color_2d_vs(float2 p:POSITION,float4 c:COLOR){
	VS_OUT o={
		mul(float4(p.x,p.y,0,1),pm),
		c
	};
	return o;
}



float4 color_2d_ps(VS_OUT vo):SV_TARGET{
	return vo.c;
}
