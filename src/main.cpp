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
void testProgram() {
    auto factor = ChromosomeFactory();
    auto chromosome = factor.buildRandomChromosome(100, 12.0L, 22.0L);
    chromosome->dump();
    cout << endl << "Fitness=" << chromosome->getFitness() << endl;
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
            10000, // 种群大小
            9, // 染色体长度
            -100.0L, // 初始范围
            100.0L, // 初始范围
            1000, // 最大迭代次数
            0.999L, // 停止迭代适应度
            5000, // 每次迭代保留多少个上一代的高适应度个体
            0.001L // 变异概率，随便变动范围系数
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
    //testProgram();
    experiment();
    return 0;
}
