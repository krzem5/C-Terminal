#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	matrix pm;
};



Texture2D tx;
SamplerState ss;



struct VS_OUT{
	float4 p:SV_POSITION;
	float2 t:TEXCOORD;
	float4 c:COLOR;
};



VS_OUT texture_tint_2d_vs(float4 p:POSITION,float2 t:TEXCOORD,float3 c:COLOR){
	VS_OUT o={
		mul(p,pm),
		t,
		float4(c.x,c.y,c.z,1)
	};
	return o;
}



float4 texture_tint_2d_ps(VS_OUT vo):SV_TARGET{
	return tx.Sample(ss,vo.t)*vo.c;
}
