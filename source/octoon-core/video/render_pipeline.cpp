#include <octoon/video/render_pipeline.h>
#include <octoon/video/renderer.h>
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
static char* begin_vertex = R"(
vec3 transformed = vec3( POSITION0 );
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
static char* worldpos_vertex = R"(
#if defined( USE_ENVMAP ) || defined( PHONG ) || defined( PHYSICAL ) || defined( LAMBERT ) || defined ( USE_SHADOWMAP )
	vec4 worldPosition = modelMatrix * vec4( transformed, 1.0 );
#endif
)";
static char* project_vertex = R"(
vec4 mvPosition = modelViewMatrix * vec4( transformed, 1.0 );
gl_Position = projectionMatrix * mvPosition;
)";
static char* map_fragment = R"(
if (mapEnable)
{
	vec4 texelColor = texture2D( map, vUv );
	texelColor = GammaToLinear( texelColor, 2.2);
	diffuseColor *= texelColor;
}
)";
static char* map_pars_fragment = R"(
	uniform sampler2D map;
	uniform bool mapEnable;
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
static char* shadowmap_pars_fragment = R"(
#ifdef USE_SHADOWMAP
	uniform sampler2D shadowMap;
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
#ifdef USE_METALNESSMAP
	vec4 texelRoughness = texture2D( roughnessMap, vUv );
	roughnessFactor *= texelRoughness.r;
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
	metalnessFactor *= texelMetalness.b;
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
static char* alphamap_pars_fragment = R"(
#ifdef USE_ALPHAMAP
	uniform sampler2D alphaMap;
#endif
)";
static char* alphamap_fragment = R"(
#ifdef USE_ALPHAMAP
	diffuseColor.a *= texture2D( alphaMap, vUv ).g;
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
	uniform float reflectivity;
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
#ifdef USE_LIGHTMAP
	uniform sampler2D lightMap;
	uniform float lightMapIntensity;
#endif
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

vec3 F_Schlick( const in vec3 specularColor, const in float dotLH ) {

	// Original approximation by Christophe Schlick '94
	// float fresnel = pow( 1.0 - dotLH, 5.0 );

	// Optimized variant (presented by Epic at SIGGRAPH '13)
	float fresnel = exp2( ( -5.55473 * dotLH - 6.98316 ) * dotLH );

	return ( 1.0 - specularColor ) * fresnel + specularColor;

} // validated

// Microfacet Models for Refraction through Rough Surfaces - equation (34)
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// alpha is "roughness squared" in Disney’s reparameterization
float G_GGX_Smith( const in float alpha, const in float dotNL, const in float dotNV ) {

	// geometry term = G(l)G(v) / 4(n⋅l)(n⋅v)

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

	return RECIPROCAL_PI * a2 / pow2( denom );

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


#if defined( USE_ENVMAP ) && defined( PHYSICAL )
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

			envMapColor.rgb = envMapTexelToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_CUBE_UV )

			vec3 queryVec = vec3( flipEnvMap * worldNormal.x, worldNormal.yz );
			vec4 envMapColor = textureCubeUV( queryVec, 1.0 );

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

			envMapColor.rgb = envMapTexelToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_CUBE_UV )

			vec3 queryReflectVec = vec3( flipEnvMap * reflectVec.x, reflectVec.yz );
			vec4 envMapColor = textureCubeUV(queryReflectVec, BlinnExponentToGGXRoughness(blinnShininessExponent));

		#elif defined( ENVMAP_TYPE_EQUIREC )

			vec2 sampleUV;
			sampleUV.y = saturate( reflectVec.y * 0.5 + 0.5 );
			sampleUV.x = atan( reflectVec.z, reflectVec.x ) * RECIPROCAL_PI2 + 0.5;

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, sampleUV, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, sampleUV, specularMIPLevel );

			#endif

			envMapColor.rgb = envMapTexelToLinear( envMapColor ).rgb;

		#elif defined( ENVMAP_TYPE_SPHERE )

			vec3 reflectView = normalize( ( viewMatrix * vec4( reflectVec, 0.0 ) ).xyz + vec3( 0.0,0.0,1.0 ) );

			#ifdef TEXTURE_LOD_EXT

				vec4 envMapColor = texture2DLodEXT( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#else

				vec4 envMapColor = texture2D( envMap, reflectView.xy * 0.5 + 0.5, specularMIPLevel );

			#endif

			envMapColor.rgb = envMapTexelToLinear( envMapColor ).rgb;

		#endif

		return envMapColor.rgb * envMapIntensity;

	}

#endif
)";
static char* lights_physical_pars_fragment = R"(
struct PhysicalMaterial {

