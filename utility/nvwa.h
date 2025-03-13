#pragma once

#define NVWA_CONCAT(x, y)         x##y
#define NVWA_PASTE(x, y)          NVWA_CONCAT(x, y)
#define NVWA_UNIQUE_NAME(prefix)  NVWA_PASTE(prefix, __COUNTER__)
