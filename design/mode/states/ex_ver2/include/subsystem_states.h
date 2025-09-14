#pragma once

#include <string>
#include <unordered_map>

enum Subsystem1State {
    S1_IDLE,
    S1_RUNNING,
    S1_ERROR,
    S1_TMP_UPDATING,
    S1_TMP_RECOVERING,
};

enum Subsystem2State {
    S2_IDLE,
    S2_ACTIVE,
    S2_ERROR,
    S2_TMP_PREPARE,
};

#define SUBSYSTEM1_STATE_LIST(M) \
    M(S1_IDLE)          \
    M(S1_RUNNING)       \
    M(S1_ERROR)         \
    M(S1_TMP_UPDATING)  \
    M(S1_TMP_RECOVERING)

#define SUBSYSTEM2_STATE_LIST(M) \
    M(S2_IDLE)          \
    M(S2_ACTIVE)        \
    M(S2_ERROR)         \
    M(S2_TMP_PREPARE)

inline std::unordered_map<std::string, int> makeSubsystem1BitMap() {
    std::unordered_map<std::string, int> m;
    int bit = 0;
    #define ENTRY(x) m[#x] = bit++;
    SUBSYSTEM1_STATE_LIST(ENTRY)
    #undef ENTRY
    return m;
}

inline std::unordered_map<std::string, int> makeSubsystem2BitMap() {
    std::unordered_map<std::string, int> m;
    int bit = 0;
    #define ENTRY(x) m[#x] = bit++;
    SUBSYSTEM2_STATE_LIST(ENTRY)
    /*
        ENTRY(S2_IDLE)
            m["S2_IDLE"] = 0;
        ENTRY(S2_ACTIVE)
            m["S2_ACTIVE"] = 1;
        ENTRY(S2_ERROR)
            m["S2_ERROR"] = 2;
        ENTRY(S2_TMP_PREPARE)
            m["S2_TMP_PREPARE"] = 3;
    */
    #undef ENTRY
    return m;
}
