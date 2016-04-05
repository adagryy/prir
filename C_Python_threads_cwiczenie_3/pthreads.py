import threading
import time
import sys
# import numpy as np
from numpy import *

#########################################################
# WARNING!!!!!!
#########################################################
# In input fileS DO NOT input matrix dimensions. 
# This will cause read errors and terminating program
# Example of correct input file with matrix:
#
# 1 0 2
# 1 3 1
#
#########################################################

N = 3 #number of threads performing calculations

threadLock = threading.Lock()
global x
x = 0
x1 = y_1 = y2 = x2 = 0
global results, first_matrix, second_matrix, to_frobenius_norm, general_sum, temp
results = first_matrix = second_matrix = temp = None
to_frobenius_norm = general_sum = 0

class myThread (threading.Thread):
	def __init__(self, threadID, name, counter):
		threading.Thread.__init__(self)
		self.threadID = threadID
		self.name = name
		self.counter = counter
	def run(self):
		print "Starting " + self.name
		for i in range(x1):
			for j in range(y2):
				global temp
				if(self.threadID == temp[i][j]):
					temp[i][j] = -1
					global first_matrix, second_matrix, y_1, general_sum, to_frobenius_norm
					multiply_matrices(first_matrix, second_matrix, i, j, y_1)
					threadLock.acquire() #we are locking this section of code by using it by different threads in the same time					
					general_sum += results[i][j]
					to_frobenius_norm += results[i][j] * results[i][j]
					threadLock.release()
		print "Exiting " + self.name

def print_time(threadName, delay, counter):
    while counter:
        time.sleep(delay)
        print "%s: %s" % (threadName, time.ctime(time.time()))
        counter -= 1

def multiply_matrices(A, B, a, d, b):
	s = 0
	for k in range(b):
		s += A[a][k]*B[k][d]
	results[a][d] = s    

first_matrix = loadtxt("first.txt") #loading input data from external file
second_matrix = loadtxt("second.txt")

if first_matrix.size == 0 or second_matrix.size == 0 or (not isinstance(N, int)) or N < 1:
	print "Incorrect input data\nProgram terminated!"
	sys.exit()	
# print first_matrix
# print second_matrix

rows_first_matrix = first_matrix.shape[0] #amount of the rows and cols of first input matrix
cols_first_matrix = first_matrix.shape[1]

# print second_matrix.size # numbers of elems in numpy array
rows_second_matrix = second_matrix.shape[0]#amount of the rows and cols of second input matrix
cols_second_matrix = second_matrix.shape[1]

x1 = rows_first_matrix
y_1 = cols_first_matrix
y2 = cols_second_matrix

# print cols_first_matrix
# print rows_second_matrix

if cols_first_matrix != rows_second_matrix:
	print "Incorrect input data\nProgram terminated!"
	sys.exit()


results_dimensions = rows_first_matrix * cols_second_matrix #amount of elems in results array, for instance 2 columns and 5 rows equals to 10 elems

threads = [] #array fo storing threads

results = zeros((rows_first_matrix, cols_second_matrix))

temp = zeros((rows_first_matrix, cols_second_matrix))

if results_dimensions <= N:
	iters = 0
	for i in range(rows_first_matrix):
		for it in range(cols_second_matrix):
			temp[i][it] = iters #this line types into temporary 2D array indices of threads which later will be calculate suitable cells in results array
			iters += 1			   #this temporary array has the same dimensions as results array
else: 							   #for instance if temp[1][0] has value 1, this mean that thread with ID 1 will calculate this cell using A and B matrices
	iters = 0
	for i in range(rows_first_matrix):
		for it in range(cols_second_matrix):
			temp[i][it] = iters #the same as above
			iters += 1
			if iters == N:
				iters = 0

# here you can see which indices in "results" are calculated by which thread by uncommenting the line under:
print temp 

#main loop creating threads
for i in range(N):
	thread = myThread(i, "Thread-" + str(i + 1), i)
	threads.append(thread)

# Start new Threads
for t in threads:
	t.start()


# Wait for all threads to complete
for t in threads:
    t.join()

print("A = ")
print first_matrix
print("B = ")
print second_matrix
print("C = ")
print results
frobenius_norm = str(sqrt(to_frobenius_norm))
print "Norma Frobeniusa wynosi: " + frobenius_norm