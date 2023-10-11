#pragma once

#ifndef __CAPMDS_HPP__
#define __CAPMDS_HPP__

#include <fstream>
#include <random>
#include <range/v3/utility/random.hpp>
#include <range/v3/view/for_each.hpp>

#include "graph.hpp"
#include "type.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/sort.hpp>

namespace capmds {

class Solver {
public:
    Solver() = default;

    Solver( int n, int m, u32 capacity );

    void init( std::ifstream& fin );

    void solve();

    void print();

private:
    u32 N0_;    // N0_ indicates the total iteration number
    u32 N1_;    // N1_ indicates the number of imporving iterations
    u32 A_;     // A_ indicates how many nodes can be copy to the next generation
    double pr_; // probability of random select

    Graph graph_;
    u32 capacity_;                // capacity of each vertex
    set<u32> dominated_vertices_; // dominated_vertices_ indicates the set of dominated vertices
    map<u32, set<u32>>
      dominating_who_;          // dominating_who_[i] indicates the set of vertices that dominate node i
    set<u32> current_solution_; // current solution
    set<u32> candidate_solution_;   // candidate solution for perturb
    set<u32> best_solution_;        // best solution
    map<u32, u32> ref_counts_;      // ref_counts_[i] indicates the number of dominating nodes of node i
    map<u32, u32> node_capacities_; // node_capacities_[i] indicates the capacity of node i

    std::random_device rd_ { "hw" };
    std::uniform_real_distribution<double> d_ { 0, 1 };

public:
    void set_N0( u32 N0 ) { N0_ = N0; }
    void set_N1( u32 N1 ) { N1_ = N1; }
    void set_A( u32 A ) { A_ = A; }
    void set_pr( double pr ) { pr_ = pr; }

public:
    u32 get_capacity( u32 v ) const;
    std::vector<u32>& get_node_capacities( std::vector<u32>& nodes ) const;
    u64 get_sum_of_v_neighbor_capacities( u32 v );

    u32 random_select( const std::vector<u32>& nodes ) const;
    u32 random_select( const set<u32>& nodes ) const;
    double random_double();

    void dominate( u32 leader, u32 worker );

public:
    u32 calculate_lower_bound();

    void H_MECU( Graph& which_graph );
    void dominating_node( Graph& current_graph );
    void dominated_node( Graph& current_graph, const u32 v );
    void redundant_removal();

    Graph Perturb( set<u32>& partial_solution );
};

}; // namespace capmds

#endif // __CAPMDS_HPP__