#include <octoon/video/collector.h>
#include <stdexcept>

namespace octoon
{
	struct IdCompare
	{
		bool operator() (const runtime::RttiInterface* lhs, const runtime::RttiInterface* rhs) const
		{
			return (std::intptr_t)lhs < (std::intptr_t)rhs;
		}
	};

	using ItemMap = std::unordered_map<void*, int>;
	using ItemSet = std::set<runtime::RttiInterface*, IdCompare>;

	template <typename UnderlyingIterator> class IteratorImpl : public Iterator
	{
	public:
		IteratorImpl(UnderlyingIterator begin, UnderlyingIterator end)
			: m_begin(begin)
			, m_end(end)
			, m_cur(begin)
		{
		}

		bool IsValid() const override
		{
			return m_cur != m_end;
		}

		void Next() override
		{
			++m_cur;
		}

		runtime::RttiInterface* Item() const override
		{
			return *m_cur;
		}

		void Reset() override
		{
			m_cur = m_begin;
		}

	private:
		UnderlyingIterator m_begin;
		UnderlyingIterator m_end;
		UnderlyingIterator m_cur;
	};

	class BundleImpl : public Bundle
	{
	public:
		BundleImpl(ItemMap const& map, ItemSet const& set)
			: m_map(map)
			, m_set(set)
		{
		}

		virtual ~BundleImpl() = default;

		ItemMap m_map;
		ItemSet m_set;
	};

	struct Collector::CollectorImpl
	{
		ItemMap m_map;
		ItemSet m_set;
	};

	Collector::Collector()
		: impl(new CollectorImpl)
	{
	}

	Collector::~Collector()
	{
	}

	void
	Collector::Clear()
	{
		impl->m_map.clear();
		impl->m_set.clear();
	}

	std::unique_ptr<Iterator>
	Collector::CreateIterator() const
	{
		return std::unique_ptr<Iterator>(new IteratorImpl<ItemSet::const_iterator>(impl->m_set.cbegin(), impl->m_set.cend()));
	}

	void
	Collector::Collect(Iterator & iter, ExpandFunc expand_func)
	{
		for (;iter.IsValid(); iter.Next())
		{
			auto cur_items = expand_func(iter.Item());
			impl->m_set.insert(cur_items.cbegin(), cur_items.cend());
		}
	}

	void
	Collector::Collect(runtime::RttiInterface* object)
	{
		assert(object);
		impl->m_set.insert(object);
	}

	void
	Collector::Collect(std::shared_ptr<runtime::RttiInterface> object)
	{
		assert(object);
		impl->m_set.insert(object.get());
	}

	void
	Collector::Commit()
	{
		impl->m_map.clear();

		int idx = 0;
		for (auto& i : impl->m_set)
			impl->m_map[i] = idx++;
	}

	void
	Collector::Finalize(FinalizeFunc finalize_func)
	{
		for (auto& i : impl->m_set)
		{
			finalize_func(i);
		}
	}

	bool
	Collector::NeedsUpdate(Bundle const* bundle, ChangedFunc changed_func) const
	{
		auto bundle_impl = static_cast<BundleImpl const*>(bundle);
		if (bundle_impl->m_set.size() != impl->m_set.size())
			return true;

		for (auto& i : impl->m_set)
		{
			if (changed_func(i))
				return true;

			auto iter = bundle_impl->m_set.find(i);
			if (iter == bundle_impl->m_set.cend())
				return true;
		}

		return false;
	}

	std::size_t
	Collector::GetNumItems() const
	{
		return impl->m_set.size();
	}

	Bundle*
	Collector::CreateBundle() const
	{
		return new BundleImpl{ impl->m_map, impl->m_set };
	}

	std::uint32_t
	Collector::GetItemIndex(runtime::RttiInterface* item) const
	{
		auto iter = impl->m_map.find(item);
		if (iter == impl->m_map.end())
			throw std::runtime_error("No such item in the collector");

		return iter->second;
	}
}