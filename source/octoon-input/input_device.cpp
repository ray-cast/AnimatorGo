#include <octoon/input/input_device.h>
#include <algorithm> // std::find

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
			this->clear_input_listener();
		}

		void
		DefaultInputDevice::set_capture_object(WindHandle window) noexcept
		{
		}

		WindHandle
		DefaultInputDevice::get_capture_object() const noexcept
		{
			return nullptr;
		}

		void
		DefaultInputDevice::enable_event_posting(bool enable) noexcept
		{
			enableEventPosting_ = enable;
		}

		bool
		DefaultInputDevice::enable_event_posting() const noexcept
		{
			return enableEventPosting_;
		}

		void
		DefaultInputDevice::add_input_listener(const IInputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it == inputListeners_.end())
			{
				listener->on_attach();
				inputListeners_.push_back(listener);
			}
		}

		void
		DefaultInputDevice::add_input_listener(IInputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it == inputListeners_.end())
			{
				listener->on_attach();
				inputListeners_.push_back(std::move(listener));
			}
		}

		void
		DefaultInputDevice::remove_input_listener(const IInputListenerPtr& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it != inputListeners_.end())
			{
				listener->on_detach();
				inputListeners_.erase(it);
			}
		}

		void
		DefaultInputDevice::remove_input_listener(IInputListenerPtr&& listener) noexcept
		{
			assert(listener);
			auto it = std::find(inputListeners_.begin(), inputListeners_.end(), listener);
			if (it != inputListeners_.end())
			{
				listener->on_detach();
				inputListeners_.erase(it);
			}
		}

		void
		DefaultInputDevice::clear_input_listener() noexcept
		{
			for (auto& listener : inputListeners_)
				listener->on_detach();
			inputListeners_.clear();
		}

		bool
		DefaultInputDevice::send_event(const InputEvent& event) noexcept
		{
			try
			{
				for (auto& it : inputListeners_)
					it->on_input_event(event);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		bool
		DefaultInputDevice::post_event(const InputEvent& event) noexcept
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
		DefaultInputDevice::peek_events(InputEvent& event) noexcept
		{
			return true;
		}

		bool
		DefaultInputDevice::poll_events(InputEvent& event) noexcept
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
		DefaultInputDevice::wait_events(InputEvent& event) noexcept
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (events_.empty())
			{
				dispose_.wait(lock);
			}

			return this->poll_events(event);
		}

		bool
		DefaultInputDevice::wait_events(InputEvent& event, int timeout) noexcept
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (events_.empty())
			{
				dispose_.wait_for(lock, std::chrono::milliseconds(timeout));
			}

			return this->poll_events(event);
		}

		void
		DefaultInputDevice::flush_event() noexcept
		{
			mutex_.lock();
			events_ = std::queue<InputEvent>();
			mutex_.unlock();
		}

		IInputDevicePtr
		DefaultInputDevice::clone() const noexcept
		{
			return std::make_shared<DefaultInputDevice>();
		}
	}
}