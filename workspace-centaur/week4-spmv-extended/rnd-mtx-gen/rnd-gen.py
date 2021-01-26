# Week 4 - SpMV Extended
# SukJoon Oh, acoustikue@yonsei.ac.kr

from scipy.sparse import random
from scipy import stats
import scipy.io
import numpy as np

max_iter = 10
spar_ratio = np.arange(0.05, 0.8, 0.05)

mat_size = 32

class CustomRandomState(np.random.RandomState):
    def randint(self, k):
        i = np.random.randint(k)
        return i - i % 2

if __name__ == "__main__":
    print("Program running: ")
    print("Target Ratio: {}".format(spar_ratio))

    for i in range(max_iter):
        for ratio in spar_ratio:
            
            # Generate
            np.random.seed(i)
            rs = CustomRandomState()

            # Samples a requested number of random values.
            rvs = stats.poisson(10, loc=5).rvs
            S = random(mat_size, mat_size, density=ratio, random_state=rs, data_rvs=rvs)

            # Export
            scipy.io.mmwrite("s%d-r%2f-mat.mtx" % (mat_size, ratio * 100), S)
            print("s%d-r%2f-mat.mtx" % (mat_size, ratio * 100))

        mat_size = mat_size * 2

