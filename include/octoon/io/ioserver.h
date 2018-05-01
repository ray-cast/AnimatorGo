// File: IoServer.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_SERVER_H_
#define OCTOON_IO_SERVER_H_

#include <map>
#include <memory>
#include <string>

#include <octoon/io/ori.h>
#include <octoon/io/fstream.h>
#include <octoon/io/archive.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace io
	{
		/*
		* A `IoServer` is a namespace for Octoon instance to seek for resources in
		* local storage, at remote host, or in compressed archive.
		*/
		class OCTOON_EXPORT IoServer final
		{
			OctoonDeclareSingleton(IoServer)
		public:
			/*
			* Register an entry to the IoServer, so that the file contained in that
			* entry an be found.
			*/
			void mount_archive(const std::string& vpath, const archive_pointer& entry);
			void mount_archive(const std::string& vpath, archive_pointer&& entry);
			void mount_archive(std::string&& vpath, archive_pointer&& entry);

			archive_pointer unmount_archive(const std::string& path);

			archive_pointer get_archive(const Orl& orl) const;

		private:
			std::map<std::string, std::shared_ptr<archive>> registry_;
		};

		using IoServerPtr = std::shared_ptr<IoServer>;
	}
}

#endif
