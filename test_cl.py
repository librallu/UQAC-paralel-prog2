
import sys

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage : {} n".format(sys.argv[0]))
		exit(1)
	n = int(sys.argv[1])
	print(n)
	for i in range(0,n):
		l = [2*n for j in range(0,n)]
		l[i] = 0
		if i > 0 :
			l[i-1] = 1
		if i < n-1 :
			l[i+1] = 1
		print(' '.join([str(j) for j in l]))
