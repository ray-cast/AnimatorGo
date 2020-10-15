#ifndef DISNEY_CL
#define DISNEY_CL

#include <../../system/Kernels/CL/utils.cl>
#include <../../system/Kernels/CL/texture.cl>
#include <../../system/Kernels/CL/payload.cl>

#define DENOM_EPS 1e-8f
#define ROUGHNESS_EPS 0.0001f

#define WHITE make_float3(1.f, 1.f, 1.f)

typedef struct DisneyShaderData
{
	float3 base_color;
	float3 diffuseColor;
	float3 specularColor;
	float3 emissive;
	float3 subsurface_color;
	float transparency;
	float metallic;
	float roughness;
	float anisotropy;
	float specular;
	float specular_tint;
	float sheen;
	float sheen_tint;
	float clearcoat;
	float clearcoat_roughness;
	float subsurface;
	float transmission;
	float refraction_ior;
	float cd_lum;
	float cs_lum;
	float cs_w;
} DisneyShaderData;

INLINE void Disney_PrepareInputs(DifferentialGeometry const* dg, TEXTURE_ARG_LIST, DisneyShaderData* shader_data)
{
	float4 color = Texture_GetValue4f(make_float4(dg->mat.disney.base_color.x,dg->mat.disney.base_color.y,dg->mat.disney.base_color.z,1), dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.base_color_map_idx));
	float cd_lum = dot(color.xyz, make_float3(0.3f, 0.6f, 0.1f));
	float3 c_tint = cd_lum > 0.f ? (color.xyz / cd_lum) : WHITE;
	
	shader_data->base_color = color.xyz;
	shader_data->transparency = 1.f - Texture_GetValue1f(dg->mat.disney.opacity, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.opacity_map_idx)) * color.w;
	shader_data->metallic = Texture_GetValue1f(dg->mat.disney.metallic, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.metallic_map_idx));
	shader_data->specular = Texture_GetValue1f(dg->mat.disney.specular, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_map_idx));
	shader_data->anisotropy = Texture_GetValue1f(dg->mat.disney.anisotropy, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.anisotropy_map_idx));
	shader_data->roughness = Texture_GetValue1f(dg->mat.disney.roughness, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.roughness_map_idx));
	shader_data->specular_tint = Texture_GetValue1f(dg->mat.disney.specular_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.specular_tint_map_idx));
	shader_data->sheen_tint = Texture_GetValue1f(dg->mat.disney.sheen_tint, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_tint_map_idx));
	shader_data->sheen = Texture_GetValue1f(dg->mat.disney.sheen, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.sheen_map_idx));
	shader_data->clearcoat_roughness = Texture_GetValue1f(dg->mat.disney.clearcoat_roughness, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_roughness_map_idx));
	shader_data->clearcoat = Texture_GetValue1f(dg->mat.disney.clearcoat, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.clearcoat_map_idx));
	shader_data->subsurface = Texture_GetValue1f(dg->mat.disney.subsurface, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.subsurface_map_idx));
	shader_data->subsurface_color = Texture_GetValue3f(dg->mat.disney.subsurface_color, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.subsurface_color_map_idx));
	shader_data->emissive = Texture_GetValue3f(dg->mat.disney.emissive.xyz, dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.emissive_map_idx));
	shader_data->transmission = dg->mat.disney.transmission;
	shader_data->refraction_ior = dg->mat.disney.refraction_ior;
	shader_data->diffuseColor = shader_data->base_color * (1.f - shader_data->metallic);
	shader_data->specularColor = mix(shader_data->specular * 0.1f * mix(WHITE, c_tint, shader_data->specular_tint), shader_data->base_color, shader_data->metallic);
	shader_data->cd_lum = cd_lum;
	shader_data->cs_lum = dot(shader_data->specularColor, make_float3(0.3f, 0.6f, 0.1f));
	shader_data->cs_w = shader_data->cs_lum / (shader_data->cs_lum + (1.f - shader_data->metallic) * cd_lum);
}

