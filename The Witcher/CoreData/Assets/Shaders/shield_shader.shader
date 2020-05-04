#shader vertex
#version 330 core

precision highp float;
precision highp int;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;


out vec3 frag_pos;
out vec2 texCoords;
out vec3 norms;

void main()
{
    vec4 pos = vec4(position, 1.0);
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);
    norms = normalize(vec3(model * vec4(normals, 0.0)));
    gl_Position = projection * view * vec4(frag_pos, 1.0f); 
    
    
};

#shader fragment
#version 330 core

precision highp float;
precision highp int;

struct Material {
    vec3 diffuse_color; 
};

uniform Material objectMaterial;
uniform vec3 hitPosition;
uniform float time;

// Ins
in vec3 frag_pos;
in vec2 texCoords;
in vec3 norms;

// Outs
out vec4 FragColor;


vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; }

float mod289(float x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec4 permute(vec4 x) {
		 return mod289(((x*34.0)+1.0)*x);
}

float permute(float x) {
		 return mod289(((x*34.0)+1.0)*x);
}

vec4 grad4(float j, vec4 ip)
	{
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	vec4 p,s;
	p.xyz = floor( fract(vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	s = vec4(lessThan(p, vec4(0.0)));
	p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www; 
	return p;
	}
						

#define F4 0.309016994374947451
float snoise(vec4 v)
	{
	const vec4  C = vec4( 0.138196601125011,  0.276393202250021, 0.414589803375032, -0.447213595499958); 
	vec4 i  = floor(v + dot(v, vec4(F4)) );
	vec4 x0 = v -   i + dot(i, C.xxxx);

	vec4 i0;
	vec3 isX = step( x0.yzw, x0.xxx );
	vec3 isYZ = step( x0.zww, x0.yyz );

	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;

	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;
	
	vec4 i3 = clamp( i0, 0.0, 1.0 );
	vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
	vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );
	
	vec4 x1 = x0 - i1 + C.xxxx;
	vec4 x2 = x0 - i2 + C.yyyy;
	vec4 x3 = x0 - i3 + C.zzzz;
	vec4 x4 = x0 + C.wwww;

	i = mod289(i); 
	float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute( permute( permute( permute (i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))+ i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))+ i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))+ i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));

	vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;
	vec4 p0 = grad4(j0,   ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);

	vec4 norm = inversesqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= inversesqrt(dot(p4,p4));

	vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 ))) + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;
}

float mod13(float x)
{
	return x - floor(x * (1.0 / 1.0)) * 1.0;
}
	
vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float snoise(vec3 v)
	{ 
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;

	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );
	
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; 
	vec3 x3 = x0 - D.yyy;      

	i = mod289(i); 
	vec4 p = permute( permute( permute( i.z + vec4(0.0, i1.z, i2.z, 1.0 )) + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;
	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  
	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );    
	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);
	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );
	
	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));
	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

	vec4 norm = inversesqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3) ) );
}


float SimplexPerlin3D( vec3 P )
{
    
    const float SKEWFACTOR = 1.0/3.0;
    const float UNSKEWFACTOR = 1.0/6.0;
    const float SIMPLEX_CORNER_POS = 0.5;
    const float SIMPLEX_TETRAHADRON_HEIGHT = 0.70710678118654752440084436210485;    

   
    P *= SIMPLEX_TETRAHADRON_HEIGHT;    
    vec3 Pi = floor( P + dot( P, vec3( SKEWFACTOR) ) );

    
    vec3 x0 = P - Pi + dot(Pi, vec3( UNSKEWFACTOR ) );
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 Pi_1 = min( g.xyz, l.zxy );
    vec3 Pi_2 = max( g.xyz, l.zxy );

    vec3 x1 = x0 - Pi_1 + UNSKEWFACTOR;
    vec3 x2 = x0 - Pi_2 + SKEWFACTOR;
    vec3 x3 = x0 - SIMPLEX_CORNER_POS;

   
    vec4 v1234_x = vec4( x0.x, x1.x, x2.x, x3.x );
    vec4 v1234_y = vec4( x0.y, x1.y, x2.y, x3.y );
    vec4 v1234_z = vec4( x0.z, x1.z, x2.z, x3.z );

    
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );

    
    vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    vec4 V1xy_V2xy = mix( Pt.xyxy, Pt.zwzw, vec4( Pi_1.xy, Pi_2.xy ) );
    Pt = vec4( Pt.x, V1xy_V2xy.xz, Pt.z ) * vec4( Pt.y, V1xy_V2xy.yw, Pt.w );
    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
    vec3 lowz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi.zzz * ZINC.xyz ) );
    vec3 highz_mods = vec3( 1.0 / ( SOMELARGEFLOATS.xyz + Pi_inc1.zzz * ZINC.xyz ) );
    Pi_1 = ( Pi_1.z < 0.5 ) ? lowz_mods : highz_mods;
    Pi_2 = ( Pi_2.z < 0.5 ) ? lowz_mods : highz_mods;
    vec4 hash_0 = fract( Pt * vec4( lowz_mods.x, Pi_1.x, Pi_2.x, highz_mods.x ) ) - 0.49999;
    vec4 hash_1 = fract( Pt * vec4( lowz_mods.y, Pi_1.y, Pi_2.y, highz_mods.y ) ) - 0.49999;
    vec4 hash_2 = fract( Pt * vec4( lowz_mods.z, Pi_1.z, Pi_2.z, highz_mods.z ) ) - 0.49999;

    
    vec4 grad_results = inversesqrt( hash_0 * hash_0 + hash_1 * hash_1 + hash_2 * hash_2 ) * ( hash_0 * v1234_x + hash_1 * v1234_y + hash_2 * v1234_z );

    
    const float FINAL_NORMALIZATION = 37.837227241611314102871574478976;

    
    vec4 kernel_weights = v1234_x * v1234_x + v1234_y * v1234_y + v1234_z * v1234_z;
    kernel_weights = max(0.5 - kernel_weights, 0.0);
    kernel_weights = kernel_weights*kernel_weights*kernel_weights;

    return dot( kernel_weights, grad_results ) * FINAL_NORMALIZATION;
}

