#include <octoon/input/input_device.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputDevice, InputDevice, "DefaultInputDevice")

		DefaultInputDevice::DefaultInputDevice() noexcept
			: _enableEventPosting(true)
		{
		}

		DefaultInputDevice::~DefaultInputDevice() noexcept
		{
			this->clearInputListener();
		}

		void
		DefaultInputDevice::setCaptureObject(WindHandle window) noexcept
		{
		}

		WindHandle
		DefaultInputDevice::getCaptureObject() const noexcept
		{
			return nullptr;
		}

		void
		DefaultInputDevice::enableEventPosting(bool enable) noexcept
		{
			_enableEventPosting = enable;
		}

		bool
		DefaultInputDevice::enableEventPosting() const noexcept
		{
			return _enableEventPosting;
		}

		void
		DefaultInputDevice::addInputListener(InputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
			if (it == _inputListeners.end())
			{
				listener->onAttach();
				_inputListeners.push_back(listener);
			}
		}

		void
			DefaultInputDevice::addInputListener(InputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
			if (it == _inputListeners.end())
			{
				listener->onAttach();
				_inputListeners.push_back(std::move(listener));
			}
		}

		void
			DefaultInputDevice::removeInputListener(InputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
			if (it != _inputListeners.end())
			{
				listener->onDetach();
				_inputListeners.erase(it);
			}
		}

		void
			DefaultInputDevice::removeInputListener(InputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
			if (it != _inputListeners.end())
			{
				listener->onDetach();
				_inputListeners.erase(it);
			}
		}

		void
			DefaultInputDevice::clearInputListener() noexcept
		{
			for (auto& listener : _inputListeners)
				listener->onDetach();
			_inputListeners.clear();
		}

		bool
			DefaultInputDevice::sendEvent(const InputEvent& event) noexcept
		{
			try
			{
				for (auto& it : _inputListeners)
					it->onInputEvent(event);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		bool
			DefaultInputDevice::postEvent(const InputEvent& event) noexcept
		{
			if (_enableEventPosting)
			{
				_mutex.lock();
				_events.push(event);
				_mutex.unlock();

				_dispose.notify_one();
			}

			return true;
		}

		bool
			DefaultInputDevice::peekEvents(InputEvent& event) noexcept
		{
			return true;
		}

		bool
			DefaultInputDevice::pollEvents(InputEvent& event) noexcept
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (lock)
			{
				if (!_events.empty())
				{
					event = _events.front();
					_events.pop();
					return true;
				}
			}

			return false;
		}

		bool
			DefaultInputDevice::waitEvents(InputEvent& event) noexcept
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (_events.empty())
			{
				_dispose.wait(lock);
			}

			return this->pollEvents(event);
		}

		bool
			DefaultInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (_events.empty())
			{
				_dispose.wait_for(lock, std::chrono::milliseconds(timeout));
			}

			return this->pollEvents(event);
		}

		void
			DefaultInputDevice::flushEvent() noexcept
		{
			_mutex.lock();
			_events = std::queue<InputEvent>();
			_mutex.unlock();
		}

		InputDevicePtr
			DefaultInputDevice::clone() const noexcept
		{
			return std::make_shared<DefaultInputDevice>();
		}
	}
}