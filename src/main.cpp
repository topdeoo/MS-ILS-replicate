#include "capmds.hpp"
#include <chrono>

// #define DEBUG

using namespace capmds;

int main( int argc, char* argv[] ) {

    Solver solver;
    std::ifstream fin;
    if ( argv[1] == nullptr ) {
        fin.open( "test/test_graph/fig1.graph" );
    } else {
        fin.open( argv[1] );
    }

    bool capacity_random = strcmp( argv[2], "-r" ) == 0;

    /* Due to the statement in paper */
    solver.set_N0( 75 );
    solver.set_N1( 150 );
    solver.set_pr( 0.4 );
    solver.set_A( 5 );

    solver.init( fin );

    if ( capacity_random ) {
        u32 frac = std::atoi( argv[3] );
        solver.set_node_capacities( frac );
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    solver.set_begin();
    solver.solve();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>( end - begin ).count() << "s"
              << std::endl;

#ifdef DEBUG
    solver.debug();
#else
    solver.print();
#endif

    return 0;
}
