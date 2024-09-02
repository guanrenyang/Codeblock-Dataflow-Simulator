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
    int cycle_idx = 0;
    while(!dfg->is_finished()) {
        // PE performs execution at the cycle,
        std::cout << "----------Start cycle " << cycle_idx << "----------" << std::endl;

        pe_array.execute_cycle();

        display(pe_array, cycle_idx);

        dfg->signal_and_update();

        cycle_idx++;
    }

    std::cout<<"Simulation finished after " <<cycle_idx << " cycles" <<std::endl;

    return 0;
}
