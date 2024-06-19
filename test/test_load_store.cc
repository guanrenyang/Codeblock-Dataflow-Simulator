#include "../include/common.h"
#include "../include/PEArray.h"


int main() {
    VectorRegisterFile vector_register_file;
    for (int i = 0; i < 2; ++i) {
        vector_register_file.emplace_back(VectorRegister());
    }
    std::shared_ptr<SPM> memory = std::make_shared<SPM>();
    LdInst load1(0, 0);
    StInst store(0, 1);
    LdInst load2(1, 1);
    load1.execute(vector_register_file, memory);
    std::cout<< "**Load memory at address 0 to register 0**\n";
    std::cout<< "the data in register 0 as below : \n";
    for (uint8_t num : vector_register_file[0].read_reg()) {
        std::cout << static_cast<int>(num) << " ";  // Cast to int for readable output
    }
    std::cout<< "\n\n";

    std::cout<< "**Store register 0 to memory at address 1**\n";
    store.execute(vector_register_file, memory);
    std::cout<< "**Load memory at address 1 to register 1**\n";
    load2.execute(vector_register_file, memory);
    std::cout<< "the data in register 1 as below : \n";
    for (uint8_t num : vector_register_file[1].read_reg()) {
        std::cout << static_cast<int>(num) << " ";  // Cast to int for readable output
    }
    std::cout<< std::endl;
    
    return 0;
}