INLINE float SchlickFresnelReflectance(float u)
{
	float m = clamp(1.f - u, 0.f, 1.f);
	float m2 = m * m;
	return m2 * m2 * m;
}

INLINE float CalculateFresnel(float etai, float etat, float ndotwi)
{
	float cosi = ndotwi;

	// Revert normal and eta if needed
	if (cosi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
		cosi = -cosi;
	}

	float eta = etai / etat;
	float sini2 = 1.f - cosi * cosi;
	float sint2 = eta * eta * sini2;
	float fresnel = 1.f;

	if (sint2 < 1.f)
	{
		float cost = native_sqrt(max(0.f, 1.f - sint2));
		fresnel = FresnelDielectric(etai, etat, cosi, cost);
	}

	return fresnel;
}

INLINE float GTR1(float ndoth, float a)
{
	if (a >= 1.f) return 1.f / PI;
	
	float a2 = a * a;
	float t = (a2 - 1.f) * ndoth * ndoth + 1.f;
	return (a2 - 1.f) / (PI * log(a2) * t);
}

INLINE float GTR2(float ndoth, float a)
{
	float a2 = a * a;
	float t = (a2 - 1.f) * ndoth * ndoth + 1.f;
	return a2 / (PI * t * t);
}

INLINE float GTR2_Aniso(float ndoth, float hdotx, float hdoty, float ax, float ay)
{
	float hdotxa2 = (hdotx / ax);
	hdotxa2 *= hdotxa2;
	float hdotya2 = (hdoty / ay);
	hdotya2 *= hdotya2;
	float denom = hdotxa2 + hdotya2 + ndoth * ndoth;
	return denom > 1e-5 ? (1.f / (PI * ax * ay * denom * denom)) : 0.f;
}

INLINE float SmithGGX_G(float ndotv, float a)
{
	float a2 = a * a;
	float b = ndotv * ndotv;
	return 1.f / (ndotv + native_sqrt(a2 + b - a2 * b));
}

INLINE float SmithGGX_G_Aniso(float ndotv, float vdotx, float vdoty, float ax, float ay)
{
	float vdotxax2 = (vdotx * ax) * (vdotx * ax);
	float vdotyay2 = (vdoty * ay) * (vdoty * ay);
	return 1.f / (ndotv + native_sqrt( vdotxax2 + vdotyay2 + ndotv * ndotv));
}

INLINE float Lambert_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	return fabs(wo.y) / PI;
}

INLINE float MicrofacetReflectionGGX_D(float roughness, float3 m)
{
	float ndotm = fabs(m.y);
	float ndotm2 = ndotm * ndotm;
	float sinmn = native_sqrt(1.f - clamp(ndotm * ndotm, 0.f, 1.f));
	float tanmn = ndotm > DENOM_EPS ? sinmn / ndotm : 0.f;
	float a2 = roughness * roughness;
	float denom = (PI * ndotm2 * ndotm2 * (a2 + tanmn * tanmn) * (a2 + tanmn * tanmn));
	return denom > DENOM_EPS ? (a2 / denom) : 1.f;
}

INLINE float MicrofacetReflectionGGX_G1(float roughness, float3 v)
{
	float ndotv = fabs(v.y);

	float sinnv = native_sqrt(1.f - clamp(ndotv * ndotv, 0.f, 1.f));
	float tannv = ndotv > DENOM_EPS ? sinnv / ndotv : 0.f;
	float a2 = roughness * roughness;
	return 2.f / (1.f + native_sqrt(1.f + a2 * tannv * tannv));
}

INLINE float MicrofacetReflectionGGX_G(float roughness, float3 wi, float3 wo)
{
	return MicrofacetReflectionGGX_G1(roughness, wi) * MicrofacetReflectionGGX_G1(roughness, wo);
}

INLINE float MicrofacetReflectionGGX_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	float3 wh = normalize(wo + wi);

	float ndotwh = fabs(wh.y);
	float hdotwo = fabs(dot(wh, wo));

	float alpha = shader_data->roughness * shader_data->roughness;
	float pdf = GTR2(ndotwh, alpha) * ndotwh / (4.f * hdotwo);

	return pdf * shader_data->cs_w;
}

