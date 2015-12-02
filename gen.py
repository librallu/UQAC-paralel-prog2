import random
from sys import argv, stdout

def testGen(n,f):
    f.write("{}\n".format(n))
    for i in range(n,2*n,1):
        f.write("{} ".format(i))
    for i in range(0,n,1):
        f.write("{} ".format(i))
    f.write('\n');
    f.close()



def testGenRandom(n,f):
    f.write("{}\n".format(n))
    a = [ random.randint(-5*n,5*n) for i in range(0,n) ]
    a.sort()
    b = [ random.randint(-5*n,5*n) for i in range(0,n) ]
    b.sort()
    a.extend(b)
    for i in a:
		f.write("{} ".format(i))
    f.write('\n');
    f.close()
    
if __name__ == "__main__":
	if len(argv) >= 2:
		n = int(argv[1])
		#testGen(n, f=stdout)
		testGenRandom(n, f=stdout)
	else:
		print(len(argv))
