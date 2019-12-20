import sys
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('N', type=int)
args = parser.parse_args()

X = np.random.rand(args.N, 1)
np.savetxt('X.txt', X, fmt='%.5f')

A = np.random.rand(args.N, args.N)
A = (A + A.T) / 2  # make it symmetrix
np.savetxt('A.txt', A, fmt='%.5f')

b = A@X
np.savetxt('b.txt', b, fmt='%.5f')
