// File: IoServer.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_SERVER_H_
#define OCTOON_IO_SERVER_H_

#include <map>
#include <memory>
#include <string>

#include <octoon/io/ori.h>
#include <octoon/io/fstream.h>
#include <octoon/io/package.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace io
	{
		/*
		* A `IoServer` is a namespace for Octoon instance to seek for resources in
		* local storage, at remote host, or in compressed package.
		*/
		class OCTOON_EXPORT IoServer final
		{
			OctoonDeclareSingleton(IoServer)
		public:
			IoServer() noexcept;
			~IoServer() noexcept;

			/*
			* Register an entry to the IoServer, so that the file contained in that
			* entry an be found.
			*/
			void mount_package(const std::string& vpath, const package_pointer& entry);
			void mount_package(const std::string& vpath, package_pointer&& entry);
			void mount_package(std::string&& vpath, package_pointer&& entry);

			package_pointer unmount_archive(const std::string& path);

			package_pointer get_archive(const Orl& orl) const;

		private:
			IoServer(const IoServer&) = delete;
			IoServer& operator=(const IoServer&) = delete;

		private:
			std::map<std::string, std::shared_ptr<package>> registry_;
		};

		using IoServerPtr = std::shared_ptr<IoServer>;
	}
}

#endif
