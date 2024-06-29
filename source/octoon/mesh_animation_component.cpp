#include <octoon/mesh_animation_component.h>
#include <octoon/runtime/except.h>
#include <octoon/runtime/guid.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/timer_feature.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/asset_database.h>

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4245)
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcMaterial/All.h>
#pragma warning(pop)

#include <codecvt>

using namespace Alembic::Abc;
using namespace Alembic::AbcGeom;

namespace octoon
{
	OctoonImplementSubClass(MeshAnimationComponent, AnimationComponent, "MeshAnimation")

	class AnimationData
	{
	public:
		std::shared_ptr<Alembic::Abc::v12::IObject> object;
	};

	struct PackedVertex
	{
		math::float3 vertex;
		math::float2 uv;
		math::float3 normal;

		bool operator<(const PackedVertex that) const {
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
		};
	};

	bool get_similar_vertex_index(const PackedVertex& packed, std::map<PackedVertex, std::uint32_t>& vertex_pack_map, std::uint32_t& result)
	{
		std::map<PackedVertex, std::uint32_t>::iterator it = vertex_pack_map.find(packed);
		if (it == vertex_pack_map.end())
			return false;
		else
		{
			result = it->second;
			return true;
		}
	}

	struct SimpleVertex
	{
		math::float3 vertex;

		bool operator<(const SimpleVertex that) const
		{
			return memcmp((void*)this, (void*)&that, sizeof(SimpleVertex)) > 0;
		};
	};

	void read_position(const IPolyMeshSchema& schema, const IPolyMeshSchema::Sample& sample, math::float3s& vertices)
	{
		const P3fArraySamplePtr& positions = sample.getPositions();
		vertices.resize(positions->size());
		std::memcpy(vertices.data(), positions->get(), positions->size() * sizeof(math::float3));
	}

	void read_uvs(const IPolyMeshSchema& schema, const IPolyMeshSchema::Sample& sample, math::float2s& uvs)
	{
		IV2fGeomParam uv_param = schema.getUVsParam();
		if (!uv_param.valid())
		{
			uvs.resize(sample.getPositions()->size());
			return;
		}

		IV2fGeomParam::Sample uv_sample(uv_param.getIndexedValue());
		if (!uv_sample.valid())
		{
			uvs.resize(sample.getPositions()->size());
			return;
		}

		if (uv_param.isIndexed())
		{
			auto uv = uv_sample.getVals()->get();
			auto indices_obj = uv_sample.getIndices();
			auto indices_count = indices_obj->size();

			uvs.clear();

			for (std::size_t i = 0; i < indices_count; i++)
			{
				std::uint32_t index = indices_obj->get()[i];
				const Alembic::AbcGeom::V2f v = uv[index];
				uvs[i] = math::float2(v.x, v.y);
			}
		}
		else
		{
			auto faceIndices = sample.getFaceIndices();
			auto uv = uv_sample.getVals()->get();
			auto numIndices = faceIndices->size();

			uvs.resize(sample.getPositions()->size());

			for (std::size_t n = 0; n < numIndices; ++n)
			{
				auto index = (unsigned int)(*faceIndices)[n];
				uvs[index] = math::float2(uv[n].x, uv[n].y);
			}
		}
	}

	void create_normals(const IPolyMeshSchema::Sample& sample_, math::float3s& normals)
	{
		const P3fArraySamplePtr& positions = sample_.getPositions();
		const Int32ArraySamplePtr& faceIndices = sample_.getFaceIndices();
		const Int32ArraySamplePtr& faceCounts = sample_.getFaceCounts();

		std::map<SimpleVertex, std::vector<std::pair <std::uint32_t, math::float3>>> vertex_map;
		std::map<SimpleVertex, std::vector<std::pair<std::uint32_t, math::float3>>>::iterator it;

		//cacluate normals
		std::uint32_t cur_index = 0;
		for (int i = 0; i < faceCounts->size(); i++) {
			int face_size = faceCounts->get()[i];

			// degenerated faces
			if (face_size < 3) {
				for (int j = 0; j < face_size; j++) {
					normals.push_back(math::float3(0, 0, 0));
				}
				cur_index += face_size;
				continue;
			}

			math::float3 poly[3];

			// get just the first normal
			for (int j = 0; j < 3; j++) {
				std::uint32_t index = (std::uint32_t)(*faceIndices)[cur_index + j];
				const Alembic::AbcGeom::V3f p = positions->get()[index];
				poly[j] = math::float3(p.x, p.y, p.z);
			}

			// caculate the normal
			math::float3 ab = poly[1] - poly[0];
			math::float3 ac = poly[2] - poly[0];
			math::float3 wn = math::normalize(math::cross(ac, ab));

			for (int j = 0; j < face_size; j++) {
				std::uint32_t index = (std::uint32_t)(*faceIndices)[cur_index + j];
				const Alembic::AbcGeom::V3f p = positions->get()[index];
				SimpleVertex vert = { math::float3(p.x, p.y, p.z) };
				vertex_map[vert].push_back(std::pair< std::uint32_t, math::float3>(cur_index + j, wn));
			}

			cur_index += face_size;
		}

		// finally average the normals
		normals.resize(cur_index);
		for (it = vertex_map.begin(); it != vertex_map.end(); it++) {
			math::float3 a;
			for (int i = 0; i < it->second.size(); i++) {
				a += it->second[i].second;
			}

			a = a / static_cast<float>(it->second.size());
			for (int i = 0; i < it->second.size(); i++) {
				normals[it->second[i].first] = a;
			}
		}
	}

