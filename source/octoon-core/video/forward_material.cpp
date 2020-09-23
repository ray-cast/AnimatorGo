#include <octoon/video/forward_material.h>
#include <octoon/video/renderer.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/hal/graphics.h>
#include <regex>

static char* common = R"(
#define PI 3.14159265359
#define PI2 6.28318530718
#define PI_HALF 1.5707963267949
#define RECIPROCAL_PI 0.31830988618
#define RECIPROCAL_PI2 0.15915494
#define LOG2 1.442695
#define EPSILON 1e-6

#define saturate(a) clamp( a, 0.0, 1.0 )
#define whiteCompliment(a) ( 1.0 - saturate( a ) )

float pow2( const in float x ) { return x*x; }
float pow3( const in float x ) { return x*x*x; }
float pow4( const in float x ) { float x2 = x*x; return x2*x2; }
float pow5( const in float x ) { float x2 = x*x; return x2*x2*x; }
float average( const in vec3 color ) { return dot( color, vec3( 0.3333 ) ); }
// expects values in the range of [0,1]x[0,1], returns values in the [0,1] range.
// do not collapse into a single function per: http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
highp float rand( const in vec2 uv ) {
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

struct IncidentLight {
	vec3 color;
	vec3 direction;
	bool visible;
};

struct ReflectedLight {
	vec3 directDiffuse;
	vec3 directSpecular;
	vec3 indirectDiffuse;
	vec3 indirectSpecular;
};

struct GeometricContext {
	vec3 position;
	vec3 normal;
	vec3 viewDir;
};

vec3 transformDirection( in vec3 dir, in mat4 matrix ) {
	return normalize( ( matrix * vec4( dir, 0.0 ) ).xyz );
}

// http://en.wikibooks.org/wiki/GLSL_Programming/Applying_Matrix_Transformations
vec3 inverseTransformDirection( in vec3 dir, in mat4 matrix ) {
	return normalize( ( vec4( dir, 0.0 ) * matrix ).xyz );
}

vec3 projectOnPlane(in vec3 point, in vec3 pointOnPlane, in vec3 planeNormal ) {
	float distance = dot( planeNormal, point - pointOnPlane );
	return - distance * planeNormal + point;
}

float sideOfPlane( in vec3 point, in vec3 pointOnPlane, in vec3 planeNormal ) {
	return sign( dot( point - pointOnPlane, planeNormal ) );
}

vec3 linePlaneIntersect( in vec3 pointOnLine, in vec3 lineDirection, in vec3 pointOnPlane, in vec3 planeNormal ) {
	return lineDirection * ( dot( planeNormal, pointOnPlane - pointOnLine ) / dot( planeNormal, lineDirection ) ) + pointOnLine;
}

mat3 transpose( const in mat3 v ) {
	mat3 tmp;
	tmp[0] = vec3(v[0].x, v[1].x, v[2].x);
	tmp[1] = vec3(v[0].y, v[1].y, v[2].y);
	tmp[2] = vec3(v[0].z, v[1].z, v[2].z);

	return tmp;
}
)";
static char* packing = R"(
vec3 packNormalToRGB( const in vec3 normal ) {
	return normalize( normal ) * 0.5 + 0.5;
}

vec3 unpackRGBToNormal( const in vec3 rgb ) {
	return 1.0 - 2.0 * rgb.xyz;
}

const float PackUpscale = 256. / 255.; // fraction -> 0..1 (including 1)
const float UnpackDownscale = 255. / 256.; // 0..1 -> fraction (excluding 1)

const vec3 PackFactors = vec3( 256. * 256. * 256., 256. * 256.,  256. );
const vec4 UnpackFactors = UnpackDownscale / vec4( PackFactors, 1. );

const float ShiftRight8 = 1. / 256.;

vec4 packDepthToRGBA( const in float v ) {
	vec4 r = vec4( fract( v * PackFactors ), v );
	r.yzw -= r.xyz * ShiftRight8; // tidy overflow
	return r * PackUpscale;
}

float unpackRGBAToDepth( const in vec4 v ) {
	return dot( v, UnpackFactors );
}

// NOTE: viewZ/eyeZ is < 0 when in front of the camera per OpenGL conventions

float viewZToOrthographicDepth( const in float viewZ, const in float near, const in float far ) {
	return ( viewZ + near ) / ( near - far );
}
float orthographicDepthToViewZ( const in float linearClipZ, const in float near, const in float far ) {
	return linearClipZ * ( near - far ) - near;
}

float viewZToPerspectiveDepth( const in float viewZ, const in float near, const in float far ) {
	return (( near + viewZ ) * far ) / (( far - near ) * viewZ );
}
float perspectiveDepthToViewZ( const in float invClipZ, const in float near, const in float far ) {
	return ( near * far ) / ( ( far - near ) * invClipZ - far );
}
)";

static char* encodings_pars_fragment = R"(
// For a discussion of what this is, please read this: http://lousodrome.net/blog/light/2013/05/26/gamma-correct-and-hdr-rendering-in-a-32-bits-buffer/

vec4 LinearToLinear( in vec4 value ) {
	return value;
}

vec4 GammaToLinear( in vec4 value, in float gammaFactor ) {
	return vec4( pow( value.xyz, vec3( gammaFactor ) ), value.w );
}
vec4 LinearToGamma( in vec4 value, in float gammaFactor ) {
	return vec4( pow( value.xyz, vec3( 1.0 / gammaFactor ) ), value.w );
}

vec4 sRGBToLinear( in vec4 value ) {
	return vec4( mix( pow( value.rgb * 0.9478672986 + vec3( 0.0521327014 ), vec3( 2.4 ) ), value.rgb * 0.0773993808, vec3( lessThanEqual( value.rgb, vec3( 0.04045 ) ) ) ), value.w );
}
vec4 LinearTosRGB( in vec4 value ) {
	return vec4( mix( pow( value.rgb, vec3( 0.41666 ) ) * 1.055 - vec3( 0.055 ), value.rgb * 12.92, vec3( lessThanEqual( value.rgb, vec3( 0.0031308 ) ) ) ), value.w );
}

vec4 RGBEToLinear( in vec4 value ) {
	return vec4( value.rgb * exp2( value.a * 255.0 - 128.0 ), 1.0 );
}
vec4 LinearToRGBE( in vec4 value ) {
	float maxComponent = max( max( value.r, value.g ), value.b );
	float fExp = clamp( ceil( log2( maxComponent ) ), -128.0, 127.0 );
	return vec4( value.rgb / exp2( fExp ), ( fExp + 128.0 ) / 255.0 );
//  return vec4( value.brg, ( 3.0 + 128.0 ) / 256.0 );
}

// reference: http://iwasbeingirony.blogspot.ca/2010/06/difference-between-rgbm-and-rgbd.html
vec4 RGBMToLinear( in vec4 value, in float maxRange ) {
	return vec4( value.xyz * value.w * maxRange, 1.0 );
}
vec4 LinearToRGBM( in vec4 value, in float maxRange ) {
	float maxRGB = max( value.x, max( value.g, value.b ) );
	float M      = clamp( maxRGB / maxRange, 0.0, 1.0 );
	M            = ceil( M * 255.0 ) / 255.0;
	return vec4( value.rgb / ( M * maxRange ), M );
}

// reference: http://iwasbeingirony.blogspot.ca/2010/06/difference-between-rgbm-and-rgbd.html
vec4 RGBDToLinear( in vec4 value, in float maxRange ) {
	return vec4( value.rgb * ( ( maxRange / 255.0 ) / value.a ), 1.0 );
}
vec4 LinearToRGBD( in vec4 value, in float maxRange ) {
	float maxRGB = max( value.x, max( value.g, value.b ) );
	float D      = max( maxRange / maxRGB, 1.0 );
	D            = min( floor( D ) / 255.0, 1.0 );
	return vec4( value.rgb * ( D * ( 255.0 / maxRange ) ), D );
}

// LogLuv reference: http://graphicrants.blogspot.ca/2009/04/rgbm-color-encoding.html

// M matrix, for encoding
const mat3 cLogLuvM = mat3( 0.2209, 0.3390, 0.4184, 0.1138, 0.6780, 0.7319, 0.0102, 0.1130, 0.2969 );
vec4 LinearToLogLuv( in vec4 value )  {
	vec3 Xp_Y_XYZp = value.rgb * cLogLuvM;
	Xp_Y_XYZp = max(Xp_Y_XYZp, vec3(1e-6, 1e-6, 1e-6));
	vec4 vResult;
	vResult.xy = Xp_Y_XYZp.xy / Xp_Y_XYZp.z;
	float Le = 2.0 * log2(Xp_Y_XYZp.y) + 127.0;
	vResult.w = fract(Le);
	vResult.z = (Le - (floor(vResult.w*255.0))/255.0)/255.0;
	return vResult;
}

// Inverse M matrix, for decoding
const mat3 cLogLuvInverseM = mat3( 6.0014, -2.7008, -1.7996, -1.3320, 3.1029, -5.7721, 0.3008, -1.0882, 5.6268 );
vec4 LogLuvToLinear( in vec4 value ) {
	float Le = value.z * 255.0 + value.w;
	vec3 Xp_Y_XYZp;
	Xp_Y_XYZp.y = exp2((Le - 127.0) / 2.0);
	Xp_Y_XYZp.z = Xp_Y_XYZp.y / value.y;
	Xp_Y_XYZp.x = value.x * Xp_Y_XYZp.z;
	vec3 vRGB = Xp_Y_XYZp.rgb * cLogLuvInverseM;
	return vec4( max(vRGB, 0.0), 1.0 );
}

)";
static char* cube_uv_reflection_fragment = R"(
#ifdef ENVMAP_TYPE_CUBE_UV
#define cubeUV_textureSize (1024.0)

int getFaceFromDirection(vec3 direction) {
	vec3 absDirection = abs(direction);
	int face = -1;
	if( absDirection.x > absDirection.z ) {
		if(absDirection.x > absDirection.y )
			face = direction.x > 0.0 ? 0 : 3;
		else
			face = direction.y > 0.0 ? 1 : 4;
	}
	else {
		if(absDirection.z > absDirection.y )
			face = direction.z > 0.0 ? 2 : 5;
		else
			face = direction.y > 0.0 ? 1 : 4;
	}
	return face;
}
#define cubeUV_maxLods1  (log2(cubeUV_textureSize*0.25) - 1.0)
#define cubeUV_rangeClamp (exp2((6.0 - 1.0) * 2.0))

vec2 MipLevelInfo( vec3 vec, float roughnessLevel, float roughness ) {
	float scale = exp2(cubeUV_maxLods1 - roughnessLevel);
	float dxRoughness = dFdx(roughness);
	float dyRoughness = dFdy(roughness);
	vec3 dx = dFdx( vec * scale * dxRoughness );
	vec3 dy = dFdy( vec * scale * dyRoughness );
	float d = max( dot( dx, dx ), dot( dy, dy ) );
	// Clamp the value to the max mip level counts. hard coded to 6 mips
	d = clamp(d, 1.0, cubeUV_rangeClamp);
	float mipLevel = 0.5 * log2(d);
	return vec2(floor(mipLevel), fract(mipLevel));
}

#define cubeUV_maxLods2 (log2(cubeUV_textureSize*0.25) - 2.0)
#define cubeUV_rcpTextureSize (1.0 / cubeUV_textureSize)