INLINE float3 MicrofacetReflectionGGX_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	float3 wh = normalize(wo + wi);

	float ndotwi = fabs(wi.y);
	float ndotwo = fabs(wo.y);
	float ndotwh = fabs(wh.y);
	float hdotwo = fabs(dot(wh, wo));
	float alpha = shader_data->roughness * shader_data->roughness;

	float ds = GTR2(ndotwh, alpha) * ndotwo;
	float gs = SmithGGX_G(ndotwo, 0.25f) * SmithGGX_G(ndotwi, 0.25f);
	float3 fs = mix(shader_data->specularColor, WHITE, SchlickFresnelReflectance(hdotwo));

	return ds * fs * gs;
}

INLINE float3 MicrofacetReflectionGGX_SampleNormal(float roughness, float2 sample)
{
	float r1 = sample.x;
	float r2 = sample.y;

	float alpha = roughness * roughness;
	float ndotwh = native_sqrt((1.f - native_powr(alpha, 1.f - r2)) / (1.f - alpha));
	float sintheta = native_sqrt(1.f - ndotwh * ndotwh);

	float phi = 2.f * PI * r1;
	float cosphi = native_cos(phi);
	float sinphi = native_sin(phi);

	return normalize(make_float3(cosphi * sintheta, ndotwh, sinphi * sintheta));
}

INLINE float3 MicrofacetReflectionGGX_Aniso_SampleNormal(float roughness, float anisotropy, float2 sample)
{
	float alpha = roughness * roughness;
	float ax = max(0.001f, alpha * ( 1.f + anisotropy));
	float ay = max(0.001f, alpha * ( 1.f - anisotropy));
	float t = native_sqrt(sample.y / (1.f - sample.y));

	return normalize(make_float3(t * ax * native_cos(2.f * PI * sample.x), 1.f, t * ay * native_sin(2.f * PI * sample.x)));
}

INLINE float MicrofacetRefractionGGX_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	const float roughness = max(shader_data->roughness, ROUGHNESS_EPS);

	float ndotwi = wi.y;
	float ndotwo = wo.y;

	if (ndotwi * ndotwo >= 0.f)
	{
		return 0.f;
	}

	float etai = 1.f;
	float etat = shader_data->refraction_ior;

	if (ndotwi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
	}

	// Calc halfway vector
	float3 ht = -(etai * wi + etat * wo);
	float3 wh = normalize(ht);

	float wodotwh = fabs(dot(wo, wh));
	float whpdf = MicrofacetReflectionGGX_D(roughness, wh) * fabs(wh.y);
	float whwo = wodotwh * etat * etat;
	float denom = dot(ht, ht);

	return denom > DENOM_EPS ? whpdf * whwo / denom : 0.f;
}

INLINE float3 MicrofacetRefractionGGX_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	const float3 ks = shader_data->base_color;
	const float roughness = max(shader_data->roughness, ROUGHNESS_EPS);

	float ndotwi = wi.y;
	float ndotwo = wo.y;

	if (ndotwi * ndotwo >= 0.f)
	{
		return 0.f;
	}

	float etai = 1.f;
	float etat = shader_data->refraction_ior;

	if (ndotwi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
	}

	float3 ht = -(etai * wi + etat * wo);
	float3 wh = normalize(ht);

	float widotwh = fabs(dot(wh, wi));
	float wodotwh = fabs(dot(wh, wo));

	float denom = dot(ht, ht);
	denom *= (fabs(ndotwi) * fabs(ndotwo));

	return denom > DENOM_EPS ? (shader_data->base_color * (widotwh * wodotwh)  * (etat)* (etat) * MicrofacetReflectionGGX_G(roughness, wi, wo) * MicrofacetReflectionGGX_D(roughness, wh) / denom) : 0.f;
}

