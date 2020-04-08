#include <octoon/video/montecarlo.h>
#include "math.h"
#include <assert.h>
#include <atomic>
#include <string>
#include <CL/cl.h>
#include <stdexcept>

namespace octoon::video
{
	float GetPhysicalLightAttenuation(const RadeonRays::float3& L)
	{
		return 1.0f / std::max(1.0f, L.sqnorm());
	}

	float GetPhysicalLightAttenuation(const RadeonRays::float3& L, float radius, float attenuationBulbSize)
	{
		const float invRadius = 1.0f / radius;
		float d = std::sqrt(L.sqnorm());
		float fadeoutFactor = std::min(1.0f, std::max(0.0f, (radius - d) * (invRadius / 0.2f)));
		d = std::max(d - attenuationBulbSize, 0.0f);
		float denom = 1.0f + d / attenuationBulbSize;
		float attenuation = fadeoutFactor * fadeoutFactor / (denom * denom);
		return attenuation;
	}

	RadeonRays::float3 InterpolateVertices(const math::float3* vec, const math::uint1* indices, int prim_id, const RadeonRays::float4& barycentrics)
	{
		auto i0 = indices[prim_id * 3];
		auto i1 = indices[prim_id * 3 + 1];
		auto i2 = indices[prim_id * 3 + 2];

		RadeonRays::float3 a(vec[i0 * 3].x, vec[i0 * 3].y, vec[i0 * 3].z);
		RadeonRays::float3 b(vec[i1 * 3].x, vec[i1 * 3].y, vec[i1 * 3].z);
		RadeonRays::float3 c(vec[i2 * 3].x, vec[i2 * 3].y, vec[i2 * 3].z);

		return a * (1 - barycentrics.x - barycentrics.y) + b * barycentrics.x + c * barycentrics.y;
	}

	RadeonRays::float3 InterpolateNormals(const math::float3* vec, const math::uint1* indices, int prim_id, const RadeonRays::float4& barycentrics)
	{
		return RadeonRays::normalize(InterpolateVertices(vec, indices, prim_id, barycentrics));
	}

	MonteCarlo::MonteCarlo() noexcept
		: numBounces_(1)
		, tileNums_(0)
		, width_(0)
		, height_(0)
		, api_(nullptr)
	{
		renderData_.numEstimate = 0;
		renderData_.fr_rays = nullptr;
		renderData_.fr_hits = nullptr;
		renderData_.fr_shadowrays = nullptr;
		renderData_.fr_shadowhits = nullptr;
	}

	MonteCarlo::MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept
		: MonteCarlo()
	{
		this->setup(w, h);
	}

	MonteCarlo::~MonteCarlo() noexcept
	{
		if (renderData_.fr_rays)
			api_->DeleteBuffer(renderData_.fr_rays);
		if (renderData_.fr_hits)
			api_->DeleteBuffer(renderData_.fr_hits);
		if (renderData_.fr_shadowhits)
			api_->DeleteBuffer(renderData_.fr_shadowhits);
		if (renderData_.fr_shadowrays)
			api_->DeleteBuffer(renderData_.fr_shadowrays);
		if (api_)
			RadeonRays::IntersectionApi::Delete(api_);
	}

	void
	MonteCarlo::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		width_ = w;
		height_ = h;