vec2 getCubeUV(vec3 direction, float roughnessLevel, float mipLevel) {
	mipLevel = roughnessLevel > cubeUV_maxLods2 - 3.0 ? 0.0 : mipLevel;
	float a = 16.0 * cubeUV_rcpTextureSize;

	vec2 exp2_packed = exp2( vec2( roughnessLevel, mipLevel ) );
	vec2 rcp_exp2_packed = vec2( 1.0 ) / exp2_packed;
	// float powScale = exp2(roughnessLevel + mipLevel);
	float powScale = exp2_packed.x * exp2_packed.y;
	// float scale =  1.0 / exp2(roughnessLevel + 2.0 + mipLevel);
	float scale = rcp_exp2_packed.x * rcp_exp2_packed.y * 0.25;
	// float mipOffset = 0.75*(1.0 - 1.0/exp2(mipLevel))/exp2(roughnessLevel);
	float mipOffset = 0.75*(1.0 - rcp_exp2_packed.y) * rcp_exp2_packed.x;

	bool bRes = mipLevel == 0.0;
	scale =  bRes && (scale < a) ? a : scale;

	vec3 r;
	vec2 offset;
	int face = getFaceFromDirection(direction);

	float rcpPowScale = 1.0 / powScale;

	if( face == 0) {
		r = vec3(direction.x, -direction.z, direction.y);
		offset = vec2(0.0+mipOffset,0.75 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? a : offset.y;
	}
	else if( face == 1) {
		r = vec3(direction.y, direction.x, direction.z);
		offset = vec2(scale+mipOffset, 0.75 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? a : offset.y;
	}
	else if( face == 2) {
		r = vec3(direction.z, direction.x, direction.y);
		offset = vec2(2.0*scale+mipOffset, 0.75 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? a : offset.y;
	}
	else if( face == 3) {
		r = vec3(direction.x, direction.z, direction.y);
		offset = vec2(0.0+mipOffset,0.5 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? 0.0 : offset.y;
	}
	else if( face == 4) {
		r = vec3(direction.y, direction.x, -direction.z);
		offset = vec2(scale+mipOffset, 0.5 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? 0.0 : offset.y;
	}
	else {
		r = vec3(direction.z, -direction.x, direction.y);
		offset = vec2(2.0*scale+mipOffset, 0.5 * rcpPowScale);
		offset.y = bRes && (offset.y < 2.0*a) ? 0.0 : offset.y;
	}
	r = normalize(r);
	float texelOffset = 0.5 * cubeUV_rcpTextureSize;
	vec2 s = ( r.yz / abs( r.x ) + vec2( 1.0 ) ) * 0.5;
	vec2 base = offset + vec2( texelOffset );
	return base + s * ( scale - 2.0 * texelOffset );
}

#define cubeUV_maxLods3 (log2(cubeUV_textureSize*0.25) - 3.0)

vec4 textureCubeUV(sampler2D texture, vec3 reflectedDirection, float roughness ) {
	float roughnessVal = roughness* cubeUV_maxLods3;
	float r1 = floor(roughnessVal);
	float r2 = r1 + 1.0;
	float t = fract(roughnessVal);
	vec2 mipInfo = MipLevelInfo(reflectedDirection, r1, roughness);
	float s = mipInfo.y;
	float level0 = mipInfo.x;
	float level1 = level0 + 1.0;
	level1 = level1 > 5.0 ? 5.0 : level1;

	// round to nearest mipmap if we are not interpolating.
	level0 += min( floor( s + 0.5 ), 5.0 );

	// Tri linear interpolation.
	vec2 uv_10 = getCubeUV(reflectedDirection, r1, level0);
	vec4 color10 = LinearToLinear(texture2D(texture, uv_10));

	vec2 uv_20 = getCubeUV(reflectedDirection, r2, level0);
	vec4 color20 = LinearToLinear(texture2D(texture, uv_20));

	vec4 result = mix(color10, color20, t);

	return vec4(result.rgb, 1.0);
}
#endif

vec4 textureLatlongUV(sampler2D texture, vec3 reflectedDirection, float roughness)
{
	vec2 uv = vec2((atan(reflectedDirection.x, reflectedDirection.z) * RECIPROCAL_PI * 0.5f + 0.5f), acos(reflectedDirection.y) * RECIPROCAL_PI);
	return LinearToLinear(textureLod(texture, uv, roughness * 7));
}

)";
static char* begin_vertex = R"(
vec3 transformed = vec3( POSITION0 );
)";
static char* worldpos_vertex = R"(
#if defined ( USE_SHADOWMAP )
	vec4 worldPosition = modelMatrix * vec4( transformed, 1.0 );
#endif
)";
static char* project_vertex = R"(
vec4 mvPosition = modelViewMatrix * vec4( transformed, 1.0 );
gl_Position = projectionMatrix * mvPosition;
)";
static char* beginnormal_vertex = R"(
vec3 objectNormal = vec3( NORMAL0 );
)";
static char* normal_pars_vertex = R"(
	out vec3 vNormal;
)";
static char* normal_flip = R"(
#ifdef DOUBLE_SIDED
	float flipNormal = ( float( gl_FrontFacing ) * 2.0 - 1.0 );
#else
	float flipNormal = 1.0;
#endif
)";
static char* normal_pars_fragment = R"(
#ifndef FLAT_SHADED
in vec3 vNormal;
#endif
)";
static char* normal_fragment = R"(
#ifdef FLAT_SHADED
	// Workaround for Adreno/Nexus5 not able able to do dFdx( vViewPosition ) ...
	vec3 fdx = vec3( dFdx( vViewPosition.x ), dFdx( vViewPosition.y ), dFdx( vViewPosition.z ) );
	vec3 fdy = vec3( dFdy( vViewPosition.x ), dFdy( vViewPosition.y ), dFdy( vViewPosition.z ) );
	vec3 normal = normalize( cross( fdx, fdy ) );
#else
	vec3 normal = normalize( vNormal ) * flipNormal;
#endif

#ifdef USE_NORMALMAP
	normal = perturbNormal2Arb( -vViewPosition, normal );
#elif defined( USE_BUMPMAP )
	normal = perturbNormalArb( -vViewPosition, normal, dHdxy_fwd() );
#endif

)";
static char* normalmap_pars_fragment = R"(
#ifdef USE_NORMALMAP

	uniform sampler2D normalMap;
	uniform vec2 normalScale;

	// Per-Pixel Tangent Space Normal Mapping
	// http://hacksoflife.blogspot.ch/2009/11/per-pixel-tangent-space-normal-mapping.html

	vec3 perturbNormal2Arb( vec3 eye_pos, vec3 surf_norm ) {

		vec3 q0 = dFdx( eye_pos.xyz );
		vec3 q1 = dFdy( eye_pos.xyz );
		vec2 st0 = dFdx( vUv.st );
		vec2 st1 = dFdy( vUv.st );

		vec3 S = normalize( q0 * st1.t - q1 * st0.t );
		vec3 T = normalize( -q0 * st1.s + q1 * st0.s );
		vec3 N = normalize( surf_norm );

		vec3 mapN = texture2D( normalMap, vUv ).xyz * 2.0 - 1.0;
		mapN.xy = normalScale * mapN.xy;
		mat3 tsn = mat3( S, T, N );
		return normalize( tsn * mapN );

	}

#endif
)";
static char* defaultnormal_vertex = R"(
vec3 transformedNormal = normalMatrix * objectNormal;
#ifdef FLIP_SIDED
	transformedNormal = - transformedNormal;
#endif
vNormal = normalize( transformedNormal );
)";
static char* color_pars_vertex = R"(
#ifdef USE_COLOR
	out vec3 vColor;
#endif
)";
static char* color_pars_fragment = R"(
#ifdef USE_COLOR
	in vec3 vColor;
#endif
)";
static char* color_vertex = R"(
#ifdef USE_COLOR
	vColor.xyz = color.xyz;
#endif
)";
static char* color_fragment = R"(
#ifdef USE_COLOR
	diffuseColor.rgb *= vColor;
#endif
)";
static char* uv_pars_vertex = R"(
	out vec2 vUv;
	uniform vec4 offsetRepeat;
)";
static char* uv_vertex = R"(
	vUv = TEXCOORD0 * offsetRepeat.zw + offsetRepeat.xy;
)";
static char* uv_pars_fragment = R"(
	in vec2 vUv;
)";
static char* uv2_pars_vertex = R"(
	out vec2 vUv2;
)";
static char* uv2_vertex = R"(
	vUv2 = TEXCOORD1;
)";
static char* uv2_pars_fragment = R"(
	in vec2 vUv2;
)";
static char* map_fragment = R"(
#ifdef USE_MAP
	vec4 texelColor = texture2D( map, vUv );
	texelColor = GammaToLinear( texelColor, gamma);
	diffuseColor *= texelColor;
#endif
)";
static char* map_pars_fragment = R"(
#ifdef USE_MAP
	uniform sampler2D map;
#endif
)";
static char* map_particle_pars_fragment = R"(
#ifdef USE_MAP
	uniform vec4 offsetRepeat;
	uniform sampler2D map;
#endif
)";
static char* map_particle_fragment = R"(
#ifdef USE_MAP
	vec4 mapTexel = texture2D( map, vec2( gl_PointCoord.x, 1.0 - gl_PointCoord.y ) * offsetRepeat.zw + offsetRepeat.xy );
	diffuseColor *= GammaToLinear(mapTexel);
#endif
)";
static char* premultiplied_alpha_fragment = R"(
#ifdef PREMULTIPLIED_ALPHA
	fragColor.rgb *= fragColor.a;
#endif
)";
static char* bumpmap_pars_fragment = R"(
#ifdef USE_BUMAPMAP
	uniform sampler2D bumpMap;
#endif
)";
static char* smoothnessmap_pars_fragment = R"(
#ifdef USE_SMOOTHNESSMAP
	uniform sampler2D smoothnessMap;
#endif
)";
static char* roughnessmap_pars_fragment = R"(
#ifdef USE_ROUGHNESSMAP
	uniform sampler2D roughnessMap;
#endif
)";
static char* roughnessmap_fragment = R"(
float roughnessFactor = roughness;
#ifdef USE_ROUGHNESSMAP
	vec4 texelRoughness = texture2D( roughnessMap, vUv );
	roughnessFactor *= texelRoughness.r;
#endif
)";
static char* specularmap_pars_fragment = R"(
#ifdef USE_SPECULARMAP
	uniform sampler2D specularMap;
#endif
)";
static char* specularmap_fragment = R"(
float specularFactor = specular;
#ifdef USE_SPECULARMAP
	vec4 texelSpecular = texture2D( specularMap, vUv );
	specularFactor *= texelSpecular.r;
#endif
)";
static char* metalnessmap_pars_fragment = R"(
#ifdef USE_METALNESSMAP
	uniform sampler2D metalnessMap;
#endif
)";
static char* metalnessmap_fragment = R"(
float metalnessFactor = metalness;
#ifdef USE_METALNESSMAP
	vec4 texelMetalness = texture2D( metalnessMap, vUv );
	metalnessFactor *= texelMetalness.r;
#endif
)";
static char* anisotropymap_pars_fragment = R"(
#ifdef USE_ANISOTROPYMAP
	uniform sampler2D anisotropyMap;
#endif
)";
static char* anisotropymap_fragment = R"(
float anisotropyFactor = anisotropy;
#ifdef USE_ANISOTROPYMAP
	vec4 texelAnisotropy = texture2D( anisotropyMap, vUv );
	anisotropyFactor *= texelAnisotropy.r;
#endif
)";
static char* sheenmap_pars_fragment = R"(
#ifdef USE_SHEENMAP
	uniform sampler2D sheenMap;
#endif
)";
static char* sheenmap_fragment = R"(
float sheenFactor = sheen;
#ifdef USE_SHEENMAP
	vec4 texelSheen = texture2D( sheenMap, vUv );
	sheenFactor *= texelSheen.r;
#endif
)";
static char* subsurfacemap_pars_fragment = R"(
#ifdef USE_SUBSURFACEMAP
	uniform sampler2D subsurfaceMap;
#endif
)";
static char* subsurfacemap_fragment = R"(
float subsurfaceFactor = subsurface;
#ifdef USE_SUBSURFACEMAP
	vec4 texelSubsurface = texture2D( subsurfaceMap, vUv );
	subsurfaceFactor *= texelSubsurface.r;
#endif
)";
static char* clearcoatmap_pars_fragment = R"(
#ifdef USE_CLEARCOATMAP
	uniform sampler2D clearCoatMap;
#endif
)";
static char* clearcoatmap_fragment = R"(
float clearCoatFactor = clearCoat;
#ifdef USE_CLEARCOATMAP
	vec4 texelClearcoat = texture2D( clearCoatMap, vUv );
	clearCoatFactor *= texelClearcoat.r;
#endif
)";
static char* clearcoatRoughnessmap_pars_fragment = R"(
#ifdef USE_CLEARCOATROUGHNESSMAP
	uniform sampler2D clearCoatRoughnessMap;