INLINE float3 MicrofacetRefractionGGX_Sample(DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	const float roughness = max(shader_data->roughness, ROUGHNESS_EPS);

	float ndotwi = wi.y;

	if (ndotwi == 0.f)
	{
		*pdf = 0.f;
		return 0.f;
	}

	float etai = 1.f;
	float etat = shader_data->refraction_ior;
	float s = 1.f;

	if (ndotwi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
		s = -s;
	}

	float3 wh = MicrofacetReflectionGGX_SampleNormal(roughness, sample);

	float c = dot(wi, wh);
	float eta = etai / etat;

	float d = 1 + eta * (c * c - 1);

	if (d <= 0.f)
	{
		*pdf = 0.f;
		return 0.f;
	}

	*wo = normalize((eta * c - s * native_sqrt(d)) * wh - eta * wi);
	*pdf = MicrofacetRefractionGGX_GetPdf(shader_data, wi, *wo);

	return MicrofacetRefractionGGX_Evaluate(shader_data, wi, *wo);
}

INLINE float3 IdealRefract_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	return 0.f;
}

INLINE float IdealRefract_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	return 0.f;
}

INLINE float3 IdealRefract_Sample(DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	float etai = 1.f;
	float etat = shader_data->refraction_ior;
	float cosi = wi.y;

	bool entering = cosi > 0.f;

	if (!entering)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
	}

	float eta = etai / etat;
	float sini2 = 1.f - cosi * cosi;
	float sint2 = eta * eta * sini2;

	if (sint2 >= 1.f)
	{
		*pdf = 0.f;
		return 0.f;
	}

	float cost = native_sqrt(max(0.f, 1.f - sint2));

	*wo = normalize(make_float3(eta * -wi.x, entering ? -cost : cost, eta * -wi.z));
	*pdf = 1.f;

	return cost > DENOM_EPS ? (eta * eta * shader_data->diffuseColor / cost) : 0.f;
}

INLINE float3 Diffuse_PennerSkin(float ss, float ndotwi, float ir, float3 transmittance)
{
	float pndl = 1.0f - clamp( ndotwi, 0.f, 1.f);
	float nndl = 1.0f - clamp(-ndotwi, 0.f, 1.f);
	float3 sss = clamp( ndotwi, 0.f, 1.f) + transmittance * pndl * pndl * native_powr(nndl, 3.0f / (ir + 0.001f)) * clamp(ir - 0.04f, 0.f, 1.f);

	return sss;
}

INLINE void Disney_ApplyShadingNormal(DifferentialGeometry* dg, TEXTURE_ARG_LIST)
{
	if (dg->mat.disney.normal_map_idx != -1)
	{
		float3 shading_normal = Texture_Sample2D(dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.normal_map_idx)).xyz;
		dg->n = normalize(shading_normal.z * dg->n + shading_normal.x * dg->dpdu + shading_normal.y * dg->dpdv);
		dg->dpdv = normalize(cross(dg->n, dg->dpdu));
		dg->dpdu = normalize(cross(dg->dpdv, dg->n));
	}
}

INLINE float Disney_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	if (wo.y <= 0.0f)
	{
		float bsdfPdf = IdealRefract_GetPdf(shader_data, wi, wo);
		float brdfPdf = (1.0f / (2 * PI)) * shader_data->subsurface * 0.5f;

		return mix(brdfPdf, bsdfPdf, shader_data->transmission);
	}
	else
	{
		float alpha = shader_data->roughness * shader_data->roughness;
		float ax = max(0.001f, alpha * ( 1.f + shader_data->anisotropy));
		float ay = max(0.001f, alpha * ( 1.f - shader_data->anisotropy));
		float3 wh = normalize(wo + wi);
		float ndotwh = fabs(wh.y);
		float hdotwo = fabs(dot(wh, wo));

		float d_pdf = Lambert_GetPdf(shader_data, wi, wo) * (1.f - shader_data->subsurface);
		float r_pdf = GTR2_Aniso(ndotwh, wh.x, wh.z, ax, ay) * ndotwh / (4.f * hdotwo);
		float c_pdf = GTR1(ndotwh, mix(0.001f, 0.1f, shader_data->clearcoat_roughness)) * ndotwh / (4.f * hdotwo);
		
		float fresnel = CalculateFresnel(1.0, shader_data->refraction_ior, wi.y);
		float bsdf = mix(r_pdf * fresnel * shader_data->cs_w, c_pdf, shader_data->clearcoat);
		float brdf = mix(mix(d_pdf, r_pdf, shader_data->cs_w), c_pdf, shader_data->clearcoat);

		return mix(mix(brdf, 0.f, shader_data->transparency), bsdf, shader_data->transmission);
	}
}

