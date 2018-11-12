from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['LOOP1'] = ["#pragma omp parallel for private (pidx) schedule(static)","#pragma omp parallel for private (pidx) schedule(dynamic)"]
        space['LOOP2'] = ["#pragma omp parallel for private (pidx) schedule(dynamic)","#pragma omp parallel for private (pidx) schedule(static)"]
        self.space = space
        self.params = self.space.keys()
        self.starting_point = ["#pragma omp parallel for private (pidx) schedule(static)","#pragma omp parallel for private (pidx) schedule(dynamic)"]

if __name__ == '__main__':
    instance = Problem()
    print(instance.space)
    print(instance.params)
    print(instance.starting_point)
