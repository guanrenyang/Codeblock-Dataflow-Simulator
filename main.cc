#include "include/common.h"
#include "include/PE.h"

int main() {
  int cycles = 100;
  PE pe;
  for (int i = 0; i < cycles; i++) {
    pe.execute_cycle();
  }

  return 0;
}
