#include <iostream>
#include <cstring>
#include <chrono>
#include "../include/RoutingTableGenerator/RoutingTableGenerator.h"
#include "../include/Algorithms/aco_omp.h"
#include "../include/Algorithms/aco_acc.h"
#include <openacc.h>

int main(int argc, char** argv) {

//        acc_device_t dev = acc_get_device_type();
//        int num = acc_get_num_devices(dev);
//        std::cout<<num<<std::endl;
//        return 0;

        int buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    // initialize the buffer to 0s
#pragma acc parallel copy(buf)
        for (int i = 0; i < 10; ++i) {
            buf[i] = 1;
        }
        if (buf[0] == 1) {
            std::cout <<"Running on CPU\n";
        } else {
            std::cout << "Running on GPU\n";
        }


//    /**
//     * arg[0] -> program
//     * arg[1] -> algorithm choice:
//     *  - basic -> no parallelization
//     *  - omp -> OpenMP
//     *  - acc -> ACC
//     *
//     * arg[2] -> number of ants
//     * arg[3] -> threads number
//     */
//    srand(0);
//
//    if(argc < 2)
//    {
//        std::cout<<"Choose algorithm version"<<std::endl;
//        return 1;
//    }
//
//    if(argc < 3)
//    {
//        std::cout<<"Choose number of ants per cycle"<<std::endl;
//        return 1;
//    }
//
//    auto graph = readGraphFromFile("../config/grid_10x10.txt");
//
//    std::shared_ptr<Algorithm> algorithm = nullptr;
//    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//
//    if(std::strcmp(argv[1], "basic") == 0){
//        algorithm = std::make_shared<ACO>(std::stoi(argv[2]), 1000, 16, 0.5, 0.5);
//    }
//    else if(std::strcmp(argv[1], "omp") == 0){
//        if( argc < 4)
//        {
//            std::cout<<"OMP Algorithm chosen - specify number of threads to be used"<<std::endl;
//            return 1;
//        }
//        algorithm = std::make_shared<ACO_OMP>(std::stoi(argv[2]), 1000, 16, 0.5, 0.5, std::stoi(argv[3]));
//    }
//    else{
//        if(std::strcmp(argv[1], "acc") == 0){
//            algorithm = std::make_shared<ACO_ACC>(std::stoi(argv[2]), 1000, 16, 0.5, 0.5);
//        }
//        else
//            std::cout << "Provided wrong algorithm version!" << std::endl;
//    }
//
//    RoutingTableGenerator routing_table_generator(graph, algorithm);
//
//    auto routing_pairs = routing_table_generator.generateRoutingTableTo(3);
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//
//    auto time_s = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
//
//    std::cout << time_s << std::endl;
//
//    return 0;
}
