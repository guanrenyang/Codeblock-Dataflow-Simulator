#include "PEArray.h"
#include "DataFlowGraph.h"

int display(PEArray & pe_array, int cycle);

std::shared_ptr<DataFlowGraph> vector_add(PEArray & pe_array);

int main() {
    PEArray pe_array(4, 4, 6*1024*1024);

    auto dfg = vector_add(pe_array);

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
