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
} DisneyShaderData;

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

float UberV2_MicrofacetDistribution_GGX_D(float roughness, float3 m)
{
	float ndotm = fabs(m.y);
	float ndotm2 = ndotm * ndotm;
	float sinmn = native_sqrt(1.f - clamp(ndotm * ndotm, 0.f, 1.f));
	float tanmn = ndotm > DENOM_EPS ? sinmn / ndotm : 0.f;
	float a2 = roughness * roughness;
	float denom = (PI * ndotm2 * ndotm2 * (a2 + tanmn * tanmn) * (a2 + tanmn * tanmn));
	return denom > DENOM_EPS ? (a2 / denom) : 1.f;
}

float UberV2_MicrofacetDistribution_GGX_G1(float roughness, float3 v, float3 m)
{
	float ndotv = fabs(v.y);
	float mdotv = fabs(dot(m, v));

	float sinnv = native_sqrt(1.f - clamp(ndotv * ndotv, 0.f, 1.f));
	float tannv = ndotv > DENOM_EPS ? sinnv / ndotv : 0.f;
	float a2 = roughness * roughness;
	return 2.f / (1.f + native_sqrt(1.f + a2 * tannv * tannv));
}

float UberV2_MicrofacetDistribution_GGX_G(float roughness, float3 wi, float3 wo, float3 wh)
{
	return UberV2_MicrofacetDistribution_GGX_G1(roughness, wi, wh) * UberV2_MicrofacetDistribution_GGX_G1(roughness, wo, wh);
}

void UberV2_MicrofacetDistribution_GGX_SampleNormal(
	// Roughness
	float roughness,
	// Differential geometry
	DisneyShaderData const* shader_data,
	// Sample
	float2 sample,
	// Outgoing  direction
	float3* wh
)
{
	float r1 = sample.x;
	float r2 = sample.y;

	// Sample halfway vector first, then reflect wi around that
	float theta = atan2(roughness * native_sqrt(r1), native_sqrt(1.f - r1));
	float costheta = native_cos(theta);
	float sintheta = native_sin(theta);

	// phi = 2*PI*ksi2
	float phi = 2.f * PI * r2;
	float cosphi = native_cos(phi);
	float sinphi = native_sin(phi);

	// Calculate wh
	*wh = make_float3(sintheta * cosphi, costheta, sintheta * sinphi);
}

float SchlickFresnelReflectance(float u)
{
	float m = clamp(1.f - u, 0.f, 1.f);
	float m2 = m * m;
	return m2 * m2 * m;
}

float CalculateFresnel(
	// IORs
	float top_ior,
	float bottom_ior,
	// Angle between normal and incoming ray
	float ndotwi
)
{
	float etai = top_ior;
	float etat = bottom_ior;
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

INLINE void Disney_PrepareInputs(DifferentialGeometry const* dg, TEXTURE_ARG_LIST, DisneyShaderData* shader_data)
{
	float4 color = Texture_GetValue4f(make_float4(dg->mat.disney.base_color.x,dg->mat.disney.base_color.y,dg->mat.disney.base_color.z,1), dg->uv, TEXTURE_ARGS_IDX(dg->mat.disney.base_color_map_idx));

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
}

float3 UberV2_IdealRefract_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	return 0.f;
}

float UberV2_IdealRefract_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	return 0.f;
}

float3 UberV2_IdealRefract_Sample(DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	const float3 ks = shader_data->base_color.xyz;

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

	return cost > DENOM_EPS ? (eta * eta * ks / cost) : 0.f;
}

float UberV2_MicrofacetRefractionGGX_GetPdf(DisneyShaderData const* shader_data, float3 wi, float3 wo)
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

	// Revert normal and eta if needed
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

	float whpdf = UberV2_MicrofacetDistribution_GGX_D(roughness, wh) * fabs(wh.y);

	float whwo = wodotwh * etat * etat;

	float denom = dot(ht, ht);

	return denom > DENOM_EPS ? whpdf * whwo / denom : 0.f;
}

float3 UberV2_MicrofacetRefractionGGX_Evaluate(DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	const float3 ks = shader_data->base_color.xyz;
	const float roughness = max(shader_data->roughness, ROUGHNESS_EPS);

	float ndotwi = wi.y;
	float ndotwo = wo.y;

	if (ndotwi * ndotwo >= 0.f)
	{
		return 0.f;
	}

	float etai = 1.f;
	float etat = shader_data->refraction_ior;

	// Revert normal and eta if needed
	if (ndotwi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
	}

	// Calc halfway vector
	float3 ht = -(etai * wi + etat * wo);
	float3 wh = normalize(ht);

	float widotwh = fabs(dot(wh, wi));
	float wodotwh = fabs(dot(wh, wo));

	float denom = dot(ht, ht);
	denom *= (fabs(ndotwi) * fabs(ndotwo));

	return denom > DENOM_EPS ? (ks * (widotwh * wodotwh)  * (etat)* (etat)*
		UberV2_MicrofacetDistribution_GGX_G(roughness, wi, wo, wh) * UberV2_MicrofacetDistribution_GGX_D(roughness, wh) / denom) : 0.f;
}

