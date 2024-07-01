# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the main source directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the test executable
add_executable(OrderBookTest orderbook_test.cpp ${CMAKE_SOURCE_DIR}/src/orderbook.cpp)

# Link Google Test to the test executable
target_link_libraries(OrderBookTest gtest_main)

# Add tests
add_test(NAME OrderBookTest COMMAND OrderBookTest)
