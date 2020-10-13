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

namespace octoon
{
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
			std::size_t index_offset = 0;

			std::vector<std::shared_ptr<material::MeshStandardMaterial>> standardMaterials;
			for (auto& material : materials)
			{
				auto standard = std::make_shared<material::MeshStandardMaterial>();
				standard->setIor(material.ior);
				standard->setColor(math::float3(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
				standard->setRoughness(material.roughness);
				standard->setMetalness(material.metallic);
				standard->setSheen(material.sheen);
				standard->setClearCoat(material.clearcoat_thickness);
				standard->setClearCoatRoughness(material.clearcoat_roughness);
				standard->setAnisotropy(material.anisotropy);

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
				auto vertices = math::float3s(shape.mesh.num_face_vertices.size());
				auto normals = math::float3s(shape.mesh.num_face_vertices.size());
				auto texcoords = math::float2s(shape.mesh.num_face_vertices.size());

				for (std::size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
				{
					std::size_t fnum = shape.mesh.num_face_vertices[f];

					for (std::size_t v = 0; v < fnum; v++)
					{
						auto vertex_index = shape.mesh.indices[index_offset + v].vertex_index * 3;
						vertices[f * fnum + v].set(attrib.vertices[vertex_index], attrib.vertices[vertex_index + 1], attrib.vertices[vertex_index + 2]);
					}

					for (std::size_t v = 0; v < fnum; v++)
					{
						auto index = shape.mesh.indices[index_offset + v].normal_index * 3;
						normals[f * fnum + v].set(attrib.normals[index], attrib.normals[index + 1], attrib.normals[index + 2]);
					}

					for (std::size_t v = 0; v < fnum; v++)
					{
						auto index = shape.mesh.indices[index_offset + v].texcoord_index * 2;
						texcoords[f * fnum + v].set(attrib.normals[index], attrib.normals[index + 1]);
					}

					index_offset += fnum;
				}

				auto mesh = std::make_shared<mesh::Mesh>();
				mesh->setName(shape.name);
				mesh->setVertexArray(std::move(vertices));
				mesh->setNormalArray(std::move(normals));
				mesh->setTexcoordArray(std::move(texcoords));

				auto object = GameObject::create();
				object->addComponent<MeshFilterComponent>(std::move(mesh));

				if (shape.mesh.material_ids.empty())
					object->addComponent<MeshRendererComponent>()->setMaterial(standardMaterials[shape.mesh.material_ids.front()]);

				objects.emplace_back(std::move(object));
			}
		}

		return objects;
	}
}