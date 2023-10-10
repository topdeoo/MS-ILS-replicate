#include "capmds.hpp"
#include <fstream>

int main() {
    std::ifstream fin;
    fin.open( "test_graph/fig1.txt" );
    capmds::Solver solver;
    solver.init( fin );

    return 0;
}
