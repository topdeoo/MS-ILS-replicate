#include "capmds.hpp"

#include "graph.hpp"
#include "type.hpp"
#include <algorithm>
#include <random>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/sort.hpp>

using namespace capmds;

/**
 * 1. Initialize the graph
 * 2. Initialize the capacity of each vertex
 * 3. Initialize the reference count of each vertex
 */
void Solver::init( std::ifstream& fin ) {
    u32 n, m, k;
    fin >> n >> m >> k;

    for ( auto i = 0; i <= n; i++ ) {
        graph_.add_vertex( i );
        node_capacities_.insert( { i, k } );
    }

    for ( auto i = 0; i < m; i++ ) {
        u32 u, v;
        fin >> u >> v;
        graph_.add_edge( u, v );
        graph_.add_edge( v, u );
    }
    // TODO initialize ref_count
}

double Solver::random_double() {
    return d_( rd_ );
}

void Solver::dominate( u32 leader, u32 worker ) {
    dominating_who_[leader].insert( worker );
    node_capacities_[leader] -= 1;
    // TODO update ref_count
}

// TODO: implement ref_count
void Solver::H_MECU() {

    Graph current_graph = graph_;

    dominating_node( current_graph );

    redundant_removal();
}

void Solver::dominating_node( Graph& current_graph ) {
    while ( current_graph.vertex_nums() ) {

        u32 v = 0;
        auto isolated_vertices = current_graph.isolate_vertex();
        auto leaf_vertices = current_graph.leaf_vertex();

        if ( isolated_vertices.first ) {

            auto u = isolated_vertices.second;

            if ( graph_.degree( u ) == 0 || ranges::all_of( graph_.get_neighbors( u ), [&]( auto v ) {
                     return this->current_solution_.find( v ) != this->current_solution_.end();
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

            auto neighbor = graph_.get_neighbors( leaf_vertices.second );
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
        for ( auto i = 0; i < node_capacities_[v]; ++i ) {
            dominate( v, neighbor[i] );
            need_remove.emplace_back( neighbor[i] );
        }
    }

    // TODO update reference count

    for ( auto& u : need_remove ) {
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
        // TODO implement ref_count
        redunant_set = redundant( redunant_set );
    }
}