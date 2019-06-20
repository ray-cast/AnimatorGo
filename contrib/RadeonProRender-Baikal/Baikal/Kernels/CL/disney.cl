#ifndef DISNEY_CL
#define DISNEY_CL

#include <../Baikal/Kernels/CL/utils.cl>
#include <../Baikal/Kernels/CL/texture.cl>
#include <../Baikal/Kernels/CL/payload.cl>

#define DENOM_EPS 1e-8f
#define ROUGHNESS_EPS 0.0001f

#define WHITE make_float3(1.f, 1.f, 1.f)

INLINE float SchlickFresnelReflectance(float u)
{
    float m = clamp(1.f - u, 0.f, 1.f);
    float m2 = m * m;
    return m2 * m2 * m;
}

float GTR1(float ndoth, float a)
{
    if (a >= 1.f) return 1.f / PI;
    
    float a2 = a * a;
    float t = 1.f + (a2 - 1.f) * ndoth * ndoth;
    return (a2 - 1.f) / (PI * log(a2) * t);
}

float GTR2(float ndoth, float a)
{
    float a2 = a * a;
    float t = 1.f + (a2 - 1.f) * ndoth * ndoth;
    return a2 / (PI * t * t);
}

float GTR2_Aniso(float ndoth, float hdotx, float hdoty, float ax, float ay)
{
    float hdotxa2 = (hdotx / ax);
    hdotxa2 *= hdotxa2;
    float hdotya2 = (hdoty / ay);
    hdotya2 *= hdotya2;
    float denom = hdotxa2 + hdotya2 + ndoth * ndoth;
    return denom > 1e-5 ? (1.f / (PI * ax * ay * denom * denom)) : 0.f;
}


float SmithGGX_G(float ndotv, float a)
{
    float a2 = a * a;
    float b = ndotv * ndotv;
    return 1.f / (ndotv + native_sqrt(a2 + b - a2 * b));
}

float SmithGGX_G_Aniso(float ndotv, float vdotx, float vdoty, float ax, float ay)
{
    float vdotxax2 = (vdotx * ax) * (vdotx * ax);
    float vdotyay2 = (vdoty * ay) * (vdoty * ay);
    return 1.f / (ndotv + native_sqrt( vdotxax2 + vdotyay2 + ndotv * ndotv));
}


INLINE float Disney_GetPdf(
    // Geometry
    DifferentialGeometry const* dg,
    // Incoming direction
    float3 wi,
    // Outgoing direction
    float3 wo,
    // Texture args
    TEXTURE_ARG_LIST
    )
{
    float3 base_color = Texture_GetValue3f(dg->mat.disney.base_color.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.base_color_map_idx));
    float metallic = Texture_GetValue1f(dg->mat.disney.metallic, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.metallic_map_idx));
    float specular = Texture_GetValue1f(dg->mat.disney.specular, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_map_idx));
    float anisotropy = Texture_GetValue1f(dg->mat.disney.anisotropy, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.anisotropy_map_idx));
    float roughness = Texture_GetValue1f(dg->mat.disney.roughness, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.roughness_map_idx));
    float specular_tint = Texture_GetValue1f(dg->mat.disney.specular_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_tint_map_idx));
    float sheen_tint = Texture_GetValue1f(dg->mat.disney.sheen_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_tint_map_idx));
    float sheen = Texture_GetValue1f(dg->mat.disney.sheen, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_map_idx));
    float clearcoat_gloss = Texture_GetValue1f(dg->mat.disney.clearcoat_gloss, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_gloss_map_idx));
    float clearcoat = Texture_GetValue1f(dg->mat.disney.clearcoat, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_map_idx));
    float subsurface = dg->mat.disney.subsurface;
    
    float aspect = native_sqrt(1.f - anisotropy * 0.9f);
    
    float ax = max(0.001f, roughness * roughness * ( 1.f + anisotropy));
    float ay = max(0.001f, roughness * roughness * ( 1.f - anisotropy));
    float3 wh = normalize(wo + wi);
    float ndotwh = fabs(wh.y);
    float hdotwo = fabs(dot(wh, wo));

    
    float d_pdf = fabs(wo.y) / PI;
    float r_pdf = GTR2_Aniso(ndotwh, wh.x, wh.z, ax, ay) * ndotwh / (4.f * hdotwo);
    float c_pdf = GTR1(ndotwh, mix(0.1f,0.001f, clearcoat_gloss)) * ndotwh / (4.f * hdotwo);
    
    float3 cd_lin = native_powr(base_color, 2.2f);
    // Luminance approximmation
    float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));
    
    // Normalize lum. to isolate hue+sat
    float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;
    
    float3 c_spec0 = mix(specular * 0.1f * mix(WHITE,
                                        c_tint, specular_tint),
                         cd_lin, metallic);
    
    float cs_lum = dot(c_spec0, make_float3(0.3f, 0.6f, 0.1f));
    
    float cs_w = cs_lum / (cs_lum + (1.f - metallic) * cd_lum);
    
    return c_pdf * clearcoat + (1.f - clearcoat) * (cs_w * r_pdf + (1.f - cs_w) * d_pdf);
}


