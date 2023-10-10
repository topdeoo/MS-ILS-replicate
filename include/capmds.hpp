#pragma once

#include <fstream>
#include <random>
#include <range/v3/utility/random.hpp>
#ifndef __CAPMDS_HPP__
#define __CAPMDS_HPP__

#include <range/v3/view/for_each.hpp>

#include "graph.hpp"
#include "type.hpp"

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
    set<u32> best_solution_;    // best solution
    map<u32, u32> ref_counts_;  // ref_counts_[i] indicates the number of dominating nodes of node i
    map<u32, u32> node_capacities_; // node_capacities_[i] indicates the capacity of node i

    std::random_device rd_ { "hw" };
    std::uniform_real_distribution<double> d_ { 0, 1 };

public:
    u32 get_capacity( u32 v ) const;
    std::vector<u32>& get_node_capacities( std::vector<u32>& nodes ) const;
    const u64 get_sum_of_v_neighbor_capacities( u32 v );

    u32 random_select( const std::vector<u32>& nodes ) const;
    u32 random_select( const set<u32>& nodes ) const;

    void dominate( u32 leader, u32 worker );

public:
    void H_MECU();
    void dominating_node( Graph& current_graph );
    void dominated_node( Graph& current_graph, const u32 v );
    void redundant_removal();

    double random_double();
    u32 calculate_lower_bound();
};

}; // namespace capmds

#endif // __CAPMDS_HPP__