		if (!init_Gbuffers(w, h)) throw std::runtime_error("init_Gbuffers() fail");
		if (!init_RadeonRays()) throw std::runtime_error("init_RadeonRays() fail");
	}

	bool
	MonteCarlo::init_Gbuffers(std::uint32_t, std::uint32_t h) noexcept
	{
		auto allocSize = width_ * height_;
		ldr_.resize(allocSize);
		hdr_.resize(allocSize);

		return true;
	}

	bool
	MonteCarlo::init_RadeonRays() noexcept
	{
		RadeonRays::IntersectionApi::SetPlatform(RadeonRays::DeviceInfo::kAny);

		auto deviceidx = std::string::npos;
		for (auto i = 0U; i < RadeonRays::IntersectionApi::GetDeviceCount(); ++i)
		{
			RadeonRays::DeviceInfo devinfo;
			RadeonRays::IntersectionApi::GetDeviceInfo(i, devinfo);

			if (devinfo.type == RadeonRays::DeviceInfo::kGpu)
			{
				std::string info_name(devinfo.name);
				if (info_name.find("Intel") != std::string::npos)
					continue;
				deviceidx = i;
				break;
			}
		}

		if (deviceidx == std::string::npos)
		{
			for (auto i = 0U; i < RadeonRays::IntersectionApi::GetDeviceCount(); ++i)
			{
				RadeonRays::DeviceInfo devinfo;
				RadeonRays::IntersectionApi::GetDeviceInfo(i, devinfo);

				if (devinfo.type == RadeonRays::DeviceInfo::kCpu)
				{
					deviceidx = i;
					break;
				}
			}
		}

		if (deviceidx == std::string::npos) return false;

		this->api_ = RadeonRays::IntersectionApi::Create((std::uint32_t)deviceidx);
		return this->api_ != nullptr;
	}

	const std::uint32_t*
	MonteCarlo::data() const noexcept
	{
		return ldr_.data();
	}

	bool
	MonteCarlo::compileScene(const std::vector<geometry::Geometry*>& geometries) noexcept(false)
	{
		bool needCommit = false;

		for (std::int32_t i = 0; i < geometries.size(); i++)
		{
			auto& geometry = geometries[i];
			if (!geometry->getVisible())
				continue;

			if (!geometry->getGlobalIllumination())
				continue;

			auto& mesh = geometry->getMesh();

			for (std::int32_t j = 0; j < geometry->getMaterials().size(); j++)
			{
				math::float3* vertdata = mesh->getVertexArray().data();
				math::uint1* indices = mesh->getIndicesArray(j).data();
				auto it = sceneId_.find(indices);
				if (it == sceneId_.end())
				{
					std::size_t nvert = mesh->getVertexArray().size();
					std::size_t nfaces = mesh->getIndicesArray(j).size() / 3;

					RadeonRays::Shape* rrShape = this->api_->CreateMesh((float*)vertdata, (int)nvert, sizeof(math::float3), (int*)indices, 0, nullptr, (int)nfaces);
					rrShape->SetId(RadeonRays::Id(this->sceneId_.size()));

					assert(rrShape != nullptr);
					this->api_->AttachShape(rrShape);

					auto shape = std::make_shared<Shape>();
					shape->mesh.positions = mesh->getVertexArray();
					shape->mesh.normals = mesh->getNormalArray();
					shape->mesh.texcoords = mesh->getTexcoordArray();
					shape->mesh.indices = mesh->getIndicesArray();

					auto mat = geometry->getMaterial(j);

					auto material = std::make_shared<Material>();
					mat->get("diffuse", material->albedo);
					mat->get("emissive", material->emissive);

					needCommit = true;

					if (rrShape->GetId() >= this->scene_.size())
					{
						this->scene_.resize((rrShape->GetId() | 1) << 1);
						this->materials_.resize((rrShape->GetId() | 1) << 1);
					}

					this->scene_[rrShape->GetId()] = shape;
					this->materials_[rrShape->GetId()] = material;
					this->sceneId_[indices] = rrShape;
				}
			}
		}

		if (needCommit)
			this->api_->Commit();

		return !this->scene_.empty();
	}

	void
	MonteCarlo::GenerateWorkspace(std::int32_t numEstimate)
	{
		if (tileNums_ < numEstimate)
		{
			renderData_.hits.resize(numEstimate);
			renderData_.samples.resize(numEstimate);
			renderData_.samplesAccum.resize(numEstimate);
			renderData_.random.resize(numEstimate);
			renderData_.weights.resize(numEstimate);
			renderData_.shadowRays.resize(numEstimate);
			renderData_.shadowHits.resize(numEstimate);

			renderData_.rays[0].resize(numEstimate);
			renderData_.rays[1].resize(numEstimate);

			if (renderData_.fr_rays)
				api_->DeleteBuffer(renderData_.fr_rays);

			if (renderData_.fr_hits)
				api_->DeleteBuffer(renderData_.fr_hits);

			if (renderData_.fr_shadowrays)
				api_->DeleteBuffer(renderData_.fr_shadowrays);

			if (renderData_.fr_shadowhits)
				api_->DeleteBuffer(renderData_.fr_shadowhits);

			renderData_.fr_rays = api_->CreateBuffer(sizeof(RadeonRays::ray) * numEstimate, nullptr);
			renderData_.fr_hits = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numEstimate, nullptr);
			renderData_.fr_shadowrays = api_->CreateBuffer(sizeof(RadeonRays::ray) * numEstimate, nullptr);
			renderData_.fr_shadowhits = api_->CreateBuffer(sizeof(RadeonRays::Intersection) * numEstimate, nullptr);

			tileNums_ = numEstimate;
		}

		this->renderData_.numEstimate = numEstimate;
	}

	void
	MonteCarlo::GenerateCamera(const camera::Camera& camera, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

		float aspect = (float)width_ / height_;
		float xstep = 2.0f / (float)this->width_;
		float ystep = 2.0f / (float)this->height_;

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;

			float x = xstep * ix - 1.0f + (renderData_.random[i].x * 2 - 1) / (float)this->width_;
			float y = ystep * iy - 1.0f + (renderData_.random[i].y * 2 - 1) / (float)this->height_;
			float z = 1.0f;

			auto& ray = rays[i];
			ray.o = RadeonRays::float4(camera.getTranslate().x, camera.getTranslate().y, camera.getTranslate().z);
			ray.d = RadeonRays::normalize(RadeonRays::float3(x * aspect, y, z - ray.o.z));
			ray.SetMaxT(std::numeric_limits<float>::max());
			ray.SetTime(0.0f);
			ray.SetMask(-1);
			ray.SetActive(true);
		}

		std::memcpy(renderData_.rays[0].data(), rays, sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_rays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GenerateRays(std::uint32_t pass) noexcept
	{
		RadeonRays::ray* data = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_rays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate, (void**)&data, &e); e->Wait(); api_->DeleteEvent(e);

		std::memset(renderData_.weights.data(), 0, sizeof(RadeonRays::float3) * this->renderData_.numEstimate);
		std::memset(renderData_.rays[(pass & 1) ^ 1].data(), 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		auto& rays = renderData_.rays[(pass & 1) ^ 1];
		auto& views = renderData_.rays[pass & 1];

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto& hit = renderData_.hits[i];
			auto& ray = rays[i];
			auto& view = views[i];

			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				auto& mesh = scene_[hit.shapeid]->mesh;
				auto& mat = materials_[hit.shapeid];

				if (mat->isEmissive())
					continue;

				auto ro = InterpolateVertices(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto norm = InterpolateNormals(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);

				RadeonRays::float3 L;
				renderData_.weights[i] = 1.0f;//Disney_Sample(norm, -view.d, mat, renderData_.random[i], L);

				assert(renderData_.weights[i].w > 0.0f);
				ray.d = L;
				ray.o = ro + L * 1e-5f;
				ray.SetMaxT(std::numeric_limits<float>::max());
				ray.SetTime(0.0f);
				ray.SetMask(-1);
				ray.SetActive(true);
			}
		}

		std::memcpy(data, renderData_.rays[(pass & 1) ^ 1].data(), sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_rays, data, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GenerateLightRays(const light::Light& light) noexcept
	{
		RadeonRays::ray* rays = nullptr;
		RadeonRays::Event* e = nullptr;
		api_->MapBuffer(renderData_.fr_shadowrays, RadeonRays::kMapWrite, 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate, (void**)&rays, &e); e->Wait(); api_->DeleteEvent(e);

		std::memset(renderData_.shadowRays.data(), 0, sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto& hit = renderData_.hits[i];
			if (hit.shapeid != RadeonRays::kNullId && hit.primid != RadeonRays::kNullId)
			{
				auto& mesh = scene_[hit.shapeid]->mesh;
				auto& mat = materials_[mesh.material_ids[hit.primid]];

				if (mat->isEmissive())
					continue;
					
				auto ro = InterpolateVertices(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto norm = InterpolateNormals(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);

				RadeonRays::float4 L = RadeonRays::float4(0, 0, 0, 0);//light.sample(ro, norm, mat, renderData_.random[i]);
				assert(std::isfinite(L[0] + L[1] + L[2]));

				if (L.w > 0.0f)
				{
					auto& ray = renderData_.shadowRays[i];
					ray.d = RadeonRays::float3(L[0], L[1], L[2]);
					ray.o = ro + ray.d * 1e-5f;
					ray.SetMaxT(L.w);
					ray.SetTime(0.0f);
					ray.SetMask(-1);
					ray.SetActive(true);
				}
			}
		}

		std::memcpy(rays, renderData_.shadowRays.data(), sizeof(RadeonRays::ray) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_shadowrays, rays, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherHits() noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		api_->MapBuffer(renderData_.fr_hits, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate, (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

		std::memcpy(renderData_.hits.data(), hit, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_hits, hit, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherShadowHits() noexcept
	{
		RadeonRays::Event* e = nullptr;
		RadeonRays::Intersection* hit = nullptr;

		api_->MapBuffer(renderData_.fr_shadowhits, RadeonRays::kMapRead, 0, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate, (void**)&hit, &e); e->Wait(); api_->DeleteEvent(e);

		std::memcpy(renderData_.shadowHits.data(), hit, sizeof(RadeonRays::Intersection) * this->renderData_.numEstimate);

		api_->UnmapBuffer(renderData_.fr_shadowhits, hit, &e); e->Wait(); api_->DeleteEvent(e);
	}

	void
	MonteCarlo::GatherFirstSampling() noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			renderData_.samples[i] = 0;
			renderData_.samplesAccum[i] = 0;

			auto& hit = renderData_.hits[i];
			if (hit.shapeid != RadeonRays::kNullId)
			{
				auto& mesh = scene_[hit.shapeid]->mesh;
				auto& mat = materials_[hit.shapeid];

				if (mat->isEmissive())
					renderData_.samplesAccum[i] += RadeonRays::float3(mat->emissive.x, mat->emissive.y, mat->emissive.z);
					
				renderData_.samples[i] = RadeonRays::float3(1, 1, 1);
			}					
		}
	}

	void
	MonteCarlo::GatherSampling(std::int32_t pass) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			auto& hit = renderData_.hits[i];
			if (hit.shapeid != RadeonRays::kNullId)
			{
				auto& mesh = scene_[hit.shapeid]->mesh;
				auto& mat = materials_[hit.primid];

				auto ro = InterpolateVertices(mesh.positions.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto atten = GetPhysicalLightAttenuation(renderData_.rays[pass & 1][i].o - ro);
					
				assert(renderData_.weights[i].w > 0);

				auto& sample = renderData_.samples[i];
				sample *= renderData_.weights[i] * (1.0f / renderData_.weights[i].w) * atten;

				if (mat->isEmissive())
				{
					renderData_.samplesAccum[i] += renderData_.samples[i] * RadeonRays::float3(mat->emissive.x, mat->emissive.y, mat->emissive.z);
				}
			}
		}
	}

	void
	MonteCarlo::GatherLightSamples(std::uint32_t pass, const light::Light& light) noexcept
	{
		auto& rays = renderData_.shadowRays;
		auto& views = renderData_.rays[pass & 1];

#pragma omp parallel for
		for (std::int32_t i = 0; i < this->renderData_.numEstimate; ++i)
		{
			if (rays[i].IsActive())
			{
				auto& hit = renderData_.hits[i];
				auto& shadowHit = renderData_.shadowHits[i];
				if (shadowHit.shapeid != RadeonRays::kNullId)
					continue;

				auto& mesh = scene_[hit.shapeid]->mesh;
				auto& mat = materials_[mesh.material_ids[hit.primid]];

				auto norm = InterpolateNormals(mesh.normals.data(), mesh.indices.data(), hit.primid, hit.uvwt);
				auto sample = renderData_.samples[i];// *light.Li(norm, -views[i].d, rays[i].d, mat, renderData_.random[i]);

				renderData_.samplesAccum[i] += sample * (1.0f / (rays[i].GetMaxT() * rays[i].GetMaxT()));
			}
		}
	}

	void
	MonteCarlo::Estimate(const camera::Camera& camera, std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size)
	{
		this->GenerateWorkspace(size.x * size.y);
		this->GenerateCamera(camera, offset, size);

		for (std::int32_t pass = 0; pass < this->numBounces_; pass++)
		{
			api_->QueryIntersection(
				renderData_.fr_rays,
				renderData_.numEstimate,
				renderData_.fr_hits,
				nullptr,
				nullptr
			);

			this->GatherHits();

			if (pass == 0)
				this->GatherFirstSampling();
			else
				this->GatherSampling(pass);

			/*for (auto& light : RenderScene::instance().getLightList())
			{
				if (light->getLayer() != camera.getLayer())
					continue;

				this->GenerateLightRays(*light);

				api_->QueryIntersection(
					renderData_.fr_shadowrays,
					renderData_.numEstimate,
					renderData_.fr_shadowhits,
					nullptr,
					nullptr
				);

				this->GatherShadowHits();
				this->GatherLightSamples(pass, *light);
			}*/

			// prepare ray for indirect lighting gathering
			if (pass + 1 < this->numBounces_)
				this->GenerateRays(pass);
		}

		this->AccumSampling(frame, offset, size);
		this->AdaptiveSampling();

		this->ColorTonemapping(frame, offset, size);
	}

	void
	MonteCarlo::AccumSampling(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < size.x * size.y; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;
			auto index = iy * this->width_ + ix;

			auto& hdr = hdr_[index];
			hdr.x += renderData_.samplesAccum[i].x;
			hdr.y += renderData_.samplesAccum[i].y;
			hdr.z += renderData_.samplesAccum[i].z;
		}
	}

	void
	MonteCarlo::AdaptiveSampling() noexcept
	{
	}

	void
	MonteCarlo::ColorTonemapping(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept
	{
#pragma omp parallel for
		for (std::int32_t i = 0; i < size.x * size.y; ++i)
		{
			auto ix = offset.x + i % size.x;
			auto iy = offset.y + i / size.x;
			auto index = iy * this->width_ + ix;

			auto& hdr = hdr_[index];
			assert(std::isfinite(hdr.x));
			assert(std::isfinite(hdr.y));
			assert(std::isfinite(hdr.z));

			std::uint8_t r = std::uint8_t(hdr.x / frame * 255);
			std::uint8_t g = std::uint8_t(hdr.y / frame * 255);
			std::uint8_t b = std::uint8_t(hdr.z / frame * 255);

			ldr_[index] = 0xFF << 24 | b << 16 | g << 8 | r;
		}
	}

	void
	MonteCarlo::render(const camera::Camera& camera, const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries, std::uint32_t frame, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		if (this->compileScene(geometries))
			this->Estimate(camera, frame, RadeonRays::int2(x, y), RadeonRays::int2(w, h));
	}
}