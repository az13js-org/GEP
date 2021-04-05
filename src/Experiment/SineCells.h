#ifndef EXPERIMENT_SINECELLS_H
#define EXPERIMENT_SINECELLS_H

/**
 * 使用正弦函数叠加的方式拟合数据。
 *
 */
#include <random>
#include <cmath>
#include <functional>
#include <iostream>

namespace Experiment {

    /**
     * 多项正弦函数的叠加
     *
     */
    class SineCells {

    private:

        /** @var default_random_engine 提供随机数 */
        static std::default_random_engine engine;

        /** @var unsigned long 数组的大小 */
        unsigned long total;

        /** @var long double* 放大系数数组 */
        long double* k = nullptr;

        /** @var long double* 拉伸系数数组 */
        long double* a = nullptr;

        /** @var long double* 平移系数数组 */
        long double* b = nullptr;

    public:

        /**
         * 初始化
         *
         * @param unsigned long total 正弦单元数量
         * @param long double minv 训练参数随机化的最小值
         * @param long double maxv 训练参数随机化的最大值
         */
        SineCells(unsigned long total, long double minv = -1.0L, long double maxv = 1.0L) {
            std::uniform_real_distribution<long double> range(minv, maxv);
            this->total = total;
            k = new long double[total];
            a = new long double[total];
            b = new long double[total];
            for (unsigned long i = 0; i < total; i++) {
                k[i] = range(engine);
                a[i] = range(engine);
                b[i] = range(engine);
            }
        }

        /**
         * 释放内存
         */
        ~SineCells() {
            delete[] k;
            delete[] a;
            delete[] b;
        }

        /**
         * 拟合数据
         *
         * @param std::function<long double(unsigned long)> input 返回给定位置输入数据的回调函数
         * @param std::function<long double(unsigned long)> output 返回给定位置输出数据的回调函数
         * @param unsigned long sizeOfDataset 训练数据的数量
         * @param unsigned long epochs 总共在训练数据上训练多少遍，默认值为1
         * @param std::function<void(unsigned long, long double)> 每训练一遍就会执行一次的回调函数，两个参数分别是回调时训练了多少遍，还有当前训练时计算出来的MSE误差。默认值为nullptr，不产生回调。
         * @param long double lr 学习率，默认是 0.001，内置实现的学习的算法是Adam
         * @param long double beta1 Adam算法的参数，默认是 0.9
         * @param long double beta2 Adam算法的参数，默认是 0.999
         * @param long double epsilon Adam算法参数，一个很小的值，防止分母等于0。限制分母最小值等于此值。，默认值为 10的负8次方，1E-8
         * @return void
         */
        void fit(
            std::function<long double(unsigned long)> input,
            std::function<long double(unsigned long)> output,
            unsigned long sizeOfDataset,
            unsigned long epochs = 1,
            std::function<void(unsigned long, long double)> callback = nullptr,
            long double lr = 0.001,
            long double beta1 = 0.9,
            long double beta2 = 0.999,
            long double epsilon = 1E-8
        ) {
            using std::sin;
            using std::cos;
            using std::sqrt;
            long double* mk = new long double[total];
            long double* ma = new long double[total];
            long double* mb = new long double[total];
            long double* vk = new long double[total];
            long double* va = new long double[total];
            long double* vb = new long double[total];
            long double* gk = new long double[total]; // 梯度的总和
            long double* ga = new long double[total]; // 梯度的总和
            long double* gb = new long double[total]; // 梯度的总和
            long double axb, sinaxb, y, error, hx, hy, derror, tmp, errors;
            long double mhat, vhat;
            for (unsigned long i = 0; i < total; i++) {
                mk[i] = 0;
                ma[i] = 0;
                mb[i] = 0;
                gk[i] = 0;
                ga[i] = 0;
                gb[i] = 0;
            }
            for (unsigned long epoch = 0; epoch < epochs; epoch++) {
                errors = 0;
                for (unsigned long j = 0; j < total; j++) {
                    gk[j] = 0;
                    ga[j] = 0;
                    gb[j] = 0;
                }
                for (unsigned long i = 0; i < sizeOfDataset; i++) {
                    hx = input(i);
                    hy = output(i);
                    y = 0.0L;
                    for (unsigned long j = 0; j < total; j++) {
                        y += k[j] * sin(a[j] * hx + b[j]);
                    }
                    error = y - hy;
                    error = error * error / 2.0L;
                    errors += error;
                    derror = y - hy;
                    for (unsigned long j = 0; j < total; j++) {
                        axb = a[j] * hx + b[j];
                        sinaxb = sin(axb);
                        tmp = derror * k[j] * cos(axb);
                        gk[j] += derror * sinaxb;
                        ga[j] += tmp * hx;
                        gb[j] += tmp;
                    }
                }
                for (unsigned long i = 0; i < total; i++) {
                    mk[i] = beta1 * mk[i] + (1.0 - beta1) * gk[i];
                    vk[i] = beta2 * vk[i] + (1.0 - beta2) * gk[i] * gk[i];
                    mhat = mk[i] / (1.0 - beta1);
                    vhat = vk[i] / (1.0 - beta2);
                    k[i] -= (lr * mhat / (sqrt(vhat) + epsilon));

                    ma[i] = beta1 * ma[i] + (1.0 - beta1) * ga[i];
                    va[i] = beta2 * va[i] + (1.0 - beta2) * ga[i] * ga[i];
                    mhat = ma[i] / (1.0 - beta1);
                    vhat = va[i] / (1.0 - beta2);
                    a[i] -= (lr * mhat / (sqrt(vhat) + epsilon));

                    mb[i] = beta1 * mb[i] + (1.0 - beta1) * gb[i];
                    vb[i] = beta2 * vb[i] + (1.0 - beta2) * gb[i] * gb[i];
                    mhat = mb[i] / (1.0 - beta1);
                    vhat = vb[i] / (1.0 - beta2);
                    b[i] -= (lr * mhat / (sqrt(vhat) + epsilon));
                }
                if (nullptr != callback) {
                    callback(epoch + 1, errors / sizeOfDataset);
                }
            }
            delete[] mk;
            delete[] ma;
            delete[] mb;
            delete[] vk;
            delete[] va;
            delete[] vb;
            delete[] gk;
            delete[] ga;
            delete[] gb;
        }

        /**
         * 向终端打印当前表达式的内容
         *
         * @return void
         */
        void dump() {
            using namespace std;
            for (unsigned long i = 0; i < total; i++) {
                if (i > 0 && k[i] > 0) {
                    cout << "+";
                }
                cout << k[i] << "*sin(" << a[i] << "*x";
                if (b[i] > 0) {
                    cout << "+";
                }
                cout << b[i] << ")";
            }
        }

    };

    /**
     * @var std::default_random_engine SineCells::engine 定义静态随机数引擎。
     */
    std::default_random_engine SineCells::engine;

}

#endif