float3 UberV2_MicrofacetRefractionGGX_Sample(DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	const float3 ks = shader_data->base_color.xyz;
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

	// Revert normal and eta if needed
	if (ndotwi < 0.f)
	{
		float tmp = etai;
		etai = etat;
		etat = tmp;
		s = -s;
	}

	float3 wh;
	UberV2_MicrofacetDistribution_GGX_SampleNormal(roughness, shader_data, sample, &wh);

	float c = dot(wi, wh);
	float eta = etai / etat;

	float d = 1 + eta * (c * c - 1);

	if (d <= 0.f)
	{
		*pdf = 0.f;
		return 0.f;
	}

	*wo = normalize((eta * c - s * native_sqrt(d)) * wh - eta * wi);

	*pdf = UberV2_MicrofacetRefractionGGX_GetPdf(shader_data, wi, *wo);

	return UberV2_MicrofacetRefractionGGX_Evaluate(shader_data, wi, *wo);
}

INLINE float3 Diffuse_PennerSkin(DifferentialGeometry const* dg, float ss, float ndotwi, float ir, float3 transmittance)
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

INLINE float Disney_GetPdf(DifferentialGeometry const* dg, DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	float3 base_color = shader_data->base_color;
	float metallic = shader_data->metallic;
	float specular = shader_data->specular;
	float anisotropy = shader_data->anisotropy;
	float roughness = shader_data->roughness;
	float specular_tint = shader_data->specular_tint;
	float sheen_tint = shader_data->sheen_tint;
	float sheen = shader_data->sheen;
	float clearcoat_roughness = shader_data->clearcoat_roughness;
	float clearcoat = shader_data->clearcoat;

	if (wo.y <= 0.0f)
	{
		float bsdfPdf = 0.0f;
		float brdfPdf = fabs(wo.y) / PI;

		return mix(brdfPdf, bsdfPdf, shader_data->transmission);
	}
	else
	{
		float alpha = roughness * roughness;
		float ax = max(0.001f, alpha * ( 1.f + anisotropy));
		float ay = max(0.001f, alpha * ( 1.f - anisotropy));
		float3 wh = normalize(wo + wi);
		float ndotwh = fabs(wh.y);
		float hdotwo = fabs(dot(wh, wo));

		float d_pdf = fabs(wo.y) / PI;
		float r_pdf = GTR2_Aniso(ndotwh, wh.x, wh.z, ax, ay) * ndotwh / (4.f * hdotwo);
		float c_pdf = GTR1(ndotwh, mix(0.001f, 0.1f, clearcoat_roughness)) * ndotwh / (4.f * hdotwo);
		
		float3 cd_lin = native_powr(base_color, 2.2f);
		float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));
		float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;
	   
		float3 c_spec0 = mix(specular * 0.1f * mix(WHITE, c_tint, specular_tint), cd_lin, metallic);
		
		float cs_lum = dot(c_spec0, make_float3(0.3f, 0.6f, 0.1f));    
		float cs_w = cs_lum / (cs_lum + (1.f - metallic) * cd_lum);
		
		float bsdf = mix(cs_w * r_pdf, c_pdf, clearcoat);
		float brdf = mix(cs_w * r_pdf + d_pdf * (1.f - cs_w), c_pdf, clearcoat);

		return mix(mix(brdf, bsdf, shader_data->transmission), 0.f, shader_data->transparency);
	}
}

