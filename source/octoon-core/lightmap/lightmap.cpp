#include <octoon/lightmap/lightmap.h>
#include <octoon/lightmap/lightmap_pack.h>
#include <octoon/camera/camera.h>

namespace octoon::bake
{
	bool lineIntersection(const math::float2& x0, const math::float2& x1, const math::float2& y0, const math::float2& y1, math::float2* res)
	{
		math::float2 dx = x1 - x0;
		math::float2 dy = y1 - y0;
		math::float2 d = x0 - y0;
		float dyx = math::cross(dy, dx);
		if (dyx == 0.0f) return false;
		dyx = math::cross(d, dx) / dyx;
		if (dyx <= 0 || dyx >= 1) return false;
		res->x = y0.x + dyx * dy.x;
		res->y = y0.y + dyx * dy.y;
		return true;
	}

	int leftOf(const math::float2& a, const math::float2& b, const math::float2& c)
	{
		float x = math::cross(b - a, c - b);
		return x < 0 ? -1 : x > 0;
	}

	int convexClip(math::float2* poly, int nPoly, const math::float2 clip[], int nClip, math::float2* res)
	{
		int nRes = nPoly;
		int dir = leftOf(clip[0], clip[1], clip[2]);
		for (int i = 0, j = nClip - 1; i < nClip && nRes; j = i++)
		{
			if (i != 0)
				for (nPoly = 0; nPoly < nRes; nPoly++)
					poly[nPoly] = res[nPoly];
			nRes = 0;
			math::float2 v0 = poly[nPoly - 1];
			int side0 = leftOf(clip[j], clip[i], v0);
			if (side0 != -dir)
				res[nRes++] = v0;
			for (int k = 0; k < nPoly; k++)
			{
				math::float2 v1 = poly[k], x;
				int side1 = leftOf(clip[j], clip[i], v1);
				if (side0 + side1 == 0 && side0 && lineIntersection(clip[j], clip[i], v0, v1, &x))
					res[nRes++] = x;
				if (k == nPoly - 1)
					break;
				if (side1 != -dir)
					res[nRes++] = v1;
				v0 = v1;
				side0 = side1;
			}
		}

		return nRes;
	}

	Lightmap::Lightmap() noexcept
		: mipLevel_(5)
	{
		this->lightmap.width = 128;
		this->lightmap.height = 128;
		this->lightmap.data.resize(this->lightmap.width * this->lightmap.height);
		this->frontBuffer_.resize(this->lightmap.width * this->lightmap.height * 4);

		this->patches_.resize(this->mipLevel_);
		this->usedTexels.resize(this->mipLevel_);
		this->directLightBuffer_.resize(this->mipLevel_);

		for (std::uint8_t i = 0; i < mipLevel_; i++)
		{
			this->usedTexels[i].resize((this->lightmap.width >> i) * (this->lightmap.height >> i));
			this->directLightBuffer_[i].resize((this->lightmap.width >> i)* (this->lightmap.height >> i));
		}
	}

	Lightmap::~Lightmap() noexcept
	{
	}

	std::uint32_t
	Lightmap::width() const noexcept
	{
		return this->lightmap.width;
	}

	std::uint32_t
	Lightmap::height() const noexcept
	{
		return this->lightmap.height;
	}

	const std::vector<math::float3>&
	Lightmap::fronBuffer() const noexcept
	{
		return this->lightmap.data;
	}

