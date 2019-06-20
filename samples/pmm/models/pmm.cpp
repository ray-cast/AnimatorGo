#include "pmm.h"

namespace octoon
{
	PMMLoader::PMMLoader()
	{
	}

	bool 
	PMMLoader::can_read(istream& reader)
	{
		PmmHeader hdr;
		return hdr.load(reader);
	}

	bool
	PMMLoader::do_load(istream& reader, PMMFile& pmm)
	{
		pmm.load(reader);
		return true;
		/*auto scene = Scene();

		auto camera = PerspectiveCamera::builder()
			.main(true)
			.set_fov(30.0)
			.set_translate(float!(0.0, 0.1, 10.0))
			.build();

		scene.add(camera);

		for model in pmm.model
		{
			auto model = ModelLoader::open(model.path);
			model.set_scale(float!(0.1,0.1,0.1));
			model.set_translate(float!(0.0,-0.8,20.0));
			scene.add(model);
		}

		return scene;*/
	}

	/*std::optional<std::vector<u8>> do_save(&self, _:&Scene)
	{
		Err(Error::LoaderError("Not Implmention yet".to_std::string()))
	}*/
}