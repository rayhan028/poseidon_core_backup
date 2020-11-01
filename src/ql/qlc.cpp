#include "queryc.hpp"
#include "graph_db.hpp"

int main(int argc, char* argv[]) {
    auto dct = p_make_ptr<dict>();
    queryc qlc(dct);

    qlc.compile("NodeScan('Person', Filter($1.Age >= 42))");
}