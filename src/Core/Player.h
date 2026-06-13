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
 * @note m_CurFrame is always expected to be a valid index within the project
 * frame list.
 * @note Playback timing is accumulated using m_TimeElapsed.
 */

#pragma once

#include "Project/Project.h"
#include "Util/Ptr.h"

namespace FuncDoodle {
	/**
	 * @class AnimationPlayer
	 * @brief Responsible for playing a users' animation.
	 *
	 * @invariant m_CurFrame is always a valid frame index within m_Proj
	 *
	 * @note Play() advances frames over time using m_TimeElapsed
	 * @note AnimationPlayer maintains internal playback state (frame + time
	 * accumulator)
	 * @note Rewind() resets playback to the first frame
	 */
	class AnimationPlayer {
		public:
		/**
		 * @fn AnimationPlayer
		 * @brief Creates a player for a project.
		 *
		 * @param proj Project whose frames will be played back.
		 */
		AnimationPlayer(const SharedPtr<ProjectFile>& proj);
		~AnimationPlayer();
		/**
		 * @fn Play
		 * @brief Advances playback based on elapsed time.
		 */
		void Play();
		/**
		 * @fn Rewind
		 * @brief Resets playback to the first frame.
		 */
		void Rewind();
		/**
		 * @fn End
		 * @brief Jumps playback to the final frame.
		 */
		void End();
		/**
		 * @fn SetPlaying
		 * @brief Sets whether playback is active.
		 *
		 * @param playing New playback state.
		 */
		constexpr void SetPlaying(bool playing) { m_Playing = playing; }
		/**
		 * @fn Playing
		 * @brief Returns whether playback is currently active.
		 *
		 * @return Playback state.
		 */
		[[nodiscard]] constexpr bool Playing() const { return m_Playing; }

		/**
		 * @fn SetProj
		 * @brief Replaces the project used for playback.
		 *
		 * @param proj Project to play.
		 */
		void SetProj(SharedPtr<ProjectFile> proj) { m_Proj = proj; }
		/**
		 * @fn Proj
		 * @brief Returns the project used for playback.
		 *
		 * @return Shared pointer to the active project.
		 */
		[[nodiscard]] SharedPtr<ProjectFile> Proj() const { return m_Proj; }

		/**
		 * @fn CurFrame
		 * @brief Returns the current playback frame index.
		 *
		 * @return Current frame index.
		 */
		[[nodiscard]] constexpr uint64_t CurFrame() const { return m_CurFrame; }
		/**
		 * @fn SetCurFrame
		 * @brief Sets the current playback frame index.
		 *
		 * @param frame Frame index to jump to.
		 */
		constexpr void SetCurFrame(uint64_t frame) { m_CurFrame = frame; }

		private:
		bool m_Playing;
		SharedPtr<ProjectFile> m_Proj;
		uint64_t m_CurFrame = 0L;
		float m_TimeElapsed = 0.0f;
	};
}  // namespace FuncDoodle
