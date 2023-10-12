#pragma once

#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include "type.hpp"
#include <queue>
#include <utility>
#include <vector>

namespace capmds {

// NOTE: assume that the graph is undirected
class Graph {
public:
    Graph() = default;
    Graph( u32 n, u32 m ) : n_( n ), m_( m ) {};
    ~Graph() = default;
    Graph( const Graph& graph ) {
        n_ = graph.n_;
        m_ = graph.m_;
        vertices_ = graph.vertices_;
        neighbors_ = graph.neighbors_;
        isolated_vertices_ = graph.isolated_vertices_;
    };
    Graph( Graph&& ) = default;
    Graph& operator=( Graph&& ) = default;

private:
    u32 n_, m_;                    // n: number of vertices, m: number of edges
    set<u32> vertices_;            // vertices_ indicates the set of vertices
    map<u32, set<u32>> neighbors_; // neighbors_[i] indicates the set of neighbors of node i
    set<u32> isolated_vertices_;   // isolated_vertices_ indicates the set of isolated vertices
    // other data structures

public:
    // function of graph
    u32 vertex_nums() const;
    u32 edge_nums() const;

    // function of vertex
    const set<u32>& vertices() const;
    void add_vertex( u32 v );
    void remove_vertex( u32 v );
    bool has_vertex( u32 v ) const;
    std::pair<bool, u32> isolate_vertex();
    std::pair<bool, u32> leaf_vertex();
    u32 degree( u32 v ) const;

    // function of edge
    void add_edge( u32 u, u32 v );
    void remove_edge( u32 u, u32 v );
    bool is_neighbor( u32 u, u32 v ) const;

    // function of neighbors
    const set<u32>& get_neighbors( u32 v );
};

}; // namespace capmds

#endif // __GRAPH_HPP__