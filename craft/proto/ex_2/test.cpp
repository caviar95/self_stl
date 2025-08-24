#include <iostream>
#include "status.pb.h"

int test() {
  Status s = FAILED;
  std::cout << "Status is: " << Status_Name(s) << std::endl;
  return 0;
}