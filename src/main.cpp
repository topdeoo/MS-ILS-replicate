#include "capmds.hpp"
#include <chrono>
#include <fmt/core.h>

using namespace capmds;

int main( int argc, char* argv[] ) {

    Solver solver;
    std::ifstream fin;
    if ( argv[1] == nullptr ) {
        fin.open( "test/test_graph/fig1.graph" );
    } else {
        fin.open( argv[1] );
    }

    solver.set_N0( 75 );
    solver.set_N1( 150 );
    solver.set_pr( 0.4 );
    solver.set_A( 5 );

    solver.init( fin );

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    solver.solve();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>( end - begin ).count() << "us"
              << std::endl;

    solver.print();
    return 0;
}
