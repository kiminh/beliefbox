/* -*- Mode: C++; -*- */
/* VER: $Id: Distribution.h,v 1.3 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $*/
// copyright (c) 2012 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef MAKE_MAIN

// -- Algorithms and models -- //
#include "PolicyEvaluation.h"
#include "PolicyIteration.h"
#include "ValueIteration.h"
#include "DiscretePolicy.h"
#include "Environment.h"
#include "ExplorationPolicy.h"
#include "Sarsa.h"
#include "SarsaDirichlet.h"
#include "QLearning.h"
#include "QLearningDirichlet.h"
#include "ModelBasedRL.h"
#include "SampleBasedRL.h"
#include "UCRL2.h"
#include "ModelCollectionRL.h"
#include "ContextBanditGaussian.h"
#include "ContextBandit.h"
#include "DiscreteMDPCollection.h"
#include "ContextBanditCollection.h"
#include "HQLearning.h"
#include "TdBma.h"

// -- Adversary -- //
#include "Adversary.h"

// -- General environments -- //
#include "MDPWrapper.h"

// -- Discrete environments -- //
#include "RandomMDP.h"
#include "Gridworld.h"
#include "OneDMaze.h"
#include "DiscreteChain.h"
#include "RiverSwim.h"
#include "OptimisticTask.h"
#include "InventoryManagement.h"
#include "DoubleLoop.h"

// -- Continuous environments -- //
#include "MountainCar.h"
#include "DiscretisedEnvironment.h"

// -- Randomness -- //
#include "RandomNumberFile.h"
#include "MersenneTwister.h"

// -- Usual includes -- //
#include <cstring>
#include <getopt.h>

struct EpisodeStatistics
{
    real total_reward;
    real discounted_reward;
    int steps;
    real mse;
    int n_runs;
    EpisodeStatistics()
        : total_reward(0.0),
          discounted_reward(0.0),
          steps(0),
          mse(0),
          n_runs(0)
    {

    }
};

struct Statistics
{
    std::vector<EpisodeStatistics> ep_stats;
    std::vector<real> reward;
    std::vector<int> n_runs;
};

Statistics EvaluateAlgorithm (int n_episodes,
                              uint n_steps,
                              OnlineAlgorithm<int,int>* algorithm,
                              DiscreteEnvironment* environment,
                              real gamma,
                              Adversary* adversary);
static const char* const help_text = "Usage: online_algorithms [options] algorithm environment\n\
\nOptions:\n\
    --algorithm:    {QLearning, Model, Sarsa, LSampling, USampling, UCRL, TdBma}\n\
    --environment:  {MountainCar, ContextBandit, RandomMDP, Gridworld, Chain, Optimistic, RiverSwim, Inventory, DoubleLoop}\n\
    --n_states:     number of states (usually there is no need to specify it)\n\
    --n_actions:    number of actions (usually there is no need to specify it)\n\
    --gamma:        reward discounting in [0,1]\n\
    --lambda:       eligibility trace parameter (for some algorithms)\n\
    --randomness:   environment randomness\n\
    --n_runs:       maximum number of runs\n\
    --n_episodes:   maximum number of episodes (ignored if < 0)\n\
    --n_steps:      maximum number of total steps\n\
    --adversary:    {Fixed, Random, Heuristic}\n\
    --grid_size:    number of grid intervals for discretised environments\n\
    --maze_name:    (Gridworld) file name for the maze\n\
    --pit_value:    value of falling in a pit (defaults to -1)\n\
    --goal_value:   value of reaching a goal (defaults to 1)\n\
    --step_value:   value at each time step (defaults to 0)\n\
    --epsilon:      use epsilon-greedy with randomness in [0,1]\n\
    --reward_prior: {Beta, Normal, Fixed}\n\
\n";


