#include "capmds.hpp"
#include <fstream>

using namespace capmds;

int main() {
    std::ifstream fin( "test_graph/fig1.graph", std::ios::in );
    Solver solver;
    solver.init( fin );
    solver.solve();
    return 0;
}
