#include "graph.hpp"

using namespace capmds;

u32 Graph::vertex_nums() const {
    return vertices_.size();
}

u32 Graph::edge_nums() const {
    return m_;
}

const set<u32>& Graph::vertices() const {
    return vertices_;
}

void Graph::add_vertex( u32 v ) {
    vertices_.insert( v );
    isolated_vertices_.insert( v );
    neighbors_[v] = set<u32>();
}

void Graph::remove_vertex( u32 v ) {
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
    if ( !has_vertex( v ) || neighbors_.find( v ) == neighbors_.end() ) {
        return 0;
    }
    return neighbors_.at( v ).size();
}

void Graph::add_edge( u32 u, u32 v ) {
    neighbors_[u].insert( v );
    neighbors_[v].insert( u );
    isolated_vertices_.erase( u );
    isolated_vertices_.erase( v );
}

void Graph::remove_edge( u32 u, u32 v ) {
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
