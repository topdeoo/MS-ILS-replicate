#include "capmds.hpp"
#include "type.hpp"
#include <chrono>
#include <utility>
#include <vector>

using namespace capmds;

void Solver::print() {
    std::cout << best_solution_.size() << std::endl;

    // for ( auto& v : best_solution_ ) {
    //     std::cout << v << " " << dominating_who_[v].size() << std::endl;
    //     for ( auto& u : dominating_who_[v] ) {
    //         std::cout << u << " ";
    //     }
    //     std::cout << std::endl;
    // }

    /* TAG for debugging */
    for ( auto& v : best_solution_ ) {
        std::cout << v << " " << best_solution_dominating_who_[v].size() << std::endl;
        for ( auto& u : best_solution_dominating_who_[v] ) {
            std::cout << u << " ";
        }
        std::cout << std::endl;
    }
}

void Solver::debug() {
    std::cout << "number of dominating and dominated is " << dominated_vertices_.size() << std::endl;
    std::cout << "number of best solution is " << best_solution_.size() << std::endl;
}

/**
 * 1. Initialize the graph
 * 2. Initialize the capacity of each vertex
 * 3. Initialize the reference count of each vertex
 */
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

void Solver::set_node_capacities( u32 frac ) {
    auto vertices = graph_.vertices();
    for ( auto& v : vertices ) {
        node_capacities_[v] = static_cast<u32>( static_cast<fp64>( graph_.degree( v ) ) / frac );
    }
}

/**
 * @brief MS-ILS : Input Graph G = (V , E) ---> graph_
 * 1. compute LB2 ---> calculate_lower_bound()
 * 2. i = 0 ---> iter = 0
 * 3. best = V ---> best_solution_ = V
 * 4. begin iteration-1
 * 5. H_MECU() get current solution ---> H_MECU( graph_ )
 * 6. not_improved = 0 (NOTE: here should update the best solution)
 * 7. begin iteration-2
 * 8. Perturb() get current graph and a new solution which store in candidate_solution_ ---> Perturb(
 * current_solution_ ) (NOTE: should store the current_solution_ before perturb)
 * 9. H_MECU() get a new current solution ---> H_MECU( current_graph )
 * 10. update the current_solution_ , the best_solution_ and not_improved
 * 11. end iteration-2
 * 12. iter += 1
 * 13. end iteration-1
 */

void Solver::solve() {
    u32 LB2 = calculate_lower_bound();
    u32 iter = 0;
    best_solution_ = graph_.vertices();
    while ( ( iter < N0_ ) && ( best_solution_.size() > LB2 ) ) {
        H_MECU( graph_ );
        if ( current_solution_.size() < best_solution_.size() ) {
            best_solution_ = current_solution_;
            /* TAG for debugging */
            best_solution_dominating_who_ = dominating_who_;
        }
        u32 not_improved = 0;
        while ( ( not_improved < N1_ ) && ( best_solution_.size() > LB2 ) ) {
            auto previos_solution = current_solution_;
            Graph current_graph = Perturb();
            current_solution_ = candidate_solution_;
            H_MECU( current_graph );
            if ( current_solution_.size() < previos_solution.size() ) {
                if ( current_solution_.size() < best_solution_.size() ) {
                    std::cout << "Can improve the best solution" << std::endl;
                    best_solution_ = current_solution_;
                    /* TAG for debugging */
                    best_solution_dominating_who_ = dominating_who_;
                }
                not_improved = 0;
            } else {
                current_solution_ = previos_solution;
                not_improved += 1;
            }
        }
        current_solution_.clear();
        dominated_vertices_.clear();
        dominating_who_.clear();
        iter += 1;
    }
}

