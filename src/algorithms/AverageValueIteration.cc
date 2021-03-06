// -*- Mode: c++ -*-
// copyright (c) 2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
// $Id: ValueIteration.c,v 1.5 2006/11/08 17:20:17 cdimitrakakis Exp cdimitrakakis $
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "AverageValueIteration.h"
#include "real.h"
#include "MathFunctions.h"
#include "Vector.h"
#include "Random.h"
#include <cmath>
#include <cassert>

#undef RECALCULATE_P_B

AverageValueIteration::AverageValueIteration(const DiscreteMDP* mdp, bool relative, bool synchronous) : RELATIVE(relative), SYNCHRONOUS(synchronous), baseline(0)
{
    assert (mdp);
    this->mdp = mdp;
    n_actions = mdp->getNActions();
    n_states = mdp->getNStates();
    Reset();
}

void AverageValueIteration::Reset()
{
    max_iter_reached = false;
    int N = n_states * n_actions;
    V.resize(n_states);
    dV.resize(n_states);
    pV.resize(n_states);
    Q.resize(n_states);
    Q_data.resize(N);
    dQ.resize(n_states);
    dQ_data.resize(N);
    pQ.resize(n_states);
    pQ_data.resize(N);
    for (int s=0; s<n_states; s++) {
        V[s] = 0.0;
        dV[s] = 0.0;
        pV[s] = 0.0;
        Q[s] = &Q_data[s*n_actions];
        dQ[s] = &dQ_data[s*n_actions];
        pQ[s] = &pQ_data[s*n_actions];
        for (int a=0; a<n_actions; a++) {
            Q[s][a] = 0.0;
            dQ[s][a] = 0.0;
            pQ[s][a] = 0.0;
        }
    }
    p_b.resize(n_states);
    //real sum = 0.0;
    for (int s=0; s<n_states; s++) {
        p_b[s] = 1./n_states;
    }
}

AverageValueIteration::~AverageValueIteration()
{
}

void AverageValueIteration::ComputeStateValues(real threshold, int max_iter)
{
    std::vector<real> p_tmp(n_states);
    std::vector<int> a_max(n_states);
    int n_policy_changes = 0;
    real pDelta = 0.0;
    real dDelta = 0.0;
    real d2Delta = 0.0;
    Delta = 0.0;
    int stuck_count = 0;
    do {
        baseline = 0.0;
        if (RELATIVE) {
            for (int s=0; s<n_states; s++) {
                baseline += p_b[s] * V[s];
            }
        }
        //baseline = = real(n_states);
        for (int s=0; s<n_states; s++) {
            real Q_a_max = -RAND_MAX;
            int c_a_max = 0;
            for (int a=0; a<n_actions; a++) {
                real S = 0.0;
                DiscreteStateSet next = mdp->getNextStates(s, a);
                for (DiscreteStateSet::iterator i=next.begin();
                     i!=next.end();
                     ++i) {
                    int s2 = *i;
                    real P = mdp->getTransitionProbability(s, a, s2);
                    real R;
                    if (SYNCHRONOUS) {
                        R = mdp->getExpectedReward(s, a) + pV[s2] - baseline;
                    } else {
                        R = mdp->getExpectedReward(s, a) + V[s2] - baseline;
                    }
                    S += P * R;
                }
                if (a==0 || Q_a_max < S) {
                    c_a_max = a;
                    Q_a_max = S;
                }
            }
            if (c_a_max != a_max[s]) {
                a_max[s] = c_a_max;
                n_policy_changes++;
            }
            V[s] = Q_a_max;
            dV[s] = pV[s] - V[s];
            if (!SYNCHRONOUS) {
                pV[s] = V[s];
            }
        }

        if (SYNCHRONOUS) {
            for (int s=0; s<n_states; s++) {
                pV[s] = V[s];
            }
        }

#ifdef RECALCULATE_P_B
        if (RELATIVE) { 
            for (int s=0; s<n_states; s++) {
                p_tmp[s] = 0.0;
            }
            for (int s=0; s<n_states; s++) {
                // calculate new p_b
                DiscreteStateSet next = mdp->getNextStates(s, a_max[s]);
                for (DiscreteStateSet::iterator i=next.begin();
                     i!=next.end();
                     ++i) {
                    int s2 = *i;
                    real P = mdp->getTransitionProbability(s, a_max[s], s2);
                    p_tmp[s2] += p_b[s] * P;
                }
            }
            for (int s=0; s<n_states; s++) {
                p_b[s] = 1.0 / (real) n_states; //p_tmp[s];
            }
        }
#endif
        pDelta = Delta;
        Delta = Span(dV);
        if (pDelta > 0) {
            d2Delta = Delta - pDelta;
            dDelta += d2Delta;
            if (fabs(d2Delta) < 0.00001*threshold) {
                stuck_count++;
                if (stuck_count >=10) {
                    printf ("STUCK\n");
                }
            } else {
                stuck_count = 0;
            }
        }
        max_iter--;
    } while(stuck_count < 10 && Delta >= threshold && (max_iter > 0 || dDelta < 0));
    //printf ("Delta: %f, dDelta: %f, d2Delta: %f\n", Delta, dDelta, d2Delta);
    if (max_iter<=0) {
        max_iter_reached = true;
    } else {
        max_iter_reached = false;
    }

}




