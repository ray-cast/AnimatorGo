#ifndef OCTOON_SIGSLOT_H_
#define OCTOON_SIGSLOT_H_

#include <functional>
#include <utility>
#include <vector>
#include <list>
#include <mutex>
#include <type_traits>

namespace octoon
{
	namespace runtime
	{
		template<typename T>
		struct result_of
		{
			using type = typename std::function<T>::result_type;
		};

		template<class T>
		struct remove_cvref
		{
			using type = typename std::remove_reference<typename std::remove_cv<T>::type>::type;
		};

		template<class T, class Enable = void>
		struct return_type 
		{
			using type = typename remove_cvref<typename result_of<T>::type>::type;
		};

		template<class T>
		struct return_type<T, typename std::enable_if<std::is_same<typename result_of<T>::type, void>::value>::type>
		{
			using type = void;
		};

		template<class T>
		struct return_type<T, typename std::enable_if<std::is_reference<typename result_of<T>::type>::value>::type>
		{
			using type = std::reference_wrapper<typename remove_cvref<typename result_of<T>::type>::type>;
		};

		template<class T> class signal_impl;

		template <typename T, typename RetType = typename result_of<T>::type>
		class signal
		{
			using SlotType = std::function<T>;
			using SignalType = signal<T>;
			using SignalImpl = signal_impl<T>;
		public:
			signal() noexcept: impl(){}

			~signal() noexcept
			{
				impl.release(*this);
			}

			signal(const signal& other): impl(other.impl){}
			signal(signal&& other): impl(other.impl){}

			signal& operator=(const signal& other)
			{
				impl = other.impl;
				return *this;
			}
			signal& operator=(signal&& other)
			{
				impl = other.impl;
				return *this;
			}

			void connect(SlotType f) { impl.connect(f); }
			void disconnect(SlotType f) { impl.disconnect(f); }

			void connect(SignalType& sig)
			{
				impl.signals_append(sig);
				sig.impl.connected_signals_append(*this);
			}

			void disconnect(SignalType& sig)
			{
				impl.signals_remove(sig);
				sig.connected_signals_remove(*this);
			}

			template <typename ... Args>
			typename return_type<T>::type call_one_slots(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_slots(impl.slots_mutex);
				typename return_type<T>::type retval;
				if (!impl.slots.empty())
				{
					auto f = impl.slots.front();
					retval = f(std::forward<Args>(args)...);
				}
				
				return retval;
			}

			template <typename ... Args>
			std::vector<typename return_type<T>::type> call_all_slots(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_slots(impl.slots_mutex);
				std::vector<typename return_type<T>::type> retval;
				for (auto &each : impl.slots)
					retval.push_back(each(std::forward<Args>(args)...));
				return retval;
			}

			template <typename ... Args>
			std::vector<typename return_type<T>::type> call_all_signals(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_signals(impl.signals_mutex);
				std::vector<typename return_type<T>::type> retval;
				for (auto each : impl.signals)
				{
					std::vector<typename return_type<T>::type> tmp = (*each)(std::forward<Args>(args)...);
					retval.insert(retval.end(), tmp.begin(), tmp.end());
				}
				return retval;
			}

			template <typename ... Args>
			std::vector<typename return_type<T>::type> operator()(Args&&... args)
			{
				std::vector<typename return_type<T>::type> retval = call_all_slots(std::forward<Args>(args)...);
				std::vector<typename return_type<T>::type> signal_retval = call_all_signals(std::forward<Args>(args)...);
				retval.insert(retval.end(), signal_retval.begin(), signal_retval.end());
				return retval;
			}
		private:
			SignalImpl impl;
			template<class S> friend class signal_impl;
		};

		template <typename T>
		class signal<T, void>
		{
			using SlotType = std::function<T>;
			using SignalType = signal<T>;
			using SignalImpl = signal_impl<T>;
		public:
			signal() noexcept : impl() {}
			~signal() noexcept 
			{
				impl.release(*this);
			}
			signal(const signal& other) : impl(other.impl) {}
			signal(signal&& other) : impl(other.impl) {}