int main (int argc, char** argv)
{
    int n_actions = 2;
    int n_states = 5;
    real gamma = 0.9;
    real lambda = 0.9;
    real alpha = 0.1;
    real randomness = 0.01;
    real pit_value = -1.0;
    real goal_value = 1.0;
    real step_value = 0.0;
    real epsilon = 0.0;
    uint n_runs = 10;
    uint n_episodes = 1000;
    uint n_steps = 100000;
    uint grid_size = 4;
    real dirichlet_mass = 0.5;

    enum DiscreteMDPCounts::RewardFamily reward_prior = DiscreteMDPCounts::BETA;

    const char * algorithm_name = "QLearning";
    const char * environment_name = "Chain";
    const char * adversary_name = "Random";

    int max_samples = 1;
    char* maze_name = NULL;
    {
        // options
        int c;
        int digit_optind = 0;
        while (1) {
            int this_option_optind = optind ? optind : 1;
            int option_index = 0;
            static struct option long_options[] = {
                {"n_states", required_argument, 0, 0}, //0
                {"n_actions", required_argument, 0, 0}, //1
                {"gamma", required_argument, 0, 0}, //2
                {"lambda", required_argument, 0, 0}, //3
                {"n_runs", required_argument, 0, 0}, //4
                {"n_episodes", required_argument, 0, 0}, //5
                {"n_steps", required_argument, 0, 0}, //6
                {"max_samples", required_argument, 0, 0}, //7
                {"multi-sample", no_argument, 0, 0}, //8
                {"maze_name", required_argument, 0, 0}, //9
                {"epsilon", required_argument, 0, 0}, //10
                {"alpha", required_argument, 0, 0}, //11 
                {"algorithm", required_argument, 0, 0}, //12
                {"environment", required_argument, 0, 0}, //13
                {"grid_size", required_argument, 0, 0}, //14
                {"randomness", required_argument, 0, 0}, //15
                {"adversary", required_argument, 0, 0}, //16 
                {"upper_bound", no_argument, 0, 0}, //17 - deprecated
                {"reward_prior", required_argument, 0, 0}, //18
                {"goal_value", required_argument, 0, 0}, //19
                {"step_value", required_argument, 0, 0}, //20
                {"pit_value", required_argument, 0, 0}, //21
                {0, 0, 0, 0}
            };
            c = getopt_long (argc, argv, "",
                             long_options, &option_index);
            if (c == -1)
                break;

            switch (c) {
            case 0:
#if 0
                printf ("option %s (%d)", long_options[option_index].name, option_index);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
#endif
                switch (option_index) {
                case 0: n_states = atoi(optarg); break;
                case 1: n_actions = atoi(optarg); break;
                case 2: gamma = atof(optarg); break;
                case 3: lambda = atof(optarg); break;
                case 4: n_runs = atoi(optarg); break;
                case 5: n_episodes = atoi(optarg); break;
                case 6: n_steps = atoi(optarg); break;
                case 7: max_samples = atoi(optarg); break;
                case 8: printf("multi-sample not implented; ignored\n"); break;
                case 9: maze_name = optarg; break;
                case 10: epsilon = atof(optarg); break; 
                case 11: alpha = atof(optarg); break; 
                case 12: algorithm_name = optarg; break;
                case 13: environment_name = optarg; break;
                case 14: grid_size = atoi(optarg); break;
                case 15: randomness = atof(optarg); break;
                case 16: adversary_name = optarg; break;
                    //case 17: use_upper_bound = true; break;
                case 18: 
                    if (!strcmp(optarg, "Beta")) {
                        reward_prior = DiscreteMDPCounts::BETA;
                    } else if (!strcmp(optarg, "Normal")) {
                        reward_prior = DiscreteMDPCounts::NORMAL;
                    } else if (!strcmp(optarg, "Fixed")) {
                        reward_prior = DiscreteMDPCounts::FIXED;
                    } else {
                        Serror("Unknown distribution type %s\n", optarg);
                        exit(-1);
                    }
                    break;
                case 19: goal_value = atof(optarg); break;
                case 20: step_value = atof(optarg); break;
                case 21: pit_value = atof(optarg); break;
                default:
                    fprintf (stderr, "%s", help_text);
                    exit(0);
                    break;
                }
                break;
            case '0':
            case '1':
            case '2':
                if (digit_optind != 0 && digit_optind != this_option_optind)
                    printf ("digits occur in two different argv-elements.\n");
                digit_optind = this_option_optind;
                printf ("option %c\n", c);
                break;
            default:
                std::cout << help_text;
                exit (-1);
            }
        }
    
        if (optind < argc) {
            printf ("non-option ARGV-elements: ");
            while (optind < argc) {
                printf ("%s ", argv[optind++]);
                
            }
            printf ("\n");
        }
    }

    assert (n_states > 0);
    assert (n_actions > 0);
    assert (gamma >= 0 && gamma <= 1);
    assert (lambda >= 0 && lambda <= 1);
    assert (randomness >= 0 && randomness <= 1);
    assert (n_runs > 0);
    assert (n_episodes >= 0);
    assert (n_steps > 0);
    assert (grid_size > 0);


    DiscreteMDPCounts* discrete_mdp = NULL;
    
    RandomNumberGenerator* rng;
    RandomNumberGenerator* environment_rng;
    
    //RandomNumberFile random_file("/home/olethros/projects/beliefbox/dat/r1e7.bin");  
    MersenneTwisterRNG mersenne_twister_env;
    environment_rng = (RandomNumberGenerator*) &mersenne_twister_env;



    MersenneTwisterRNG mersenne_twister;
    rng = (RandomNumberGenerator*) &mersenne_twister;


    srand48(34987235);
    srand(34987235);
    setRandomSeed(34987235);
    environment_rng->manualSeed(228240153);
    rng->manualSeed(1361690241);

    std::cout << "Starting test program" << std::endl;
    
    std::cout << "Starting evaluation" << std::endl;
    // remember to use n_runs
    Statistics statistics;
    if (n_episodes > 0) {
        statistics.ep_stats.resize(n_episodes);
    }
    statistics.reward.resize(n_steps);
    statistics.n_runs.resize(n_steps);
    for (uint i=0; i<n_steps; ++i) {
        statistics.reward[i] = 0;
        statistics.n_runs[i] = 0;
    }
    for (uint run=0; run<n_runs; ++run) {
        std::cout << "Run: " << run << " - Creating environment.." << std::endl;

        MountainCar continuous_mountain_car;
        continuous_mountain_car.setRandomness(randomness);
        continuous_mountain_car.Reset();

        DiscreteEnvironment* environment = NULL;
        if (!strcmp(environment_name, "RandomMDP")) { 
            environment = new RandomMDP (n_actions,
                                         n_states,
                                         randomness,
                                         step_value,
                                         pit_value,
                                         goal_value,
                                         rng,
                                         false);
        } else if (!strcmp(environment_name, "Gridworld")) { 
            environment = new Gridworld(maze_name, randomness, pit_value, goal_value, step_value);
        } else if (!strcmp(environment_name, "ContextBandit")) { 
            environment = new ContextBandit(n_states, n_actions, rng);
        } else if (!strcmp(environment_name, "OneDMaze")) { 
            environment = new OneDMaze(n_states, rng);
        } else if (!strcmp(environment_name, "Chain")) { 
            environment = new DiscreteChain (n_states);
        } else if (!strcmp(environment_name, "Optimistic")) { 
            environment = new OptimisticTask (0.1, 0.01);
        } else if (!strcmp(environment_name, "RiverSwim")) { 
            environment = new RiverSwim();
        } else if (!strcmp(environment_name, "DoubleLoop")) { 
            environment = new DoubleLoop();
        } else if (!strcmp(environment_name, "Inventory")) { 
            int period = n_actions - 1;
            int max_items = n_states - 1;
            real demand = randomness;
            real margin = 1.1;
            environment = new InventoryManagement(period,
                                                  max_items,
                                                  demand,
                                                  margin);
        } else if (!strcmp(environment_name, "MountainCar")) { 
            environment = 
                new DiscretisedEnvironment<MountainCar> (continuous_mountain_car,
                                                         grid_size);
        } else {
            fprintf(stderr, "Uknown environment %s\n", environment_name);
        }


        // making sure the number of states & actions is correct
        n_states = environment->getNStates();
        n_actions = environment->getNActions();
        
        std::cout <<  "Created environment: " << environment_name
                  << " with " << n_states << " states, "
                  << n_actions << " actions.\n";


        Adversary* adversary = NULL;
        if (!strcmp(adversary_name, "Random")) {
            adversary = new RandomAdversary(environment->getNStates(),
                                            environment->getNActions(),
                                            gamma);
        } else if (!strcmp(adversary_name, "Heuristic")) {
            adversary = new HeuristicAdversary(environment->getNStates(),
                                               environment->getNActions(),
                                               gamma);
        } else if (!strcmp(adversary_name, "Fixed")) {
            adversary = new FixedAdversary(environment->getNStates(),
                                           environment->getNActions(),
                                           gamma);
        }

        //std::cout << "Creating exploration policy" << std::endl;
        VFExplorationPolicy* exploration_policy = NULL;
        exploration_policy = new EpsilonGreedy(n_actions, epsilon);
    
    
        //std::cout << "Creating online algorithm" << std::endl;
        OnlineAlgorithm<int, int>* algorithm = NULL;
        MDPModel* model = NULL;
        //Gridworld* g2 = gridworld;
        if (!strcmp(algorithm_name, "Oracle")) {
            algorithm = NULL;
        } else if (!strcmp(algorithm_name, "Sarsa")) { 
            algorithm = new Sarsa(n_states,
                                  n_actions,
                                  gamma,
                                  lambda,
                                  alpha,
                                  exploration_policy);
        } else if (!strcmp(algorithm_name, "QLearning")) { 
            algorithm = new QLearning(n_states,
                                      n_actions,
                                      gamma,
                                      lambda,
                                      alpha,
                                      exploration_policy);
        } else if (!strcmp(algorithm_name, "HQLearning")) { 
            algorithm = new HQLearning(
                                       4,
                                       n_states,
                                       n_actions,
                                       gamma,
                                       lambda,
                                       alpha,
                                       0.01,
                                       1.0);
        } else if (!strcmp(algorithm_name, "QLearningDirichlet")) { 
            algorithm = new QLearningDirichlet(n_states,
                                               n_actions,
                                               gamma,
                                               lambda,
                                               alpha,
                                               exploration_policy);
        } else if (!strcmp(algorithm_name, "SarsaDirichlet")) { 
            algorithm = new SarsaDirichlet(n_states,
                                           n_actions,
                                           gamma,
                                           lambda,
                                           alpha,
                                           exploration_policy);
        } else if (!strcmp(algorithm_name, "Model")) {
            discrete_mdp =  new DiscreteMDPCounts(n_states, n_actions,
                                                  dirichlet_mass,
                                                  reward_prior);
            model= (MDPModel*) discrete_mdp;
            algorithm = new ModelBasedRL(n_states,
                                         n_actions,
                                         gamma,
                                         epsilon,
                                         model,
                                         rng);
        } else if (!strcmp(algorithm_name, "UCRL")) {
            discrete_mdp =  new DiscreteMDPCounts(n_states, n_actions,
                                                  dirichlet_mass,
                                                  reward_prior);
            model= (MDPModel*) discrete_mdp;
            algorithm = new UCRL2(n_states,
                                  n_actions,
                                  gamma,
                                  discrete_mdp,
                                  rng,
								  epsilon);
        } else if (!strcmp(algorithm_name, "LSampling")) {
            discrete_mdp =  new DiscreteMDPCounts(n_states, n_actions,
                                                  dirichlet_mass,
                                                  reward_prior);
            model= (MDPModel*) discrete_mdp;
            algorithm = new SampleBasedRL(n_states,
                                          n_actions,
                                          gamma,
                                          epsilon,
                                          model,
                                          rng,
                                          max_samples,
                                          false);
        } else if (!strcmp(algorithm_name, "USampling")) {
            discrete_mdp =  new DiscreteMDPCounts(n_states, n_actions,
                                                  dirichlet_mass,
                                                  reward_prior);
            model= (MDPModel*) discrete_mdp;
            algorithm = new SampleBasedRL(n_states,
                                          n_actions,
                                          gamma,
                                          epsilon,
                                          model,
                                          rng,
                                          max_samples,
                                          true);
        } else if (!strcmp(algorithm_name, "ContextBanditGaussian")) {
            model= (MDPModel*)
                new ContextBanditGaussian(n_states,
                                          n_actions,
                                          0.5, 0.0, 1.0);
            algorithm = new ModelBasedRL(n_states,
                                         n_actions,
                                         gamma,
                                         epsilon,
                                         model,
                                         rng,
                                         false);
        } else if (!strcmp(algorithm_name, "Aggregate")) {
            model= (MDPModel*)
                new ContextBanditAggregate(false, 3, 2,
                                           n_states, 4,
                                           n_actions,
                                           0.5, 0.0, 1.0);
            algorithm = new ModelBasedRL(n_states,
                                         n_actions,
                                         gamma,
                                         epsilon,
                                         model,
                                         rng,
                                         false);
        } else if (!strcmp(algorithm_name, "Collection")) {
            DiscreteMDPCollection* collection = NULL;
            collection =  new DiscreteMDPCollection(2,
                                                    n_states,
                                                    n_actions);
            model= (MDPModel*) collection;
            
            algorithm = new ModelCollectionRL(n_states,
                                              n_actions,
                                              gamma,
                                              epsilon,
                                              collection,
                                              rng,
                                              true);
        } else if (!strcmp(algorithm_name, "ContextBanditCollection")) {
            ContextBanditCollection* collection = 
                new ContextBanditCollection(8,
                                            n_states,
                                            n_actions,
                                            0.5, 0.0, 1.0);
            model= (MDPModel*) collection;

            algorithm = new ModelBasedRL(n_states,
                                         n_actions,
                                         gamma,
                                         epsilon,
                                         collection,
                                         rng,
                                         false);

        } else if(!strcmp(algorithm_name, "TdBma")) {
            algorithm = new TdBma(n_states,
                                  n_actions,
                                  gamma,
                                  lambda,
                                  alpha,
                                  exploration_policy);
        } else {
            Serror("Unknown algorithm: %s\n", algorithm_name);
        }

        //std::cerr << "run : " << run << std::endl;
        //DiscreteMDP* actual_mdp = environment->getMDP();
        //MDPWrapper wrapper(actual_mdp);
        Statistics run_statistics = EvaluateAlgorithm(n_episodes,
                                                      n_steps,
                                                      algorithm,
                                                      environment,
                                                      gamma,
                                                      adversary);
        //delete actual_mdp;
        if (statistics.ep_stats.size() < run_statistics.ep_stats.size())  {
            statistics.ep_stats.resize(run_statistics.ep_stats.size());
        }
            
        for (uint i=0; i<run_statistics.ep_stats.size(); ++i) {
            statistics.ep_stats[i].total_reward += run_statistics.ep_stats[i].total_reward;
            statistics.ep_stats[i].discounted_reward += run_statistics.ep_stats[i].discounted_reward;
            statistics.ep_stats[i].steps += run_statistics.ep_stats[i].steps;
            statistics.ep_stats[i].mse += run_statistics.ep_stats[i].mse;
            statistics.ep_stats[i].n_runs ++;
        }
        
        for (uint i=0; i<run_statistics.reward.size(); ++i) {
            statistics.reward[i] += run_statistics.reward[i];
            statistics.n_runs[i]++;
        }
        delete adversary;
        delete model;
        delete environment;
        delete algorithm;
        delete exploration_policy;
    }
    

    for (uint i=0; i<statistics.ep_stats.size(); ++i) {
        statistics.ep_stats[i].total_reward /= (float) n_runs;
        statistics.ep_stats[i].discounted_reward /= (float) n_runs;
        statistics.ep_stats[i].steps /= n_runs;
        statistics.ep_stats[i].mse /= n_runs;
        std::cout << statistics.ep_stats[i].n_runs << " "
                  << statistics.ep_stats[i].total_reward << " "
                  << statistics.ep_stats[i].discounted_reward << " # EPISODE_RETURN"
                  << std::endl;
        std::cout << statistics.ep_stats[i].steps << " "
                  << statistics.ep_stats[i].mse << "# MSE"
                  << std::endl;
    }

    for (uint i=0; i<statistics.reward.size(); ++i) {
        statistics.reward[i] /= (float) n_runs;
        std::cout << statistics.n_runs[i] << " "
                  << statistics.reward[i] << " # INST_PAYOFF"
                  << std::endl;
        if (statistics.n_runs[i] == 0) {
            break;
        }
    }

    std::cout << "Done" << std::endl;


    
    return 0;
}

