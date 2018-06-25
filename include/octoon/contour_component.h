#ifndef OCTOON_CONTOUR_COMPONENT_H_
#define OCTOON_CONTOUR_COMPONENT_H_

#include <octoon/model/contour.h>
#include <octoon/game_component.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT ContourComponent : public GameComponent
	{
		OctoonDeclareSubClass(ContourComponent, GameComponent)
	public:
		typedef std::function<void(const model::ContourPtr&)> OnContourReplaceEvent;
		typedef std::vector<OnContourReplaceEvent*> OnContourReplaceEvents;

	public:
		ContourComponent() noexcept;
		ContourComponent(model::Contour&& mesh, bool sharedContour = false) noexcept;
		ContourComponent(model::ContourPtr&& mesh, bool sharedContour = false) noexcept;
		ContourComponent(const model::Contour& mesh, bool sharedContour = false) noexcept;
		ContourComponent(const model::ContourPtr& mesh, bool sharedContour = false) noexcept;
		virtual ~ContourComponent() noexcept;

		void setContour(model::Contour&& mesh, bool sharedContour = false) noexcept;
		void setContour(model::ContourPtr&& mesh, bool sharedContour = false) noexcept;
		void setContour(const model::ContourPtr& mesh, bool sharedContour = false) noexcept;
		const model::ContourPtr& getContour() const noexcept;

		bool isSharedContour() const noexcept;

		void uploadContourData() noexcept;

		void addContourListener(OnContourReplaceEvent* func) noexcept;
		void removeContourListener(const OnContourReplaceEvent* func) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onContourReplace(const model::ContourPtr& mesh) noexcept;

	private:
		ContourComponent(const ContourComponent&) = delete;
		ContourComponent& operator=(const ContourComponent&) = delete;

	private:
		bool isSharedContour_;
		model::ContourPtr contour_;
		OnContourReplaceEvents delegates_;
	};
}

#endif