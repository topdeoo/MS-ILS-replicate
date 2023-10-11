#include "graph.hpp"
#include <cassert>
#include <fstream>
#include <string>

using namespace std;
using namespace capmds;

int main( int argc, char* argv[] ) {
    assert( argc == 3 );
    string inputfilename( argv[1] );
    ifstream fin( inputfilename );
    assert( fin.is_open() );

    u32 n, m, k;
    fin >> n >> m >> k;

    Graph graph;

    for ( u32 i = 0; i < n; i++ ) {
        graph.add_vertex( i );
    }

    for ( u32 i = 0; i < m; i++ ) {
        u32 u, v;
        fin >> u >> v;
        graph.add_edge( u, v );
    }

    fin.close();

    vector<u32> solutions;
    string solutionfilename( argv[2] );
    fin.open( solutionfilename );
    assert( fin.is_open() );

    string s;
    fin >> s;

    u32 solution_num;
    fin >> solution_num;

    set<u32> x;
    while ( !fin.eof() ) {
        u32 v, t;
        fin >> v >> t;

        assert( t <= k );
        x.insert( v );

        for ( u32 j = 0; j < t; j++ ) {
            u32 u;
            fin >> u;
            assert( graph.is_neighbor( v, u ) );
            x.insert( u );
        }
    }

    assert( x.size() == n );

    return 0;
}