	vec3	diffuseColor;
	float	specularRoughness;
	vec3	specularColor;

	#ifndef STANDARD
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
		float roughness = material.specularRoughness;

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

	#ifndef STANDARD
		float clearCoatDHR = material.clearCoat * clearCoatDHRApprox( material.clearCoatRoughness, dotNL );
	#else
		float clearCoatDHR = 0.0;
	#endif

	reflectedLight.directSpecular += ( 1.0 - clearCoatDHR ) * irradiance * BRDF_Specular_GGX( directLight, geometry, material.specularColor, material.specularRoughness );

	reflectedLight.directDiffuse += ( 1.0 - clearCoatDHR ) * irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );

	#ifndef STANDARD

		reflectedLight.directSpecular += irradiance * material.clearCoat * BRDF_Specular_GGX( directLight, geometry, vec3( DEFAULT_SPECULAR_COEFFICIENT ), material.clearCoatRoughness );

	#endif

}

void RE_IndirectDiffuse_Physical( const in vec3 irradiance, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

	reflectedLight.indirectDiffuse += irradiance * BRDF_Diffuse_Lambert( material.diffuseColor );

}

void RE_IndirectSpecular_Physical( const in vec3 radiance, const in vec3 clearCoatRadiance, const in GeometricContext geometry, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {

	#ifndef STANDARD
		float dotNV = saturate( dot( geometry.normal, geometry.viewDir ) );
		float dotNL = dotNV;
		float clearCoatDHR = material.clearCoat * clearCoatDHRApprox( material.clearCoatRoughness, dotNL );
	#else
		float clearCoatDHR = 0.0;
	#endif

	reflectedLight.indirectSpecular += ( 1.0 - clearCoatDHR ) * radiance * BRDF_Specular_GGX_Environment( geometry, material.specularColor, material.specularRoughness );

	#ifndef STANDARD

		reflectedLight.indirectSpecular += clearCoatRadiance * material.clearCoat * BRDF_Specular_GGX_Environment( geometry, vec3( DEFAULT_SPECULAR_COEFFICIENT ), material.clearCoatRoughness );

	#endif

}

#define RE_Direct				RE_Direct_Physical
#define RE_Direct_RectArea		RE_Direct_RectArea_Physical
#define RE_IndirectDiffuse		RE_IndirectDiffuse_Physical
#define RE_IndirectSpecular		RE_IndirectSpecular_Physical

#define Material_BlinnShininessExponent( material )   GGXRoughnessToBlinnExponent( material.specularRoughness )
#define Material_ClearCoat_BlinnShininessExponent( material )   GGXRoughnessToBlinnExponent( material.clearCoatRoughness )

// ref: http://www.frostbite.com/wp-content/uploads/2014/11/course_notes_moving_frostbite_to_pbr_v2.pdf
float computeSpecularOcclusion( const in float dotNV, const in float ambientOcclusion, const in float roughness ) {

	return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + ambientOcclusion );

}
)";

		static char* lights_physical_fragment = R"(
PhysicalMaterial material;
material.diffuseColor = diffuseColor.rgb * ( 1.0 - metalnessFactor );
material.specularRoughness = clamp( roughnessFactor, 0.04, 1.0 );
#ifdef STANDARD
	material.specularColor = mix( vec3( DEFAULT_SPECULAR_COEFFICIENT ), diffuseColor.rgb, metalnessFactor );
#else
	material.specularColor = mix( vec3( MAXIMUM_SPECULAR_COEFFICIENT * pow2( reflectivity ) ), diffuseColor.rgb, metalnessFactor );
	material.clearCoat = saturate( clearCoat ); // Burley clearcoat model
	material.clearCoatRoughness = clamp( clearCoatRoughness, 0.04, 1.0 );
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

	#ifdef USE_LIGHTMAP

		vec3 lightMapIrradiance = texture2D( lightMap, vUv2 ).xyz * lightMapIntensity;

		#ifndef PHYSICALLY_CORRECT_LIGHTS

			lightMapIrradiance *= PI; // factor of PI should not be present; included here to prevent breakage

		#endif

		irradiance += lightMapIrradiance;

	#endif

	#if ( NUM_HEMI_LIGHTS > 0 )

		for ( int i = 0; i < NUM_HEMI_LIGHTS; i ++ ) {

			irradiance += getHemisphereLightIrradiance( hemisphereLights[ i ], geometry );

		}

	#endif

	#if defined( USE_ENVMAP ) && defined( PHYSICAL ) && defined( ENVMAP_TYPE_CUBE_UV )

		// TODO, replace 8 with the real maxMIPLevel
		irradiance += getLightProbeIndirectIrradiance( /*lightProbe,*/ geometry, 8 );

	#endif

	RE_IndirectDiffuse( irradiance, geometry, material, reflectedLight );

