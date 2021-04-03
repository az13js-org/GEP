#ifndef GENETICALGORITHM_MAINPROCESS_H
#define GENETICALGORITHM_MAINPROCESS_H

#include "Population.h"
#include "PopulationFactory.h"
#include "Utils/GlobalCppRandomEngine.h"
#include "Chromosome.h"
#include <random>
#include <iostream>

namespace GeneticAlgorithm {

    /**
     * 算法主流程
     */
    class MainProcess {

    private:
        // 染色体or个体的数量
        unsigned long numberOfChromosome;
        // 染色体的长度
        unsigned long lengthOfChromosome;
        // 随机初始范围
        long double min;
        // 随机初始范围
        long double max;
        // 每次迭代从上一代保留多少个个体
        unsigned long keep;
        // 每次迭代上上一道销毁多少个个体，这个根据numberOfChromosome和lengthOfChromosome算出来的
        unsigned long kill;
        // 运行过程中保留当前迭代属于第几次迭代
        unsigned long loopNow;
        // 保留每次迭代算出来的最大适应度
        long double maxFitness;
        // 迭代时存储选中的染色体or个体
        Chromosome** selectedChromosome = nullptr;
        // 迭代时存储新生成的个体or染色体
        Chromosome** newChromosome = nullptr;
        // 变异概率
        long double r;
        // 存储一个Population实例
        Population* population = nullptr;
        // 是否开启调试
        bool debug = false;

    public:
        // 构造方法
        MainProcess() {
        }

        // 销毁对象时用于释放内存
        ~MainProcess() {
            this->freeMemory();
        }

        // 主流程运行
        void run(
            unsigned long numberOfChromosome,
            unsigned long lengthOfChromosome,
            long double min,
            long double max,
            unsigned long maxLoop,
            long double stopFitness,
            unsigned long keep,
            long double r
        ) {
            using namespace std;
            this->freeMemory(); // 防止重复调用run()没有释放上一次的内存
            this->numberOfChromosome = numberOfChromosome;
            this->lengthOfChromosome = lengthOfChromosome;
            this->min = min;
            this->max = max;
            this->keep = keep;
            this->kill = numberOfChromosome - keep;
            this->r = r;
            this->init();
            this->sort();
            this->maxFitness = this->population->getMaxFitnessChromosome()->getFitness();

            if (this->debug) {
                cout << "代数=0, 最大适应度=" << this->maxFitness << ", 个体信息：";
                this->population->getMaxFitnessChromosome()->dump();
            }

            while (this->loopNow < maxLoop && this->maxFitness < stopFitness) {
                this->select();
                this->crossover();
                this->mutation();
                this->generated();
                this->sort();
                this->maxFitness = this->population->getMaxFitnessChromosome()->getFitness();
                this->loopNow++;
                if (this->debug) {
                    cout << "代数=" << this->loopNow << ", 最大适应度=" << this->maxFitness << ", 个体信息：";
                    this->population->getMaxFitnessChromosome()->dump();
                }
            }
            if (this->debug) {
                cout << "结束。" << endl;
            }
        }

        // 继续运行
        void runContinue(
            unsigned long maxLoop, // 这一次的最大迭代次数
            long double stopFitness, // 达到多大的适应度就立刻停止迭代
            unsigned long keep, // 每次迭代保留多少个上一代的个体
            long double r // 基因突变的概率
        ) {
            using namespace std;
            if (nullptr == this->population || nullptr == this->selectedChromosome || nullptr == this->newChromosome) {
                return;
            }
            if (this->keep != keep) { // 之前是keep=1的话，会因为优化而不会排序
                if (1 == this->keep && keep > 1) { // 之前是keep=1的话，会因为优化而不会排序
                    this->keep = keep;
                    this->kill = this->numberOfChromosome - keep;
                    this->sort(); // 先排序避免后满淘汰掉较优解
                } else {
                    this->keep = keep;
                    this->kill = this->numberOfChromosome - keep;
                }
                // 尺寸发生变化，删除旧的再申请新空间
                delete[] this->selectedChromosome;
                delete[] this->newChromosome;
                this->selectedChromosome = new Chromosome*[2 * this->kill];
                this->newChromosome = new Chromosome*[this->kill];
            }
            this->r = r;
            unsigned long i = 0;
            this->maxFitness = this->population->getMaxFitnessChromosome()->getFitness();
            while (i < maxLoop && this->maxFitness < stopFitness) {
                this->select();
                this->crossover();
                this->mutation();
                this->generated();
                this->sort();
                this->maxFitness = this->population->getMaxFitnessChromosome()->getFitness();
                this->loopNow++;
                if (this->debug) {
                    cout << "代数=" << this->loopNow << ", 最大适应度=" << this->maxFitness << ", 个体信息：";
                    this->population->getMaxFitnessChromosome()->dump();
                }
                i++;
            }
            if (this->debug) {
                cout << "结束。" << endl;
            }
        }

