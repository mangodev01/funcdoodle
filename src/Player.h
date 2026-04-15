#pragma once

#include "Project.h"
#include "Ptr.h"

namespace FuncDoodle {
	class AnimationPlayer {
		public:
		AnimationPlayer(const SharedPtr<ProjectFile>& proj);
		~AnimationPlayer();
		void Play();
		void Rewind();
		void End();
		constexpr inline void SetPlaying(bool playing) { m_Playing = playing; }
		constexpr inline bool Playing() const { return m_Playing; }

		inline void SetProj(SharedPtr<ProjectFile> proj) { m_Proj = proj; }
		inline SharedPtr<ProjectFile> Proj() const { return m_Proj; }

		constexpr inline unsigned long CurFrame() const { return m_CurFrame; }
		constexpr inline void SetCurFrame(unsigned long frame) {
			m_CurFrame = frame;
		}

		private:
		bool m_Playing;
		SharedPtr<ProjectFile> m_Proj;
		unsigned long m_CurFrame = 0L;
		float m_TimeElapsed = 0.0f;
	};
}  // namespace FuncDoodle
