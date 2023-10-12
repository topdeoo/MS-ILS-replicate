#include "graph.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
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
    if ( s.empty() ) {
        cout << solutionfilename << " have not run yet" << endl;
        return 0;
    }

    u32 solution_num;
    fin >> solution_num;

    set<u32> x;
    while ( !fin.eof() ) {
        u32 v, t;
        fin >> v >> t;

        if ( t > k ) {
            cout << solutionfilename << " wrong" << endl;
            return 0;
        }
        x.insert( v );

        for ( u32 j = 0; j < t; j++ ) {
            u32 u;
            fin >> u;
            if ( !graph.is_neighbor( u, v ) ) {
                cout << solutionfilename << " wrong" << endl;
                return 0;
            }
            x.insert( u );
        }
    }

    if ( x.size() == n )
        cout << solutionfilename << " correct" << endl;
    else
        cout << solutionfilename << " wrong" << endl;

    return 0;
}
