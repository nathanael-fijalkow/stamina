#!/usr/bin/python
import random
import subprocess
import re
tempFile = '/tmp/acme_test'
def toform(c):
    if c==0:
        return '_'
    else:
        return str(c)

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-n', help='size of the automaton',
                    type=int,required=True)
parser.add_argument('-m', help='size of the alphabet',
                    type=int,required=True)
parser.add_argument('-o','--output',help='output to file')
parser.add_argument('-a',help='accumulative generation',action='store_true')
parser.add_argument('acme1', help='path to the Acme++ executable')
parser.add_argument('acme2', help='path to the AcmeML executable')
args = parser.parse_args()

def gen(n,m):
    output = str(n) + '\n' + 'p' + '\n'
    for i in range(0,m):
        output += chr(ord('a')+i)
    output+='\n'
    initial = random.randint(0,n-1)
    output+=str(initial) + '\n'
    l=range(0,args.n)
    l.remove(initial)
    for i in random.sample(l,random.randint(1,n-1)):
        output+=str(i)+' '

    output+='\n\n'
    for i in range(0,m):
        output+=chr(ord('a')+i)+'\n'
        for j in range(0,n):
            for k in range(0,n):
                output += toform(random.randint(0,1)) + ' '
            output+='\n'
        output+='\n'
    return output
if(args.output):
    out = open(args.output,'w')
if args.a:
    fr=2
else:
    fr=args.n

x1=None
x2=None

for i in range(fr,args.n+1):
    f = open(tempFile,'w')
    f.write(gen(i,args.m))
    f.close()

    s = subprocess.Popen(['time','-f "%U"','./'+args.acme1,tempFile],
                         stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    (acme_out,time_out)=s.communicate()
    print 'Acme++ took ' + time_out
    m=re.match(r".*(\d+).*",time_out)
    if m:
        x1=m.group(0)
    s = subprocess.Popen(['time','-f "%U"','./'+args.acme2,'-mma',tempFile,'-silent'],
                         stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    (acme_out,time_out)=s.communicate()
    print 'AcmeML took ' + time_out
    print acme_out
    m=re.match(r".*(\d+).*",time_out)
    if m:
        x2=m.group(0)
    if(args.output and x1 and x2):
        out.write(x1+' '+x2+'\n')
        x1=None
        x2=None
out.close()
