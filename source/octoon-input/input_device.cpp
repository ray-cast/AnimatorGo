#include <octoon/input/input_device.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputDevice, IInputDevice, "DefaultInputDevice")

		DefaultInputDevice::DefaultInputDevice() noexcept
			: enableEventPosting_(true)
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
			enableEventPosting_ = enable;
		}

		bool
		DefaultInputDevice::enableEventPosting() const noexcept
		{
			return enableEventPosting_;
		}

		void
		DefaultInputDevice::addInputListener(InputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it == inputListeners_.end())
			{
				listener->onAttach();
				inputListeners_.push_back(listener);
			}
		}

		void
		DefaultInputDevice::addInputListener(InputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it == inputListeners_.end())
			{
				listener->onAttach();
				inputListeners_.push_back(std::move(listener));
			}
		}

		void
		DefaultInputDevice::removeInputListener(InputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it != inputListeners_.end())
			{
				listener->onDetach();
				inputListeners_.erase(it);
			}
		}

		void
		DefaultInputDevice::removeInputListener(InputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it != inputListeners_.end())
			{
				listener->onDetach();
				inputListeners_.erase(it);
			}
		}

		void
		DefaultInputDevice::clearInputListener() noexcept
		{
			for (auto& listener : inputListeners_)
				listener->onDetach();
			inputListeners_.clear();
		}

		bool
		DefaultInputDevice::sendEvent(const InputEvent& event) noexcept
		{
			try
			{
				for (auto& it : inputListeners_)
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
			if (enableEventPosting_)
			{
				mutex_.lock();
				events_.push(event);
				mutex_.unlock();

				dispose_.notify_one();
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
			std::unique_lock<std::mutex> lock(mutex_);
			if (lock)
			{
				if (!events_.empty())
				{
					event = events_.front();
					events_.pop();
					return true;
				}
			}

			return false;
		}

		bool
		DefaultInputDevice::waitEvents(InputEvent& event) noexcept
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (events_.empty())
			{
				dispose_.wait(lock);
			}

			return this->pollEvents(event);
		}

		bool
		DefaultInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (events_.empty())
			{
				dispose_.wait_for(lock, std::chrono::milliseconds(timeout));
			}

			return this->pollEvents(event);
		}

		void
		DefaultInputDevice::flushEvent() noexcept
		{
			mutex_.lock();
			events_ = std::queue<InputEvent>();
			mutex_.unlock();
		}

		InputDevicePtr
		DefaultInputDevice::clone() const noexcept
		{
			return std::make_shared<DefaultInputDevice>();
		}
	}
}