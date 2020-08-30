#ifndef OCTOON_VIDEO_COLLECTOR_H_
#define OCTOON_VIDEO_COLLECTOR_H_

#include <set>
#include <functional>
#include <octoon/video/render_object.h>

namespace octoon::video
{
    class Bundle
    {
    public:
        virtual ~Bundle() = default;
    };

    class Iterator
    {
    public:
        Iterator() = default;
        virtual ~Iterator() = default;

        virtual bool IsValid() const = 0;
        virtual void Next() = 0;

        virtual runtime::RttiInterface* Item() const = 0;
        virtual void Reset() = 0;

        template <typename T> typename T* ItemAs() const { return dynamic_cast<T*>(Item()); }

        Iterator(Iterator const&) = delete;
        Iterator& operator = (Iterator const&) = delete;
    };

    class Collector final
    {
    public:
        using ExpandFunc = std::function<std::set<runtime::RttiInterface*>(runtime::RttiInterface*)>;
        using ChangedFunc = std::function<bool(runtime::RttiInterface*)>;
        using FinalizeFunc = std::function<void(runtime::RttiInterface*)>;

        Collector();
        virtual ~Collector();

        void Clear();
        std::unique_ptr<Iterator> CreateIterator() const;
        void Collect(Iterator& iter, ExpandFunc expand_func);
        void Collect(runtime::RttiInterface* object);
        void Collect(std::shared_ptr<runtime::RttiInterface> object);

        void Commit();
        bool NeedsUpdate(Bundle const* bundle, ChangedFunc cahnged_func) const;
        std::size_t GetNumItems() const;
        Bundle* CreateBundle() const;
        std::uint32_t GetItemIndex(runtime::RttiInterface* item) const;
        void Finalize(FinalizeFunc finalize_func);

        Collector(Collector const&) = delete;
        Collector& operator = (Collector const&) = delete;

    private:
        struct CollectorImpl;
        std::unique_ptr<CollectorImpl> impl;
    };
}

#endif