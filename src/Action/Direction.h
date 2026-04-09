#include "Ptr.h"
#include "Selection.h"

namespace FuncDoodle {
	class ProjectFile;

	enum class Direction { None = 0, Left, Right, Up, Down };

	struct MoveSelectionActionContext {
			unsigned long frameIndex;
			WeakPtr<Selection> sel;
			Direction moveDir;
			const SharedPtr<ProjectFile>& proj;
	};
}  // namespace FuncDoodle