	void read_normals(const IPolyMeshSchema& schema_, const IPolyMeshSchema::Sample& sample_, math::float3s& normals)
	{
		IN3fGeomParam normal_param = schema_.getNormalsParam();
		if (!normal_param.valid())
		{
			create_normals(sample_, normals);
			return;
		}
		
		IN3fGeomParam::Sample normal_sample(normal_param.getIndexedValue());
		if (!normal_sample.valid())
		{
			create_normals(sample_, normals);
			return;
		}

		if (normal_param.isIndexed())
		{
			auto n = normal_sample.getVals()->get();
			auto indices_obj = normal_sample.getIndices();
			auto indices_count = indices_obj->size();

			normals.clear();

			for (std::size_t i = 0; i < indices_count; i++)
			{
				std::uint32_t index = indices_obj->get()[i];
				const Alembic::AbcGeom::N3f v = n[index];
				normals[i] = math::float3(v.x, v.y, v.z);
			}
		}
		else
		{
			normals.resize(normal_sample.getVals()->size());
			std::memcpy(normals.data(), normal_sample.getVals()->get(), normals.size() * sizeof(math::float3));
		}
	}

	void read_indices(const IPolyMeshSchema& schema_, const IPolyMeshSchema::Sample& sample, math::uint1s& indices)
	{
		auto faceIndices = sample.getFaceIndices();
		auto faceCounts = sample.getFaceCounts();

		std::size_t numFaces = faceCounts->size();
		std::size_t numIndices = faceIndices->size();
		std::size_t numPoints = sample.getPositions()->size();

		if (numFaces >= 1 && numIndices >= 1 && numPoints >= 1)
		{
			std::size_t faceIndexBegin = 0;
			std::size_t faceIndexEnd = 0;

			indices.clear();
			indices.reserve(numIndices);

			for (std::size_t face = 0, faceIndice = 0; face < numFaces; ++face)
			{
				faceIndexBegin = faceIndexEnd;
				std::size_t count = (*faceCounts)[face];
				faceIndexEnd = faceIndexBegin + count;

				if (faceIndexEnd > numIndices || faceIndexEnd < faceIndexBegin)
				{
					std::cerr << "Mesh update quitting on face: "
							<< face
							<< " because of wonky numbers"
							<< ", faceIndexBegin = " << faceIndexBegin
							<< ", faceIndexEnd = " << faceIndexEnd
							<< ", numIndices = " << numIndices
							<< ", count = " << count
							<< std::endl;
					break;
				}

				bool goodFace = true;
				for (std::size_t fidx = faceIndexBegin; fidx < faceIndexEnd; ++fidx)
				{
					if ((std::size_t)((*faceIndices)[fidx]) >= numPoints)
					{
						std::cout << "Mesh update quitting on face: " << face
								<< " because of bad indices"
								<< ", indexIndex = " << fidx
								<< ", vertexIndex = " << (*faceIndices)[fidx]
								<< ", numPoints = " << numPoints
								<< std::endl;
						goodFace = false;
						break;
					}
				}

				if (goodFace && count > 2)
				{
					indices.emplace_back((*faceIndices)[faceIndexBegin + 2]);
					indices.emplace_back((*faceIndices)[faceIndexBegin + 1]);
					indices.emplace_back((*faceIndices)[faceIndexBegin + 0]);

					for (std::size_t c = 3; c < count; ++c)
					{
						indices.emplace_back((*faceIndices)[faceIndexBegin + c]);
						indices.emplace_back((*faceIndices)[faceIndexBegin + c - 1]);
						indices.emplace_back((*faceIndices)[faceIndexBegin + 0]);
					}
				}

				faceIndice += count;
			}
		}
		else
		{
			std::cerr << "Mesh update quitting because bad arrays"
					<< ", numFaces = " << numFaces
					<< ", numIndices = " << numIndices
					<< ", numPoints = " << numPoints
					<< std::endl;
		}
	}

