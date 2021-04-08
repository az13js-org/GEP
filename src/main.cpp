#include "Dataset.h"
#include "Experiment/SineCells.h"
#include <iostream>

using namespace Experiment;
using namespace std;

int main()
{
    auto s = SineCells(30, -1.0L, 1.0L);
    unsigned long epochs = 10000000;
    s.fit(
        [](unsigned long i) {
            return Dataset::getInstance()->getInput(i);
        },
        [](unsigned long i) {
            return Dataset::getInstance()->getOutput(i);
        },
        Dataset::getInstance()->getTotal(),
        epochs,
        [&s, epochs](unsigned long epoch, long double mse) {
            //return;
            if (1 == epoch) {
                cout << "EPOCH,MSE" << endl;
            }
            if (epoch % 500 != 0 && epoch < epochs) {
                return;
            }
            cout << epoch << "," << mse << endl;
            //if (epoch == epochs) {
                cout << "\"";
                s.dump();
                cout << "\"" << endl;
            //}
        },
        0.001,
        0.9,
        0.999
    );
    return 0;
}