#endif
)";
static char* clearcoatRoughnessmap_fragment = R"(
float clearCoatRoughnessFactor = clearCoatRoughness;
#ifdef USE_CLEARCOATROUGHNESSMAP
	vec4 texelClearcoatRoughness = texture2D( clearCoatRoughnessMap, vUv );
	clearCoatRoughnessFactor *= texelClearcoatRoughness.r;
#endif
)";
static char* emissivemap_pars_fragment = R"(
#ifdef USE_EMISSIVEMAP
	uniform sampler2D emissiveMap;
#endif
)";
static char* emissivemap_fragment = R"(
#ifdef USE_EMISSIVEMAP
	vec4 emissiveColor = texture2D( emissiveMap, vUv );
	emissiveColor.rgb = GammaToLinear(emissiveColor, 2.2).rgb;
	totalEmissiveRadiance *= emissiveColor.rgb;
#endif
)";
static char* alphatest_fragment = R"(
#ifdef ALPHATEST
	if ( diffuseColor.a < ALPHATEST ) discard;
#endif
)";
static char* opacitymap_pars_fragment = R"(
#ifdef USE_OPACITYMAP
	uniform sampler2D opacityMap;
#endif
)";
static char* opacitymap_fragment = R"(
#ifdef USE_OPACITYMAP
	diffuseColor.a *= texture2D( opacityMap, vUv ).g;
#endif
)";
static char* aomap_pars_fragment = R"(
#ifdef USE_AOMAP
	uniform sampler2D aoMap;
	uniform float aoMapIntensity;
#endif
)";
static char* envmap_pars_fragment = R"(
#if defined( USE_ENVMAP ) || defined( PHYSICAL )
	uniform float envMapIntensity;
#endif
#ifdef USE_ENVMAP
	#if ! defined( PHYSICAL ) && ( defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) || defined( PHONG ) )
		in vec3 vWorldPosition;
	#endif

	#ifdef ENVMAP_TYPE_CUBE
		uniform samplerCube envMap;
	#else
		uniform sampler2D envMap;
	#endif
	uniform float flipEnvMap;

	#if defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) || defined( PHONG ) || defined( PHYSICAL )
		uniform float refractionRatio;
	#else
		in vec3 vReflect;
	#endif
#endif
)";
static char* lightmap_pars_fragment = R"(
uniform bool lightMapEnable;
uniform sampler2D lightMap;
uniform float lightMapIntensity;
)";
static char* tonemapping_pars_fragment = R"(
vec3 ACES(vec3 x)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;
	return (x * (A * x + B)) / (x * (C * x + D) + E);
}
)";
static char* tonemapping_fragment = R"(
#if defined( TONE_MAPPING )
  fragColor.rgb = ACES( fragColor.rgb );
#endif
)";
static char* bsdfs = R"(
float punctualLightIntensityToIrradianceFactor( const in float lightDistance, const in float cutoffDistance, const in float decayExponent ) {

	if( decayExponent > 0.0 ) {

#if defined ( PHYSICALLY_CORRECT_LIGHTS )

		// based upon Frostbite 3 Moving to Physically-based Rendering
		// page 32, equation 26: E[window1]
		// http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
		// this is intended to be used on spot and point lights who are represented as luminous intensity
		// but who must be converted to luminous irradiance for surface lighting calculation
		float distanceFalloff = 1.0 / max( pow( lightDistance, decayExponent ), 0.01 );
		float maxDistanceCutoffFactor = pow2( saturate( 1.0 - pow4( lightDistance / cutoffDistance ) ) );
		return distanceFalloff * maxDistanceCutoffFactor;

#else

		return pow( saturate( -lightDistance / cutoffDistance + 1.0 ), decayExponent );

#endif

	}

	return 1.0;

}

vec3 BRDF_Diffuse_Lambert( const in vec3 diffuseColor ) {

	return RECIPROCAL_PI * diffuseColor;

} // validated

vec3 BRDF_Diffuse_Burley(const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 diffuseColor, const in float roughness, const in float subsurface) {
	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );

	float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotVH = saturate( dot( geometry.viewDir, halfDir ) );

	float f_wo = pow5(1 - dotNV);
	float f_wi = pow5(1 - dotNL);

	float energyBias = 0.5 * roughness;
	float energyFactor = mix(1, 1 / 1.51, roughness);
	
	float Fd90 = energyBias + 2.0 * dotVH * dotVH * roughness;
	float FdV = mix(1, Fd90, f_wo);
	float FdL = mix(1, Fd90, f_wi);
	float Fd = FdV * FdL * energyFactor;

    float fss90 = dotVH * dotVH * roughness;
    float fss = mix(1.f, fss90, f_wo) * mix(1.f, fss90, f_wi);
    float ss = 1.25f * (fss * (1.f / (dotNV + dotNL) - 0.5f) + 0.5f);

	return RECIPROCAL_PI * diffuseColor * mix(Fd, ss, subsurface);
} // validated

vec3 F_Schlick( const in vec3 specularColor, const in float dotLH ) {

	// Original approximation by Christophe Schlick '94
	// float fresnel = pow5( 1.0 - dotLH);

	// Optimized variant (presented by Epic at SIGGRAPH '13)
	float fresnel = exp2( ( -5.55473 * dotLH - 6.98316 ) * dotLH );

	return ( 1.0 - specularColor ) * fresnel + specularColor;

} // validated

// Microfacet Models for Refraction through Rough Surfaces - equation (34)
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// alpha is "roughness squared" in Disney’s reparameterization
float G_GGX_Smith( const in float alpha, const in float dotNL, const in float dotNV ) {

	// geometry term = G(l)G(v) / 4(n·l)(n·v)

	float a2 = pow2( alpha );

	float gl = dotNL + sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );
	float gv = dotNV + sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );

	return 1.0 / ( gl * gv );

} // validated

// Moving Frostbite to Physically Based Rendering 2.0 - page 12, listing 2
// http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
float G_GGX_SmithCorrelated( const in float alpha, const in float dotNL, const in float dotNV ) {

	float a2 = pow2( alpha );

	// dotNL and dotNV are explicitly swapped. This is not a mistake.
	float gv = dotNL * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );
	float gl = dotNV * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );

	return 0.5 / max( gv + gl, EPSILON );
}

// Microfacet Models for Refraction through Rough Surfaces - equation (33)
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// alpha is "roughness squared" in Disney’s reparameterization
float D_GGX( const in float alpha, const in float dotNH ) {

	float a2 = pow2( alpha );

	float denom = pow2( dotNH ) * ( a2 - 1.0 ) + 1.0; // avoid alpha = 0 with dotNH = 1

	return a2 / pow2(PI * denom);

}

vec3 BRDF_Specular_Sheen( const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float roughness )
{
	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );

	float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotLH = saturate( dot( incidentLight.direction, halfDir ) );

	float sin2 = (1 - dotNH * dotNH);
	float spec = (2 + 1 / roughness) * pow(sin2, 0.5 / roughness) / (2  * PI);

	vec3 F = F_Schlick( specularColor, dotLH );

	float G = 4 * (dotNL + dotNV - dotNL * dotNV);
	spec /= G;

	return spec * F * G;
}

// GGX Distribution, Schlick Fresnel, GGX-Smith Visibility
vec3 BRDF_Specular_GGX( const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float roughness ) {

	float alpha = pow2( roughness ); // UE4's roughness

	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );

	float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotLH = saturate( dot( incidentLight.direction, halfDir ) );

	vec3 F = F_Schlick( specularColor, dotLH );

	float G = G_GGX_SmithCorrelated( alpha, dotNL, dotNV );

	float D = D_GGX( alpha, dotNH );

	return F * ( G * D );

} // validated

// GGX Distribution, Schlick Fresnel, GGX-Smith Visibility
vec3 BRDF_Specular_GGX_Aniso( const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float roughness, const in float anisotropy ) {
	vec3 X = normalize(cross(geometry.normal, vec3(1,0,0)));
	vec3 Y = normalize(cross(geometry.normal, X));

	float alpha = pow2( roughness ); // UE4's roughness

	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );

	float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotLH = saturate( dot( incidentLight.direction, halfDir ) );

    float ax = max(0.001f, alpha * ( 1.f + anisotropy));
    float ay = max(0.001f, alpha * ( 1.f - anisotropy));
	float denom = pow2(dot(X, halfDir) / ax) + pow2(dot(Y, halfDir) / ay) + dotNH * dotNH;
	float D = denom > 1e-5 ? (1.f / (PI * ax * ay * denom * denom)) : 0.f;

	vec3 F = F_Schlick( specularColor, dotLH );
	float G = G_GGX_SmithCorrelated( alpha, dotNL, dotNV );

	return F * ( G * D );

} // validated

// Rect Area Light

// Area light computation code adapted from:
// Real-Time Polygonal-Light Shading with Linearly Transformed Cosines
// By: Eric Heitz, Jonathan Dupuy, Stephen Hill and David Neubelt
// https://drive.google.com/file/d/0BzvWIdpUpRx_d09ndGVjNVJzZjA/view
// https://eheitzresearch.wordpress.com/415-2/
// http://blog.selfshadow.com/sandbox/ltc.html

vec2 LTC_Uv( const in vec3 N, const in vec3 V, const in float roughness ) {

	const float LUT_SIZE  = 64.0;
	const float LUT_SCALE = ( LUT_SIZE - 1.0 ) / LUT_SIZE;
	const float LUT_BIAS  = 0.5 / LUT_SIZE;

	float theta = acos( dot( N, V ) );

	// Parameterization of texture:
	// sqrt(roughness) -> [0,1]
	// theta -> [0, PI/2]
	vec2 uv = vec2(
		sqrt( saturate( roughness ) ),
		saturate( theta / ( 0.5 * PI ) ) );

	// Ensure we don't have nonlinearities at the look-up table's edges
	// see: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter24.html
	//      "Shader Analysis" section
	uv = uv * LUT_SCALE + LUT_BIAS;

	return uv;

}

// Real-Time Area Lighting: a Journey from Research to Production
// By: Stephen Hill & Eric Heitz
// http://advances.realtimerendering.com/s2016/s2016_ltc_rnd.pdf
// An approximation for the form factor of a clipped rectangle.
float LTC_ClippedSphereFormFactor( const in vec3 f ) {

	float l = length( f );

	return max( ( l * l + f.z ) / ( l + 1.0 ), 0.0 );

}

// Real-Time Polygonal-Light Shading with Linearly Transformed Cosines
// also Real-Time Area Lighting: a Journey from Research to Production
// http://advances.realtimerendering.com/s2016/s2016_ltc_rnd.pdf
// Normalization by 2*PI is incorporated in this function itself.
// theta/sin(theta) is approximated by rational polynomial
vec3 LTC_EdgeVectorFormFactor( const in vec3 v1, const in vec3 v2 ) {

	float x = dot( v1, v2 );

	float y = abs( x );
	float a = 0.86267 + (0.49788 + 0.01436 * y ) * y;
	float b = 3.45068 + (4.18814 + y) * y;
	float v = a / b;

	float theta_sintheta = (x > 0.0) ? v : 0.5 * inversesqrt( 1.0 - x * x ) - v;

	return cross( v1, v2 ) * theta_sintheta;

}

