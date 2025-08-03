#include <iostream>
#include "status.pb.h"

#undef Status_Name
const std::string& Status_Name(Status value);

int main() {
  Status s = FAILED;
  std::cout << "Status is: " << Status_Name(s) << std::endl;
  return 0;
}
