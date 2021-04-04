/*
 * $ cd build
 * $ CXX=g++ cmake ../src
 * $ cmake --build .
 *
 */
#include "GeneticAlgorithm/MainProcess.h"
#include "GeneticAlgorithm/Multithreading.h"
#include "Dataset.h"
#include <random>
#include <iostream>

using namespace GeneticAlgorithm;
using namespace std;

/**
 * 打印数据集
 *
 * @return void
 */
void printDataset() {
    auto d = Dataset::getInstance();
    for (unsigned long i = 0; i < d->getTotal(); i++) {
        cout << i << "," << d->getInput(i) << "," << d->getOutput(i) << endl;
    }
}

/**
 * 测试程序添加正弦计算的效果
 *
 * @return void
 */
void testProgramAddSin() {
    auto factor = ChromosomeFactory();
    auto chromosome = factor.buildRandomChromosome(100, 12.0L, 22.0L);
    chromosome->dump();
    cout << "Fitness=" << chromosome->getFitness() << endl;
    delete chromosome;
}

/**
 * 运行 GEP
 *
 * @return void
 */
void experiment() {
    try {
        MainProcess mainProcess = MainProcess();
        mainProcess.setDebug(true);
        mainProcess.run(
            500, // 种群大小
            80, // 染色体长度
            -2.0L, // 初始范围
            2.0L, // 初始范围
            10000, // 最大迭代次数
            9.0L, // 停止迭代适应度
            250, // 每次迭代保留多少个上一代的高适应度个体
            0.01L // 变异概率，随便变动范围系数
        );
    } catch (const char* message) {
        cout << message << endl;
    }
}

int main()
{
    using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
    random_device randomSeed;
    GlobalCppRandomEngine::engine.seed(randomSeed());
    //printDataset();
    //testProgramAddSin();
    experiment();
    return 0;
}
