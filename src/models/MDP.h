// -*- Mode: c++ -*-
// copyright (c) 2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
// $Id: MDP.h,v 1.3 2006/11/06 23:42:32 olethros Exp cdimitrakakis $
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MDP_H
#define MDP_H

class Distribution;

#include "real.h"
#include "TransitionDistribution.h"
#include "RewardDistribution.h"



/** Abstract MDP class */
class AbstractMDP {
public:
    virtual ~AbstractMDP() {}
};


// This is just a template for MDPs
template <typename StateType, typename ActionType>
class MDP : public AbstractMDP
{
protected:
    StateType state;
    TransitionDistribution<StateType, ActionType>& transition_distribution;
    RewardDistribution<StateType, ActionType>& reward_distribution;
public:
    MDP(TransitionDistribution<StateType, ActionType>& transition_distribution_, RewardDistribution<StateType, ActionType>& reward_distribution_)
        : transition_distribution(transition_distribution_), reward_distribution(reward_distribution_) {}
  virtual ~MDP() {}
	StateType getState() const
	{
		return state;
	}
    virtual real getTransitionProbability (const StateType& s,
                                           const ActionType& a,
                                           const StateType& s2) const
    {
        return transition_distribution.pdf(s, a, s2);
    }
    virtual real getRewardProbability (const StateType& s,
                                       const ActionType& a, 
                                       real r) const
    {
        return reward_distribution.pdf(s, a, r);
    }

    virtual real getExpectedReward (const StateType& s,
                                    const ActionType& a) const
    {
        return reward_distribution.expected(s, a);
    }
	
    virtual StateType generateState(const StateType& s,
                            const ActionType& a) const
    {
        return transition_distribution.generate(s, a);
    }
    virtual real generateReward(const StateType& s,
                                const ActionType& a) const
    {
        return reward_distribution.generate(s, a);
    }
    // generate a new state given the current state and action, then set the current state to be the new state.
    virtual real Act (ActionType& a)
    {
        real r = generateReward(state, a);
        state = generateState(state, a);
        return r;
    }
    virtual StateType generateState(const ActionType& a)
    {
        return generateState(state, a);
    }
};

#endif