        // 设置debug模式，为true的时候打印调试信息
        void setDebug(bool enableDebug) {
            this->debug = enableDebug;
        }

        // 获取迭代次数。如果在一开始初始化的那代种群就达到停止的条件，那么返回0
        unsigned long getLoopNumber() {
            return this->loopNow;
        }

        // 获取最大的适应度
        long double getMaxFitness() {
            return this->maxFitness;
        }

        // 获取Fitness最大值的Chromosome
        Chromosome* getMaxFitnessChromosome() {
            return this->population->getMaxFitnessChromosome();
        }

        // 替换一个不是最好的个体为指定的个体
        void replaceChromosome(Chromosome* chromosome) {
            Chromosome* maxChromosome = this->population->getMaxFitnessChromosome();
            for (unsigned long offset = this->numberOfChromosome - 1; offset + 2 > 1; offset--) {
                if ((void*)this->population->getChromosome(offset) != (void*)maxChromosome) {
                    this->population->replaceChromosome(offset, chromosome);
                    return;
                }
            }
        }

    private:

        // 私有，初始化
        void init() {
            this->population = PopulationFactory().buildRandomPopulation(this->numberOfChromosome, this->lengthOfChromosome, this->min, this->max);
            this->loopNow = 0;
            this->maxFitness = 0.0;
            this->selectedChromosome = new Chromosome*[2 * this->kill];
            this->newChromosome = new Chromosome*[this->kill];
        }

        // 私有，对种群中个体按照适应度大小排序
        void sort() {
            if (1 != this->keep) { // 为了优化流程，只保留一个的时候不必排序
                this->population->sort();
            }
        }

        // 私有，选择个体
        void select() {
            using namespace GeneticAlgorithm::Utils;
            using namespace std;
            Chromosome* selectChromosome1;
            Chromosome* selectChromosome2;
            unsigned long generate = 2 * this->kill;
            uniform_int_distribution<unsigned long> range(0, this->numberOfChromosome - 1);
            // 运行 generate 次选择
            for (unsigned long i = 0; i < generate; i++) {
                selectChromosome1 = this->population->getChromosome(range(GlobalCppRandomEngine::engine));
                selectChromosome2 = this->population->getChromosome(range(GlobalCppRandomEngine::engine));
                if (selectChromosome1->getFitness() > selectChromosome2->getFitness()) {
                    this->selectedChromosome[i] = selectChromosome1;
                } else {
                    this->selectedChromosome[i] = selectChromosome2;
                }
            }
        }

        // 私有，交叉运算
        void crossover() {
            for (unsigned long i = 0; i < this->kill; i++) {
                this->newChromosome[i] = this->selectedChromosome[2 * i]->crossover(this->selectedChromosome[1 + 2 * i]);
            }
        }

        // 私有，变异
        void mutation() {
            if (0 >= this->r) {
                return;
            }
            for (unsigned long i = 0; i < this->kill; i++) {
                this->newChromosome[i]->mutation(this->r);
            }
        }

        // 私有，新个体替换上一代中不需要保留的个体
        void generated() {
            if (1 != this->keep) {
                for (unsigned long i = this->keep; i < this->numberOfChromosome; i++) {
                    this->population->replaceChromosome(i, this->newChromosome[i - this->keep]);
                }
            } else {
                unsigned long replaceOffset = 0, newChromosomePoolOffset = 0;
                Chromosome* maxFitnessChromosome = this->population->getMaxFitnessChromosome();
                for (unsigned long i = 0; i < this->numberOfChromosome; i++) {
                    if ((void*)(this->population->getChromosome(replaceOffset)) != (void*)maxFitnessChromosome) {
                        this->population->replaceChromosome(replaceOffset, this->newChromosome[newChromosomePoolOffset]);
                        newChromosomePoolOffset++;
                    }
                    replaceOffset++;
                }
            }
        }

        // 私有，把上一次run中申请的内存释放
        void freeMemory() {
            if (nullptr != this->population) {
                delete this->population;
                this->population = nullptr;
            }
            // selectedChromosome 指向 Population 实例里面的染色体，Population 会删除不需要在这里单独删染色体
            if (nullptr != this->selectedChromosome) {
                delete[] this->selectedChromosome;
                this->selectedChromosome = nullptr;
            }
            // 这些染色体会替换 Population 里面的，Population 会在替换时自己删掉，不需要在这里单独删染色体
            if (nullptr != this->newChromosome) {
                delete[] this->newChromosome;
                this->newChromosome = nullptr;
            }
        }

    };

}

#endif