#endif

#if defined( USE_ENVMAP ) && defined( RE_IndirectSpecular )

	// TODO, replace 8 with the real maxMIPLevel
	vec3 radiance = getLightProbeIndirectRadiance( /*specularLightProbe,*/ geometry, Material_BlinnShininessExponent( material ), 8 );

	#ifndef STANDARD
		vec3 clearCoatRadiance = getLightProbeIndirectRadiance( /*specularLightProbe,*/ geometry, Material_ClearCoat_BlinnShininessExponent( material ), 8 );
	#else
		vec3 clearCoatRadiance = vec3( 0.0 );
	#endif

	RE_IndirectSpecular( radiance, clearCoatRadiance, geometry, material, reflectedLight );

#endif

)";
static std::unordered_map<std::string, std::string_view> ShaderChunk = {
	{"common", common },
	{"packing", packing },
	{"color_vertex", color_vertex },
	{"color_pars_vertex", color_pars_vertex },
	{"color_pars_fragment", color_pars_fragment },
	{"color_fragment", color_fragment },
	{"uv_vertex", uv_vertex },
	{"uv_pars_vertex", uv_pars_vertex },
	{"uv_pars_fragment", uv_pars_fragment },
	{"beginnormal_vertex", beginnormal_vertex},
	{"defaultnormal_vertex", defaultnormal_vertex},
	{"normal_pars_vertex", normal_pars_vertex},
	{"normal_flip", normal_flip},
	{"normal_fragment", normal_fragment},
	{"normal_pars_fragment", normal_pars_fragment},
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
	{"encodings_pars_fragment", encodings_pars_fragment },
	{"alphatest_fragment", alphatest_fragment },
	{"alphamap_pars_fragment", alphamap_pars_fragment },
	{"alphamap_fragment", alphamap_fragment },
	{"lightmap_pars_fragment", lightmap_pars_fragment },
	{"aomap_pars_fragment", aomap_pars_fragment },
	{"envmap_pars_fragment", envmap_pars_fragment },
	{"shadowmap_pars_fragment", shadowmap_pars_fragment},
	{"bumpmap_pars_fragment", bumpmap_pars_fragment},
	{"smoothnessmap_pars_fragment", smoothnessmap_pars_fragment},
	{"roughnessmap_pars_fragment", roughnessmap_pars_fragment},
	{"roughnessmap_fragment", roughnessmap_fragment},
	{"metalnessmap_fragment", metalnessmap_fragment},
	{"metalnessmap_pars_fragment", metalnessmap_pars_fragment},
	{"emissivemap_fragment", emissivemap_fragment},
	{"emissivemap_pars_fragment", emissivemap_pars_fragment },	
	{"bsdfs", bsdfs },
	{"lights_pars", lights_pars },
	{"lights_physical_pars_fragment", lights_physical_pars_fragment },
	{"lights_physical_fragment", lights_physical_fragment },
	{"lights_template", lights_template },
};

namespace octoon::video
{
	RenderPipeline::RenderPipeline() noexcept
	{
	}

	RenderPipeline::RenderPipeline(const material::MaterialPtr& material, const RenderProfile& context) noexcept
	{
		this->setMaterial(material, context);
	}

	RenderPipeline::~RenderPipeline() noexcept
	{
	}

	void
	RenderPipeline::setMaterial(const material::MaterialPtr& material, const RenderProfile& context) noexcept
	{
		if (this->material_ != material)
		{
			this->material_ = material;
			this->updateMaterial(this->material_, context);
		}
	}
		
	const material::MaterialPtr&
	RenderPipeline::getMaterial() const noexcept
	{
		return this->material_;
	}

	const hal::GraphicsPipelinePtr&
	RenderPipeline::getPipeline() const noexcept
	{
		return pipeline_;
	}

	const hal::GraphicsDescriptorSetPtr&
	RenderPipeline::getDescriptorSet() const noexcept
	{
		return descriptorSet_;
	}

