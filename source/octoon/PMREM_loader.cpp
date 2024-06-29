#include <octoon/pmrem_loader.h>
#include <octoon/runtime/except.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/video/renderer.h>
#include <octoon/material/material.h>
#include <octoon/mesh/plane_mesh.h>
#include <octoon/camera/ortho_camera.h>

namespace octoon
{
constexpr auto pmrem_vert = R"(
out vec2 vUv;
void main()
{
	vUv = TEXCOORD0;
	gl_Position = vec4(vec3(POSITION0), 1);
})";

constexpr auto irradiance_frag = R"(
#define PI 3.14159265359
#define PI2 6.28318530718
#define RECIPROCAL_PI 0.31830988618

#define saturate(a) clamp( a, 0.0, 1.0 )

uniform sampler2D environmentMap;
in vec2 vUv;

vec3 SphereNormal(vec2 uv)
{
	vec3 normal;
	normal.x = -sin(uv.y * PI) * sin(uv.x * PI2);
	normal.y = cos(uv.y * PI);
	normal.z = sin(uv.y * PI) * cos(uv.x * PI2);
	return normal;
}

vec4 textureLatlongUV(sampler2D texture, vec3 L, float mipLevel)
{
	vec2 uv = vec2((atan(L.x, L.z) * RECIPROCAL_PI * 0.5f + 0.5f), acos(L.y) * RECIPROCAL_PI);
	return textureLod(texture, uv, mipLevel);
}

void main()
{
	vec3 irradiance = vec3(0.0);  

	vec3 normal = SphereNormal(vUv);
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up         = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += textureLatlongUV(environmentMap, sampleVec, 0).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	fragColor = vec4(PI * irradiance * (1.0 / float(nrSamples)), 1.0);
}
)";

constexpr auto radiance_frag = R"(
#define PI 3.14159265359
#define PI2 6.28318530718
#define RECIPROCAL_PI 0.31830988618

#define saturate(a) clamp( a, 0.0, 1.0 )

in vec2 vUv;

uniform sampler2D environmentMap;
uniform float roughness;
uniform float environmentSize;

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

    return normalize(sampleVec);
}

vec3 SphereNormal(vec2 uv)
{
	vec3 normal;
	normal.x = -sin(uv.y * PI) * sin(uv.x * PI2);
	normal.y = cos(uv.y * PI);
	normal.z = sin(uv.y * PI) * cos(uv.x * PI2);
	return normalize(normal);
}

vec4 textureLatlongUV(sampler2D texture, vec3 L, float mipLevel)
{
	float phi = atan(L.x, L.z);
	float r = sqrt(L.x*L.x + L.y*L.y + L.z*L.z);
	phi = phi >= 0 ? phi : phi + 2 * PI;
	vec2 uv = vec2(phi * RECIPROCAL_PI * 0.5f + 0.5f, acos(L.y / r) * RECIPROCAL_PI);
	return textureLod(texture, uv, mipLevel);
}

float D_GGX(float nh, float roughness)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float spec = (nh * m2 - nh) * nh + 1;
    return m2 / (spec * spec);
}

void main()
{
	const uint SAMPLE_COUNT = 256u;

    vec3 N = SphereNormal(vUv);
    vec3 R = N;
    vec3 V = R;

    vec4 prefilteredColor = vec4(0.0);

    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = clamp(normalize(2.0 * dot(V, H) * H - V), -1.0f, 1.0f);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
			float NdotH = saturate(dot(N, H));
			float VdotH = saturate(dot(V, H));
			float pdf = D_GGX(NdotH, roughness) * NdotH / (4.0f * VdotH);

			float fOmegaS = 1.0 / (SAMPLE_COUNT * pdf + 1e-5f);
			float fOmegaP = 4.0 * PI / environmentSize;

			float fMipBias = 1.0f;
			float fMipLevel = roughness == 0.0f ? 0.0f : max(0.5f * log2(fOmegaS / fOmegaP) + fMipBias, 0.0f);

            prefilteredColor.rgb += textureLatlongUV(environmentMap, L, fMipLevel).rgb * NdotL;
            prefilteredColor.a += NdotL;
        }
    }

    fragColor = vec4(prefilteredColor.rgb / prefilteredColor.a, 1.0);
})";

constexpr auto copy_frag = R"(
#define PI 3.14159265359
#define PI2 6.28318530718
#define RECIPROCAL_PI 0.31830988618

in vec2 vUv;

uniform sampler2D environmentMap;

