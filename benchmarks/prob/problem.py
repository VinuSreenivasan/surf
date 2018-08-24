from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['LOOP1'] = ["#pragma unroll","#pragma nounroll","#pragma clang loop id(myloop)"]
        #space['LOOP2'] = ["#pragma nounroll"]
        #space['LOOP3'] = ["#pragma clang loop id(myloop)"]
        #space['LOOP4'] = ["#pragma unroll"]
        self.space = space
        self.params = self.space.keys()
        self.starting_point = ["#pragma unroll"]

if __name__ == '__main__':
    instance = Problem()
    print(instance.space)
    print(instance.params)
    print(instance.starting_point)
