from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['p1'] = (2, 10)
        space['p2'] = (8, 1024)
        space['p3'] = [2 , 4, 8, 16, 32, 64, 128]
        space['p4'] = ['a', 'b', 'c']
        self.space = space
        self.params = self.space.keys()
        self.starting_point = [10, 1000, 64, 'c']

if __name__ == '__main__':
    instance = Problem()
    print(instance.space)
    print(instance.params)