	void
	RenderPipeline::update(const camera::Camera& camera, const geometry::Geometry& geometry, const RenderProfile& context) noexcept
	{
		if (this->material_)
		{
			if (this->modelMatrix_)
				this->modelMatrix_->uniform4fmat(geometry.getTransform());
			
			if (this->modelViewMatrix_)
				this->modelViewMatrix_->uniform4fmat(camera.getView() * geometry.getTransform());
			
			if (this->projectionMatrix_)
				this->projectionMatrix_->uniform4fmat(camera.getProjection());
			
			if (this->normalMatrix_)
				this->normalMatrix_->uniform3fmat((math::float3x3)geometry.getTransform());

			if (this->ambientLightColor_)
				this->ambientLightColor_->uniform3f(context.light.ambientLightColors);

			if (this->directionalLights_)
				this->directionalLights_->uniformBuffer(context.light.directionLightBuffer);

			this->updateParameters();
		}
	}

	void
	RenderPipeline::parseIncludes(std::string& str)
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
	RenderPipeline::replaceLightNums(std::string& str, const LightModule& parameters)
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
		replace(str, "NUM_RECT_AREA_LIGHTS", std::to_string(parameters.numArea));
		replace(str, "NUM_POINT_LIGHTS", std::to_string(parameters.numPoint));
		replace(str, "NUM_HEMI_LIGHTS", std::to_string(parameters.numEnvironment));
	}

	void
	RenderPipeline::setupProgram(const material::MaterialPtr& material, const RenderProfile& context)
	{
		auto shader = material->getShader();

		std::string vertexShader = "#version 330\n\t";
		vertexShader += R"(
				layout(location = 0) in vec4 POSITION0;
				layout(location = 1) in vec2 TEXCOORD0;
				layout(location = 2) in vec3 NORMAL0;

				uniform mat4 modelMatrix;
				uniform mat4 modelViewMatrix;
				uniform mat4 projectionMatrix;
				uniform mat4 viewMatrix;
				uniform mat3 normalMatrix;
				uniform vec3 cameraPosition;

				#ifdef USE_COLOR
					attribute vec3 color;
				#endif
			)";
		vertexShader += shader->vs;

		std::string fragmentShader = "#version 330\n\t";
		fragmentShader += "layout(location  = 0) out vec4 fragColor;\n";
		//fragmentShader += "#define TONE_MAPPING\n";
		fragmentShader += "#define PHYSICALLY_CORRECT_LIGHTS\n";
		fragmentShader += shader->fs;

		this->parseIncludes(vertexShader);
		this->parseIncludes(fragmentShader);

		this->replaceLightNums(vertexShader, context.light);
		this->replaceLightNums(fragmentShader, context.light);

		hal::GraphicsProgramDesc programDesc;
		programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::VertexBit, vertexShader, "main", hal::GraphicsShaderLang::GLSL)));
		programDesc.addShader(Renderer::instance()->createShader(hal::GraphicsShaderDesc(hal::GraphicsShaderStageFlagBits::FragmentBit, fragmentShader, "main", hal::GraphicsShaderLang::GLSL)));
		this->program_ = Renderer::instance()->createProgram(programDesc);
	}

	void
	RenderPipeline::setupRenderState(const material::MaterialPtr& material)
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
	RenderPipeline::updateMaterial(const material::MaterialPtr& material, const RenderProfile& context) noexcept(false)
	{
		if (material) {
			this->setupRenderState(material);
			this->setupProgram(material, context);

			hal::GraphicsInputLayoutDesc layoutDesc;
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
			layoutDesc.addVertexLayout(hal::GraphicsVertexLayout(0, "NORMAL", 0, hal::GraphicsFormat::R32G32B32SFloat));
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

				auto normalMatrix = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "normalMatrix"; });
				if (normalMatrix != end)
					normalMatrix_ = *normalMatrix;

				modelViewMatrix_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "modelViewMatrix"; });
				projectionMatrix_ = *std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "projectionMatrix"; });

				auto ambientLightColor = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "ambientLightColor"; });
				if (ambientLightColor != end)
					ambientLightColor_ = *ambientLightColor;

				auto directionalLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "DirectionalLights"; });
				if (directionalLights != end)
					directionalLights_ = *directionalLights;

				auto spotLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "spotLights"; });
				if (spotLights != end)
					spotLights_ = *spotLights;

				auto rectAreaLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "rectAreaLights"; });
				if (rectAreaLights != end)
					rectAreaLights_ = *rectAreaLights;

				auto hemisphereLights = std::find_if(begin, end, [](const hal::GraphicsUniformSetPtr& set) { return set->getName() == "hemisphereLights"; });
				if (hemisphereLights != end)
					hemisphereLights_ = *hemisphereLights;

				this->updateParameters();
			}
		}
		else
		{
			pipeline_ = nullptr;
		}
	}

	void
	RenderPipeline::updateParameters() noexcept
	{
		if (this->material_->needUpdate())
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

			this->material_->needUpdate(false);
		}
	}
}