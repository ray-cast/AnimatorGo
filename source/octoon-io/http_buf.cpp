#include <octoon/io/http_buf.h>
#include <curl/curl.h>
#include <iostream>
#include <cstring>

namespace octoon
{
	namespace io
	{
		std::size_t 
		httpbuf::WriteMemoryCallback(void* contents, std::size_t size, std::size_t nmemb, void* userp) noexcept
		{
			std::size_t realsize = size * nmemb;
			httpbuf* mem = (httpbuf*)userp;

			auto index = mem->size();
			mem->memory_.resize(mem->size() + realsize);
			std::memcpy(&mem->memory_[index], contents, realsize);
			
			return realsize;
		}

		httpbuf::httpbuf() noexcept
			: curl_(nullptr)
			, next_(0)
		{
		}

		httpbuf::~httpbuf() noexcept
		{
			this->close();
		}

		bool
		httpbuf::is_open() const noexcept
		{
			return curl_ != nullptr;
		}

		bool
		httpbuf::get(const char* url, std::uint32_t timeout) noexcept
		{
			assert(!this->is_open());

			CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
			if (res != CURLE_OK)
			{
				std::cout << "curl_global_init() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			struct curl_slist* head = NULL;
			head = curl_slist_append(head, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");

			curl_ = curl_easy_init();
			curl_easy_setopt(curl_, CURLOPT_URL, url);
			curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
			curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
			curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this);
			curl_easy_setopt(curl_, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, head);

			res = ::curl_easy_perform(curl_);
			if (res != CURLE_OK)
			{
				std::cout << "curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			return true;
		}

		bool
		httpbuf::get(const std::string& url, std::uint32_t timeout) noexcept
		{
			return this->get(url.c_str(), timeout);
		}

		bool
		httpbuf::post(const char* url, const std::string& data, const std::string& header_dict, std::uint32_t timeout) noexcept
		{
			assert(!this->is_open());

			CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
			if (res != CURLE_OK)
			{
				std::cout << "curl_global_init() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			struct curl_slist* headers = nullptr;
			headers = curl_slist_append(headers, header_dict.c_str());

			curl_ = curl_easy_init(); /* init the curl session */
			curl_easy_setopt(curl_, CURLOPT_POST, 1L);
			curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.data());
			curl_easy_setopt(curl_, CURLOPT_URL, url);
			curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
			curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
			curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this);
			curl_easy_setopt(curl_, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		
			res = ::curl_easy_perform(curl_);
			if (res != CURLE_OK)
			{
				std::cout << "curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			return true;
		}

		bool
		httpbuf::post(const std::string& url, const std::string& data, const std::string& header_dict, std::uint32_t timeout) noexcept
		{
			return this->post(url.c_str(), data, header_dict, timeout);
		}

		streamsize
		httpbuf::read(char* str, std::streamsize cnt) noexcept
		{		
			auto buf_size = memory_.size();
			std::size_t actual_copyable = 0;

			if (buf_size - next_ > cnt)
			{
				actual_copyable = cnt;
				std::memcpy(str, memory_.data() + next_, actual_copyable);
				next_ += cnt;
			}
			else
			{
				actual_copyable = buf_size - next_;
				std::memcpy(str, memory_.data() + next_, actual_copyable);
				next_ = buf_size;
			}

			return actual_copyable;
		}

		streamsize
		httpbuf::write(const char* str, std::streamsize cnt) noexcept
		{
			return false;
		}

		streamoff
		httpbuf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			size_t base = 0;
			switch (dir)
			{
			case ios_base::beg:
				base = 0;
				break;
			case ios_base::end:
				base = next_;
				break;
			case ios_base::cur:
				base = memory_.size();
				break;
			}

			std::size_t resultant = base + pos;
			if (resultant >= 0 && resultant <= memory_.size())
			{
				next_ = resultant;
				return true;
			}

			return false;
		}

		streamoff
		httpbuf::tellg() noexcept
		{
			return next_;
		}

		streamsize
		httpbuf::size() const noexcept
		{
			return memory_.size();
		}

		int
		httpbuf::flush() noexcept
		{
			return 0;
		}

		bool
		httpbuf::close() noexcept
		{
			if (curl_)
			{
				curl_easy_cleanup(curl_);	/* cleanup curl stuff */
				curl_global_cleanup();		/* we're done with libcurl, so clean it up */

				curl_ = nullptr;
			}

			return true;
		}

		runtime::json
		httpbuf::json() const noexcept
		{
			if (memory_.empty())
				return runtime::json();
			std::string_view str = std::string_view(memory_.data(), memory_.size());
			return runtime::json::parse(std::string(str));
		}
	}
}