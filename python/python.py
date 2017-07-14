import math
print("functions:")
def fibonacci(n):
    result=[]
    a,b=0,1
    while a<n:        
        result.append(a)
        a,b=b,a+b
    return result
    
print(fibonacci(10))
print("")

print("default arguments:")
def f(a, L=[]):
    L.append(a)
    return L

print(f(1))
print(f(2))

print("*" * 80)
def f2(a, L=None):
    if L is None:
        print ("L is none")
        L=[]
    L.append(a)
    return L

print( f2(1))
print( f2(2))

print("Keyword Arguments")
def parrot(voltage, state='a stiff', action='voom', type='Norwegian Blue'):
    print("-- This parrot wouldn't", action, end=' ')
    print("if you put", voltage, "volts through it.")
    print("-- Lovely plumage, the", type)
    print("-- It's", state, "!")
    
parrot(1000)
print("")

def cheeseshop(kind, *arguments, **keywords):
    print("--Do you hava any " + kind + "?")
    print("--I'm sorry, we're out of " + kind)
    for arg in arguments:
        print(arg)
    
    print("-" * 40)
    for kw in keywords:
        print(kw + ":" + keywords[kw])
    
cheeseshop("Limburger", "It's very runny, sir.",
           "It's really very, VERY runny, sir.",
           shopkeeper="Michael Palin",
           client="John Cleese",
           sketch="Cheese Shop Sketch")

print("*"*80)
print("Arbitrary Argument Lists")
def concat(*args, sep="/"):
    """ what???
    """
    return sep.join(args)

print(concat.__doc__)
print(concat("earth", "mars", "venus"))

words = ['cat', 'window', 'defenestrate']
for w in words[:]:
    if len(w) > 6:
        words.insert(0, w)

print(words)
sep='\\'
print(sep.join(words))

nums = range(2, 50)
for i in range(2, 8):
    nums = filter(lambda x: x == i or x % i, nums)
    print(i, ":", nums)

print([nums])

vec = [-4, -2, 0, 2, 4]
for x in vec:
    if x>= 0:
        print((x, x**2))

matrix = [
    [1, 2, 3, 4],
    [5, 6, 7, 8],
    [9, 10, 11, 12],
]    
transposed = []
for i in range(4):
    transposed.append([row[i] for row in matrix])
    print(transposed)

with open("/home/sheldon/nums.txt", "r+") as f:
    for i in range(5):
            for j in range(10):
                    f.write(repr(i) + " ")
            f.write("\n")
    print(f.read())
    f.seek(2, 0)
    f.read(1)


def n_copies(str, n):
    l=2
    if len(str) < l:
        l = len(str)
    str=str[:l]
    result = ""
    for i in range(n):
        result = result + str
    return result
  
print(n_copies("adfadf", 5))
print(n_copies("a", 5))
    
def concatenate(*items):
    s = ""
    for n in items:
        s += str(n)
    return s

list=[2, 3, 6, 5]
print(concatenate(*list))
        
def print_evens(list):
    for n in list:
        if n == 237:
            break;
        
        if n%2 == 0:
            print(n)
numbers = [    
    386, 462, 47, 418, 907, 344, 236, 375, 823, 566, 597, 978, 328, 615, 953, 345, 
    399, 162, 758, 219, 918, 237, 412, 566, 826, 248, 866, 950, 626, 949, 687, 217, 
    815, 67, 104, 58, 512, 24, 892, 894, 767, 553, 81, 379, 843, 831, 445, 742, 717, 
    958,743, 527
    ]

    
color_list_1 = set(["White", "Black", "Red"])
color_list_2 = set(["Red", "Green"])

print(color_list_1.difference(color_list_2))  
  
x, y = 4, 3
result = ((x + y) **2)
print("({} + {}) ^ 2) = {}".format(x, y, result))

def distance(x1, y1, x2, y2):
    return math.sqrt((x1-x2)**2 + (y1-y2)**2)

print(distance(0,3,4,0))

import platform
import os

import threading, zipfile

class AsyncZip(threading.Thread):
    def __init__(self, infile, outfile):
        threading.Thread.__init__(self)
        self.infile = infile
        self.outfile = outfile

    def run(self):
        f = zipfile.ZipFile(self.outfile, 'w', zipfile.ZIP_DEFLATED)
        f.write(self.infile)
        f.close()
        print('Finished background zip of:', self.infile)

