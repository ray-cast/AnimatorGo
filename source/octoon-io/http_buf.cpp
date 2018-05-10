#include <octoon/io/http_buf.h>
#include <curl/curl.h>
#include <iostream>

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
			memcpy(&mem->memory_[index], contents, realsize);
			
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
		httpbuf::open(const char* url, ios_base::openmode mode) noexcept
		{
			assert(!this->is_open());

			CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
			if (res != CURLE_OK)
			{
				std::cout << "curl_global_init() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			curl_ = curl_easy_init(); /* init the curl session */
			curl_easy_setopt(curl_, CURLOPT_URL, url); /* specify URL to get */
			curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); /* send all data to this function  */
			curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this); /* we pass our 'chunk' struct to the callback function */
			curl_easy_setopt(curl_, CURLOPT_USERAGENT, "libcurl-agent/1.0"); /* some servers don't like requests that are made without a user-agent field, so we provide one */
		
			res = ::curl_easy_perform(curl_);
			if (res != CURLE_OK)
			{
				std::cout << "curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
				return false;
			}

			return true;
		}

		bool
		httpbuf::open(const std::string& url, ios_base::openmode mode) noexcept
		{
			return this->open(url.c_str(), mode);
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
	}
}