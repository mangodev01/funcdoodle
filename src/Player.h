/**
 * @file Player.h
 * @brief Defines AnimationPlayer, responsible for animation playback control.
 *
 * This file contains the AnimationPlayer class, which manages playback of
 * a ProjectFile animation over time.
 *
 * Responsibilities include:
 * - Controlling play/pause state of the animation
 * - Advancing frames based on elapsed time
 * - Managing current frame index
 * - Rewinding and resetting playback state
 *
 * The player operates directly on a ProjectFile instance and maintains
 * internal timing state used for frame progression.
 *
 * @note m_CurFrame is always expected to be a valid index within the project frame list.
 * @note Playback timing is accumulated using m_TimeElapsed.
 */

#pragma once

#include "Project.h"
#include "Ptr.h"

namespace FuncDoodle {
	/**
	 * @class AnimationPlayer
	 * @brief Responsible for playing a users' animation.
	 *
	 * @invariant m_CurFrame is always a valid frame index within m_Proj
	 *
	 * @note Play() advances frames over time using m_TimeElapsed
	 * @note AnimationPlayer maintains internal playback state (frame + time accumulator)
	 * @note Rewind() resets playback to the first frame
	 */
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
