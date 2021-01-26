# Week 4 - SpMV Extended
# SukJoon Oh, acoustikue@yonseia.ac.kr

from scipy.sparse import random
from scipy import stats
import scipy.io
import numpy as np

max_iter = 10
spar_ratio = 0.05

mat_size = 32

class CustomRandomState(np.random.RandomState):
    def randint(self, k):
        i = np.random.randint(k)
        return i - i % 2

    for i in range(1, max_iter):
        # Generate
        np.random.seed(1)
        rs = CustomRandomState()

        # Samples a requested number of random values.
        rvs = stats.poisson(10, loc=5).rvs
        S = random(mat_size, mat_size, density=spar_ratio, random_state=rs, data_rvs=rvs)

        # Export
        scipy.io.mmwrite("s{0}-r{1}-mat.mtx".format(), S)

