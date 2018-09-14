Directory structure 
===================
```
benchmarks
    directory for problems
experiments
    directory for saving the running the experiments and storing the results
search
    directory for source files
```
Install instructions
====================

With anaconda do the following:

```
conda create --name ytopt -c intel intelpython3_core python=3.6
source activate ytopt
conda install h5py
conda install scikit-learn
conda install pandas
conda install mpi4py
conda install -c conda-forge keras
conda install -c conda-forge scikit-optimize
git clone https://github.com/scikit-optimize/scikit-optimize.git
cd scikit-optimize
pip install -e.
```
Usage
=====
```
cd search

usage: async-search.py [-h] [-v] [--prob_dir [PROB_DIR]] [--exp_dir [EXP_DIR]]
                       [--exp_id [EXP_ID]] [--max_evals [MAX_EVALS]]
                       [--max_time [MAX_TIME]]

optional arguments:
  -h, --help            show this help message and exit
  -v, --version         show program's version number and exit
  --prob_dir [PROB_DIR]
                        problem directory
  --exp_dir [EXP_DIR]   experiments directory
  --exp_id [EXP_ID]     experiments id
  --max_evals [MAX_EVALS]
                        maximum number of evaluations
  --max_time [MAX_TIME]
                        maximum time in secs
```
Example
=======
```
mpiexec -np 2 python async-search.py --prob_dir=../benchmarks/prob  --exp_dir=../experiments/ --exp_id=exp-01 --max_evals=10 --max_time=60 
```
How to define your own autotuning problem
=========================================
The search space and a default starting point is defined in problem.py

```
from collections import OrderedDict
class Problem():
    def __init__(self):
        space = OrderedDict()
        #problem specific parameters
        space['LOOP1'] = ["#pragma unroll","#pragma nounroll","#pragma clang loop id(myloop)"]
        self.space = space
        self.params = self.space.keys()
        self.starting_point = ["#pragma unroll"]
```
In evalaute.py, you have to define three functions.

First, define how to construct the command line in 
```
def commandLine(x, params) 
```

Second, define how to evalaute a point in
```
def evaluate(x, evalCounter, params, prob_dir, job_dir, result_dir): 
```

Third, define how to read the results in 
```
def readResults(fname, evalnum):
```

And you have a helper function in evaluate.py to replace loop marker with corresponding pragma,
```
def generate(cmd, inputfile, outputfile)
```

Finally, in job.tmpl, add the required functionality.
