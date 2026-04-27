#pragma once

namespace FuncDoodle {
	class Action {
		public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
		virtual ~Action() = default;

		friend class DrawAction;
		friend class FillAction;
		friend class DeleteFrameAction;
	};
}
