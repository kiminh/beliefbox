/* -*- Mode: C++; -*- */
/* VER: $Id: Policy.h,v 1.8 2006/10/23 08:33:24 olethros Exp cdimitrakakis $*/
// copyright (c) 2012 by Nikolaos Tziortziotis <ntziorzi@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "LSPI.h"

LSPI::LSPI(real gamma_, real Delta_, int n_dimension_, int n_actions_, int max_iteration_, RBFBasisSet* bfs_, Rollout<Vector,int,AbstractPolicy<Vector, int> >* Samples_)
:gamma(gamma_),
Delta(Delta_), 
n_dimension(n_dimension_), 
n_actions(n_actions_), 
max_iteration(max_iteration_),
bfs(bfs_), Samples(Samples_), 
policy(n_dimension, n_actions, bfs)
{
	assert(gamma>=0 && gamma <=1);
	n_basis = n_actions*(bfs->size() + 1);
	algorithm = 1;
	A.Resize(n_basis, n_basis);
	b.Resize(n_basis);
	w.Resize(n_basis);
}

LSPI::LSPI(real gamma_, real Delta_, int n_dimension_, int n_actions_, int max_iteration_, int algorithm_, RBFBasisSet* bfs_, Rollout<Vector,int,AbstractPolicy<Vector, int> >* Samples_)
	:gamma(gamma_),
	Delta(Delta_), 
	n_dimension(n_dimension_), 
	n_actions(n_actions_), 
	max_iteration(max_iteration_),
	algorithm(algorithm_),
	bfs(bfs_), Samples(Samples_), 
    policy(n_dimension, n_actions, bfs)
{
	assert(gamma>=0 && gamma <=1);
	assert(algorithm>=1 && algorithm<=2);
	n_basis = n_actions*(bfs->size() + 1);
	A.Resize(n_basis, n_basis);
	b.Resize(n_basis);
	w.Resize(n_basis);
}

LSPI::~LSPI()
{
}

Vector LSPI::BasisFunction(const Vector& state, int action)
{
	bfs->Evaluate(state);
	Vector Phi_state = bfs->F();
	Vector Phi(n_basis);
	Phi[(bfs->size() + 1)*action] = 1.0;
	
	for(int i = 0; i<bfs->size(); ++i)
	{
		Phi[(bfs->size() + 1)*action + i + 1] = Phi_state[i];
	}
	return Phi;
}

void LSPI::LSTDQ()
{
	Vector Phi_;
	Vector Phi;
	Matrix res;
    A.Clear();
	b.Clear();

	for(int i=0; i<Samples->getNRollouts(); ++i)
	{
		for(int j=0; j<Samples->getNSamples(i); ++j)
		{
			Phi_ = BasisFunction(Samples->getState(i,j), Samples->getAction(i,j));
			if(Samples->getEndsim(i,j)){
				res = OuterProduct(Phi_, Phi_);
			}
			else{
				Phi = BasisFunction(Samples->getNextState(i,j),policy.SelectAction(Samples->getNextState(i,j)));
				res = OuterProduct(Phi_,(Phi_ - (Phi*gamma)));
			}
			A += res;
			b += Phi_*Samples->getReward(i,j);
		}
	}
	const Matrix w_ = A.Inverse_LU();
	w = w_*b;
}

void LSPI::LSTDQ_OPT()
{
	Vector Phi_;
	Vector Phi;
	Vector Phi_dif;
	Matrix res;
	real d = 0.000001;
	A = Matrix::Unity(n_basis,n_basis) * (1/d);
	b.Clear();
	
	for(int i=0; i<Samples->getNRollouts(); ++i)
	{
		for(int j=0; j<Samples->getNSamples(i); ++j)
		{
			Phi_ = BasisFunction(Samples->getState(i,j), Samples->getAction(i,j));
			if(Samples->getEndsim(i,j)){
				Phi_dif = Phi_;
			}
			else{
				Phi = BasisFunction(Samples->getNextState(i,j),policy.SelectAction(Samples->getNextState(i,j)));
				Phi_dif = Phi_ - (Phi*gamma);
			}
			res = OuterProduct(Phi_,Phi_dif);
			const Matrix p = A;
			real v = Product(p*Phi_,Phi_dif);
			A -= (((A*res)*A) / (v + 1));
			b += Phi_ * Samples->getReward(i,j);
		}
	}
	const Matrix w_ = A;
	w = w_*b;
}

void LSPI::PolicyIteration()
{
	Vector old_w;
	real distance;
	int iteration = 0;
	while(1)
	{
		//Policy Evaluation
		if(algorithm == 1){
			LSTDQ();
		}
		else if(algorithm == 2){
			LSTDQ_OPT();
		}

		//Policy Improvement
		old_w = policy.getWeights();
		policy.Update(w);
		iteration++;
		
		//Stop criterion
		distance = (old_w - w).L2Norm();
//		logmsg("LSPI iteration %d, d: %f\n", iteration, distance); fflush(stdout);
		if( distance < Delta || iteration > max_iteration){
			if(distance > Delta){
				logmsg("LSPI finished after %d iterations without convergence into a fixed point\n",iteration);
			}
			else{
				logmsg("LSPI converged after %d iterations\n",iteration);
			}	
			break;
		}
	}
}

real LSPI::getValue(const Vector& state, int action)
{
	return Product(BasisFunction(state,action),w);
}