	void
	Lightmap::render(const Camera& camera)
	{
#if 0
		for (std::uint8_t level = 0; level < this->mipLevel_; level++)
		{
			auto columns = patches_[level].size();
			for (std::int32_t i = 0; i < columns; ++i)
			{
				auto& sourcePatch = this->patches_[level][i];
				this->directLightBuffer_[level][sourcePatch.texelIndex] += sourcePatch.emissive;
			}
		}
#endif

		this->computeIndirectLightBounce(camera);

#if 0
		auto columns = patches_[0].size();
		for (std::int32_t i = 0; i < columns; ++i)
		{
			auto& sourcePatch = this->patches_[0][i];
			if (math::any(sourcePatch.emissive))
				this->lightmap.data[sourcePatch.texelIndex] += math::PI;
		}
#endif
		/*for (std::size_t y = 0; y < this->lightmap.height * 2; y++)
		{
			for (std::size_t x = 0; x < this->lightmap.width * 2; x++)
			{
				auto u = x / 2;
				auto v = y / 2;

				if (this->usedTexels[u * this->lightmap.width + v])
				{
					auto u1 = std::min<int>(u + 1, this->lightmap.width - 1);
					auto v1 = std::min<int>(v + 1, this->lightmap.height - 1);

					math::float3 texel[4];
					texel[0] = this->backBuffer_[v * this->lightmap.height + u];
					texel[1] = this->backBuffer_[v * this->lightmap.height + u1];
					texel[2] = this->backBuffer_[v1 * this->lightmap.height + u];
					texel[3] = this->backBuffer_[v1 * this->lightmap.height + u1];

					math::float3 avg = texel[0] + texel[1] + texel[2] + texel[3];

					this->frontBuffer_[y * this->lightmap.height * 2 + x] = avg;
				}
			}
		}*/
	}

	void
	Lightmap::renderLight(const DirectionalLight& light)
	{
		this->computeDirectLight(light);
	}

	void
	Lightmap::renderLight(const EnvironmentLight& light)
	{
	}

	void
	Lightmap::computeDirectLight(const DirectionalLight& light)
	{
		auto lightDir = math::normalize(-light.getForward());
		auto lightColor = light.getColor() * light.getIntensity() / math::PI;

		for (std::uint8_t level = 0; level < this->mipLevel_; level++)
		{
			std::memset(this->directLightBuffer_[level].data(), 0, this->directLightBuffer_[level].size() * sizeof(math::float3));

			auto columns = patches_[level].size();

#		pragma omp parallel for
			for (std::int32_t i = 0; i < columns; ++i)
			{
				auto& sourcePatch = this->patches_[level][i];

				math::Raycast ray;
				ray.origin = sourcePatch.position + lightDir * 0.01f;
				ray.normal = lightDir;

				auto nl = std::max(math::dot(lightDir, sourcePatch.normal), 0.0f);
				if (nl > 0.0f)
				{
					MeshHit hit;
					if (!this->mesh_->raycast(ray, hit))
						this->directLightBuffer_[level][sourcePatch.texelIndex] += lightColor * sourcePatch.color * nl;
				}
			}
		}
	}

	void
	Lightmap::computeIndirectLightBounce(const Camera& camera)
	{
		std::memset(this->lightmap.data.data(), 0, this->lightmap.data.size() * sizeof(math::float3));

		auto columns = patches_[0].size();

#		pragma omp parallel for
		for (std::int32_t i = 0; i < columns; ++i)
		{
			auto& sourcePatch = this->patches_[0][i];

			auto& destPaths = this->patches_[4];
			auto& destDirectLightBuffer = this->directLightBuffer_[4];

			auto destColumns = destPaths.size();

			math::float3 value = math::float3::Zero;
			for (std::size_t j = 0; j < destColumns; ++j)
			{
				auto& destPatch = destPaths[j];
				auto L = destPatch.position - sourcePatch.position;
				auto distance2 = math::length2(L);
				if (distance2 > 0)
				{
					auto distance = std::sqrt(distance2);
					L /= distance;

					auto cosi = math::dot(sourcePatch.normal, L);
					auto cosj = -math::dot(destPatch.normal, L);
					
					auto factor = cosi * cosj;
					if (factor > 1e-4f)
					{
						MeshHit hit;
						math::Raycast ray;
						ray.origin = sourcePatch.position + sourcePatch.normal * 0.01f;
						ray.normal = L;
						ray.maxDistance = distance - 0.1f;

						if (!this->mesh_->raycast(ray, hit))
							value += destDirectLightBuffer[destPatch.texelIndex] * (factor * destPatch.area / std::max(1.0f, distance2) / math::PI);
					}
				}
			}

			this->lightmap.data[sourcePatch.texelIndex] = value;
		}
	}

