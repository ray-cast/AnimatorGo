#include <octoon/io/json_reader.h>
#include <octoon/io/fstream.h>
#include <octoon/runtime/json.h>

namespace octoon
{
	namespace io
	{
		void reader(archivebuf& node, const nlohmann::json& json) except;

		void reader(archivebuf& node, const std::vector<nlohmann::json>& arrays) except
		{
			std::size_t index = 0;

			for (auto& object : arrays)
			{
				switch (object.type())
				{
				case nlohmann::json::value_t::boolean:
					node[index++] = object.get<nlohmann::json::boolean_t>();
					break;
				case nlohmann::json::value_t::number_float:
					node[index++] = object.get<nlohmann::json::number_float_t>();
					break;
				case nlohmann::json::value_t::number_integer:
					node[index++] = object.get<nlohmann::json::number_integer_t>();
					break;
				case nlohmann::json::value_t::number_unsigned:
					node[index++] = object.get<nlohmann::json::number_unsigned_t>();
					break;
				case nlohmann::json::value_t::string:
					node[index++] = object.get<nlohmann::json::string_t>();
					break;
				case nlohmann::json::value_t::object:
					reader(node[index++], object.get<nlohmann::json::object_t>());
					break;
				case nlohmann::json::value_t::array:
					node[index].emplace(archivebuf::array);
					reader(node[index++], object.get<nlohmann::json::array_t>());
					break;
				}
			}
		}

		void reader(archivebuf& node, const nlohmann::json& json) except
		{
			for (auto& it = json.begin(); it != json.end(); ++it)
			{
				auto& value = it.value();

				switch (value.type())
				{
				case nlohmann::json::value_t::boolean:
					node.push_back(it.key(), value.get<nlohmann::json::boolean_t>());
					break;
				case nlohmann::json::value_t::number_float:
					node.push_back(it.key(), value.get<nlohmann::json::number_float_t>());
					break;
				case nlohmann::json::value_t::number_integer:
					node.push_back(it.key(), value.get<nlohmann::json::number_integer_t>());
					break;
				case nlohmann::json::value_t::number_unsigned:
					node.push_back(it.key(), value.get<nlohmann::json::number_unsigned_t>());
					break;
				case nlohmann::json::value_t::string:
					node.push_back(it.key(), value.get<nlohmann::json::string_t>());
					break;
				case nlohmann::json::value_t::object:
					reader(node[it.key()], value);
					break;
				case nlohmann::json::value_t::array:
				{
					node.push_back(it.key(), archivebuf(archivebuf::array));
					reader(node.back(), value.get<nlohmann::json::array_t>());
				}
				break;
				default:
					break;
				}
			}
		}

		void write(nlohmann::json& node, archivebuf& json) except;

		void write(nlohmann::json& node, const archivebuf::array_t& arrays) except
		{
			std::size_t index = 0;

			for (auto& object : arrays)
			{
				switch (object.type())
				{
				case archivebuf::type_t::boolean:
					node[index++] = object.get<archivebuf::boolean_t>();
					break;
				case archivebuf::type_t::number_float:
					node[index++] = object.get<archivebuf::number_float_t>();
					break;
				case archivebuf::type_t::number_integer:
					node[index++] = object.get<archivebuf::number_integer_t>();
					break;
				case archivebuf::type_t::number_unsigned:
					node[index++] = object.get<archivebuf::number_unsigned_t>();
					break;
				case archivebuf::type_t::string:
					node[index++] = object.get<archivebuf::string_t>();
					break;
				case archivebuf::type_t::object:
					write(node[index++], object.get<archivebuf::object_t>());
					break;
				case archivebuf::type_t::array:

					//node[index].emplace<nlohmann::json::array_t>(nlohmann::json());
					write(node[index++], object.get<archivebuf::array_t>());
					break;
				}
			}
		}

		void write(nlohmann::json& node, archivebuf& json) except
		{
			for (auto& it : json)
			{
				auto& value = it.second;

				switch (value.type())
				{
				case archivebuf::type_t::boolean:
					node[it.first] = value.get<archivebuf::boolean_t>();
					break;
				case archivebuf::type_t::number_float:
					node[it.first] = value.get<archivebuf::number_float_t>();
					break;
				case archivebuf::type_t::number_integer:
					node[it.first] = value.get<archivebuf::number_integer_t>();
					break;
				case archivebuf::type_t::number_unsigned:
					node[it.first] = value.get<archivebuf::number_unsigned_t>();
					break;
				case archivebuf::type_t::string:
					node[it.first] = value.get<archivebuf::string_t>();
					break;
				case archivebuf::type_t::object:
					write(node[it.first], value);
					break;
				case archivebuf::type_t::array:
				{
					//node[it.first].emplace<nlohmann::json::array_t>(nlohmann::json::array_t());
					write(node.back(), value.get<archivebuf::array_t>());
				}
				break;
				default:
					break;
				}
			}
		}

		JsonReader::JsonReader() noexcept
			: iarchive(&_json)
		{
		}

		JsonReader::JsonReader(istream& stream) except
			: iarchive(&_json)
		{
			this->open(stream);
		}

		JsonReader::JsonReader(const std::string& path) except
			: iarchive(&_json)
		{
			this->open(path);
		}

		JsonReader::~JsonReader() noexcept
		{
			this->close();
		}

		JsonReader&
		JsonReader::open(istream& stream, const ios_base::open_mode mode) except
		{
			try
			{
				auto length = stream.size();
				if (length == 0)
				{
					this->setstate(ios_base::failbit);
					return *this;
				}

				std::string data;
				data.resize((std::string::size_type)length);

				if (!stream.read((char*)data.c_str(), (std::string::size_type)length))
				{
					this->setstate(ios_base::failbit);
					return *this;
				}

				auto json = nlohmann::json::parse(data.begin(), data.end());
				reader(_json, json);

				this->clear(ios_base::goodbit, mode);
				return *this;
			}
			catch (const std::exception& e)
			{
				this->setstate(ios_base::failbit);
				throw std::runtime_error(e.what());
			}
		}

		JsonReader&
		JsonReader::open(const std::string& path) except
		{
			ifstream stream;
			if (stream.open(path))
				return this->open(stream);
			else
			{
				this->setstate(ios_base::failbit);
				return *this;
			}
		}

		bool
		JsonReader::is_open() const noexcept
		{
			return _json.size();
		}

		void
		JsonReader::close() noexcept
		{
			_json.clear();
		}

		JsonWrite::JsonWrite() noexcept
			: oarchive(&_json)
		{
		}

		JsonWrite::~JsonWrite() noexcept
		{
			this->close();
		}

		JsonWrite&
		JsonWrite::save(ostream& stream, const ios_base::open_mode mode) except
		{
			try
			{
				ios_base::clear(ios_base::failbit, mode);

				nlohmann::json json;
				write(json, _json);

				return *this;
			}
			catch (const std::exception& e)
			{
				this->setstate(ios_base::failbit);
				throw std::runtime_error(e.what());
			}
		}

		JsonWrite&
		JsonWrite::save(const std::string& path) except
		{
			ofstream stream;
			if (stream.open(path))
				return this->save(stream);
			else
			{
				this->setstate(ios_base::failbit);
				return *this;
			}
		}

		void
		JsonWrite::close() noexcept
		{
			_json.clear();
		}

		bool
		JsonWrite::is_open() const noexcept
		{
			return _json.size();
		}
	}
}