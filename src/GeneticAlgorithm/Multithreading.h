#ifndef GENETICALGORITHM_MULTITHREADING_H
#define GENETICALGORITHM_MULTITHREADING_H

#include "MainProcess.h"
#include "Chromosome.h"
#include "ChromosomeFactory.h"
#include "Utils/GlobalCppRandomEngine.h"
#include <thread>
#include <random>

namespace GeneticAlgorithm {

    /**
     * 多线程主流程
     */
    class Multithreading {

    public:
        // 构造方法
        Multithreading(unsigned long threadNumber) {
            if (threadNumber < 1) {
                throw "threadNumber < 1";
            }
            this->threadNumber = threadNumber;
            this->process = new MainProcess*[threadNumber];
            for (unsigned long i = 0; i < threadNumber; i++) {
                this->process[i] = new MainProcess();
            }
        }

        // 销毁对象
        ~Multithreading() {
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                delete this->process[i];
            }
            delete[] process;
        }

        // 主流程运行
        void run(
            unsigned long numberOfChromosome, // 种群中个体数量
            unsigned long lengthOfChromosome, // 每个个体的基因长度
            long double min, // 一开始初始种群时，随机数范围最小值
            long double max, // 一开始初始种群时，随机数范围最大值
            unsigned long maxLoop, // 最大迭代次数
            long double stopFitness, // 达到多大的适应度就立刻停止迭代
            unsigned long keep, // 每次迭代保留多少个上一代的个体
            long double r // 基因突变的概率
        ) {
            using namespace std;
            thread** threads = new thread*[this->threadNumber];
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                threads[i] = new thread([](
                    MainProcess* process,
                    unsigned long numberOfChromosome,
                    unsigned long lengthOfChromosome,
                    long double min,
                    long double max,
                    unsigned long maxLoop,
                    long double stopFitness,
                    unsigned long keep,
                    long double r
                ) {
                    process->run(numberOfChromosome, lengthOfChromosome, min, max, maxLoop, stopFitness, keep, r);
                }, this->process[i], numberOfChromosome, lengthOfChromosome, min, max, maxLoop, stopFitness, keep, r);
            }
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                threads[i]->join();
                delete threads[i];
            }
            delete[] threads;
        }

        // 继续运行
        void runContinue(
            unsigned long maxLoop, // 这一次的最大迭代次数
            long double stopFitness, // 达到多大的适应度就立刻停止迭代
            unsigned long keep, // 每次迭代保留多少个上一代的个体
            long double r // 基因突变的概率
        ) {
            using namespace std;
            thread** threads = new thread*[this->threadNumber];
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                threads[i] = new thread([](
                    MainProcess* process,
                    unsigned long maxLoop,
                    long double stopFitness,
                    unsigned long keep,
                    long double r
                ) {
                    process->runContinue(maxLoop, stopFitness, keep, r);
                }, this->process[i], maxLoop, stopFitness, keep, r);
            }
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                threads[i]->join();
                delete threads[i];
            }
            delete[] threads;
        }

        // 种群间最好个体随机复制性交换
        void exchange() {
            if (1 == this->threadNumber) {
                return;
            }
            ChromosomeFactory factory = ChromosomeFactory();
            Chromosome** chromosomeData = new Chromosome*[this->threadNumber];
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                chromosomeData[i] = factory.buildFromChromosome(this->process[i]->getMaxFitnessChromosome());
            }
            unsigned long select;
            Chromosome* tmp;
            for (unsigned long i = 0; i < this->threadNumber - 1; i++) {
                std::uniform_int_distribution<unsigned long> ran(i, this->threadNumber - 1);
                select = ran(Utils::GlobalCppRandomEngine::engine);
                tmp = chromosomeData[select];
                chromosomeData[select] = chromosomeData[i];
                chromosomeData[i] = tmp;
            }
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                this->process[i]->replaceChromosome(chromosomeData[i]);
            }
            delete[] chromosomeData;
        }

        // 设置debug模式，为true的时候打印调试信息
        void setDebug(bool enableDebug) {
            for (unsigned long i = 0; i < this->threadNumber; i++) {
                this->process[i]->setDebug(enableDebug);
            }
        }

        // 获取迭代次数。如果在一开始初始化的那代种群就达到停止的条件，那么返回0
        unsigned long getLoopNumber() {
            return this->process[0]->getLoopNumber();
        }

        // 获取最大的适应度
        long double getMaxFitness() {
            long double f = this->process[0]->getMaxFitness();
            long double tmp;
            for (unsigned long i = 1; i < this->threadNumber; i++) {
                tmp = this->process[i]->getMaxFitness();
                if (tmp > f) {
                    f = tmp;
                }
            }
            return f;
        }

        // 获取Fitness最大值的Chromosome
        Chromosome* getMaxFitnessChromosome() {
            auto f = this->process[0]->getMaxFitness();
            auto chromosome = this->process[0]->getMaxFitnessChromosome();
            long double tmp = 0.0;
            for (unsigned long i = 1; i < this->threadNumber; i++) {
                tmp = this->process[i]->getMaxFitness();
                if (tmp > f) {
                    f = tmp;
                    chromosome = this->process[i]->getMaxFitnessChromosome();
                }
            }
            return chromosome;
        }

    private:
        // 线程数
        unsigned long threadNumber;
        // MainProcess对象
        MainProcess** process = nullptr;
    };

}

#endif
