#include "capmds.hpp"
#include "graph.hpp"
#include <algorithm>
#include <cstddef>
#include <fmt/core.h>
#include <math.h>
#include <range/v3/algorithm/sort.hpp>

using namespace capmds;

void Solver::print() {
    std::cout << best_solution_.size() << std::endl;
    for ( auto& v : best_solution_ ) {
        std::cout << v << " " << dominating_who_[v].size() << std::endl;
        for ( auto& u : dominating_who_[v] ) {
            std::cout << u << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * 1. Initialize the graph
 * 2. Initialize the capacity of each vertex
 * 3. Initialize the reference count of each vertex
 */
// FIXME capacity of each vertex can be variable
void Solver::init( std::ifstream& fin ) {
    u32 n, m, k;
    fin >> n >> m >> k;

    graph_ = Graph( n, m );

    // TAG assume that the graph is 0-based
    for ( u32 i = 0; i < n; i++ ) {
        graph_.add_vertex( i );
        node_capacities_.insert( { i, k } );
    }

    for ( u32 i = 0; i < m; i++ ) {
        u32 u, v;
        fin >> u >> v;
        graph_.add_edge( u, v );
        graph_.add_edge( v, u );
        ref_counts_.insert( { i, 0 } );
    }
}

void Solver::solve() {
    u32 LB2 = calculate_lower_bound();
    u32 iter = 0;
    best_solution_ = graph_.vertices();
    while ( ( iter < N0_ ) && ( best_solution_.size() > LB2 ) ) {
        H_MECU( graph_ );
        best_solution_ = current_solution_;
        u32 not_improved = 0;
        while ( ( not_improved < N1_ ) && ( best_solution_.size() > LB2 ) ) {
            auto previos_solution = current_solution_;
            Graph current_graph = Perturb( current_solution_ );
            H_MECU( current_graph );
            if ( current_solution_.size() < previos_solution.size() ) {
                if ( current_solution_.size() < best_solution_.size() ) {
                    best_solution_ = current_solution_;
                }
                not_improved = 0;
            } else {
                current_solution_ = previos_solution;
                not_improved += 1;
            }
        }
        iter += 1;
    }
}

double Solver::random_double() {
    return d_( rd_ );
}

u32 Solver::random_select( const std::vector<u32>& nodes ) const {
    std::srand( std::time( 0 ) );
    auto idx = std::rand() % nodes.size();
    return nodes[idx];
}

u32 Solver::random_select( const set<u32>& nodes ) const {
    std::srand( std::time( 0 ) );
    auto idx = std::rand() % nodes.size();
    return *std::next( nodes.begin(), idx );
}

u64 Solver::get_sum_of_v_neighbor_capacities( u32 v ) {
    u64 sum = 0;
    auto neighbor = graph_.get_neighbors( v );
    for ( auto& v_i : neighbor ) {
        sum += std::min( node_capacities_[v_i], graph_.degree( v_i ) );
    }
    return sum;
}

void Solver::dominate( u32 leader, u32 worker ) {
    dominating_who_[leader].insert( worker );
    node_capacities_[leader] -= 1;
    ref_counts_[worker] += 1;
}

u32 Solver::calculate_lower_bound() {
    auto vertices = graph_.vertices();
    auto list = std::vector<std::pair<u32, u32>>();
    for ( auto& v : vertices ) {
        list.emplace_back( v, std::min( node_capacities_[v], graph_.degree( v ) ) + 1 );
        // std::cout << v << " " << std::min( node_capacities_[v], graph_.degree( v ) ) + 1 <<
        // std::endl;
    }

    ranges::sort( list, [&]( const std::pair<u32, u32>& a, const std::pair<u32, u32>& b ) {
        return a.second > b.second;
    } );
    u32 sum = 0, result = 0;
    for ( auto& [v, ec] : list ) {
        if ( sum >= vertices.size() ) {
            break;
        }
        sum += ec;
        result += 1;
    }
    return result;
}

/**
 * Input:
 * A partial solution S ----> current_solution_
 * input graph G = (V , E) -----> graph_
 * the current graph Gc = (Vc , Ec ) ----> which_graph
 */
void Solver::H_MECU( Graph& which_graph ) {

    Graph current_graph = which_graph;

    ref_counts_.clear();
    for ( auto& v : current_solution_ ) {
        ref_counts_.insert( { v, dominating_who_[v].size() } );
    }

    dominating_node( current_graph );

    redundant_removal();

    /* Debugging */
    // std::cout << "current solution size is " << current_solution_.size() << std::endl;
}

void Solver::dominating_node( Graph& current_graph ) {
    while ( current_graph.vertex_nums() ) {

        u32 v = 0;
        auto isolated_vertices = current_graph.isolate_vertex();
        auto leaf_vertices = current_graph.leaf_vertex();

        if ( isolated_vertices.first ) {

            auto u = isolated_vertices.second;

            if ( graph_.degree( u ) == 0 || ranges::all_of( graph_.get_neighbors( u ), [&]( auto v ) {
                     return current_solution_.find( v ) != current_solution_.end();
                 } ) ) {
                current_solution_.insert( u );
                v = u;

            } else {

                u32 maxn = 0;
                auto neighbor = graph_.get_neighbors( u );

                for ( auto& v_i : neighbor ) {
                    u32 tmp = 0;
                    if ( maxn < ( tmp = std::min( node_capacities_[v_i], graph_.degree( v_i ) ) ) ) {
                        maxn = tmp;
                        v = v_i;
                    }
                }

                current_graph.add_vertex( v );
                neighbor = graph_.get_neighbors( v );

                for ( auto& v_i : neighbor ) {
                    if ( current_graph.has_vertex( v_i ) ) {
                        current_graph.add_edge( v, v_i );
                        current_graph.add_edge( v_i, v );
                    }
                }

                continue;
            }

        } else if ( leaf_vertices.first ) {

            auto neighbor = current_graph.get_neighbors( leaf_vertices.second );
            v = random_select( neighbor );
            current_solution_.insert( v );

        } else {

            u32 EC_max = 0, N_EC_min = UINT32_MAX;

            auto N_EC = [&]( u32 u ) {
                u64 sum = 0;
                auto neighbor = current_graph.get_neighbors( u );
                for ( auto& v_i : neighbor ) {
                    sum += std::min( node_capacities_[v_i], graph_.degree( v_i ) );
                }
                return sum;
            };

            for ( auto& u : current_graph.vertices() ) {

                auto EC_u = std::min( node_capacities_[u], graph_.degree( u ) );

                if ( EC_u > EC_max ) {
                    v = u, EC_max = EC_u;
                } else if ( EC_u == EC_max ) {
                    auto N_EC_u = N_EC( u );
                    if ( N_EC_u < N_EC_min ) {
                        v = u, N_EC_min = N_EC_u;
                    }
                }
            }

            current_solution_.insert( v );
        }
        // Hence v cover itself
        ref_counts_[v] += 1;
        dominated_node( current_graph, v );
    }
}

void Solver::dominated_node( Graph& current_graph, const u32 v ) {
    auto need_remove = std::vector<u32>();

    if ( node_capacities_[v] >= current_graph.degree( v ) ) {
        for ( auto& v_j : current_graph.get_neighbors( v ) ) {
            dominate( v, v_j );
            need_remove.emplace_back( v_j );
        }
    } else {
        auto neighbor = std::vector<u32>( current_graph.get_neighbors( v ).begin(),
                                          current_graph.get_neighbors( v ).end() );
        ranges::sort( neighbor, [&]( const u32& a, const u32& b ) {
            return current_graph.degree( a ) < current_graph.degree( b );
        } );
        const u32 k = node_capacities_[v];
        for ( u32 i = 0; i < k; ++i ) {
            dominate( v, neighbor[i] );
            need_remove.emplace_back( neighbor[i] );
        }
    }

    for ( auto& u : need_remove ) {
        ref_counts_[u] += 1;
        current_graph.remove_vertex( u );
    }
    current_graph.remove_vertex( v );
}

void Solver::redundant_removal() {
    auto redundant = [&]( const set<u32>& belongs_to ) {
        auto redundant_set = set<u32>();
        for ( auto& v : belongs_to ) {
            if ( ref_counts_[v] >= 2 && ranges::all_of( dominating_who_[v], [&]( u32 u ) {
                     return ref_counts_[u] >= 2;
                 } ) ) {
                redundant_set.insert( v );
            }
        }
        return redundant_set;
    };

    auto redunant_set = redundant( current_solution_ );

    while ( !redunant_set.empty() ) {
        auto u_0 = random_double();
        u32 v = 0;
        if ( u_0 <= pr_ ) {
            v = random_select( redunant_set );
        } else {
            v = graph_.get_lower_degree_vertex();
        }
        current_solution_.erase( v );
        redunant_set.erase( v );

        // Hence v cover itself
        ref_counts_[v] -= 1;
        for ( auto& u : dominating_who_[v] ) {
            ref_counts_[u] -= 1;
        }

        redunant_set = redundant( redunant_set );
    }
}

Graph Solver::Perturb( set<u32>& partial_solution ) {
    Graph current_graph = graph_;
    candidate_solution_.clear();
    size_t k = ( partial_solution.size() + 1 ) / A_;
    while ( candidate_solution_.size() < k ) {
        auto v = random_select( partial_solution );
        candidate_solution_.insert( v );
        partial_solution.erase( v );
        for ( auto& u : graph_.get_neighbors( v ) ) {
            partial_solution.erase( u );
        }
        dominated_node( current_graph, v );
    }
    return current_graph;
}
