cmake_minimum_required(VERSION 3.9)
project(google_pizza)


SET(CMAKE_CXX_FLAGS -pthread)

set(CMAKE_CXX_STANDARD 17)


add_executable(google_pizza_greedy_solution main.cpp pizza_cutter.cpp pizza_cutter.h pizza_cutter_helper.cpp )