vec3 SphereNormal(vec2 uv)
{
	vec3 normal;
	normal.x = -sin(uv.y * PI) * sin(uv.x * PI2);
	normal.y = cos(uv.y * PI);
	normal.z = sin(uv.y * PI) * cos(uv.x * PI2);
	return normalize(normal);
}

vec4 textureLatlongUV(sampler2D texture, vec3 L, float mipLevel)
{
	float phi = atan(L.x, L.z);
	float r = sqrt(L.x*L.x + L.y*L.y + L.z*L.z);
	phi = phi >= 0 ? phi : phi + 2 * PI;
	vec2 uv = vec2(phi * RECIPROCAL_PI * 0.5f + 0.5f, acos(L.y / r) * RECIPROCAL_PI);
	return textureLod(texture, uv, mipLevel);
}

void main()
{
	fragColor = textureLatlongUV(environmentMap, SphereNormal(vUv), 0);
}
)";

	std::shared_ptr<Texture>
	PMREMLoader::load(const std::shared_ptr<Texture>& environmentMap, std::uint8_t mipNums) noexcept(false)
	{
		if (environmentMap)
		{
			std::uint32_t width = 32 << (mipNums - 1);
			std::uint32_t height = 16 << (mipNums - 1);

			auto renderContext = Renderer::instance()->getGraphicsDevice();

			auto colorTexture = std::make_shared<Texture>(Format::R32G32B32SFloat, width, height);
			colorTexture->setMipBase(0);
			colorTexture->setMipLevel(8);
			colorTexture->apply();

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setSize(width, height);
			depthTextureDesc.setTexDim(TextureDimension::Texture2D);
			depthTextureDesc.setTexFormat(GraphicsFormat::D16UNorm);
			depthTextureDesc.setMipBase(0);
			depthTextureDesc.setMipNums(8);
			auto depthTexture = renderContext->createTexture(depthTextureDesc);
			if (!depthTexture)
				throw runtime_error::create("createTexture() failed");

			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R32G32B32SFloat));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::D16UNorm));

			std::vector<GraphicsFramebufferPtr> framebuffers(mipNums);

			for (std::uint8_t i = 0; i < mipNums; i++)
			{
				GraphicsFramebufferDesc framebufferDesc;
				framebufferDesc.setWidth(std::max<std::uint32_t>(width >> i, 1));
				framebufferDesc.setHeight(std::max<std::uint32_t>(height >> i, 1));
				framebufferDesc.setFramebufferLayout(renderContext->createFramebufferLayout(framebufferLayoutDesc));
				framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture, i, 0));
				framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture->getNativeTexture(), i, 0));

				framebuffers[i] = renderContext->createFramebuffer(framebufferDesc);
				if (!framebuffers[i])
					throw runtime_error::create("createFramebuffer() failed");
			}

			auto radiance = std::make_shared<Material>(std::make_shared<Shader>(pmrem_vert, radiance_frag));
			radiance->set("environmentMap", environmentMap);
			radiance->set("environmentSize", (float)environmentMap->width() * environmentMap->height());
			radiance->setDepthEnable(false);
			radiance->setDepthWriteEnable(false);

			auto copyMaterial = std::make_shared<Material>(std::make_shared<Shader>(pmrem_vert, copy_frag));
			copyMaterial->set("environmentMap", environmentMap);
			copyMaterial->setDepthEnable(false);
			copyMaterial->setDepthWriteEnable(false);

			Geometry radianceGeometry;
			radianceGeometry.setMesh(std::make_shared<PlaneMesh>(2.0f, 2.0f));
			radianceGeometry.setMaterial(radiance);

			Geometry copyGeometry;
			copyGeometry.setMesh(std::make_shared<PlaneMesh>(2.0f, 2.0f));
			copyGeometry.setMaterial(copyMaterial);

			OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 100.f);

			for (std::uint8_t i = 0; i < mipNums; i++)
			{
				auto scene = std::make_shared<RenderScene>();
				scene->addCamera(&camera);

				camera.setFramebuffer(framebuffers[i]);
				radiance->set("roughness", float(i) / (mipNums - 1));

				if (i == 0)
					scene->addGeometry(&copyGeometry);
				else
					scene->addGeometry(&radianceGeometry);

				Renderer::instance()->render(scene);
			}

			return colorTexture;
		}
		else
		{
			return nullptr;
		}
	}

	std::shared_ptr<Texture>
	PMREMLoader::load(std::string_view filepath, std::uint8_t mipNums) noexcept(false)
	{
		auto texture = std::make_shared<Texture>((std::string)filepath);
		texture->setMipLevel(8);
		texture->apply();
		return load(texture, mipNums);
	}
}