#!/usr/bin/env python3

import random

f = open('noclk.tgf', 'w')

for i in range(1, 1001):
	f.write(str(i) + " N" + str(i).zfill(4) + "\n")

f.write("#\n")

random.seed()

for i in range(501, 1001):
	# Choose 4 random numbers
	for j in range(4):
		r = random.randrange(501, 1001)
		if r == i:
			continue

		f.write(str(i) + " " + str(r) + "\n")

for i in range(1, 501):
	# Choose 4 random numbers
	for j in range(4):
		r = random.randrange(1, 501)
		if r == i:
			continue

		f.write(str(i) + " " + str(r) + "\n")

f.close()