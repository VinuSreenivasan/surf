#!/usr/bin/env python
from __future__ import print_function
import re
import os
import sys
import time
import json
import math
import os
import argparse
seed = 12345

def create_parser():
    'command line parser for keras'

    parser = argparse.ArgumentParser(add_help=True)
    group = parser.add_argument_group('required arguments')

    parser.add_argument('--p1', action='store', dest='p1',
                        nargs='?', const=2, type=int, default='2',
                        help='parameter p1 value')
    parser.add_argument('--p2', action='store', dest='p2',
                        nargs='?', const=2, type=int, default='2',
                        help='parameter p2 value')
    parser.add_argument('--p3', action='store', dest='p3',
                        nargs='?', const=2, type=int, default='2',
                        help='parameter p3 value')
    parser.add_argument("--p4", nargs='?', type=str,
                        default='a',
                        help="parameter p4 value")

    return(parser)

parser = create_parser()
cmdline_args = parser.parse_args()
param_dict = vars(cmdline_args)

p1 = param_dict['p1']
p2 = param_dict['p2']
p3 = param_dict['p3']
p4 = param_dict['p4']

if p4 == 'a':
    pval = p1*p2*p3
else:
    pval = p1+p2+p3

print('OUTPUT:%1.3f'%pval)
