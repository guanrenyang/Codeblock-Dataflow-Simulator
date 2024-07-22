#include "PEArray.h"
#include "DataFlowGraph.h"
#include <fstream>


int display(PEArray & pe_array, int cycle);

std::shared_ptr<DataFlowGraph> kmeans(PEArray & pe_array);

int main() {
    PEArray pe_array(4, 4, 6*1024*1024);

    auto dfg = kmeans(pe_array);
    
    std::ofstream outfile("output.txt");
    std::streambuf* coutBuf = std::cout.rdbuf();
    std::cout.rdbuf(outfile.rdbuf());

    display(pe_array, 0);
    // TODO: make the main loop to exit when all the instructions are completed
    int cycles = 2400;
    for (int i = 1; i <= cycles; i++) {
        // PE performs execution at the cycle,
        std::cout << "----------Start cycle " << i << "----------" << std::endl;

        pe_array.execute_cycle();

        display(pe_array, i);

        dfg->signal_and_update();
    }
    
    std::cout.rdbuf(coutBuf);
    outfile.close();
    return 0;
}