INLINE float3 Disney_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	float3 bsdf = 0;
	if (shader_data->transmission > 0.0f)
	{
		if (wo.y <= 0.0f)
		{
			bsdf = IdealRefract_Evaluate(shader_data, wi, wo);
		}
		else
		{
			bsdf = MicrofacetReflectionGGX_Evaluate(shader_data, wi, wo);
		}
	}

	float3 brdf = 0;
	if (shader_data->transmission < 1.0f)
	{
		float ndotwi = fabs(wi.y);
		float ndotwo = fabs(wo.y);

		float3 h = normalize(wi + wo);
		float ndoth = fabs(h.y);
		float hdotwo = fabs(dot(h, wo));

		float f_wo = SchlickFresnelReflectance(ndotwo);
		float f_wi = SchlickFresnelReflectance(ndotwi);

		if (wo.y <= 0.0f)
		{
			if (shader_data->subsurface > 0.0f)
			{
				float fd90 = 0.5f;
    			float Fd = mix(1.f, fd90, f_wo) * mix(1.f, fd90, f_wi);

				brdf = (1.f / PI) * native_sqrt(shader_data->diffuseColor) * shader_data->subsurface * Fd * (1.0f - shader_data->metallic);
			}
		}
		else
		{
			float fss90 = hdotwo * hdotwo * shader_data->roughness;
			float fss = mix(1.f, fss90, f_wo) * mix(1.f, fss90, f_wi);
			float ss = 1.25f * (fss * (1.f / (ndotwo + ndotwi) - 0.5f) + 0.5f) * clamp(wo.y, 0.0f, 1.0f);
			float3 bssrdf = Diffuse_PennerSkin(ss, wo.y, 0.5f, shader_data->subsurface_color);

			float fd90 = 0.5f + 2 * hdotwo * hdotwo * shader_data->roughness;
			float fd = mix(1.f, fd90, f_wo) * mix(1.f, fd90, f_wi) * ndotwo;
			float3 d_brdf = shader_data->diffuseColor * mix(fd, 0, shader_data->subsurface);

			float alpha = shader_data->roughness * shader_data->roughness;
			float ax = max(0.001f, alpha * (1.f + shader_data->anisotropy));
			float ay = max(0.001f, alpha * (1.f - shader_data->anisotropy));
			float ds = GTR2_Aniso(ndoth, h.x, h.z, ax, ay) * ndotwo;
			float fh = SchlickFresnelReflectance(hdotwo);
			float3 fs = mix(shader_data->specularColor, WHITE, fh);
			
			float gs;
			gs  = SmithGGX_G_Aniso(ndotwo, wo.x, wo.z, ax, ay);
			gs *= SmithGGX_G_Aniso(ndotwi, wi.x, wi.z, ax, ay);
			
			float cd_lum = dot(shader_data->base_color, make_float3(0.3f, 0.6f, 0.1f));			
			float3 c_tint = cd_lum > 0.f ? (shader_data->base_color / cd_lum) : WHITE;    
			float3 c_sheen = mix(WHITE, c_tint, shader_data->sheen_tint);
			float3 f_sheen = fh * shader_data->sheen * c_sheen * ndotwo;
			
			float dr = GTR1(ndoth, mix(0.001f, 0.1f,  shader_data->clearcoat_roughness)) * ndotwo;
			float fr = mix(0.04f, 1.f, fh);
			float gr = SmithGGX_G(ndotwo, 0.25f) * SmithGGX_G(ndotwi, 0.25f);

			brdf = mix((1.f / PI) * (d_brdf + f_sheen * (1.f - shader_data->metallic)) + gs * fs * ds + shader_data->clearcoat * gr * fr * dr, 0.f, shader_data->transparency);
		}
	}

	return mix(brdf, bsdf, shader_data->transmission);
}