float Cellular3D(vec3 P)
{
   
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;

    
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );

    
    vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
    vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
    vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
    vec4 hash_x0 = fract( Pt * lowz_mod.xxxx ) * 2.0 - 1.0;
    vec4 hash_x1 = fract( Pt * highz_mod.xxxx ) * 2.0 - 1.0;
    vec4 hash_y0 = fract( Pt * lowz_mod.yyyy ) * 2.0 - 1.0;
    vec4 hash_y1 = fract( Pt * highz_mod.yyyy ) * 2.0 - 1.0;
    vec4 hash_z0 = fract( Pt * lowz_mod.zzzz ) * 2.0 - 1.0;
    vec4 hash_z1 = fract( Pt * highz_mod.zzzz ) * 2.0 - 1.0;

    
    const float JITTER_WINDOW = 0.166666666;	
    hash_x0 = ( ( hash_x0 * hash_x0 * hash_x0 ) - sign( hash_x0 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
    hash_y0 = ( ( hash_y0 * hash_y0 * hash_y0 ) - sign( hash_y0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
    hash_x1 = ( ( hash_x1 * hash_x1 * hash_x1 ) - sign( hash_x1 ) ) * JITTER_WINDOW + vec4( 0.0, 1.0, 0.0, 1.0 );
    hash_y1 = ( ( hash_y1 * hash_y1 * hash_y1 ) - sign( hash_y1 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 1.0, 1.0 );
    hash_z0 = ( ( hash_z0 * hash_z0 * hash_z0 ) - sign( hash_z0 ) ) * JITTER_WINDOW + vec4( 0.0, 0.0, 0.0, 0.0 );
    hash_z1 = ( ( hash_z1 * hash_z1 * hash_z1 ) - sign( hash_z1 ) ) * JITTER_WINDOW + vec4( 1.0, 1.0, 1.0, 1.0 );

    
    vec4 dx1 = Pf.xxxx - hash_x0;
    vec4 dy1 = Pf.yyyy - hash_y0;
    vec4 dz1 = Pf.zzzz - hash_z0;
    vec4 dx2 = Pf.xxxx - hash_x1;
    vec4 dy2 = Pf.yyyy - hash_y1;
    vec4 dz2 = Pf.zzzz - hash_z1;
    vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
    vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
    d1 = min(d1, d2);
    d1.xy = min(d1.xy, d1.wz);
    return min(d1.x, d1.y) * ( 9.0 / 12.0 ); 
}

float Perlin3D( vec3 P )
{
    
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;
    vec3 Pf_min1 = Pf - 1.0;

    
    Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / 69.0 )) * 69.0;
    vec3 Pi_inc1 = step( Pi, vec3( 69.0 - 1.5 ) ) * ( Pi + 1.0 );

    
    vec4 Pt = vec4( Pi.xy, Pi_inc1.xy ) + vec2( 50.0, 161.0 ).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    const vec3 SOMELARGEFLOATS = vec3( 635.298681, 682.357502, 668.926525 );
    const vec3 ZINC = vec3( 48.500388, 65.294118, 63.934599 );
    vec3 lowz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
    vec3 highz_mod = vec3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
    vec4 hashx0 = fract( Pt * lowz_mod.xxxx );
    vec4 hashx1 = fract( Pt * highz_mod.xxxx );
    vec4 hashy0 = fract( Pt * lowz_mod.yyyy );
    vec4 hashy1 = fract( Pt * highz_mod.yyyy );
    vec4 hashz0 = fract( Pt * lowz_mod.zzzz );
    vec4 hashz1 = fract( Pt * highz_mod.zzzz );

   
    vec4 grad_x0 = hashx0 - 0.49999;
    vec4 grad_y0 = hashy0 - 0.49999;
    vec4 grad_z0 = hashz0 - 0.49999;
    vec4 grad_x1 = hashx1 - 0.49999;
    vec4 grad_y1 = hashy1 - 0.49999;
    vec4 grad_z1 = hashz1 - 0.49999;
    vec4 grad_results_0 = inversesqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
    vec4 grad_results_1 = inversesqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( vec2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + vec2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );

    
    vec3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
    vec4 res0 = mix( grad_results_0, grad_results_1, blend.z );
    vec4 blend2 = vec4( blend.xy, vec2( 1.0 - blend.xy ) );
    float final = dot( res0, blend2.zxzx * blend2.wwyy );
    return ( final * 1.1547005383792515290182975610039 );  
}


mat4 calculateLookAtMatrix(vec3 eye, vec3 center, vec3 up) {

    vec3 zaxis = normalize(center - eye); 
    vec3 xaxis = normalize(cross(zaxis, up));
    vec3 yaxis = cross(xaxis, zaxis);
    
    mat4 lookAtMatrix;
    
    lookAtMatrix[0] = vec4(xaxis.x, yaxis.x, zaxis.x, 0.0);
    lookAtMatrix[1] = vec4(xaxis.y, yaxis.y, zaxis.y, 0.0);
    lookAtMatrix[2] = vec4(xaxis.z, yaxis.z, zaxis.z, 0.0);
    lookAtMatrix[3] = vec4(dot(xaxis, -eye), dot(yaxis, -eye), dot(zaxis, -eye), 1.0);

    return lookAtMatrix;
}

mat4 calculateLookAtMatrix(vec3 eye, vec3 center, float roll) {
    return calculateLookAtMatrix(eye, center, vec3(sin(roll), cos(roll), 0.0));
}

float Voronesque( in vec3 p ){
    
    
    vec3 i  = floor(p + dot(p, vec3(0.333333)) );  p -= i - dot(i, vec3(0.166666)) ;
    
    
    vec3 i1 = step(0., p-p.yzx), i2 = max(i1, 1.0-i1.zxy); i1 = min(i1, 1.0-i1.zxy);    
    
    
    vec3 p1 = p - i1 + 0.166666, p2 = p - i2 + 0.333333, p3 = p - 0.5;
    
    vec3 rnd = vec3(1.0); 
    
    
    vec4 v = max(0.5 - vec4(dot(p, p), dot(p1, p1), dot(p2, p2), dot(p3, p3)), 0.);
    
    
    vec4 d = vec4( dot(i, rnd), dot(i + i1, rnd), dot(i + i2, rnd), dot(i + 1., rnd) ); 
    
    
    d = fract(sin(d)*262144.)*v*2.; 
 
   
    v.x = max(d.x, d.y), v.y = max(d.z, d.w), v.z = max(min(d.x, d.y), min(d.z, d.w)), v.w = min(v.x, v.y); 
   
    return  max(v.x, v.y)- max(v.z, v.w); 
    
}

void main() {

    vec3 color = objectMaterial.diffuse_color;


    vec3 worldPosition = frag_pos;

   
    vec3 worldNormal = norms;
    
    vec3 standardVec = worldNormal * 4.0;
    standardVec.y += time * 0.3;
    
    float noiseLevel = 0.5;
    
    mat4 hitPointMat = calculateLookAtMatrix(hitPosition.xyz, vec3(0.0), 0.0);
    vec3 hpNormal = (hitPointMat * vec4( worldNormal, 0.0 )).xyz;
    
    vec3 noiseVec = hpNormal * 2.0;
    noiseVec.z -= time * 1.6;
    
    noiseLevel *= 1.5 * sin(noiseVec.z * 8.0);
    
    float noiseMult = ((1.0 - pow(Voronesque(standardVec), 0.1))) + abs(snoise(noiseVec)) * noiseLevel * 0.15;
    
    float offset = 1.0 - smoothstep( 0.0, 1.0, distance(hitPosition, worldPosition) );
    offset *= 3.0;
    
    vec3 col = color * (0.9 + offset) * noiseMult;
    
    FragColor = vec4( col, 1.0 );

}