vec3 LTC_Evaluate( const in vec3 N, const in vec3 V, const in vec3 P, const in mat3 mInv, const in vec3 rectCoords[ 4 ] ) {

	// bail if point is on back side of plane of light
	// assumes ccw winding order of light vertices
	vec3 v1 = rectCoords[ 1 ] - rectCoords[ 0 ];
	vec3 v2 = rectCoords[ 3 ] - rectCoords[ 0 ];
	vec3 lightNormal = cross( v1, v2 );

	if( dot( lightNormal, P - rectCoords[ 0 ] ) < 0.0 ) return vec3( 0.0 );

	// construct orthonormal basis around N
	vec3 T1, T2;
	T1 = normalize( V - N * dot( V, N ) );
	T2 = - cross( N, T1 ); // negated from paper; possibly due to a different assumed handedness of world coordinate system

	// compute transform
	mat3 mat = mInv * transpose( mat3( T1, T2, N ) );

	// transform rect
	vec3 coords[ 4 ];
	coords[ 0 ] = mat * ( rectCoords[ 0 ] - P );
	coords[ 1 ] = mat * ( rectCoords[ 1 ] - P );
	coords[ 2 ] = mat * ( rectCoords[ 2 ] - P );
	coords[ 3 ] = mat * ( rectCoords[ 3 ] - P );

	// project rect onto sphere
	coords[ 0 ] = normalize( coords[ 0 ] );
	coords[ 1 ] = normalize( coords[ 1 ] );
	coords[ 2 ] = normalize( coords[ 2 ] );
	coords[ 3 ] = normalize( coords[ 3 ] );

	// calculate vector form factor
	vec3 vectorFormFactor = vec3( 0.0 );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 0 ], coords[ 1 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 1 ], coords[ 2 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 2 ], coords[ 3 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 3 ], coords[ 0 ] );

	// adjust for horizon clipping
	vec3 result = vec3( LTC_ClippedSphereFormFactor( vectorFormFactor ) );

	return result;

}

// End Rect Area Light

// ref: https://www.unrealengine.com/blog/physically-based-shading-on-mobile - environmentBRDF for GGX on mobile
vec3 BRDF_Specular_GGX_Environment( const in GeometricContext geometry, const in vec3 specularColor, const in float roughness ) {

	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );

	const vec4 c0 = vec4( - 1, - 0.0275, - 0.572, 0.022 );

	const vec4 c1 = vec4( 1, 0.0425, 1.04, - 0.04 );

	vec4 r = roughness * c0 + c1;

	float a004 = min( r.x * r.x, exp2( - 9.28 * dotNV ) ) * r.x + r.y;

	vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;

	return specularColor * AB.x + AB.y;

} // validated

vec3 BRDF_Specular_Sheen_Environment( const in GeometricContext geometry, const in vec3 specularColor, const in float roughness ) {
	float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );

	const vec4 c0 = vec4(0.24,  0.93, 0.01, 0.20);
	const vec4 c1 = vec4(2.00, -1.30, 0.40, 0.03);

	float s = 1.0 - dotNV;
	float e = s - c0.y;
	float g = c0.x * exp2(-(e * e) / (2.0 * c0.z)) + s * c0.w;
	float n = roughness * c1.x + c1.y;
	float r = max(1.0 - n * n, c1.z) * g;

	return specularColor * r + r * c1.w;
}

float G_BlinnPhong_Implicit( /* const in float dotNL, const in float dotNV */ ) {

	// geometry term is (n dot l)(n dot v) / 4(n dot l)(n dot v)
	return 0.25;

}

float D_BlinnPhong( const in float shininess, const in float dotNH ) {

	return RECIPROCAL_PI * ( shininess * 0.5 + 1.0 ) * pow( dotNH, shininess );

}

vec3 BRDF_Specular_BlinnPhong( const in IncidentLight incidentLight, const in GeometricContext geometry, const in vec3 specularColor, const in float shininess ) {

	vec3 halfDir = normalize( incidentLight.direction + geometry.viewDir );

	//float dotNL = saturate( dot( geometry.normal, incidentLight.direction ) );
	//float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
	float dotNH = saturate( dot( geometry.normal, halfDir ) );
	float dotLH = saturate( dot( incidentLight.direction, halfDir ) );

	vec3 F = F_Schlick( specularColor, dotLH );

	float G = G_BlinnPhong_Implicit( /* dotNL, dotNV */ );

	float D = D_BlinnPhong( shininess, dotNH );

	return F * ( G * D );

} // validated

// source: http://simonstechblog.blogspot.ca/2011/12/microfacet-brdf.html
float GGXRoughnessToBlinnExponent( const in float ggxRoughness ) {
	return ( 2.0 / pow2( ggxRoughness + 0.0001 ) - 2.0 );
}

float BlinnExponentToGGXRoughness( const in float blinnExponent ) {
	return sqrt( 2.0 / ( blinnExponent + 2.0 ) );
}
)";
static char* lights_pars = R"(
uniform vec3 ambientLightColor;

vec3 getAmbientLightIrradiance( const in vec3 ambientLightColor ) {

	vec3 irradiance = ambientLightColor;

	#ifndef PHYSICALLY_CORRECT_LIGHTS

		irradiance *= PI;

	#endif

	return irradiance;

}

#if NUM_DIR_LIGHTS > 0

	struct DirectionalLight {
		vec3 direction;
		vec3 color;

		int shadow;
		float shadowBias;
		float shadowRadius;
		vec2 shadowMapSize;
	};

	uniform DirectionalLights {
		DirectionalLight lights[NUM_DIR_LIGHTS];
	}directionalLights;

	void getDirectionalDirectLightIrradiance( const in DirectionalLight directionalLight, const in GeometricContext geometry, out IncidentLight directLight ) {

		directLight.color = directionalLight.color;
		directLight.direction = directionalLight.direction;
		directLight.visible = true;

	}

#endif


#if NUM_POINT_LIGHTS > 0

	struct PointLight {
		vec3 position;
		vec3 color;
		float distance;
		float decay;

		int shadow;
		float shadowBias;
		float shadowRadius;
		vec2 shadowMapSize;
	};

	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];

	// directLight is an out parameter as having it as a return value caused compiler errors on some devices
	void getPointDirectLightIrradiance( const in PointLight pointLight, const in GeometricContext geometry, out IncidentLight directLight ) {

		vec3 lVector = pointLight.position - geometry.position;
		directLight.direction = normalize( lVector );

		float lightDistance = length( lVector );

		directLight.color = pointLight.color;
		directLight.color *= punctualLightIntensityToIrradianceFactor( lightDistance, pointLight.distance, pointLight.decay );
		directLight.visible = ( directLight.color != vec3( 0.0 ) );

	}

#endif


#if NUM_SPOT_LIGHTS > 0
	struct SpotLight {
		vec3 position;
		vec3 direction;
		vec3 color;
		float distance;
		float decay;
		float coneCos;
		float penumbraCos;

		int shadow;
		float shadowBias;
		float shadowRadius;
		vec2 shadowMapSize;
	};

	uniform SpotLight spotLights[ NUM_SPOT_LIGHTS ];

	// directLight is an out parameter as having it as a return value caused compiler errors on some devices
	void getSpotDirectLightIrradiance( const in SpotLight spotLight, const in GeometricContext geometry, out IncidentLight directLight  ) {

		vec3 lVector = spotLight.position - geometry.position;
		directLight.direction = normalize( lVector );

		float lightDistance = length( lVector );
		float angleCos = dot( directLight.direction, spotLight.direction );

		if ( angleCos > spotLight.coneCos ) {

			float spotEffect = smoothstep( spotLight.coneCos, spotLight.penumbraCos, angleCos );

			directLight.color = spotLight.color;
			directLight.color *= spotEffect * punctualLightIntensityToIrradianceFactor( lightDistance, spotLight.distance, spotLight.decay );
			directLight.visible = true;

		} else {

			directLight.color = vec3( 0.0 );
			directLight.visible = false;

		}
	}

#endif

#if NUM_RECT_AREA_LIGHTS > 0

	struct RectAreaLight {
		vec3 color;
		vec3 position;
		vec3 halfWidth;
		vec3 halfHeight;
	};

	// Pre-computed values of LinearTransformedCosine approximation of BRDF
	// BRDF approximation Texture is 64x64
	uniform sampler2D ltcMat; // RGBA Float
	uniform sampler2D ltcMag; // Alpha Float (only has w component)

	uniform RectAreaLight rectAreaLights[ NUM_RECT_AREA_LIGHTS ];

#endif

#if NUM_HEMI_LIGHTS > 0
	struct HemisphereLight {
		vec3 direction;
		vec3 skyColor;
		vec3 groundColor;
	};

	uniform HemisphereLight hemisphereLights[ NUM_HEMI_LIGHTS ];

	vec3 getHemisphereLightIrradiance( const in HemisphereLight hemiLight, const in GeometricContext geometry ) {

		float dotNL = dot( geometry.normal, hemiLight.direction );
		float hemiDiffuseWeight = 0.5 * dotNL + 0.5;

		vec3 irradiance = mix( hemiLight.groundColor, hemiLight.skyColor, hemiDiffuseWeight );

		#ifndef PHYSICALLY_CORRECT_LIGHTS

			irradiance *= PI;

		#endif

		return irradiance;

	}
#endif


#if defined( USE_ENVMAP )
	vec3 getLightProbeIndirectIrradiance( /*const in SpecularLightProbe specularLightProbe,*/ const in GeometricContext geometry, const in int maxMIPLevel ) {

		vec3 worldNormal = inverseTransformDirection( geometry.normal, viewMatrix );

		#ifdef ENVMAP_TYPE_CUBE

			vec3 queryVec = vec3( flipEnvMap * worldNormal.x, worldNormal.yz );

			// TODO: replace with properly filtered cubemaps and access the irradiance LOD level, be it the last LOD level
			// of a specular cubemap, or just the default level of a specially created irradiance cubemap.

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = textureCubeLodEXT( envMap, queryVec, float( maxMIPLevel ) );

			#else

				// force the bias high to get the last LOD level as it is the most blurred.
				vec4 envMapColor = textureCube( envMap, queryVec, float( maxMIPLevel ) );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_CUBE_UV )

			vec3 queryVec = vec3( flipEnvMap * worldNormal.x, worldNormal.yz );
			vec4 envMapColor = textureCubeUV(envMap, queryVec, 1.0 );

		#elif defined( ENVMAP_TYPE_LATLONG_UV )

			vec3 queryVec = vec3( flipEnvMap * worldNormal.x, worldNormal.yz );
			vec4 envMapColor = textureLatlongUV(envMap, queryVec, 1.0);

		#else

			vec4 envMapColor = vec4( 0.0 );

		#endif

		return PI * envMapColor.rgb * envMapIntensity;

	}

	float getSpecularMIPLevel( const in float blinnShininessExponent, const in int maxMIPLevel ) {

		//float envMapWidth = pow( 2.0, maxMIPLevelScalar );
		//float desiredMIPLevel = log2( envMapWidth * sqrt( 3.0 ) ) - 0.5 * log2( pow2( blinnShininessExponent ) + 1.0 );

		float maxMIPLevelScalar = float( maxMIPLevel );
		float desiredMIPLevel = maxMIPLevelScalar - 0.79248 - 0.5 * log2( pow2( blinnShininessExponent ) + 1.0 );

		// clamp to allowable LOD ranges.
		return clamp( desiredMIPLevel, 0.0, maxMIPLevelScalar );

	}

	vec3 getLightProbeIndirectRadiance( /*const in SpecularLightProbe specularLightProbe,*/ const in GeometricContext geometry, const in float blinnShininessExponent, const in int maxMIPLevel ) {
		#ifdef ENVMAP_MODE_REFLECTION

			vec3 reflectVec = reflect( -geometry.viewDir, geometry.normal );

		#else

			vec3 reflectVec = refract( -geometry.viewDir, geometry.normal, refractionRatio );

		#endif

		reflectVec = inverseTransformDirection( reflectVec, viewMatrix );

		float specularMIPLevel = getSpecularMIPLevel( blinnShininessExponent, maxMIPLevel );

		#ifdef ENVMAP_TYPE_CUBE

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = textureCubeLodEXT( envMap, queryReflectVec, specularMIPLevel );

			#else

				vec4 envMapColor = textureCube( envMap, queryReflectVec, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_CUBE_UV )

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );
			vec4 envMapColor = textureCubeUV(envMap, queryReflectVec, BlinnExponentToGGXRoughness(blinnShininessExponent));
		#elif defined( ENVMAP_TYPE_LATLONG_UV )

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );
			vec4 envMapColor = textureLatlongUV(envMap, queryReflectVec, BlinnExponentToGGXRoughness(blinnShininessExponent));

		#elif defined( ENVMAP_TYPE_EQUIREC )

			vec2 sampleUV;
			sampleUV.y = saturate( reflectVec.y * 0.5 + 0.5 );
			sampleUV.x = atan( reflectVec.z, reflectVec.x ) * RECIPROCAL_PI2 + 0.5;

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, sampleUV, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, sampleUV, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_SPHERE )

			vec3 reflectView = normalize( ( viewMatrix * vec4( reflectVec, 0.0 ) ).xyz + vec3( 0.0,0.0,1.0 ) );

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#endif

		return envMapColor.rgb * envMapIntensity;

	}

	vec3 getLightProbeIndirectRadianceAniso( /*const in SpecularLightProbe specularLightProbe,*/ const in GeometricContext geometry, const in float roughness, const in float anisotropy, const in int maxMIPLevel ) {

		vec3 X = normalize(cross(geometry.normal, vec3(1,0,0)));
		vec3 Y = normalize(cross(geometry.normal, X));

		vec3 ax = cross(-geometry.viewDir, Y);
		vec3 ay = cross(ax, Y);

		vec3 bentNormal = mix(geometry.normal, normalize(mix(geometry.normal, ay, anisotropy * anisotropy)), roughness);

		#ifdef ENVMAP_MODE_REFLECTION

			vec3 reflectVec = reflect( -geometry.viewDir, bentNormal );

		#else

			vec3 reflectVec = refract( -geometry.viewDir, bentNormal, refractionRatio );

		#endif

		reflectVec = inverseTransformDirection( reflectVec, viewMatrix );

		float blinnShininessExponent = GGXRoughnessToBlinnExponent(roughness);
		float specularMIPLevel = getSpecularMIPLevel( blinnShininessExponent, maxMIPLevel );

		#ifdef ENVMAP_TYPE_CUBE

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = textureCubeLodEXT( envMap, queryReflectVec, specularMIPLevel );

			#else

				vec4 envMapColor = textureCube( envMap, queryReflectVec, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_CUBE_UV )

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );
			vec4 envMapColor = textureCubeUV(envMap, queryReflectVec, BlinnExponentToGGXRoughness(blinnShininessExponent));
		#elif defined( ENVMAP_TYPE_LATLONG_UV )

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );
			vec4 envMapColor = textureLatlongUV(envMap, queryReflectVec, BlinnExponentToGGXRoughness(blinnShininessExponent));

		#elif defined( ENVMAP_TYPE_EQUIREC )

			vec2 sampleUV;
			sampleUV.y = saturate( reflectVec.y * 0.5 + 0.5 );
			sampleUV.x = atan( reflectVec.z, reflectVec.x ) * RECIPROCAL_PI2 + 0.5;

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, sampleUV, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, sampleUV, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_SPHERE )

			vec3 reflectView = normalize( ( viewMatrix * vec4( reflectVec, 0.0 ) ).xyz + vec3( 0.0,0.0,1.0 ) );

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#endif

			envMapColor.rgb = LinearToLinear( envMapColor ).rgb;

		#endif

		return envMapColor.rgb * envMapIntensity;

	}

