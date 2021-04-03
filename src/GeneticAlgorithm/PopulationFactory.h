#ifndef GENETICALGORITHM_POPULATIONFACTORY_H
#define GENETICALGORITHM_POPULATIONFACTORY_H

#include "PopulationFactory.h"
#include "ChromosomeFactory.h"
#include "Population.h"

namespace GeneticAlgorithm {

    /* 工厂类，生成种群
     */
    class PopulationFactory {

    public:

        /**
         * 返回种群实体
         * @param unsigned long numberOfChromosome 种群的大小，
         * @param unsigned long lengthOfChromosome 个体染色体的长度
         * @param long double min 数字区域数字最小值
         * @param long double min 数字区域数字最大值
         * @return Population*
         */
         Population* buildRandomPopulation(unsigned long numberOfChromosome, unsigned long lengthOfChromosome, long double min, long double max) {
             auto chromosomeFactory = ChromosomeFactory();
             auto population = new Population(numberOfChromosome);
             for (unsigned long i = 0; i < numberOfChromosome; i++) {
                 population->setChromosome(i, chromosomeFactory.buildRandomChromosome(lengthOfChromosome, min, max));
             }
             return population;
         }

    };

}

#endif
