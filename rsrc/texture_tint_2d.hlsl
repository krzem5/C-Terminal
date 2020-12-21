#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	matrix pm;
};



Texture2D tx:register(t0);
SamplerState ss:register(s0);



struct VS_OUT{
	float4 p:SV_POSITION;
	float2 t:TEXCOORD;
	float3 c:COLOR;
};



VS_OUT texture_tint_2d_vs(float4 p:POSITION,float2 t:TEXCOORD,float3 c:COLOR){
	VS_OUT o={
		mul(p,pm),
		t,
		c
	};
	return o;
}



float4 texture_tint_2d_ps(VS_OUT vo):SV_TARGET{
	return float4(vo.c.x,vo.c.y,vo.c.z,tx.Sample(ss,vo.t).r);
}