INLINE float3 Disney_Evaluate(DifferentialGeometry const* dg, DisneyShaderData const* shader_data, float3 wi, float3 wo)
{
	float3 base_color = shader_data->base_color;
	float metallic = shader_data->metallic;
	float specular = shader_data->specular;
	float anisotropy = shader_data->anisotropy;
	float roughness = shader_data->roughness;
	float specular_tint = shader_data->specular_tint;
	float sheen_tint = shader_data->sheen_tint;
	float sheen = shader_data->sheen;
	float clearcoat_roughness = shader_data->clearcoat_roughness;
	float clearcoat = shader_data->clearcoat;
	float subsurface = shader_data->subsurface;
	float3 subsurface_color = shader_data->subsurface_color;

	float ndotwi = fabs(wi.y);
	float ndotwo = fabs(wo.y);
	
	float3 h = normalize(wi + wo);
	float ndoth = fabs(h.y);
	float hdotwo = fabs(dot(h, wo));
	float visiability = clamp(wo.y, 0.0f, 1.0f);
	
	float3 cd_lin = base_color;
	float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));
	
	float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;    
	float3 c_spec0 = mix(specular * 0.1f * mix(WHITE, c_tint, specular_tint), cd_lin, metallic);    
	float3 c_sheen = mix(WHITE, c_tint, sheen_tint);

	float3 bsdf = 0;
	if (shader_data->transmission > 0.0f)
	{
		if (wo.y <= 0.0f)
		{
			float fresnel = CalculateFresnel(1.0, shader_data->refraction_ior, wi.y);
			bsdf = shader_data->transmission * (1.0f-fresnel) / fabs(wo.y) * (1.0f - metallic);
		}
		else
		{
			float alpha = roughness * roughness;
			float ax = max(0.001f, alpha * ( 1.f + anisotropy));
			float ay = max(0.001f, alpha * ( 1.f - anisotropy));
			float ds = GTR2_Aniso(ndoth, h.x, h.z, ax, ay) * ndotwo;
			float fh = SchlickFresnelReflectance(hdotwo);
			float3 fs = mix(c_spec0, WHITE, fh);

			float gs;
			gs  = SmithGGX_G_Aniso(ndotwo, wo.x, wo.z, ax, ay);
			gs *= SmithGGX_G_Aniso(ndotwi, wi.x, wi.z, ax, ay);

			bsdf = gs * fs * ds;
		}
	}

	float3 brdf = 0;
	if (shader_data->transmission < 1.0f && wo.y > 0.0f)
	{			
		float f_wo = SchlickFresnelReflectance(ndotwo);
		float f_wi = SchlickFresnelReflectance(ndotwi);
		
		float fd90 = 0.5f + 2 * hdotwo * hdotwo * roughness;
		float fd = mix(1.f, fd90, f_wo) * mix(1.f, fd90, f_wi) * ndotwo;
		
		float fss90 = hdotwo * hdotwo * roughness;
		float fss = mix(1.f, fss90, f_wo) * mix(1.f, fss90, f_wi);
		float ss = 1.25f * (fss * (1.f / (ndotwo + ndotwi) - 0.5f) + 0.5f) * visiability;
		
		float alpha = roughness * roughness;
		float ax = max(0.001f, alpha * ( 1.f + anisotropy));
		float ay = max(0.001f, alpha * ( 1.f - anisotropy));
		float ds = GTR2_Aniso(ndoth, h.x, h.z, ax, ay) * ndotwo;
		float fh = SchlickFresnelReflectance(hdotwo);
		float3 fs = mix(c_spec0, WHITE, fh);
		
		float gs;
		gs  = SmithGGX_G_Aniso(ndotwo, wo.x, wo.z, ax, ay);
		gs *= SmithGGX_G_Aniso(ndotwi, wi.x, wi.z, ax, ay);
		
		float3 f_sheen = fh * sheen * c_sheen * ndotwo;
		
		float dr = GTR1(ndoth, mix(0.001f, 0.1f, clearcoat_roughness)) * ndotwo;
		float fr = mix(0.04f, 1.f, fh);
		float gr = SmithGGX_G(ndotwo, 0.25f) * SmithGGX_G(ndotwi, 0.25f);

		brdf = mix(((1.f / PI) * (mix(fd, Diffuse_PennerSkin(dg, ss, wo.y, 0.5f, subsurface_color), subsurface) * cd_lin) + f_sheen) * (1.f - metallic) + gs * fs * ds + clearcoat * gr * fr * dr, 0.f, shader_data->transparency);
	}

	return mix(brdf, bsdf, shader_data->transmission);
}