fp64 Solver::random_double() {
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

/**
 * when leader dominates worker, do the following things:
 * 1. add worker to the dominating_who_[leader]
 * 2. decrease the capacity of leader
 * 3. increase the reference count of worker
 */
void Solver::dominate( u32 leader, u32 worker ) {
    dominated_vertices_.insert( worker );
    dominating_who_[leader].insert( worker );
    node_capacities_[leader] -= 1;
    ref_counts_[worker] += 1;
}

void Solver::remove_from_solution( u32 v ) {
    current_solution_.erase( v );
    ref_counts_[v] -= 1;
    if ( ref_counts_[v] < 1 ) {
        dominated_vertices_.erase( v );
    }
    for ( auto& u : dominating_who_[v] ) {
        ref_counts_[u] -= 1;
        if ( ref_counts_[u] < 1 ) {
            dominated_vertices_.erase( u );
        }
    }
    dominating_who_.erase( v );
}

/**
 * NOTE this function is not sure to be correct
 * 1. sort the vector of vertices by the min(c(v), deg(v)) of each vertex in non-increasing order
 * 2. calculate the sum of the first k vertices's min(c(v), deg(v))(i.e. ec(v))
 * 3. if the sum is greater than or equal to the number of vertices, then return k
 */

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
 * @details
 * 1. initialize the reference count of each vertex in current_solution_
 * 2. initialize the current graph Gc = (Vc , Ec )
 * 3. dominating_node( Gc )
 * 4. redundant_removal()
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

/**
 * while the current graph is not empty, do the following things:
 * 1. isolate vertex rules
 * 2. leaf vertex rules
 * 3. multiple adjacent vertices rules
 * 4. rules above select a vertex v, we add v into current_solution_ (NOTE: we should update the refence
 * count of v here since v can cover itself)
 * 5. dominated_node( current_graph, v ) to apply the dominated node rules
 */

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
            dominated_vertices_.insert( v );
        }
        // Hence v cover itself
        ref_counts_[v] += 1;
        auto need_remove = dominated_node( current_graph, v );
        for ( auto& u : need_remove ) {
            ref_counts_[u] += 1;
            current_graph.remove_vertex( u );
        }
        current_graph.remove_vertex( v );
    }
}

/**
 * @brief here we apply the greedy algorithm to select the vertices that can be dominated by v
 * 1. if the capacity of v is greater than or equal to the degree of v, then we can dominate all the
 * neighbors of v
 * 2. else we sort the neighbors of v by the degree of each vertex(NOTE: in Gc) in non-decreasing order,
 * and we select the first k(NOTE: k indicates the capacity of vertex v) vertices to dominate
 * 3. we should delete v and vertice who are just dominated by v from Gc
 */

std::vector<u32> Solver::dominated_node( Graph& current_graph, const u32 v ) {
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

    return need_remove;
}

/**
 * 1. select a set called R whose elements staify the following conditions: v is in the current solution
 * and the reference count of v is greater than or equal to 2 and all the reference count of all
 * vertices that are dominated by v are greater than or equal to 2
 * 2. while R is not empty, do the following things:
 * 3. we have probability pr_ to select a vertex from R randomly, else we select the vertex whose degree
 * in G is minimum
 * 4. remove the vertex from current_solution_ and R
 * 5. update the reference count of the vertex and the vertices that are dominated by the vertex
 * 6. recalculate R
 */

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
            // TODO optimize this part
            std::vector<std::pair<u32, u32>> temp = std::vector<std::pair<u32, u32>>();
            for ( auto& v_i : redunant_set ) {
                temp.emplace_back( v_i, graph_.degree( v_i ) );
            }
            ranges::sort( temp, [&]( const std::pair<u32, u32>& a, const std::pair<u32, u32>& b ) {
                return a.second < b.second;
            } );
            v = temp[0].first;
        }

        redunant_set.erase( v );

        remove_from_solution( v );

        redunant_set = redundant( redunant_set );
    }
}

/**
 * 1. initialize the current graph Gc = (Vc , Ec ) and clear the candidate_solution_
 * 2. randomly select a vertex v from partial_solution(i.e. current_solution_)
 * 3. add v into candidate_solution_
 * 4. apply the dominated node rules to Gc (NOTE: the procedure just dominates the vertices but does not
 * remove the vertices from Gc)
 * 5. remove all selected vertices from the partial_solution and current graph Gc
 */

Graph Solver::Perturb() {
    Graph current_graph = graph_;
    candidate_solution_.clear();
    size_t k = static_cast<size_t>( static_cast<fp64>( current_solution_.size() ) / A_ );
    while ( candidate_solution_.size() < k ) {
        // NOTE here the paper does not mention the partial_solution can be empty
        // If the partial_solution is empty, the candidate_solution_ will not copy k
        // vertices into itself (?)
        if ( current_solution_.empty() ) {
            break;
        }
        auto v = random_select( current_solution_ );

        auto need_remove = dominated_node( current_graph, v );

        candidate_solution_.insert( v );

        for ( auto& u : need_remove ) {
            ref_counts_[u] += 1;
            current_graph.remove_vertex( u );
            if ( current_solution_.find( u ) != current_solution_.end() ) {
                remove_from_solution( u );
            }
        }
        current_graph.remove_vertex( v );
        remove_from_solution( v );
    }
    return current_graph;
}