/*** Evaluate an algorithm

     n_steps: maximun number of total steps. If negative, then ignore.
     n_episodes: maximum number of episodes. Cannot be negative.
*/

Statistics EvaluateAlgorithm (int n_episodes,
                              uint n_steps,
                              OnlineAlgorithm<int, int>* algorithm,
                              DiscreteEnvironment* environment,
                              real gamma,
                              Adversary* adversary)
{
    std::cout << "# evaluating..." << environment->Name() << std::endl;
    
    Statistics statistics;
    if (n_episodes > 0) {
        statistics.ep_stats.reserve(n_episodes); 
    }
    statistics.reward.reserve(n_steps);

    FixedDiscretePolicy* oracle_policy = NULL;
    if (!algorithm) {
        const DiscreteMDP* mdp = environment->getMDP();
        ValueIteration value_iteration(mdp, gamma);
        value_iteration.ComputeStateValues(1e-9);
        oracle_policy = value_iteration.getPolicy();
        delete mdp;
    }

    real discount = 1.0;
    int current_time = 0;
    environment->Reset();

    std:: cout << "(running)" << std::endl;

    int episode = -1;
    bool action_ok = false;
    real total_reward = 0.0;
    real discounted_reward = 0.0;


    for (uint step = 0; step < n_steps; ++step) {
        if (!action_ok) {
            int state = environment->getState();
            real reward = environment->getReward();
            //printf ("%d %f # initial reward\n", state, reward);
            if (algorithm) {
                algorithm->Act(reward, state);
            } else {
                oracle_policy->Reset(state);
            }
            

            statistics.reward.resize(step + 1);
            statistics.reward[step] = reward;
            //printf("r_%d = %f\n", step, reward);
            if (episode >= 0) {
                statistics.ep_stats[episode].steps++;
                statistics.ep_stats[episode].total_reward += reward;
                statistics.ep_stats[episode].discounted_reward += discount * reward;
            }
            total_reward += reward;
            discounted_reward += discount * reward;

            //printf ("%d %d %f\n", state, action, reward);
            episode++;
            //printf ("# episode %d complete, step %d\n", episode, step);

            if (n_episodes > 0 && episode >= n_episodes) {
                logmsg ("Breaking after %d episodes,  %d steps\n", episode, step);
                break;
            } else {
                statistics.ep_stats.resize(episode + 1);
                statistics.ep_stats[episode].total_reward = 0.0;
                statistics.ep_stats[episode].discounted_reward = 0.0;
                statistics.ep_stats[episode].steps = 0;
                discount = 1.0;
                environment->Reset();
                adversary->setReward();
                if (algorithm) {
                    algorithm->Reset();
                    const Matrix& R = adversary->getRewardMatrix();
                    algorithm->setFixedRewards(R);
                } else {
                    DiscreteMDP* mdp = environment->getMDP();
                    mdp->setFixedRewards(adversary->getRewardMatrix());
                    ValueIteration value_iteration(mdp, gamma);
                    value_iteration.ComputeStateValues(1e-9);
                    //value_iteration.getStateValues().print(stdout);
                    delete oracle_policy;
                    oracle_policy = value_iteration.getPolicy();
                    oracle_policy->Reset(state);
                    delete mdp;
                }
                //printf("Reward matrix:\n");
                //adversary->getRewardMatrix().print(stdout);
                
                action_ok = true;
                current_time = 0;
            }
        }
        
        int state = environment->getState();
        real reward = adversary->getReward();

        statistics.reward.resize(step + 1);
        //printf("r_%d = %f\n", step, reward);
        statistics.reward[step] = reward;
        statistics.ep_stats[episode].steps++;
        statistics.ep_stats[episode].total_reward += reward;
        statistics.ep_stats[episode].discounted_reward += discount * reward;
        total_reward += reward;
        discounted_reward += discount * reward;

        discount *= gamma;

        int action;
        if (algorithm) {
            action = algorithm->Act(reward, state);
        } else {
            oracle_policy->Observe(reward, state);
            action = oracle_policy->SelectAction();
        }
        adversary->Observe(state, action);

        action_ok = environment->Act(action);
        if (urandom() < 1.0 - gamma) {
            action_ok = false;
        }
        current_time++;
    }
    printf(" %f %f # RUN_REWARD\n", total_reward, discounted_reward);
      
    if ((int) statistics.ep_stats.size() != n_episodes) {
        statistics.ep_stats.resize(statistics.ep_stats.size() - 1);
    }
    printf ("# Exiting after %d episodes, %d steps\n",
            episode, n_steps);

    delete oracle_policy;
    return statistics;
}

#endif