float3 Disney_Sample(DifferentialGeometry* dg, DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	if (sample.y <= shader_data->transmission)
	{
		sample.y /= shader_data->transmission;

		float fresnel = CalculateFresnel(1.0, shader_data->refraction_ior, wi.y);
		if (sample.x < fresnel)
		{
			sample.x /= fresnel;

			Bxdf_SetFlags(dg, kBxdfFlagsBrdf | kBxdfFlagsSingular);
			Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);

			float3 wh = MicrofacetReflectionGGX_SampleNormal(shader_data->roughness, sample);

			*wo = -wi + 2.f * fabs(dot(wi, wh)) * wh;
			*pdf = MicrofacetReflectionGGX_GetPdf(shader_data, wi, *wo);

			return MicrofacetReflectionGGX_Evaluate(shader_data, wi, *wo);
		}
		else
		{
			sample.x -= fresnel;
			sample.x /= (1 - fresnel);

			Bxdf_SetFlags(dg, kBxdfFlagsSingular);
			Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleRefraction);

			return IdealRefract_Sample(shader_data, sample, wi, wo, pdf);
		}
	}
	else
	{
		sample.y -= shader_data->transmission;
		sample.y /= (1.f - shader_data->transmission);

		if (sample.y <= shader_data->transparency)
		{
			sample.y /= shader_data->transparency;

			Bxdf_SetFlags(dg, kBxdfFlagsTransparency | kBxdfFlagsSingular);
			Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleTransparency);
			
			*wo = -wi;
			*pdf = 1.f;

			float coswo = fabs((*wo).y);
			return coswo > 1e-5f ? (1.f / coswo) : 0.f;
		}
		else
		{
			sample.y -= shader_data->transparency;
			sample.y /= (1 - shader_data->transparency);

			int bxdf_flags = kBxdfFlagsBrdf;
			if (sample.x <= shader_data->clearcoat)
			{
				sample.x /= (shader_data->clearcoat);

				Bxdf_SetFlags(dg, kBxdfFlagsBrdf | kBxdfFlagsSingular);
				Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);
				
				float3 wh = MicrofacetReflectionGGX_SampleNormal(mix(0.001f, 0.1f, shader_data->clearcoat_roughness), sample);

				*wo = -wi + 2.f*fabs(dot(wi, wh)) * wh;
			}
			else
			{
				sample.x -= (shader_data->clearcoat);
				sample.x /= (1.f - shader_data->clearcoat);

				if (sample.y < shader_data->cs_w)
				{
					sample.y /= shader_data->cs_w;

					Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);
					Bxdf_SetFlags(dg, kBxdfFlagsBrdf | kBxdfFlagsSingular);

					float3 wh = MicrofacetReflectionGGX_Aniso_SampleNormal(shader_data->roughness, shader_data->anisotropy, sample);
					*wo = -wi + 2.f * fabs(dot(wi, wh)) * wh;
				}
				else
				{
					sample.y -= shader_data->cs_w;
					sample.y /= (1.f - shader_data->cs_w);

					if (sample.y <= shader_data->subsurface)
					{
						sample.y /= shader_data->subsurface;

						Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleDiffuse);
						Bxdf_SetFlags(dg, kBxdfFlagsDiffuse);
						
						*wo = Sample_MapToHemisphere(sample, make_float3(0.f, -1.f, 0.f) , 0.f);
					}
					else
					{
						sample.y -= shader_data->subsurface;
						sample.y /= (1.f - shader_data->subsurface);

						Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleDiffuse);
						Bxdf_SetFlags(dg, kBxdfFlagsBrdf | kBxdfFlagsDiffuse);
						
						*wo = Sample_MapToHemisphere(sample, make_float3(0.f, 1.f, 0.f) , 1.f);
					}
				}
			}
			
			*wo = normalize(*wo);
			*pdf = Disney_GetPdf(shader_data, wi, *wo);
			
			return Disney_Evaluate(shader_data, wi, *wo);
		}
	}
}

#endif