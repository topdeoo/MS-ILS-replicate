#include "graph.hpp"
#include "type.hpp"
#include <utility>

using namespace capmds;

u32 Graph::vertex_nums() const {
    return n_;
}

u32 Graph::edge_nums() const {
    return m_;
}

const set<u32>& Graph::vertices() const {
    return vertices_;
}

void Graph::add_vertex( u32 v ) {
    n_++;
    vertices_.insert( v );
    isolated_vertices_.insert( v );
}

void Graph::remove_vertex( u32 v ) {
    n_--;
    vertices_.erase( v );
    isolated_vertices_.erase( v );
    auto neighbors = neighbors_.at( v );
    for ( auto neighbor : neighbors ) {
        neighbors_.at( neighbor ).erase( v );
    }
    neighbors_.erase( v );
}

bool Graph::has_vertex( u32 v ) const {
    return vertices_.find( v ) != vertices_.end();
}

std::pair<bool, u32> Graph::isolate_vertex() {
    auto result = std::make_pair( false, 0 );
    if ( isolated_vertices_.size() == 0 ) {
        return result;
    }
    result.first = true;
    result.second = *isolated_vertices_.begin();
    return result;
}

std::pair<bool, u32> Graph::leaf_vertex() {
    auto result = std::make_pair( false, 0 );
    for ( auto v : vertices_ ) {
        if ( degree( v ) == 1 ) {
            result.first = true;
            result.second = v;
            return result;
        }
    }
    return result;
}

u32 Graph::degree( u32 v ) const {
    return neighbors_.at( v ).size();
}

// TODO can be optimized
u32 Graph::get_lower_degree_vertex() const {
    u32 min_degree = 0;
    u32 min_degree_vertex = 0;
    for ( auto v : vertices_ ) {
        if ( min_degree > degree( v ) ) {
            min_degree = degree( v );
            min_degree_vertex = v;
        }
    }
    return min_degree_vertex;
}

void Graph::add_edge( u32 u, u32 v ) {
    m_++;
    neighbors_[u].insert( v );
    neighbors_[v].insert( u );
    isolated_vertices_.erase( u );
    isolated_vertices_.erase( v );
}

void Graph::remove_edge( u32 u, u32 v ) {
    m_--;
    neighbors_[u].erase( v );
    neighbors_[v].erase( u );
    if ( neighbors_[u].size() == 0 ) {
        isolated_vertices_.insert( u );
    }
    if ( neighbors_[v].size() == 0 ) {
        isolated_vertices_.insert( v );
    }
}

bool Graph::is_neighbor( u32 u, u32 v ) const {
    return neighbors_.at( u ).find( v ) != neighbors_.at( u ).end();
}

const set<u32>& Graph::get_neighbors( u32 v ) {
    return neighbors_.at( v );
}
