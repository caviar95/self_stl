cmake_minimum_required(VERSION 3.10)
project(ClassSpecializationDemo)

add_executable(main
    a.cpp
    b.cpp
    my_template.cpp  # 特化类定义在此
    main.cpp
)

set_target_properties(main PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)