#include "Player.h"

#include "Project.h"

#include "Gui.h"

namespace FuncDoodle {
	AnimationPlayer::AnimationPlayer(const SharedPtr<ProjectFile>& proj)
		: m_Proj(proj) {
		m_Playing = false;
	}
	AnimationPlayer::~AnimationPlayer() {}

	void AnimationPlayer::Play() {
		if (!m_Playing)
			return;
		float dt = ImGui::GetIO().DeltaTime;
		float fps = static_cast<float>(m_Proj->AnimFPS());
		m_TimeElapsed += dt;
		m_CurFrame = static_cast<unsigned long>(m_TimeElapsed * fps) %
					 m_Proj->AnimFrameCount();
	}
	void AnimationPlayer::Rewind() {
		m_CurFrame = 0;
		m_TimeElapsed = 0.0f;
	}
	void AnimationPlayer::End() {
		m_CurFrame =
			m_Proj->AnimFrameCount() -
			1;	// -1 cos cpp ain't stupid and uses chad 0 based indexing unlike
				// python, stupid lua, and other stupid languages
	}
}  // namespace FuncDoodle
