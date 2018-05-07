#ifndef OCTOON_SIGSLOT_H_
#define OCTOON_SIGSLOT_H_

#include <functional>
#include <utility>
#include <vector>
#include <list>
#include <mutex>

namespace octoon
{
	namespace runtime
	{
		template <class T>
		class signal
		{
			using SlotType = std::function<T>;
			using SignalType = signal<T>;
		public:
			signal() noexcept
				: slots()
			{
			}

			~signal() noexcept
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(auto each:connected_signals)
					each->signals_remove(*this);
			}

			signal(const signal& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}
			signal(signal&& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}

			signal& operator=(const signal& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
				return *this;
			}
			signal& operator=(signal&& other)
			{
				signal tmp(other);
				this->swap(tmp);
				return *this;
			}

			void swap(signal& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				std::swap(slots, other.slots);
			}

			void swap(signal&& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}

			void connect(SlotType f)
			{
				std::lock_guard<std::mutex> guard_slots(slots_mutex);
				if(!f)return;
				for(std::list<SlotType>::iterater it = slots.begin();
					it < slots.end(); ++it)
				{
					if(f.target == it->target)
						return;
				}
				slots.push_back(f);
			}

			void connect(SignalType& sig)
			{
				this->signals_append(sig);
				sig.connected_signals_append(*this);
			}

			void disconnect(SlotType f)
			{
				std::lock_guard<std::mutex> guard_slots(slots_mutex);
				if(!f)return;
				for(std::list<SlotType>::iterater it = slots.begin();
					it < slots.end(); ++it)
				{
					if(&sig == &**it)
					{
						slots.erase(it);
						break;
					}
				}
			}

			void disconnect(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(std::list<SignalType *>::iterater it = signals.begin();
					it < signals.end(); ++it)
				{
					if(&sig == &**it)
					{
						signals.erase(it);
						sig.connected_signals_remove(*this);
						return;
					}
				}
			}

			template <typename ... Args>  
			void call_one_slots(SlotType f, Args&&... args)
			{
				f(std::forward<Args>(args)...);
			}

			template <typename ... Args>  
			void call_all_slots(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_slots(slots_mutex);
				for(auto &each : slots)
					call_one_slots(each, std::forward<Args>(args)...);
			}

			template <typename ... Args>  
			void operator()(Args&&... args)
			{
				call_all_slots(std::forward<Args>(args)...);
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(auto each : signals)
					each->call_all_slots(std::forward<Args>(args)...);
			}

		private:
			bool connected_signals_append(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(std::list<SignalType *>::iterater it = connected_signals.begin();
					it < connected_signals.end(); ++it)
				{
					if(&sig == &**it)
						return false;
				}
				connected_signals.push_back(&sig);
				return true;
			}

			bool connected_signals_remove(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(std::list<SignalType *>::iterater it = connected_signals.begin();
					it < connected_signals.end(); ++it)
				{
					if(&sig == &**it)
					{
						connected_signals.erase(it);
						return true;
					}
				}
				return false;
			}

			bool signals_append(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(std::list<SignalType *>::iterater it = signals.begin();
					it < signals.end(); ++it)
				{
					if(&sig == &**it)
						return false;
				}
				signals.push_back(&sig);
				return true;
			}

			bool signals_remove(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for(std::list<SignalType *>::iterater it = signals.begin();
					it < signals.end(); ++it)
				{
					if(&sig == &**it)
					{
						signals.erase(it);
						return true;
					}
				}
				return false;
			}
		private:
			std::mutex slots_mutex;
			std::mutex signals_mutex;
			std::list<SlotType> slots;
			std::list<SignalType *> signals;
			std::list<SignalType *> connected_signals; // which signals connect me
		};
	}
}

#endif