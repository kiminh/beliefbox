// -*- Mode: c++ -*-
// copyright (c) 2009 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef MAKE_MAIN

#include <cstdio>
#include <cstdlib>
#include <vector>
#include "BPSRModel.h"
#include "POMDPGridworld.h"
#include "MersenneTwister.h"

int main(void)
{
    std::vector<real> rewards(4);
    rewards[0] = -1.0;
    rewards[1] = -0.1;
    rewards[2] = 0.0;
    rewards[3] = 1.0;
    int n_actions = 4;
    real random = 0.01;
    int tree_depth = 4;
    POMDPGridworld environment("/home/dimitrak/projects/beliefbox/dat/maze1", 8, 8, n_actions, random);
    
    int n_obs = environment.getNObs();
    BPSRModel bpsr_model(n_obs, n_actions, rewards, tree_depth);
    MersenneTwisterRNG mersenne_twister;
    RandomNumberGenerator& rng = mersenne_twister;

    rng.seed();
    int T = 100;
    for (int t=0; t<T; ++t) {
        int obs = environment.getObservation();
        int action = rng.discrete_uniform(n_actions);
        environment.Act(action);
    }
}


#endif