#endif
)";
static char* lights_physical_pars_fragment = R"(
struct PhysicalMaterial {

	vec3	diffuseColor;
	float	roughness;
	float	anisotropy;
	vec3	specularColor;

	#ifndef STANDARD
		float sheen;
		float subsurface;
		float clearCoat;
		float clearCoatRoughness;
	#endif

};

#define MAXIMUM_SPECULAR_COEFFICIENT 0.16
#define DEFAULT_SPECULAR_COEFFICIENT 0.04

// Clear coat directional hemishperical reflectance (this approximation should be improved)
float clearCoatDHRApprox( const in float roughness, const in float dotNL ) {

	return DEFAULT_SPECULAR_COEFFICIENT + ( 1.0 - DEFAULT_SPECULAR_COEFFICIENT ) * ( pow( 1.0 - dotNL, 5.0 ) * pow( 1.0 - roughness, 2.0 ) );

}

#if NUM_RECT_AREA_LIGHTS > 0

	void RE_Direct_RectArea_Physical( const in RectAreaLight rectAreaLight, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

		vec3 normal = geometry.normal;
		vec3 viewDir = geometry.viewDir;
		vec3 position = geometry.position;
		vec3 lightPos = rectAreaLight.position;
		vec3 halfWidth = rectAreaLight.halfWidth;
		vec3 halfHeight = rectAreaLight.halfHeight;
		vec3 lightColor = rectAreaLight.color;
		float roughness = material.roughness;

		vec3 rectCoords[ 4 ];
		rectCoords[ 0 ] = lightPos - halfWidth - halfHeight; // counterclockwise
		rectCoords[ 1 ] = lightPos + halfWidth - halfHeight;
		rectCoords[ 2 ] = lightPos + halfWidth + halfHeight;
		rectCoords[ 3 ] = lightPos - halfWidth + halfHeight;

		vec2 uv = LTC_Uv( normal, viewDir, roughness );

		float norm = texture2D( ltcMag, uv ).a;

		vec4 t = texture2D( ltcMat, uv );

		mat3 mInv = mat3(
			vec3(   1,   0, t.y ),
			vec3(   0, t.z,   0 ),
			vec3( t.w,   0, t.x )
		);

		reflectedLight.directSpecular += lightColor * material.specularColor * norm * LTC_Evaluate( normal, viewDir, position, mInv, rectCoords ); // no fresnel

		reflectedLight.directDiffuse += lightColor * material.diffuseColor * LTC_Evaluate( normal, viewDir, position, mat3( 1 ), rectCoords );

	}

#endif

void RE_Direct_Physical( const in IncidentLight directLight, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

	float dotNL = saturate( dot( geometry.normal, directLight.direction ) );

	vec3 irradiance = dotNL * directLight.color;

	#ifndef PHYSICALLY_CORRECT_LIGHTS

		irradiance *= PI; // punctual light

	#endif

	vec3 spec = BRDF_Specular_GGX_Aniso( directLight, geometry, material.specularColor, material.roughness, material.anisotropy);

	#ifndef STANDARD
		float clearCoatDHR = material.clearCoat * clearCoatDHRApprox( material.clearCoatRoughness, dotNL );
		vec3 sheenSpec = BRDF_Specular_Sheen(directLight, geometry, material.specularColor, material.roughness);
		reflectedLight.directSpecular += ( 1.0 - clearCoatDHR ) * irradiance * mix(spec, sheenSpec, material.sheen);
		reflectedLight.directSpecular += irradiance * material.clearCoat * BRDF_Specular_GGX( directLight, geometry, vec3( DEFAULT_SPECULAR_COEFFICIENT ), material.clearCoatRoughness );
	#else
		float clearCoatDHR = 0.0;
		reflectedLight.directSpecular += ( 1.0 - clearCoatDHR ) * irradiance * spec;
	#endif

	reflectedLight.directDiffuse += ( 1.0 - clearCoatDHR ) * irradiance * BRDF_Diffuse_Burley(directLight, geometry, material.diffuseColor, material.roughness, material.subsurface );
}

void RE_IndirectDiffuse_Physical( const in vec3 irradiance, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

	reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor ) * (1 - BRDF_Specular_GGX_Environment( geometry, material.specularColor, material.roughness ));

}

void RE_IndirectSpecular_Physical( const in vec3 radiance, const in vec3 clearCoatRadiance, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

	#ifndef STANDARD
		float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
		float dotNL = dotNV;
		float clearCoatDHR = material.clearCoat * clearCoatDHRApprox( material.clearCoatRoughness, dotNL );
	#else
		float clearCoatDHR = 0.0;
	#endif

	vec3 spec = BRDF_Specular_GGX_Environment( geometry, material.specularColor, material.roughness );

	#ifndef STANDARD
		vec3 sheenSpec = BRDF_Specular_Sheen_Environment( geometry, material.specularColor, material.roughness );
		reflectedLight.indirectSpecular += ( 1.0 - clearCoatDHR ) * radiance * mix(spec, sheenSpec, material.sheen);
	#else
		reflectedLight.indirectSpecular += ( 1.0 - clearCoatDHR ) * radiance * spec;
	#endif

	#ifndef STANDARD

		reflectedLight.indirectSpecular += clearCoatRadiance * material.clearCoat * BRDF_Specular_GGX_Environment( geometry, vec3( DEFAULT_SPECULAR_COEFFICIENT ), material.clearCoatRoughness );

	#endif

}

#define RE_Direct				RE_Direct_Physical
#define RE_Direct_RectArea		RE_Direct_RectArea_Physical
#define RE_IndirectDiffuse		RE_IndirectDiffuse_Physical
#define RE_IndirectSpecular		RE_IndirectSpecular_Physical

#define Material_BlinnShininessExponent( material )   GGXRoughnessToBlinnExponent( material.roughness )
#define Material_ClearCoat_BlinnShininessExponent( material )   GGXRoughnessToBlinnExponent( material.clearCoatRoughness )

// ref: http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
float computeSpecularOcclusion( const in float dotNV, const in float ambientOcclusion, const in float roughness ) {

	return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + ambientOcclusion );

}
)";

static char* lights_physical_fragment = R"(
PhysicalMaterial material;
material.diffuseColor = diffuseColor.rgb * ( 1.0 - metalnessFactor );
material.roughness = clamp( roughnessFactor, 0.04, 1.0 );
#ifdef STANDARD
	material.specularColor = mix( vec3( DEFAULT_SPECULAR_COEFFICIENT ), diffuseColor.rgb, metalnessFactor );
#else
	material.specularColor = mix( vec3( MAXIMUM_SPECULAR_COEFFICIENT * pow2( specularFactor ) ), diffuseColor.rgb, metalnessFactor );
	material.sheen = sheenFactor;
	material.subsurface = subsurfaceFactor;
	material.anisotropy = anisotropyFactor;
	material.clearCoat = saturate( clearCoatFactor ); // Burley clearcoat model
	material.clearCoatRoughness = clamp( clearCoatRoughnessFactor, 0.04, 1.0 );
#endif
)";
static char* lights_template = R"(
/**
 * This is a template that can be used to light a material, it uses pluggable
 * RenderEquations (RE)for specific lighting scenarios.
 *
 * Instructions for use:
 * - Ensure that both RE_Direct, RE_IndirectDiffuse and RE_IndirectSpecular are defined
 * - If you have defined an RE_IndirectSpecular, you need to also provide a Material_LightProbeLOD. <---- ???
 * - Create a material parameter that is to be passed as the third parameter to your lighting functions.
 *
 * TODO:
 * - Add area light support.
 * - Add sphere light support.
 * - Add diffuse light probe (irradiance cubemap) support.
 */

GeometricContext geometry;

geometry.position = - vViewPosition;
geometry.normal = normal;
geometry.viewDir = normalize( vViewPosition );

IncidentLight directLight;

#if ( NUM_POINT_LIGHTS > 0 ) && defined( RE_Direct )

	PointLight pointLight;

	for ( int i = 0; i < NUM_POINT_LIGHTS; i ++ ) {

		pointLight = pointLights[ i ];

		getPointDirectLightIrradiance( pointLight, geometry, directLight );

		#ifdef USE_SHADOWMAP
		directLight.color *= all( bvec2( pointLight.shadow, directLight.visible ) ) ? getPointShadow( pointShadowMap[ i ], pointLight.shadowMapSize, pointLight.shadowBias, pointLight.shadowRadius, vPointShadowCoord[ i ] ) : 1.0;
		#endif

		RE_Direct( directLight, geometry, material, reflectedLight );

	}

#endif

#if ( NUM_SPOT_LIGHTS > 0 ) && defined( RE_Direct )

	SpotLight spotLight;

	for ( int i = 0; i < NUM_SPOT_LIGHTS; i ++ ) {

		spotLight = spotLights[ i ];

		getSpotDirectLightIrradiance( spotLight, geometry, directLight );

		#ifdef USE_SHADOWMAP
		directLight.color *= all( bvec2( spotLight.shadow, directLight.visible ) ) ? getShadow( spotShadowMap[ i ], spotLight.shadowMapSize, spotLight.shadowBias, spotLight.shadowRadius, vSpotShadowCoord[ i ] ) : 1.0;
		#endif

		RE_Direct( directLight, geometry, material, reflectedLight );

	}

#endif

