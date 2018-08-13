from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['LOOP1'] = ["#pragma unroll","#pragma clang loop id(myloop)","#pragma nounroll"]
        #space['loop2'] = ["#pragma clang loop fuse", "#pragma clang loop peel"]
        #space['loop3'] = ["#pragma clang loop split", "#pragma clang loop distribute"]
        #space['loop4'] = ["#pragma clang loop specialize", "#pragma clang loop permute"]
        self.space = space
        self.params = self.space.keys()
        self.starting_point = ["#pragma unroll"]

if __name__ == '__main__':
    instance = Problem()
    print(instance.space)
    print(instance.params)
    print(instance.starting_point)
