#include <string>
#include "status.pb.h"  // 提供 Status 类型

// 替代 protobuf 的默认 Status_name 实现
const std::string& Status_Name(Status value) {
  static const std::string names[] = {"OK", "FAILED", "UNKNOWN"};
  if (value < 0 || value > 1) return names[2];
  return names[value];
}
