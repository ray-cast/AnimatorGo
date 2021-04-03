#include <octoon/obj_loader.h>
#include <octoon/mesh_loader.h>
#include <octoon/texture_loader.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/transform_component.h>
#include <octoon/point_light_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <set>
#include <unordered_map>

namespace octoon
{
	struct Index
	{
		std::uint32_t v;
		std::uint32_t n;
		std::uint32_t uv;
	};

	OBJLoader::OBJLoader() noexcept
	{
	}

	OBJLoader::~OBJLoader() noexcept
	{
	}

	bool
	OBJLoader::doCanRead(io::istream& stream) noexcept
	{
		return false;
	}

	bool
	OBJLoader::doCanRead(const char* type) noexcept
	{
		return std::strncmp(type, "obj", 3) == 0;
	}

	GameObjects
	OBJLoader::load(std::string_view filepath) noexcept(false)
	{
		GameObjects objects;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, std::string(filepath).c_str());
		if (ret)
		{
			auto defaultMaterial = std::make_shared<MeshStandardMaterial>();

			std::vector<std::shared_ptr<MeshStandardMaterial>> standardMaterials;
			for (auto& material : materials)
			{
				auto standard = std::make_shared<MeshStandardMaterial>();
				standard->setColor(math::float3(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
				standard->setRoughness(material.roughness);
				standard->setMetalness(material.metallic);
				standard->setSheen(material.sheen);
				standard->setClearCoat(material.clearcoat_thickness);
				standard->setClearCoatRoughness(material.clearcoat_roughness);
				standard->setAnisotropy(material.anisotropy);
				standard->setRefractionRatio(material.ior);

				if (!material.diffuse_texname.empty())
					standard->setColorMap(TextureLoader::load(material.diffuse_texname));

				if (!material.normal_texname.empty())
					standard->setNormalMap(TextureLoader::load(material.normal_texname));

				if (!material.roughness_texname.empty())
					standard->setRoughnessMap(TextureLoader::load(material.roughness_texname));

				if (!material.metallic_texname.empty())
					standard->setMetalnessMap(TextureLoader::load(material.metallic_texname));

				if (!material.sheen_texname.empty())
					standard->setSheenMap(TextureLoader::load(material.sheen_texname));

				if (!material.emissive_texname.empty())
					standard->setEmissiveMap(TextureLoader::load(material.emissive_texname));
			}

			for (auto& shape : shapes)
			{
				std::size_t index_offset = 0;
				std::set<std::tuple<int,int,int>> vertexSet;

				for (std::size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
				{
					std::size_t fnum = shape.mesh.num_face_vertices[f];

					for (std::size_t v = 0; v < fnum; v++)
					{
						auto index = shape.mesh.indices[index_offset + v];
						vertexSet.insert(std::make_tuple(index.vertex_index, index.normal_index, index.texcoord_index));
					}

					index_offset += fnum;
				}

				auto vertices = math::float3s(vertexSet.size());
				auto normals = math::float3s(vertexSet.size());
				auto texcoords = math::float2s(vertexSet.size());
				auto indices = math::uint32s(index_offset);

				std::uint32_t idx = 0;

				std::map<std::tuple<int, int, int>, std::uint32_t> vertexMap;
				for (auto& index : vertexSet)
					vertexMap[index] = idx++;

				if (!attrib.vertices.empty())
				{
					idx = 0;

					for (auto& index : vertexSet)
					{
						auto v = std::get<0>(index) * 3;
						vertices[idx++].set(attrib.vertices[v], attrib.vertices[v + 1], attrib.vertices[v + 2]);
					}
				}

				if (!attrib.normals.empty())
				{
					idx = 0;

					for (auto& index : vertexSet)
					{
						auto v = std::get<1>(index) * 3;
						normals[idx++].set(attrib.normals[v], attrib.normals[v + 1], attrib.normals[v + 2]);
					}
				}

				if (!attrib.texcoords.empty())
				{
					idx = 0;

					for (auto& index : vertexSet)
					{
						auto v = std::get<2>(index) * 2;
						v = v < 0 ? attrib.texcoords.size() + v : v;
						texcoords[idx++].set(attrib.texcoords[v], 1.f - attrib.texcoords[v + 1]);
					}
				}

				for (std::size_t f = 0, offset = 0; f < shape.mesh.num_face_vertices.size(); f++)
				{
					std::size_t fnum = shape.mesh.num_face_vertices[f];

					for (std::size_t v = 0; v < fnum; v++)
					{
						auto index = shape.mesh.indices[offset + v];
						indices[offset + v] = vertexMap.at(std::make_tuple(index.vertex_index, index.normal_index, index.texcoord_index));
					}

					offset += fnum;
				}

				auto mesh = std::make_shared<Mesh>();
				mesh->setName(shape.name);
				mesh->setVertexArray(std::move(vertices));
				mesh->setNormalArray(std::move(normals));
				mesh->setTexcoordArray(std::move(texcoords));
				mesh->setIndicesArray(std::move(indices));
				mesh->computeBoundingBox();

				auto object = GameObject::create();
				object->addComponent<MeshFilterComponent>(std::move(mesh));

				if (shape.mesh.material_ids.empty())
					object->addComponent<MeshRendererComponent>()->setMaterial(standardMaterials[shape.mesh.material_ids.front()]);
				else
					object->addComponent<MeshRendererComponent>()->setMaterial(defaultMaterial);

				objects.emplace_back(std::move(object));
			}
		}

		return objects;
	}
}