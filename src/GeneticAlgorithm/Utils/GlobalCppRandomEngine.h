#ifndef GENETICALGORITHM_UTILS_GLOBALCPPRANDOMENGINE_H
#define GENETICALGORITHM_UTILS_GLOBALCPPRANDOMENGINE_H

#include <random>
#include <iostream>

namespace GeneticAlgorithm::Utils {

    /* 提供使用的随机数工具类
     */
    class GlobalCppRandomEngine {

    public:
        /* 公共的静态的成员，default_random_engine类型的提供全局使用的随机数引擎。统一使用
         * 这个确保不会在同一次运行中使用重复的随机数。
         */
        static std::default_random_engine engine;

    };

    std::default_random_engine GlobalCppRandomEngine::engine;

}

#endif