	void
	Lightmap::setGeometry(const Geometry& geometry) noexcept
	{
		mesh_ = geometry.getMesh();

		auto& vertices = mesh_->getVertexArray();
		auto& normals = mesh_->getNormalArray();
		auto& texcoords = mesh_->getTexcoordArray(1);

		if (!texcoords.empty())
		{
			for (std::uint32_t level = 0; level < this->mipLevel_; level++)
			{
				for (std::size_t i = 0; i < mesh_->getNumSubsets(); i++)
				{
					auto& indices = mesh_->getIndicesArray(i);
					auto& material = geometry.getMaterial(i);

					for (std::size_t j = 0; j < indices.size(); j += 3)
					{
						auto i1 = indices[j];
						auto i2 = indices[j + 1];
						auto i3 = indices[j + 2];

						auto& v1 = vertices[i1];
						auto& v2 = vertices[i2];
						auto& v3 = vertices[i3];

						math::float3 diffuse;
						if (material->get("diffuse", diffuse))
							this->trySamplingConservativeTriangleRasterizerPatch(level, v1, v2, v3, texcoords[i1], texcoords[i2], texcoords[i3], normals[i1], normals[i2], normals[i3], diffuse);
					}
				}
			}
		}
	}

	bool
	Lightmap::isConservativeTriangleRasterizerFinished()
	{
		return this->meshPosition.rasterizer.y >= this->meshPosition.rasterizer.maxy;
	}

	void
	Lightmap::moveToNextConservativeTriangleRasterizerPosition()
	{
		this->meshPosition.rasterizer.x += 1;

		while (this->meshPosition.rasterizer.x >= this->meshPosition.rasterizer.maxx)
		{
			this->meshPosition.rasterizer.x = this->meshPosition.rasterizer.minx;
			this->meshPosition.rasterizer.y += 1;

			if (this->isConservativeTriangleRasterizerFinished())
				break;
		}
	}

	bool
	Lightmap::trySamplingConservativeTriangleRasterizerPosition(std::uint32_t level, Patch& patch)
	{
		if (isConservativeTriangleRasterizerFinished())
			return false;

		math::float2 pixel[16];
		pixel[0] = math::float2((float)this->meshPosition.rasterizer.x, (float)this->meshPosition.rasterizer.y);
		pixel[1] = math::float2((float)this->meshPosition.rasterizer.x + 1, (float)this->meshPosition.rasterizer.y);
		pixel[2] = math::float2((float)this->meshPosition.rasterizer.x + 1, (float)this->meshPosition.rasterizer.y + 1);
		pixel[3] = math::float2((float)this->meshPosition.rasterizer.x, (float)this->meshPosition.rasterizer.y + 1);

		math::float2 res[16];
		int count = convexClip(pixel, 4, this->meshPosition.triangle.uv, 3, res);
		if (count > 0)
		{
			math::float2 centroid = res[0];
			float rectArea = res[count - 1].x * res[0].y - res[count - 1].y * res[0].x;
			for (int i = 1; i < count; i++)
			{
				centroid = centroid + res[i];
				rectArea += res[i - 1].x * res[i].y - res[i - 1].y * res[i].x;
			}

			if (std::abs(rectArea) > 0.0f)
			{
				math::float2 uv = math::barycentric(this->meshPosition.triangle.uv[0], this->meshPosition.triangle.uv[1], this->meshPosition.triangle.uv[2], centroid / (float)count);
				if (math::isfinite(uv))
				{
					auto& p0 = this->meshPosition.triangle.p[0];
					auto& p1 = this->meshPosition.triangle.p[1];
					auto& p2 = this->meshPosition.triangle.p[2];

					math::float3 v1 = p1 - p0;
					math::float3 v2 = p2 - p0;

					this->meshPosition.sample.position = p0 + (v2 * uv.x) + (v1 * uv.y);
					this->meshPosition.sample.direction = math::normalize(math::cross(v1, v2));

					if (math::isfinite(this->meshPosition.sample.position) &&
						math::isfinite(this->meshPosition.sample.direction) &&
						math::length2(this->meshPosition.sample.direction) > 0.5f)
					{
						patch.texelIndex = this->meshPosition.rasterizer.y * (this->lightmap.width >> level) + this->meshPosition.rasterizer.x;
						patch.area = this->meshPosition.triangle.areaP * rectArea / this->meshPosition.triangle.areaUV;
						patch.v1 = this->meshPosition.triangle.p[0];
						patch.v2 = this->meshPosition.triangle.p[1];
						patch.v3 = this->meshPosition.triangle.p[2];
						patch.position = this->meshPosition.sample.position;
						patch.normal = this->meshPosition.sample.direction;
						patch.color = this->meshPosition.triangle.color;
						if (this->meshPosition.triangle.color.y > (this->meshPosition.triangle.color.x + this->meshPosition.triangle.color.z))
							patch.emissive = this->meshPosition.triangle.color;
						else
							patch.emissive = math::float3::Zero;

						return true;
					}
				}
			}
		}

		return false;
	}