INLINE float3 Disney_Evaluate(
    // Geometry
    DifferentialGeometry const* dg,
    // Incoming direction
    float3 wi,
    // Outgoing direction
    float3 wo,
    // Texture args
    TEXTURE_ARG_LIST
    )
{
    float3 base_color = Texture_GetValue3f(dg->mat.disney.base_color.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.base_color_map_idx));
    float metallic = Texture_GetValue1f(dg->mat.disney.metallic, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.metallic_map_idx));
    float specular = Texture_GetValue1f(dg->mat.disney.specular, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_map_idx));
    float anisotropy = Texture_GetValue1f(dg->mat.disney.anisotropy, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.anisotropy_map_idx));
    float roughness = Texture_GetValue1f(dg->mat.disney.roughness, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.roughness_map_idx));
    float specular_tint = Texture_GetValue1f(dg->mat.disney.specular_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_tint_map_idx));
    float sheen_tint = Texture_GetValue1f(dg->mat.disney.sheen_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_tint_map_idx));
    float sheen = Texture_GetValue1f(dg->mat.disney.sheen, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_map_idx));
    float clearcoat_gloss = Texture_GetValue1f(dg->mat.disney.clearcoat_gloss, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_gloss_map_idx));
    float clearcoat = Texture_GetValue1f(dg->mat.disney.clearcoat, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_map_idx));
    float subsurface = dg->mat.disney.subsurface;
    
    float ndotwi = fabs(wi.y);
    float ndotwo = fabs(wo.y);
    
    float3 h = normalize(wi + wo);
    float ndoth = fabs(h.y);
    float hdotwo = fabs(dot(h, wo));
    
    float3 cd_lin = native_powr(base_color, 2.2f);
    // Luminance approximmation
    float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));
    
    // Normalize lum. to isolate hue+sat
    float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;
    
    float3 c_spec0 = mix(specular * 0.1f * mix(WHITE,
                                    c_tint, specular_tint),
                                    cd_lin, metallic);
    
    float3 c_sheen = mix(WHITE, c_tint, sheen_tint);
    
    // Diffuse fresnel - go from 1 at normal incidence to 0.5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float f_wo = SchlickFresnelReflectance(ndotwo);
    float f_wi = SchlickFresnelReflectance(ndotwi);
    
    float fd90 = 0.5f + 2 * hdotwo * hdotwo * roughness;
    float fd = mix(1.f, fd90, f_wo) * mix(1.f, fd90, f_wi);
    
    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // fss90 used to "flatten" retroreflection based on roughness
    float fss90 = hdotwo * hdotwo * roughness;
    float fss = mix(1.f, fss90, f_wo) * mix(1.f, fss90, f_wi);
    float ss = 1.25f * (fss * (1.f / (ndotwo + ndotwi) - 0.5f) + 0.5f);
    
    // Specular
    float ax = max(0.001f, roughness * roughness * ( 1.f + anisotropy));
    float ay = max(0.001f, roughness * roughness * ( 1.f - anisotropy));
    float ds = GTR2_Aniso(ndoth, h.x, h.z, ax, ay);
    float fh = SchlickFresnelReflectance(hdotwo);
    float3 fs = mix(c_spec0, WHITE, fh);
    
    float gs;
    gs  = SmithGGX_G_Aniso(ndotwo, wo.x, wo.z, ax, ay);
    gs *= SmithGGX_G_Aniso(ndotwi, wi.x, wi.z, ax, ay);
    
    // Sheen
    float3 f_sheen = fh * sheen * c_sheen;
    
    // clearcoat (ior = 1.5 -> F0 = 0.04)
    float dr = GTR1(ndoth, mix(0.1f,0.001f, clearcoat_gloss));
    float fr = mix(0.04f, 1.f, fh);
    float gr = SmithGGX_G(ndotwo, 0.25f) * SmithGGX_G(ndotwi, 0.25f);
    
    return ((1.f / PI) * mix(fd, ss, subsurface) * cd_lin + f_sheen) *
            (1.f - metallic) + gs * fs * ds + clearcoat * gr * fr * dr;
}

