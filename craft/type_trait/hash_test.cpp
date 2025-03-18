#include <type_traits>  
#include <unordered_map>  
#include <vector>

template<typename T, typename = void>  
struct is_hashable : std::false_type {};  

template<typename T>  
struct is_hashable<T, std::void_t<decltype(std::hash<T>{}(std::declval<T>()))>>  
    : std::true_type {};  

// Test Case  
static_assert(is_hashable<int>::value);  
static_assert(!is_hashable<std::vector<int>>::value);