#if ( NUM_DIR_LIGHTS > 0 ) && defined( RE_Direct )

	DirectionalLight directionalLight;

	for ( int i = 0; i < NUM_DIR_LIGHTS; i ++ ) {
		directionalLight.direction = directionalLights.lights[i].direction;
		directionalLight.color = directionalLights.lights[i].color;
		directionalLight.shadow = directionalLights.lights[i].shadow;
		directionalLight.shadowBias = directionalLights.lights[i].shadowBias;
		directionalLight.shadowRadius = directionalLights.lights[i].shadowRadius;
		directionalLight.shadowMapSize = directionalLights.lights[i].shadowMapSize;

		getDirectionalDirectLightIrradiance( directionalLight, geometry, directLight );

		#ifdef USE_SHADOWMAP
		directLight.color *= all( bvec2( directionalLight.shadow, directLight.visible ) ) ? getShadow( directionalShadowMap[ i ], directionalLight.shadowMapSize, directionalLight.shadowBias, directionalLight.shadowRadius, vDirectionalShadowCoord[ i ] ) : 1.0;
		#endif

		RE_Direct( directLight, geometry, material, reflectedLight );

	}

#endif

#if ( NUM_RECT_AREA_LIGHTS > 0 ) && defined( RE_Direct_RectArea )

	RectAreaLight rectAreaLight;

	for ( int i = 0; i < NUM_RECT_AREA_LIGHTS; i ++ ) {

		rectAreaLight = rectAreaLights[ i ];
		RE_Direct_RectArea( rectAreaLight, geometry, material, reflectedLight );

	}

#endif

#if defined( RE_IndirectDiffuse )

	vec3 irradiance = getAmbientLightIrradiance( ambientLightColor );

	if (lightMapEnable)
	{
		vec3 lightMapIrradiance = texture2D( lightMap, vUv2 ).xyz * lightMapIntensity;

		#ifndef PHYSICALLY_CORRECT_LIGHTS
			lightMapIrradiance *= PI;
		#endif

		irradiance += lightMapIrradiance;
	}

	#if ( NUM_HEMI_LIGHTS > 0 )

		for ( int i = 0; i < NUM_HEMI_LIGHTS; i ++ ) {

			irradiance += getHemisphereLightIrradiance( hemisphereLights[ i ], geometry );

		}

	#endif

	#if defined( USE_ENVMAP ) && defined( PHYSICAL )

		// TODO, replace 7 with the real maxMIPLevel
		irradiance += getLightProbeIndirectIrradiance( /*lightProbe,*/ geometry, 7 );

	#endif

	RE_IndirectDiffuse( irradiance, geometry, material, reflectedLight );
#endif

#if defined( USE_ENVMAP ) && defined( RE_IndirectSpecular )

	// TODO, replace 7 with the real maxMIPLevel
	vec3 radiance = getLightProbeIndirectRadianceAniso( /*specularLightProbe,*/ geometry, material.roughness, material.anisotropy, 7 );

	#ifndef STANDARD
		vec3 clearCoatRadiance = getLightProbeIndirectRadiance( /*specularLightProbe,*/ geometry, Material_ClearCoat_BlinnShininessExponent( material ), 7 );
	#else
		vec3 clearCoatRadiance = vec3( 0.0 );
	#endif

	RE_IndirectSpecular( radiance, clearCoatRadiance, geometry, material, reflectedLight );

#endif
)";

static char* shadowmap_vertex = R"(
#ifdef USE_SHADOWMAP

	#if NUM_DIR_LIGHTS > 0

	for ( int i = 0; i < NUM_DIR_LIGHTS; i ++ ) {

		vDirectionalShadowCoord[ i ] = directionalShadowMatrix[ i ] * worldPosition;

	}

	#endif

	#if NUM_SPOT_LIGHTS > 0

	for ( int i = 0; i < NUM_SPOT_LIGHTS; i ++ ) {

		vSpotShadowCoord[ i ] = spotShadowMatrix[ i ] * worldPosition;

	}

	#endif

	#if NUM_POINT_LIGHTS > 0

	for ( int i = 0; i < NUM_POINT_LIGHTS; i ++ ) {

		vPointShadowCoord[ i ] = pointShadowMatrix[ i ] * worldPosition;

	}

	#endif

	/*
	#if NUM_RECT_AREA_LIGHTS > 0

		// TODO (abelnation): update vAreaShadowCoord with area light info

	#endif
	*/

#endif
)";
static char* shadowmap_pars_vertex = R"(
#ifdef USE_SHADOWMAP

	#if NUM_DIR_LIGHTS > 0

		uniform mat4 directionalShadowMatrix[ NUM_DIR_LIGHTS ];
		out vec4 vDirectionalShadowCoord[ NUM_DIR_LIGHTS ];

	#endif

	#if NUM_SPOT_LIGHTS > 0

		uniform mat4 spotShadowMatrix[ NUM_SPOT_LIGHTS ];
		out vec4 vSpotShadowCoord[ NUM_SPOT_LIGHTS ];

	#endif

	#if NUM_POINT_LIGHTS > 0

		uniform mat4 pointShadowMatrix[ NUM_POINT_LIGHTS ];
		out vec4 vPointShadowCoord[ NUM_POINT_LIGHTS ];

	#endif

	/*
	#if NUM_RECT_AREA_LIGHTS > 0

		// TODO (abelnation): uniforms for area light shadows

	#endif
	*/

#endif

)";
static char* shadowmap_pars_fragment = R"(
#ifdef USE_SHADOWMAP

	#if NUM_DIR_LIGHTS > 0

		uniform sampler2D directionalShadowMap[ NUM_DIR_LIGHTS ];
		in vec4 vDirectionalShadowCoord[ NUM_DIR_LIGHTS ];

	#endif

	#if NUM_SPOT_LIGHTS > 0

		uniform sampler2D spotShadowMap[ NUM_SPOT_LIGHTS ];
		in vec4 vSpotShadowCoord[ NUM_SPOT_LIGHTS ];

	#endif

	#if NUM_POINT_LIGHTS > 0

		uniform sampler2D pointShadowMap[ NUM_POINT_LIGHTS ];
		in vec4 vPointShadowCoord[ NUM_POINT_LIGHTS ];

	#endif

	/*
	#if NUM_RECT_AREA_LIGHTS > 0

		// TODO (abelnation): create uniforms for area light shadows

	#endif
	*/

	float texture2DCompare( sampler2D depths, vec2 uv, float compare ) {

		return step( compare, unpackRGBAToDepth(texture2D( depths, uv )));

	}

	float texture2DShadowLerp( sampler2D depths, vec2 size, vec2 uv, float compare ) {

		const vec2 offset = vec2( 0.0, 1.0 );

		vec2 texelSize = vec2( 1.0 ) / size;
		vec2 centroidUV = floor( uv * size + 0.5 ) / size;

		float lb = texture2DCompare( depths, centroidUV + texelSize * offset.xx, compare );
		float lt = texture2DCompare( depths, centroidUV + texelSize * offset.xy, compare );
		float rb = texture2DCompare( depths, centroidUV + texelSize * offset.yx, compare );
		float rt = texture2DCompare( depths, centroidUV + texelSize * offset.yy, compare );

		vec2 f = fract( uv * size + 0.5 );

		float a = mix( lb, lt, f.y );
		float b = mix( rb, rt, f.y );
		float c = mix( a, b, f.x );

		return c;

	}

	float getShadow( sampler2D shadowMap, vec2 shadowMapSize, float shadowBias, float shadowRadius, vec4 shadowCoord ) {

		float shadow = 1.0;

		shadowCoord.xyz /= shadowCoord.w;
		shadowCoord.z += shadowBias;

		// if ( something && something ) breaks ATI OpenGL shader compiler
		// if ( all( something, something ) ) using this instead

		bvec4 inFrustumVec = bvec4 ( shadowCoord.x >= 0.0, shadowCoord.x <= 1.0, shadowCoord.y >= 0.0, shadowCoord.y <= 1.0 );
		bool inFrustum = all( inFrustumVec );

		bvec2 frustumTestVec = bvec2( inFrustum, shadowCoord.z <= 1.0 );

		bool frustumTest = all( frustumTestVec );

		if ( frustumTest ) {

		#if defined( SHADOWMAP_TYPE_PCF )

			vec2 texelSize = vec2( 1.0 ) / shadowMapSize;

			float dx0 = - texelSize.x * shadowRadius;
			float dy0 = - texelSize.y * shadowRadius;
			float dx1 = + texelSize.x * shadowRadius;
			float dy1 = + texelSize.y * shadowRadius;

			shadow = (
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy, shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, dy1 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy1 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, dy1 ), shadowCoord.z )
			) * ( 1.0 / 9.0 );

		#elif defined( SHADOWMAP_TYPE_PCF_SOFT )

			vec2 texelSize = vec2( 1.0 ) / shadowMapSize;

			float dx0 = - texelSize.x * shadowRadius;
			float dy0 = - texelSize.y * shadowRadius;
			float dx1 = + texelSize.x * shadowRadius;
			float dy1 = + texelSize.y * shadowRadius;

			shadow = (
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx0, dy0 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( 0.0, dy0 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx1, dy0 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx0, 0.0 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy, shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx1, 0.0 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx0, dy1 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( 0.0, dy1 ), shadowCoord.z ) +
				texture2DShadowLerp( shadowMap, shadowMapSize, shadowCoord.xy + vec2( dx1, dy1 ), shadowCoord.z )
			) * ( 1.0 / 9.0 );

		#else // no percentage-closer filtering:

			shadow = texture2DCompare( shadowMap, shadowCoord.xy, shadowCoord.z );

		#endif

		}

		return shadow;

	}

	// cubeToUV() maps a 3D direction vector suitable for cube texture mapping to a 2D
	// vector suitable for 2D texture mapping. This code uses the following layout for the
	// 2D texture:
	//
	// xzXZ
	//  y Y
	//
	// Y - Positive y direction
	// y - Negative y direction
	// X - Positive x direction
	// x - Negative x direction
	// Z - Positive z direction
	// z - Negative z direction
	//
	// Source and test bed:
	// https://gist.github.com/tschw/da10c43c467ce8afd0c4

	vec2 cubeToUV( vec3 v, float texelSizeY ) {

		// Number of texels to avoid at the edge of each square

		vec3 absV = abs( v );

		// Intersect unit cube

		float scaleToCube = 1.0 / max( absV.x, max( absV.y, absV.z ) );
		absV *= scaleToCube;

		// Apply scale to avoid seams

		// two texels less per square (one texel will do for NEAREST)
		v *= scaleToCube * ( 1.0 - 2.0 * texelSizeY );

		// Unwrap

		// space: -1 ... 1 range for each square
		//
		// #X##		dim    := ( 4 , 2 )
		//  # #		center := ( 1 , 1 )

		vec2 planar = v.xy;

		float almostATexel = 1.5 * texelSizeY;
		float almostOne = 1.0 - almostATexel;

		if ( absV.z >= almostOne ) {

			if ( v.z > 0.0 )
				planar.x = 4.0 - v.x;

		} else if ( absV.x >= almostOne ) {

			float signX = sign( v.x );
			planar.x = v.z * signX + 2.0 * signX;

		} else if ( absV.y >= almostOne ) {

			float signY = sign( v.y );
			planar.x = v.x + 2.0 * signY + 2.0;
			planar.y = v.z * signY - 2.0;

		}

		// Transform to UV space

		// scale := 0.5 / dim
		// translate := ( center + 0.5 ) / dim
		return vec2( 0.125, 0.25 ) * planar + vec2( 0.375, 0.75 );

	}

	float getPointShadow( sampler2D shadowMap, vec2 shadowMapSize, float shadowBias, float shadowRadius, vec4 shadowCoord ) {

		vec2 texelSize = vec2( 1.0 ) / ( shadowMapSize * vec2( 4.0, 2.0 ) );

		// for point lights, the uniform @vShadowCoord is re-purposed to hold
		// the distance from the light to the world-space position of the fragment.
		vec3 lightToPosition = shadowCoord.xyz;

		// bd3D = base direction 3D
		vec3 bd3D = normalize( lightToPosition );
		// dp = distance from light to fragment position
		float dp = ( length( lightToPosition ) - shadowBias ) / 1000.0;

		#if defined( SHADOWMAP_TYPE_PCF ) || defined( SHADOWMAP_TYPE_PCF_SOFT )

			vec2 offset = vec2( - 1, 1 ) * shadowRadius * texelSize.y;

			return (
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xyy, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yyy, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xyx, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yyx, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xxy, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yxy, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xxx, texelSize.y ), dp ) +
				texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yxx, texelSize.y ), dp )
			) * ( 1.0 / 9.0 );

		#else // no percentage-closer filtering

			return texture2DCompare( shadowMap, cubeToUV( bd3D, texelSize.y ), dp );

		#endif

	}

