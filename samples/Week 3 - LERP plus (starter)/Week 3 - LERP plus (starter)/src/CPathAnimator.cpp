/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

CPathAnimator.cpp
Simple component for animating an object along a path.

As a convention in NOU, we put "C" before a class name to signify
that we intend the class for use as a component with the ENTT framework.
*/

#include "CPathAnimator.h"

namespace nou
{
	CPathAnimator::CPathAnimator(Entity& owner)
	{
		m_owner = &owner;

		m_segmentIndex = 0;
		m_segmentTimer = 0.0f;

		m_segmentTravelTime = 1.0f;
		m_mode = PathSampler::PathMode::LERP;
	}

	void CPathAnimator::SetMode(PathSampler::PathMode mode)
	{
		m_mode = mode;
		m_segmentIndex = 0;
	}

	void CPathAnimator::Update(const PathSampler::KeypointSet& keypoints, float deltaTime)
	{
		// TODO: Implement this function
		// Steps to follow:

		// Make sure we have points to use
		
			// Increment our t value
			m_segmentTimer += deltaTime;

			// Handle switching segments each time t = 1
			if (m_segmentTimer >= m_segmentTravelTime) {
				if (m_segmentIndex + 1 < keypoints.size()) m_segmentIndex++;
				else m_segmentIndex = 0;
				m_segmentTimer = 0.f;
			}

			// Work out t using segment timer and segment travel time

			

			// Ensure we have at least 2 points to LERP along
			// Need at least 2 points for 1 segment

			// Determine the indices of the points to LERP along (in our vector of key points)

			// Perform LERP using function in PathSampler
		if (m_segmentIndex + 1 < keypoints.size()) m_owner ->transform.m_pos = PathSampler::LERP(keypoints[m_segmentIndex]->transform.m_pos, keypoints[m_segmentIndex + 1]->transform.m_pos, m_segmentTimer);
		else if (m_segmentIndex + 1 == keypoints.size()) m_owner->transform.m_pos = PathSampler::LERP(keypoints[m_segmentIndex]->transform.m_pos, keypoints[0]->transform.m_pos, m_segmentTimer);
	}
}