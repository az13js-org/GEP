/*
 * $ cd build
 * $ CXX=g++ cmake ../src
 * $ cmake --build .
 *
 */
#include "GeneticAlgorithm/MainProcess.h"
#include "GeneticAlgorithm/Multithreading.h"
#include <random>
#include <iostream>

using namespace GeneticAlgorithm;
using namespace std;

int useMainProcess() {
    try {
        MainProcess mainProcess = MainProcess();
        mainProcess.setDebug(true);
        mainProcess.run(
            300, // 种群大小
            20, // 染色体长度
            1.0L, // 初始范围
            4.0L, // 初始范围
            100, // 最大迭代次数
            0.99L, // 停止迭代适应度
            150, // 每次迭代保留多少个上一代的高适应度个体
            0.1L // 变异概率，随便变动范围系数
        );
    } catch (const char* message) {
        cout << message << endl;
    }
    return 0;
}

int useMultithreading() {
    try {
        Multithreading mainProcess = Multithreading(4);
        mainProcess.setDebug(false);
        mainProcess.run(
            300, // 种群大小
            20, // 染色体长度
            1.0L, // 初始范围
            4.0L, // 初始范围
            10, // 最大迭代次数
            0.99L, // 停止迭代适应度
            150, // 每次迭代保留多少个上一代的高适应度个体
            0.1L // 变异概率，随便变动范围系数
        );
        cout << "Max fitness=" << mainProcess.getMaxFitness() << endl;
        mainProcess.getMaxFitnessChromosome()->dump();
        cout << endl;
        for (int i = 0; i < 100; i++) {
            mainProcess.exchange();
            mainProcess.runContinue(10, 0.99L, 150, 0.1L);
            cout << i << ",Max fitness=" << mainProcess.getMaxFitness() << endl;
            mainProcess.getMaxFitnessChromosome()->dump();
            cout << endl;
        }
    } catch (const char* message) {
        cout << message << endl;
    }
    return 0;
}

int main()
{
    using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
    random_device randomSeed;
    GlobalCppRandomEngine::engine.seed(randomSeed());
    return useMainProcess();
    //return useMultithreading();
}