			signal& operator=(const signal& other)
			{
				impl = other.impl;
				return *this;
			}
			signal& operator=(signal&& other)
			{
				impl = other.impl;
				return *this;
			}

			void connect(SlotType f) { impl.connect(f); }
			void disconnect(SlotType f) { impl.connect(f); }

			void connect(SignalType& sig)
			{
				impl.signals_append(sig);
				sig.impl.connected_signals_append(*this);
			}

			void disconnect(SignalType& sig)
			{
				impl.signals_remove(sig);
				sig.connected_signals_remove(*this);
			}

			template <typename ... Args>
			void call_one_slots(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_slots(impl.slots_mutex);
				if (!impl.slots.empty())
				{
					auto f = impl.slots.front();
					f(std::forward<Args>(args)...);
				}
			}

			template <typename ... Args>
			void call_all_slots(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_slots(impl.slots_mutex);
				for (auto &each : impl.slots)
					each(std::forward<Args>(args)...);
			}

			template <typename ... Args>
			void call_all_signals(Args&&... args)
			{
				std::lock_guard<std::mutex> guard_signals(impl.signals_mutex);
				for (auto each : impl.signals)
					(*each)(std::forward<Args>(args)...);
			}

			template <typename ... Args>
			void operator()(Args&&... args)
			{
				call_all_slots(std::forward<Args>(args)...);
				call_all_signals(std::forward<Args>(args)...);
			}
		private:
			SignalImpl impl;

			template<class S> friend class signal_impl;
		};

		template<class T>
		class signal_impl
		{
			using SlotType = std::function<T>;
			using SignalType = signal<T>;
			using SignalImplType = signal_impl<T>;
		public:
			signal_impl() noexcept {}
			~signal_impl() noexcept{}

			signal_impl(const signal_impl& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}
			signal_impl(signal_impl&& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}

			signal_impl& operator=(const signal_impl& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
				return *this;
			}
			signal_impl& operator=(signal_impl&& other)
			{
				signal<T> tmp(other);
				this->swap(tmp);
				return *this;
			}

			void swap(signal_impl& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				std::swap(slots, other.slots);
			}

			void swap(signal_impl&& other)
			{
				std::lock_guard<std::mutex> guard(slots_mutex);
				std::lock_guard<std::mutex> guard_other(other.slots_mutex);
				slots = other.slots;
			}

			void release(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for (auto each : signals)
					each->impl.connected_signals_remove(sig);
				for (auto each : connected_signals)
					each->impl.signals_remove(sig);
			}

			void connect(SlotType f)
			{
				std::lock_guard<std::mutex> guard_slots(slots_mutex);
				if (!f)return;
				for (auto it = slots.begin(); it != slots.end(); ++it)
				{
					if (f.target_type() == it->target_type())
						return;
				}
				slots.push_back(f);
			}

			void disconnect(SlotType sig)
			{
				std::lock_guard<std::mutex> guard_slots(slots_mutex);
				if (!sig)return;
				for (auto it = slots.begin(); it != slots.end(); ++it)
				{
					if (&sig == &**it)
					{
						slots.erase(it);
						break;
					}
				}
			}

			bool connected_signals_append(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for (auto it = connected_signals.begin(); it != connected_signals.end(); ++it)
				{
					if (&sig == &**it)
						return false;
				}
				connected_signals.push_back(&sig);
				return true;
			}

			bool connected_signals_remove(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for (auto it = connected_signals.begin(); it != connected_signals.end(); ++it)
				{
					if (&sig == &**it)
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
				for (auto it = signals.begin(); it != signals.end(); ++it)
				{
					if (&sig == &**it)
						return false;
				}
				signals.push_back(&sig);
				return true;
			}

			bool signals_remove(SignalType& sig)
			{
				std::lock_guard<std::mutex> guard_signals(signals_mutex);
				for (auto it = signals.begin(); it != signals.end(); ++it)
				{
					if (&sig == &**it)
					{
						signals.erase(it);
						return true;
					}
				}
				return false;
			}
		public:
			std::mutex slots_mutex;
			std::mutex signals_mutex;
			std::list<SlotType> slots;
			std::list<SignalType *> signals;
			std::list<SignalType *> connected_signals; // which signals connect me
		};
	}
}

#endif