	std::string utf8_to_gb2312(std::string_view strUtf8)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
		std::wstring wTemp = cutf8.from_bytes(std::string(strUtf8));
#ifdef _MSC_VER
		std::locale loc("zh-CN");
#else
		std::locale loc("zh_CN.GB18030");
#endif
		const wchar_t* pwszNext = nullptr;
		char* pszNext = nullptr;
		mbstate_t state = {};

		std::vector<char> buff(wTemp.size() * 2);
		int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
			(loc).out(state,
				wTemp.data(), wTemp.data() + wTemp.size(), pwszNext,
				buff.data(), buff.data() + buff.size(), pszNext);

		if (std::codecvt_base::ok == res)
			return std::string(buff.data(), pszNext);

		return "";
	}

	MeshAnimationComponent::MeshAnimationComponent() noexcept
		: enableAnimation_(true)
		, minTime_(0.0f)
		, maxTime_(0.0f)
		, gameObject_(std::make_shared<GameObject>())
	{
		animationState_.finish = false;
		animationState_.time = 0;
		animationState_.timeLength = 0;
	}

	MeshAnimationComponent::MeshAnimationComponent(const std::filesystem::path& path) noexcept(false)
		: MeshAnimationComponent()
	{
		path_ = path;
	}

	MeshAnimationComponent::~MeshAnimationComponent() noexcept
	{
	}

	bool
	MeshAnimationComponent::setFilePath(const std::filesystem::path& path) noexcept(false)
	{
		this->path_ = path;

		Alembic::AbcCoreFactory::IFactory factor;
		auto archive = factor.getArchive(utf8_to_gb2312((char*)path.u8string().c_str()));
		if (archive.valid())
		{
			auto object = archive.getTop();

			AnimationData animationData;
			animationData.object = std::make_shared<IObject>(object);

			this->animationState_.time = 0.0f;
			this->animationState_.timeLength = 0.0f;
			this->animationState_.finish = false;

			this->setName(object.getName());
			this->createAnimationData(*this, animationData);

			return true;
		}
		else
		{
			return false;
		}
	}

	const std::filesystem::path&
	MeshAnimationComponent::getFilePath() const noexcept
	{
		return path_;
	}

	bool
	MeshAnimationComponent::play(std::string_view status) noexcept
	{
		this->setName(status);
		this->addComponentDispatch(GameDispatchType::FixedUpdate);

		enableAnimation_ = true;
		return enableAnimation_;
	}

	void
	MeshAnimationComponent::pause() noexcept
	{
		enableAnimation_ = false;
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MeshAnimationComponent::reset() noexcept
	{
		this->setTime(0.0f);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MeshAnimationComponent::setTime(float time) noexcept
	{
		animationState_.time = time;

		this->onAnimationUpdate();

		for (auto& child : gameObject_->getChildren())
		{
			auto component = child->getComponent<MeshAnimationComponent>();
			if (component)
				component->setTime(time);
		}
	}

	float
	MeshAnimationComponent::getTime() const noexcept
	{
		return animationState_.time;
	}

	void
	MeshAnimationComponent::sample(float delta) noexcept
	{
	}

	void
	MeshAnimationComponent::evaluate(float delta) noexcept
	{
		animationState_.time += delta;

		this->onAnimationUpdate();

		for (auto& child : gameObject_->getChildren())
		{
			auto component = child->getComponent<MeshAnimationComponent>();
			if (component)
				component->evaluate(delta);
		}
	}

	void
	MeshAnimationComponent::setMaterials(std::unordered_map<std::string, MaterialPtr>&& materials)
	{
		materials_ = std::move(materials);
	}

	void
	MeshAnimationComponent::setMaterials(const std::unordered_map<std::string, MaterialPtr>& materials)
	{
		materials_ = materials;
	}

	const std::unordered_map<std::string, MaterialPtr>&
	MeshAnimationComponent::getMaterials() const
	{
		return materials_;
	}

	const AnimatorStateInfo&
	MeshAnimationComponent::getCurrentAnimatorStateInfo() const noexcept
	{
		return animationState_;
	}

	void
	MeshAnimationComponent::load(const nlohmann::json& json) noexcept(false)
	{
		GameComponent::load(json);

		std::unordered_map<std::string, MaterialPtr> materials;

		for (auto& material : json["materials"])
		{
			auto data = material["data"].get<nlohmann::json::string_t>();
			auto name = material["name"].get<std::string>();
			materials[name] = AssetDatabase::instance()->loadAssetAtPath<Material>(AssetDatabase::instance()->getAssetPath(data));
		}

		this->setMaterials(std::move(materials));

		if (json.contains("path"))
			this->setFilePath(json["path"].get<std::string>());
		if (json.contains("time"))
			this->setTime(json["time"].get<float>());
	}

	void
	MeshAnimationComponent::save(nlohmann::json& json) const noexcept(false)
	{
		GameComponent::save(json);

		json["path"] = (char*)this->path_.u8string().c_str();
		json["time"] = this->animationState_.time;

		for (auto& pair : this->materials_)
		{
			if (!AssetDatabase::instance()->contains(pair.second))
				AssetDatabase::instance()->createAsset(pair.second, std::filesystem::path("Assets/Materials").append(make_guid() + ".mat"));

			nlohmann::json materialJson;
			materialJson["data"] = AssetDatabase::instance()->getAssetGuid(pair.second);
			materialJson["name"] = pair.first;

			json["materials"].push_back(materialJson);
		}
	}

	GameComponentPtr
	MeshAnimationComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshAnimationComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void 
	MeshAnimationComponent::onActivate() noexcept
	{
		if (!path_.empty())
		{
			this->setFilePath(path_);
			this->sample(0.0f);
		}

		this->removeComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	MeshAnimationComponent::onDeactivate() noexcept
	{
		gameObject_->cleanupChildren();
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MeshAnimationComponent::onFixedUpdate() noexcept
	{
		if (enableAnimation_)
		{
			auto timeFeature = this->getFeature<TimerFeature>();
			if (timeFeature)
			{
				auto delta = timeFeature->delta();
				if (delta != 0.0f)
					animationState_.time += delta;

				this->onAnimationUpdate();
			}
		}
	}

	void
	MeshAnimationComponent::onMoveBefore() noexcept(false)
	{
	}

	void
	MeshAnimationComponent::onMoveAfter() noexcept(false)
	{
		if (gameObject_)
		{
			auto transform = this->getComponent<TransformComponent>();
			gameObject_->getComponent<TransformComponent>()->setTransform(transform->getTransform());
		}
	}

	void
	MeshAnimationComponent::onAnimationUpdate() noexcept
	{
		auto& object_header = this->animationData_->object->getHeader();
		if (IPoints::matches(object_header))
			std::cout << object_header.getMetaData().serialize() << std::endl;
		else if (ICamera::matches(object_header))
			std::cout << object_header.getMetaData().serialize() << std::endl;
		else if (IPolyMesh::matches(object_header))
		{
			if (animationState_.time >= minTime_&& animationState_.time <= maxTime_)
			{
				auto polyMesh = std::dynamic_pointer_cast<IPolyMesh>(this->animationData_->object);

				auto& schema = polyMesh->getSchema();
				if (schema.isConstant())
				{
					auto mf = this->getComponent<MeshFilterComponent>();
					if (mf)
					{
						auto mesh = mf->getMesh();
						if (!mf->getMesh())
						{
							mesh = std::make_shared<Mesh>();
							mesh->setIndicesArray(math::uint1s());
							mf->setMesh(mesh);
						}

						IPolyMeshSchema::Sample sample;
						read_position(schema, sample, mesh->getVertexArray());
						read_uvs(schema, sample, mesh->getTexcoordArray());
						read_indices(schema, sample, mesh->getIndicesArray());

						mesh->computeBoundingBox();
						mesh->computeVertexNormals();

						mf->uploadMeshData();
					}

					animationState_.finish = true;
				}
				else
				{
					IPolyMeshSchema::Sample sample;

					auto sampleSelector = Alembic::Abc::ISampleSelector(animationState_.time, ISampleSelector::kNearIndex);
					schema.get(sample, sampleSelector);

					auto mf = this->getComponent<MeshFilterComponent>();
					if (mf)
					{
						auto mesh = mf->getMesh();
						if (!mf->getMesh())
						{
							mesh = std::make_shared<Mesh>();
							mesh->setIndicesArray(math::uint1s());
							mf->setMesh(mesh);
						}

						read_uvs(schema, sample, mesh->getTexcoordArray());
						read_position(schema, sample, mesh->getVertexArray());
						read_indices(schema, sample, mesh->getIndicesArray());

						mesh->computeBoundingBox();
						mesh->computeVertexNormals();

						mf->uploadMeshData();
					}

					animationState_.finish = false;
				}
			}
			else
			{
				animationState_.finish = true;
			}
		}
	}

	void 
	MeshAnimationComponent::createAnimationData(MeshAnimationComponent& parent, const AnimationData& animationData) noexcept(false)
	{
		auto& object_header = animationData.object->getHeader();
		if (IPolyMesh::matches(object_header))
		{
			animationData_ = std::make_shared<AnimationData>();
			animationData_->object = std::make_shared<IPolyMesh>(*animationData.object);

			auto& schema = std::dynamic_pointer_cast<IPolyMesh>(animationData_->object)->getSchema();
			if (!schema.isConstant())
			{
				std::size_t numSamps = schema.getNumSamples();
				if (numSamps > 0)
				{
					auto iTsmp = schema.getTimeSampling();
					minTime_ = static_cast<float>(iTsmp->getSampleTime(0));
					maxTime_ = static_cast<float>(iTsmp->getSampleTime(numSamps - 1));

					animationState_.timeLength = maxTime_;
				}
			}
		}
		else if (IXform::matches(object_header))
		{
			auto xform = std::make_shared<IXform>(*animationData.object, kWrapExisting);

			XformSample xs;
			xform->getSchema().get(xs);

			auto matrix = xs.getMatrix();
			auto transform = this->getComponent<TransformComponent>();
			transform->setTransform(math::float4x4(
				(float)matrix.x[0][0], (float)matrix.x[0][1], (float)matrix.x[0][2], (float)matrix.x[0][3],
				(float)matrix.x[1][0], (float)matrix.x[1][1], (float)matrix.x[1][2], (float)matrix.x[1][3],
				(float)matrix.x[2][0], (float)matrix.x[2][1], (float)matrix.x[2][2], (float)matrix.x[2][3],
				(float)matrix.x[3][0], (float)matrix.x[3][1], (float)matrix.x[3][2], (float)matrix.x[3][3]));

			animationData_ = std::make_shared<AnimationData>();
			animationData_->object = xform;
		}
		else
		{
			animationData_ = std::make_shared<AnimationData>();
			animationData_->object = animationData.object;
		}

		std::size_t numChildren = animationData_->object->getNumChildren();
		for (std::size_t i = 0; i < numChildren; ++i)
		{
			auto child = animationData_->object->getChild(i);
			auto& child_header = child.getHeader();

			if (IPolyMesh::matches(child_header))
			{
				AnimationData childData;
				childData.object = std::make_shared<IPolyMesh>(child);

				auto name = child.getName();
				auto gameObject = std::make_shared<GameObject>(std::string_view(name));
				gameObject->addComponent<MeshFilterComponent>();

				auto meshRender = gameObject->addComponent<MeshRendererComponent>();
				meshRender->setGlobalIllumination(true);

				if (parent.materials_.contains(name))
					meshRender->setMaterial(parent.materials_[name]);
				else
				{
					auto material = std::make_shared<MeshStandardMaterial>(name, math::float3(0.9f, 0.9f, 0.9f));
					parent.materials_[name] = material;
					meshRender->setMaterial(std::move(material));
				}

				auto mesh = gameObject->addComponent<MeshAnimationComponent>();
				mesh->createAnimationData(parent, childData);

				animationState_.timeLength = std::max(animationState_.timeLength, mesh->animationState_.timeLength);

				gameObject_->addChild(std::move(gameObject));
			}
			else if (IXform::matches(child_header))
			{
				AnimationData childData;
				childData.object = std::make_shared<IXform>(child);

				auto gameObject = std::make_shared<GameObject>(std::string_view(child.getName()));
				auto mesh = gameObject->addComponent<MeshAnimationComponent>();
				mesh->createAnimationData(parent, childData);

				animationState_.timeLength = std::max(animationState_.timeLength, mesh->animationState_.timeLength);

				gameObject_->addChild(std::move(gameObject));
			}
		}
	}
}