#endif
)";

static std::unordered_map<std::string, std::string_view> ShaderChunk = {
	{"common", common },
	{"packing", packing },
	{"cube_uv_reflection_fragment", cube_uv_reflection_fragment},
	{"encodings_pars_fragment", encodings_pars_fragment },
	{"color_vertex", color_vertex },
	{"color_pars_vertex", color_pars_vertex },
	{"color_pars_fragment", color_pars_fragment },
	{"color_fragment", color_fragment },
	{"uv_vertex", uv_vertex },
	{"uv_pars_vertex", uv_pars_vertex },
	{"uv_pars_fragment", uv_pars_fragment },
	{"uv2_vertex", uv2_vertex },
	{"uv2_pars_vertex", uv2_pars_vertex },
	{"uv2_pars_fragment", uv2_pars_fragment },
	{"beginnormal_vertex", beginnormal_vertex},
	{"defaultnormal_vertex", defaultnormal_vertex},
	{"normal_pars_vertex", normal_pars_vertex},
	{"normal_flip", normal_flip},
	{"normal_fragment", normal_fragment},
	{"normal_pars_fragment", normal_pars_fragment},
	{"normalmap_pars_fragment", normalmap_pars_fragment},
	{"begin_vertex", begin_vertex },
	{"project_vertex", project_vertex },
	{"worldpos_vertex", worldpos_vertex },
	{"map_fragment", map_fragment },
	{"map_pars_fragment", map_pars_fragment },
	{"map_particle_pars_fragment", map_particle_pars_fragment },
	{"map_particle_fragment", map_particle_fragment },
	{"premultiplied_alpha_fragment", premultiplied_alpha_fragment },
	{"tonemapping_pars_fragment", tonemapping_pars_fragment },
	{"tonemapping_fragment", tonemapping_fragment },
	{"alphatest_fragment", alphatest_fragment },
	{"opacitymap_pars_fragment", opacitymap_pars_fragment },
	{"opacitymap_fragment", opacitymap_fragment },
	{"lightmap_pars_fragment", lightmap_pars_fragment },
	{"aomap_pars_fragment", aomap_pars_fragment },
	{"envmap_pars_fragment", envmap_pars_fragment },
	{"bumpmap_pars_fragment", bumpmap_pars_fragment},
	{"smoothnessmap_pars_fragment", smoothnessmap_pars_fragment},
	{"roughnessmap_pars_fragment", roughnessmap_pars_fragment},
	{"roughnessmap_fragment", roughnessmap_fragment},
	{"specularmap_pars_fragment", specularmap_pars_fragment},
	{"specularmap_fragment", specularmap_fragment},
	{"anisotropymap_fragment", anisotropymap_fragment},
	{"anisotropymap_pars_fragment", anisotropymap_pars_fragment},
	{"metalnessmap_fragment", metalnessmap_fragment},
	{"metalnessmap_pars_fragment", metalnessmap_pars_fragment},
	{"sheenmap_fragment", sheenmap_fragment},
	{"sheenmap_pars_fragment", sheenmap_pars_fragment},
	{"subsurfacemap_pars_fragment", subsurfacemap_pars_fragment},
	{"subsurfacemap_fragment", subsurfacemap_fragment},	
	{"clearcoatmap_pars_fragment", clearcoatmap_pars_fragment},
	{"clearcoatRoughnessmap_pars_fragment", clearcoatRoughnessmap_pars_fragment},
	{"clearcoatmap_fragment", clearcoatmap_fragment},
	{"clearcoatRoughnessmap_fragment", clearcoatRoughnessmap_fragment},
	{"emissivemap_fragment", emissivemap_fragment},
	{"emissivemap_pars_fragment", emissivemap_pars_fragment },	
	{"bsdfs", bsdfs },
	{"lights_pars", lights_pars },
	{"lights_physical_pars_fragment", lights_physical_pars_fragment },
	{"lights_physical_fragment", lights_physical_fragment },
	{"lights_template", lights_template },
	{"shadowmap_vertex", shadowmap_vertex},
	{"shadowmap_pars_vertex", shadowmap_pars_vertex},
	{"shadowmap_pars_fragment", shadowmap_pars_fragment},
};

namespace octoon::video
{
	ForwardMaterial::ForwardMaterial() noexcept
	{
	}

	ForwardMaterial::ForwardMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept
	{
		this->setMaterial(material, context);
	}

	ForwardMaterial::~ForwardMaterial() noexcept
	{
		material_.reset();

		ambientLightColor_.reset();
		directionalLights_.reset();
		pointLights_.reset();
		spotLights_.reset();
		rectAreaLights_.reset();
		hemisphereLights_.reset();
		flipEnvMap_.reset();
		envMap_.reset();
		envMapIntensity_.reset();

		viewMatrix_.reset();
		normalMatrix_.reset();
		modelMatrix_.reset();
		modelViewMatrix_.reset();
		projectionMatrix_.reset();

		program_.reset();
		renderState_.reset();
		descriptorSet_.reset();
		pipeline_.reset();
	}

	void
	ForwardMaterial::setMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept
	{
		if (this->material_ != material)
		{
			this->material_ = material;
			this->updateMaterial(this->material_, context);
		}
	}
		
	const material::MaterialPtr&
	ForwardMaterial::getMaterial() const noexcept
	{
		return this->material_;
	}

	const hal::GraphicsPipelinePtr&
	ForwardMaterial::getPipeline() const noexcept
	{
		return pipeline_;
	}

	const hal::GraphicsDescriptorSetPtr&
	ForwardMaterial::getDescriptorSet() const noexcept
	{
		return descriptorSet_;
	}

	void
	ForwardMaterial::update(const camera::Camera& camera, const geometry::Geometry& geometry, const ForwardScene& context) noexcept
	{
		if (this->material_)
		{
			if (this->modelMatrix_)
				this->modelMatrix_->uniform4fmat(geometry.getTransform());
			
			if (this->viewMatrix_)
				this->viewMatrix_->uniform4fmat(camera.getView());

			if (this->viewProjMatrix_)
				this->viewProjMatrix_->uniform4fmat(camera.getViewProjection());

			if (this->modelViewMatrix_)
				this->modelViewMatrix_->uniform4fmat(camera.getView() * geometry.getTransform());
			
			if (this->projectionMatrix_)
				this->projectionMatrix_->uniform4fmat(camera.getProjection());
			
			if (this->normalMatrix_)
				this->normalMatrix_->uniform3fmat((math::float3x3)camera.getView() * (math::float3x3)geometry.getTransform());

			if (this->ambientLightColor_)
				this->ambientLightColor_->uniform3f(context.ambientLightColors);

			if (this->spotLights_)
				this->spotLights_->uniformBuffer(context.spotLightBuffer);

			if (this->pointLights_)
				this->pointLights_->uniformBuffer(context.pointLightBuffer);

			if (this->rectAreaLights_)
				this->rectAreaLights_->uniformBuffer(context.rectangleLightBuffer);

			if (this->directionalLights_)
				this->directionalLights_->uniformBuffer(context.directionLightBuffer);

			if (this->flipEnvMap_)
				this->flipEnvMap_->uniform1f(1.0f);

			if (this->envMap_ && context.environmentLights.size())
				this->envMap_->uniformTexture(context.environmentLights.front().radiance.lock());

			if (this->envMapIntensity_&& context.environmentLights.size())
				this->envMapIntensity_->uniform1f(context.environmentLights.front().intensity);

			if (this->directionalShadowMaps_.size() > 0)
			{
				for (std::size_t i = 0, j = 0; i < context.directionalLights.size(); i++)
				{
					auto& it = context.directionalLights[i];
					if (it.shadow)
					{
						this->directionalShadowMaps_[j]->uniformTexture(context.directionalShadows[i]);
						this->directionalShadowMatrixs_[j]->uniform4fmat(context.directionalShadowMatrix[i]);
						j++;
					}
				}
			}

			this->updateParameters();
		}
	}

	void
	ForwardMaterial::parseIncludes(std::string& str)
	{
		std::regex pattern("#include +<([^<>]*)>");
		std::smatch m;

		std::size_t pos = 0;

		std::string::const_iterator start = str.begin();
		std::string::const_iterator end = str.end();

		while (std::regex_search(start, end, m, pattern))
		{
			auto position = m.position() + ShaderChunk[m[1]].length();

			str.replace(pos + m.position(), m[0].length(), ShaderChunk[m[1]]);

			pos += position;

			start = str.begin() + pos;
			end = str.end();
		}
	}

	void
	ForwardMaterial::replaceLightNums(std::string& str, const ForwardScene& parameters)
	{
		auto replace = [](std::string& str, std::string_view patten, std::string_view right)
		{
			auto pos = str.find(patten);
			while (pos != std::string::npos)
			{
				str.replace(pos, patten.length(), right);
				pos = str.find(patten, pos + 1);
			}
		};

		replace(str, "NUM_DIR_LIGHTS", std::to_string(parameters.numDirectional));
		replace(str, "NUM_SPOT_LIGHTS", std::to_string(parameters.numSpot));
		replace(str, "NUM_RECT_AREA_LIGHTS", std::to_string(parameters.numRectangle));
		replace(str, "NUM_POINT_LIGHTS", std::to_string(parameters.numPoint));
		replace(str, "NUM_HEMI_LIGHTS", std::to_string(parameters.numHemi));
	}

	void
	ForwardMaterial::setupProgram(const material::MaterialPtr& material, const ForwardScene& context)
	{
		auto shader = material->getShader();

		std::string vertexShader = "#version 330\n\t";
		vertexShader += R"(
				layout(location = 0) in vec4 POSITION0;
				layout(location = 1) in vec2 TEXCOORD0;
				layout(location = 2) in vec3 NORMAL0;
				layout(location = 3) in vec2 TEXCOORD1;

				uniform mat4 modelMatrix;
				uniform mat4 modelViewMatrix;
				uniform mat4 projectionMatrix;
				uniform mat4 viewMatrix;
				uniform mat4 viewProjMatrix;
				uniform mat3 normalMatrix;
				uniform vec3 cameraPosition;

				#ifdef USE_COLOR
					attribute vec3 color;
				#endif
				
				#define USE_SHADOWMAP
			)";
		vertexShader += shader->vs;

		std::string fragmentShader = "#version 330\n\t";
		fragmentShader += "layout(location  = 0) out vec4 fragColor;\n";
		fragmentShader += "uniform mat4 viewMatrix;\n";
		//fragmentShader += "#define TONE_MAPPING\n";
		fragmentShader += "#define ENVMAP_TYPE_LATLONG_UV\n";
		fragmentShader += "#define ENVMAP_MODE_REFLECTION\n";
		fragmentShader += "#define PHYSICAL\n";
		fragmentShader += "#define SHADOWMAP_TYPE_PCF_SOFT\n";
		fragmentShader += "#define USE_ENVMAP\n";
		fragmentShader += "#define USE_SHADOWMAP\n";

