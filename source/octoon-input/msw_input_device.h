// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef OCTOON_INPUT_MSW_DEVICE_H_
#define OCTOON_INPUT_MSW_DEVICE_H_

#include <octoon/input/input_device.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT MSWInputDevice final : public DefaultInputDevice
		{
			OctoonDeclareSubClass(MSWInputDevice, InputDevice)
		public:
			MSWInputDevice() noexcept;
			~MSWInputDevice() noexcept;

			virtual void setCaptureObject(WindHandle window) noexcept;
			virtual WindHandle getCaptureObject() const noexcept;

			virtual bool peekEvents(InputEvent& event) noexcept;
			virtual bool pollEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept;
			virtual void flushEvent() noexcept;

			virtual InputDevicePtr clone() const noexcept;

		private:
			virtual void update() noexcept;

		private:
			MSWInputDevice(const MSWInputDevice&) noexcept = delete;
			MSWInputDevice& operator=(const MSWInputDevice&) noexcept = delete;

		private:

			HWND _window;

			bool _isButtonPress;

			int _button;

			int _mouseX;
			int _mouseY;
		};
	}
}

#endif