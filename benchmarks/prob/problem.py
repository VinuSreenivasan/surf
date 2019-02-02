from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['LOOP1'] = ["#pragma omp parallel for"]
        space['LOOP2'] = ["#pragma omp parallel for"]
        space['LOOP3'] = ["#pragma omp simd"]
        self.space = space
        self.params = self.space.keys()
        self.starting_point = ["#pragma omp parallel for","#pragma omp parallel for", "#pragma omp simd"]

if __name__ == '__main__':
    instance = Problem()
    print(instance.space)
    print(instance.params)
    print(instance.starting_point)