		if (material->isInstanceOf<material::MeshStandardMaterial>())
		{
			auto standard = material->downcast<material::MeshStandardMaterial>();
			if (standard->getColorMap())
				fragmentShader += "#define USE_MAP\n";
			if (standard->getOpacityMap())
				fragmentShader += "#define USE_OPACITYMAP\n";			
			if (standard->getNormalMap())
				fragmentShader += "#define USE_NORMALMAP\n";
			if (standard->getRoughnessMap())
				fragmentShader += "#define USE_ROUGHNESSMAP\n";
			if (standard->getSpecularMap())
				fragmentShader += "#define USE_SPECULARMAP\n";
			if (standard->getMetalnessMap())
				fragmentShader += "#define USE_METALNESSMAP\n";
			if (standard->getAnisotropyMap())
				fragmentShader += "#define USE_ANISOTROPYMAP\n";
			if (standard->getSheenMap())
				fragmentShader += "#define USE_SHEENMAP\n";
			if (standard->getClearCoatMap())
				fragmentShader += "#define USE_CLEARCOATMAP\n";
			if (standard->getClearCoatRoughnessMap())
				fragmentShader += "#define USE_CLEARCOATROUGHNESSMAP\n";
			if (standard->getEmissiveMap())
				fragmentShader += "#define USE_EMISSIVEMAP\n";
		}
		else
		{
			octoon::hal::GraphicsTexturePtr texture;
			if (material->get("map", texture))
				fragmentShader += "#define USE_MAP\n";
			if (material->get("opacityMap", texture))
				fragmentShader += "#define USE_OPACITYMAP\n";
		}

		fragmentShader += shader->fs;

		this->parseIncludes(vertexShader);
		this->parseIncludes(fragmentShader);

		this->replaceLightNums(vertexShader, context);
		this->replaceLightNums(fragmentShader, context);

		hal::GraphicsProgramDesc programDesc;
		programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, vertexShader, "main", hal::GraphicsShaderLang::GLSL)));
		programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, fragmentShader, "main", hal::GraphicsShaderLang::GLSL)));
		this->program_ = Renderer::instance()->createProgram(programDesc);
	}

	void
	ForwardMaterial::setupRenderState(const material::MaterialPtr& material)
	{
		hal::GraphicsStateDesc stateDesc;
		stateDesc.setColorBlends(material->getColorBlends());
		stateDesc.setCullMode(material->getCullMode());
		stateDesc.setPolygonMode(material->getPolygonMode());
		stateDesc.setPrimitiveType(material->getPrimitiveType());
		stateDesc.setFrontFace(material->getFrontFace());
		stateDesc.setScissorTestEnable(material->getScissorTestEnable());
		stateDesc.setLinear2sRGBEnable(material->getLinear2sRGBEnable());
		stateDesc.setMultisampleEnable(material->getMultisampleEnable());
		stateDesc.setRasterizerDiscardEnable(material->getRasterizerDiscardEnable());
		stateDesc.setLineWidth(material->getLineWidth());
		stateDesc.setDepthEnable(material->getDepthEnable());
		stateDesc.setDepthWriteEnable(material->getDepthWriteEnable());
		stateDesc.setDepthBoundsEnable(material->getDepthBoundsEnable());
		stateDesc.setDepthMin(material->getDepthMin());
		stateDesc.setDepthMax(material->getDepthMax());
		stateDesc.setDepthFunc(material->getDepthFunc());
		stateDesc.setDepthBiasEnable(material->getDepthBiasEnable());
		stateDesc.setDepthBias(material->getDepthBias());
		stateDesc.setDepthSlopeScaleBias(material->getDepthSlopeScaleBias());
		stateDesc.setDepthBiasClamp(material->getDepthBiasClamp());
		stateDesc.setDepthClampEnable(material->getDepthClampEnable());
		stateDesc.setStencilEnable(material->getStencilEnable());
		stateDesc.setStencilFrontRef(material->getStencilFrontRef());
		stateDesc.setStencilFrontFunc(material->getStencilFrontFunc());
		stateDesc.setStencilFrontReadMask(material->getStencilFrontReadMask());
		stateDesc.setStencilFrontWriteMask(material->getStencilFrontWriteMask());
		stateDesc.setStencilFrontFail(material->getStencilFrontFail());
		stateDesc.setStencilFrontZFail(material->getStencilFrontZFail());
		stateDesc.setStencilFrontPass(material->getStencilFrontPass());
		stateDesc.setStencilBackRef(material->getStencilBackRef());
		stateDesc.setStencilBackFunc(material->getStencilBackFunc());
		stateDesc.setStencilBackReadMask(material->getStencilBackReadMask());
		stateDesc.setStencilBackWriteMask(material->getStencilBackWriteMask());
		stateDesc.setStencilBackFail(material->getStencilBackFail());
		stateDesc.setStencilBackZFail(material->getStencilBackZFail());
		stateDesc.setStencilBackPass(material->getStencilBackPass());
		stateDesc.setDepthEnable(material->getDepthEnable());
		stateDesc.setDepthWriteEnable(material->getDepthWriteEnable());
		stateDesc.setDepthBoundsEnable(material->getDepthBoundsEnable());
		stateDesc.setDepthMin(material->getDepthMin());
		stateDesc.setDepthMax(material->getDepthMax());
		stateDesc.setDepthFunc(material->getDepthFunc());
		stateDesc.setDepthBiasEnable(material->getDepthBiasEnable());
		stateDesc.setDepthBias(material->getDepthBias());
		stateDesc.setDepthSlopeScaleBias(material->getDepthSlopeScaleBias());
		stateDesc.setDepthBiasClamp(material->getDepthBiasClamp());
		stateDesc.setDepthClampEnable(material->getDepthClampEnable());
		stateDesc.setStencilEnable(material->getStencilEnable());
		stateDesc.setStencilFrontRef(material->getStencilFrontRef());
		stateDesc.setStencilFrontFunc(material->getStencilFrontFunc());
		stateDesc.setStencilFrontReadMask(material->getStencilFrontReadMask());
		stateDesc.setStencilFrontWriteMask(material->getStencilFrontWriteMask());
		stateDesc.setStencilFrontFail(material->getStencilFrontFail());
		stateDesc.setStencilFrontZFail(material->getStencilFrontZFail());
		stateDesc.setStencilFrontPass(material->getStencilFrontPass());
		stateDesc.setStencilBackRef(material->getStencilBackRef());
		stateDesc.setStencilBackFunc(material->getStencilBackFunc());
		stateDesc.setStencilBackReadMask(material->getStencilBackReadMask());
		stateDesc.setStencilBackWriteMask(material->getStencilBackWriteMask());
		stateDesc.setStencilBackFail(material->getStencilBackFail());
		stateDesc.setStencilBackZFail(material->getStencilBackZFail());
		stateDesc.setStencilBackPass(material->getStencilBackPass());
		this->renderState_ = Renderer::instance()->createRenderState(stateDesc);
	}

	void
	ForwardMaterial::updateMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept(false)
	{
		if (material) {
			this->setupRenderState(material);
			this->setupProgram(material, context);

			hal::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "NORMAL", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 1, hal::GraphicsFormat::R32G32SFloat));

			layoutDesc.addVertexBinding(hal::GraphicsVertexBinding(0, layoutDesc.getVertexSize()));

			hal::GraphicsDescriptorSetLayoutDesc descriptor_set_layout;
			descriptor_set_layout.setUniformComponents(this->program_->getActiveParams());

			hal::GraphicsPipelineDesc pipeline;
			pipeline.setGraphicsInputLayout(Renderer::instance()->createInputLayout(layoutDesc));
			pipeline.setGraphicsState(this->renderState_);
			pipeline.setGraphicsProgram(this->program_);
			pipeline.setGraphicsDescriptorSetLayout(Renderer::instance()->createDescriptorSetLayout(descriptor_set_layout));

			pipeline_ = Renderer::instance()->createRenderPipeline(pipeline);
			if (pipeline_)
			{
				hal::GraphicsDescriptorSetDesc descriptorSet;
				descriptorSet.setGraphicsDescriptorSetLayout(pipeline.getDescriptorSetLayout());
				descriptorSet_ = Renderer::instance()->createDescriptorSet(descriptorSet);
				if (!descriptorSet_)
					return;

				auto begin = descriptorSet_->getUniformSets().begin();
				auto end = descriptorSet_->getUniformSets().end();

				auto modelMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "modelMatrix"; });
				if (modelMatrix != end)
					modelMatrix_ = *modelMatrix;

				auto viewMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "viewMatrix"; });
				if (viewMatrix != end)
					viewMatrix_ = *viewMatrix;

				auto viewProjMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "viewProjMatrix"; });
				if (viewProjMatrix != end)
					viewProjMatrix_ = *viewProjMatrix;

				auto normalMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "normalMatrix"; });
				if (normalMatrix != end)
					normalMatrix_ = *normalMatrix;

				auto modelViewMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "modelViewMatrix"; });
				if (modelViewMatrix != end)
					modelViewMatrix_ = *modelViewMatrix;

				auto projectionMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "projectionMatrix"; });
				if (projectionMatrix != end)
					projectionMatrix_ = *projectionMatrix;

				auto ambientLightColor = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambientLightColor"; });
				if (ambientLightColor != end)
					ambientLightColor_ = *ambientLightColor;

				auto directionalLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "DirectionalLights"; });
				if (directionalLights != end)
					directionalLights_ = *directionalLights;

				auto pointLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "pointLights"; });
				if (pointLights != end)
					pointLights_ = *pointLights;

				auto spotLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "spotLights"; });
				if (spotLights != end)
					spotLights_ = *spotLights;

				auto rectAreaLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "rectAreaLights"; });
				if (rectAreaLights != end)
					rectAreaLights_ = *rectAreaLights;

				auto hemisphereLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "hemisphereLights"; });
				if (hemisphereLights != end)
					hemisphereLights_ = *hemisphereLights;
				
				auto flipEnvMap = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "flipEnvMap"; });
				if (flipEnvMap != end)
					flipEnvMap_ = *flipEnvMap;

				auto envmap = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "envMap"; });
				if (envmap != end)
					envMap_ = *envmap;

				auto envmapIntensity = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "envMapIntensity"; });
				if (envmapIntensity != end)
					envMapIntensity_ = *envmapIntensity;

				for (std::size_t i = 0; i < context.directionalLights.size(); i++)
				{
					auto& it = context.directionalLights[i];
					if (it.shadow)
					{
						auto shadowMap = std::find_if(begin, end, [i](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "directionalShadowMap[" + std::to_string(i) + "]"; });
						if (shadowMap != end)
							this->directionalShadowMaps_.emplace_back(*shadowMap);
						auto shadowMatrix = std::find_if(begin, end, [i](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "directionalShadowMatrix[" + std::to_string(i) + "]"; });
						if (shadowMatrix != end)
							this->directionalShadowMatrixs_.emplace_back(*shadowMatrix);
					}
				}

				this->updateParameters(true);
			}
		}
		else
		{
			pipeline_ = nullptr;
		}
	}

	void
	ForwardMaterial::updateParameters(bool force) noexcept
	{
		if (this->material_->isDirty() || force)
		{
			auto begin = descriptorSet_->getUniformSets().begin();
			auto end = descriptorSet_->getUniformSets().end();

			for (auto& prop : material_->getMaterialParams())
			{
				auto it = std::find_if(begin, end, [&](const hal::GraphicsUniformSetPtr& set) { return set->getName() == prop.key; });
				if (it != end)
				{
					auto uniform = *it;
					switch (prop.type)
					{
					case material::PropertyTypeInfoBool:
					{
						auto value = (bool*)prop.data;
						uniform->uniform1b(*value);
					}
					break;
					case material::PropertyTypeInfoInt | material::PropertyTypeInfoBuffer:
					{
						auto value = (int*)prop.data;
						if (prop.length == 4)
							uniform->uniform1i(value[0]);
						else if (prop.length == 8)
							uniform->uniform2i(value[0], value[1]);
						else if (prop.length == 12)
							uniform->uniform3i(value[0], value[1], value[2]);
						else if (prop.length == 16)
							uniform->uniform4i(value[0], value[1], value[2], value[3]);
					}
					break;
					case material::PropertyTypeInfoFloat | material::PropertyTypeInfoBuffer:
					{
						auto value = (float*)prop.data;
						if (prop.length == 4)
							uniform->uniform1f(value[0]);
						else if (prop.length == 8)
							uniform->uniform2f(value[0], value[1]);
						else if (prop.length == 12)
							uniform->uniform3f(value[0], value[1], value[2]);
						else if (prop.length == 16)
							uniform->uniform4f(value[0], value[1], value[2], value[3]);
					}
					break;
					case material::PropertyTypeInfoTexture:
					{
						uniform->uniformTexture(prop.texture);
					}
					break;
					default:
						break;
					}
				}
			}
		}
	}
}