	void
	Lightmap::trySamplingConservativeTriangleRasterizerPatch(std::uint32_t level, const math::float3 &p1, const math::float3 &p2, const math::float3 &p3, const math::float2 &tc1, const math::float2 &tc2, const math::float2 &tc3, const math::float3 &n1, const math::float3 &n2, const math::float3 &n3, const math::float3& color) noexcept
	{
		this->meshPosition.triangle.p[0] = p1;
		this->meshPosition.triangle.p[1] = p2;
		this->meshPosition.triangle.p[2] = p3;
		this->meshPosition.triangle.areaP = math::surfaceArea(math::Triangle(p1, p2, p3)) * 0.5f;

		auto scale = math::float2(float(this->lightmap.width >> level), float(this->lightmap.height >> level));
		this->meshPosition.triangle.uv[0] = tc1 * scale;
		this->meshPosition.triangle.uv[1] = tc2 * scale;
		this->meshPosition.triangle.uv[2] = tc3 * scale;
		this->meshPosition.triangle.areaUV = math::surfaceArea(math::Triangle(
			math::float3(this->meshPosition.triangle.uv[0], 0),
			math::float3(this->meshPosition.triangle.uv[1], 0),
			math::float3(this->meshPosition.triangle.uv[2], 0))) * 0.5f;

		math::Box2 box;
		box.encapsulate(this->meshPosition.triangle.uv[0]);
		box.encapsulate(this->meshPosition.triangle.uv[1]);
		box.encapsulate(this->meshPosition.triangle.uv[2]);

		this->meshPosition.triangle.color = color;

		auto min = math::floor(box.min);
		auto max = math::ceil(box.max);

		this->meshPosition.rasterizer.minx = math::max(int(min.x) - 1, 0);
		this->meshPosition.rasterizer.miny = math::max(int(min.y) - 1, 0);
		this->meshPosition.rasterizer.maxx = math::min(int(max.x) + 1, this->lightmap.width >> level);
		this->meshPosition.rasterizer.maxy = math::min(int(max.y) + 1, this->lightmap.height >> level);

		this->meshPosition.rasterizer.x = this->meshPosition.rasterizer.minx;
		this->meshPosition.rasterizer.y = this->meshPosition.rasterizer.miny;

		if (this->meshPosition.rasterizer.x <= this->meshPosition.rasterizer.maxx &&
			this->meshPosition.rasterizer.y <= this->meshPosition.rasterizer.maxy)
		{
			while (!isConservativeTriangleRasterizerFinished())
			{
				Patch patch;

				if (trySamplingConservativeTriangleRasterizerPosition(level, patch))
				{
					if (!usedTexels[level][patch.texelIndex])
					{
						this->patches_[level].push_back(patch);
						usedTexels[level][patch.texelIndex] = true;
					}
				}

				moveToNextConservativeTriangleRasterizerPosition();
			}
		}
	}
}