float3 Disney_Sample(DifferentialGeometry* dg, DisneyShaderData const* shader_data, float2 sample, float3 wi, float3* wo, float* pdf)
{
	if (sample.y < shader_data->transparency)
	{
		sample.y -= shader_data->transparency;
		sample.y /= (1 - shader_data->transparency);

		int bxdf_flags = (kBxdfFlagsTransparency | kBxdfFlagsSingular);
		Bxdf_SetFlags(dg, bxdf_flags);
		Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleTransparency);
		
		*wo = -wi;
		*pdf = 1.f;

		float coswo = fabs((*wo).y);
		return coswo > 1e-5f ? (1.f / coswo) : 0.f;
	}
	else
	{
		if (sample.y < shader_data->transmission)
		{
			float fresnel = CalculateFresnel(1.0, shader_data->refraction_ior, wi.y);
			if (sample.y < fresnel)
			{
				float alpha = shader_data->roughness * shader_data->roughness;
				float ax = max(0.001f, alpha * ( 1.f + shader_data->anisotropy));
				float ay = max(0.001f, alpha * ( 1.f - shader_data->anisotropy));
				float t = native_sqrt(sample.y / (1.f - sample.y));
				float3 wh = normalize(make_float3(t * ax * native_cos(2.f * PI * sample.x), 1.f, t * ay * native_sin(2.f * PI * sample.x)));
				
				Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);
				int bxdf_flags = kBxdfFlagsBrdf | kBxdfFlagsSingular;
				Bxdf_SetFlags(dg, bxdf_flags);

				*wo = -wi + 2.f * fabs(dot(wi, wh)) * wh;
				*pdf = Disney_GetPdf(dg, shader_data, wi, *wo);
			
				return Disney_Evaluate(dg, shader_data, wi, *wo);
			}
			else
			{
				sample.y -= shader_data->transmission;
				sample.y /= (1 - shader_data->transmission);

				int bxdf_flags = kBxdfFlagsSingular;
				Bxdf_SetFlags(dg, bxdf_flags);
				Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleRefraction);

				return UberV2_IdealRefract_Sample(shader_data, sample, wi, wo, pdf);
			}
		}
		else
		{
			float3 wh;

			int bxdf_flags = kBxdfFlagsBrdf;
			if (sample.x < shader_data->clearcoat)
			{
				sample.x /= (shader_data->clearcoat);
				
				float a = mix(0.001f, 0.1f, shader_data->clearcoat_roughness);
				float ndotwh = native_sqrt((1.f - native_powr(a*a, 1.f - sample.y)) / (1.f - a*a));
				float sintheta = native_sqrt(1.f - ndotwh * ndotwh);
				wh = normalize(make_float3(native_cos(2.f * PI * sample.x) * sintheta,
										   ndotwh,
										   native_sin(2.f * PI * sample.x) * sintheta));

				Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);
				bxdf_flags |= kBxdfFlagsSingular;
				Bxdf_SetFlags(dg, bxdf_flags);

				*wo = -wi + 2.f*fabs(dot(wi, wh)) * wh;
			}
			else
			{
				sample.x -= (shader_data->clearcoat);
				sample.x /= (1.f - shader_data->clearcoat);

				float3 cd_lin = shader_data->base_color;
				float cd_lum = dot(cd_lin, make_float3(0.3f, 0.6f, 0.1f));

				float3 c_tint = cd_lum > 0.f ? (cd_lin / cd_lum) : WHITE;
				float3 c_spec0 = mix(shader_data->specular * 0.3f * mix(WHITE, c_tint, shader_data->specular_tint), cd_lin, shader_data->metallic);

				float cs_lum = dot(c_spec0, make_float3(0.3f, 0.6f, 0.1f));
				float cs_w = cs_lum / (cs_lum + (1.f - shader_data->metallic) * cd_lum);
				
				if (sample.y < cs_w)
				{
					sample.y /= cs_w;

					float alpha = shader_data->roughness * shader_data->roughness;
					float ax = max(0.001f, alpha * ( 1.f + shader_data->anisotropy));
					float ay = max(0.001f, alpha * ( 1.f - shader_data->anisotropy));
					float t = native_sqrt(sample.y / (1.f - sample.y));
					wh = normalize(make_float3(t * ax * native_cos(2.f * PI * sample.x), 1.f, t * ay * native_sin(2.f * PI * sample.x)));
					
					Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);
					bxdf_flags |= kBxdfFlagsSingular;
					Bxdf_SetFlags(dg, bxdf_flags);

					*wo = -wi + 2.f * fabs(dot(wi, wh)) * wh;
				}
				else
				{
					sample.y -= cs_w;
					sample.y /= (1.f - cs_w);

					Bxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleDiffuse);
					bxdf_flags |= kBxdfFlagsBrdf;
					Bxdf_SetFlags(dg, bxdf_flags);
					
					*wo = Sample_MapToHemisphere(sample, make_float3(0.f, 1.f, 0.f) , 1.f);
				}
			}
			
			*wo = normalize(*wo);
			*pdf = Disney_GetPdf(dg, shader_data, wi, *wo);
			
			return Disney_Evaluate(dg, shader_data, wi, *wo);
		}
	}
}

#endif