INLINE float3 Disney_Sample(
                            // Geometry
                            DifferentialGeometry const* dg,
                            // Incoming direction
                            float3 wi,
                            // Texture args
                            TEXTURE_ARG_LIST,
                            // Sample
                            float2 sample,
                            // Outgoing  direction
                            float3* wo,
                            // PDF at wo
                            float* pdf
                            )
{
    float3 base_color = Texture_GetValue3f(dg->mat.disney.base_color.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.base_color_map_idx));
    float metallic = Texture_GetValue1f(dg->mat.disney.metallic, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.metallic_map_idx));
    float specular = Texture_GetValue1f(dg->mat.disney.specular, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_map_idx));
    float anisotropy = Texture_GetValue1f(dg->mat.disney.anisotropy, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.anisotropy_map_idx));
    float roughness = Texture_GetValue1f(dg->mat.disney.roughness, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.roughness_map_idx));
    float specular_tint = Texture_GetValue1f(dg->mat.disney.specular_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_tint_map_idx));
    float sheen_tint = Texture_GetValue1f(dg->mat.disney.sheen_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_tint_map_idx));
    float sheen = Texture_GetValue1f(dg->mat.disney.sheen, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_map_idx));
    float clearcoat_gloss = Texture_GetValue1f(dg->mat.disney.clearcoat_gloss, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_gloss_map_idx));
    float clearcoat = Texture_GetValue1f(dg->mat.disney.clearcoat, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_map_idx));
    float subsurface = dg->mat.disney.subsurface;
    
    float ax = max(0.001f, roughness * roughness * ( 1.f + anisotropy));
    float ay = max(0.001f, roughness * roughness * ( 1.f - anisotropy));
    
    float mis_weight = 1.f;
    float3 wh;
    
    
    if (sample.x < clearcoat)
    {
        sample.x /= (clearcoat);
        
        float a = mix(0.1f,0.001f, clearcoat_gloss);
        float ndotwh = native_sqrt((1.f - native_powr(a*a, 1.f - sample.y)) / (1.f - a*a));
        float sintheta = native_sqrt(1.f - ndotwh * ndotwh);
        wh = normalize(make_float3(native_cos(2.f * PI * sample.x) * sintheta,
                                   ndotwh,
                                   native_sin(2.f * PI * sample.x) * sintheta));
        
        *wo = -wi + 2.f*fabs(dot(wi, wh)) * wh;
        
    }
    else
    {
        sample.x -= (clearcoat);
        sample.x /= (1.f - clearcoat);

        float3 cd_lin = native_powr(base_color, 2.2f);
        // Luminance approximmation
        float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));

        // Normalize lum. to isolate hue+sat
        float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;

        float3 c_spec0 = mix(specular * 0.3f * mix(WHITE,
            c_tint, specular_tint),
            cd_lin, metallic);

        float cs_lum = dot(c_spec0, make_float3(0.3f, 0.6f, 0.1f));

        float cs_w = cs_lum / (cs_lum + (1.f - metallic) * cd_lum);
        
        if (sample.y < cs_w)
        {
            sample.y /= cs_w;
            
            float t = native_sqrt(sample.y / (1.f - sample.y));
            wh = normalize(make_float3(t * ax * native_cos(2.f * PI * sample.x),
                                              1.f,
                                              t * ay * native_sin(2.f * PI * sample.x)));
            
            *wo = -wi + 2.f*fabs(dot(wi, wh)) * wh;
        }
        else
        {
            sample.y -= cs_w;
            sample.y /= (1.f - cs_w);
            
            *wo = Sample_MapToHemisphere(sample, make_float3(0.f, 1.f, 0.f) , 1.f);
            
            wh = normalize(*wo + wi);
        }
    }

    //float ndotwh = fabs(wh.y);
    //float hdotwo = fabs(dot(wh, *wo));
    
    *pdf = Disney_GetPdf(dg, wi, *wo, TEXTURE_ARGS);
    
    return Disney_Evaluate(dg, wi, *wo, TEXTURE_ARGS);
}

#endif

