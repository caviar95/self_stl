#include <iostream>
#include "status.pb.h"
#include "test.h"

#undef Status_Name
const std::string& Status_Name(Status value);

int main() {
  return test();
}
