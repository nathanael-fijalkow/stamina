#!/usr/bin/python
import random
import subprocess
import re
import bisect
import datetime
import time
import sys
import os
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
parser.add_argument('-t', '--timeout',help='how long to wait for the processes',
                    type=int,default=0)
parser.add_argument('-o','--output',help='output to file')
parser.add_argument('-a',help='accumulative generation',action='store_true')
parser.add_argument('-r','--repeat',help='repeat the experiment x times',
                     type=int, default=1)
parser.add_argument('-d','--density',help='pick density',type=float,default=0)
parser.add_argument('-s',help='simple picking of transitions',action='store_true')
parser.add_argument('acme1', help='path to the Acme++ executable')
parser.add_argument('acme2', help='path to the AcmeML executable')
args = parser.parse_args()

def maybeTimeout(s):
    if args.timeout != 0: 
        start = datetime.datetime.now()
        while s.poll() is None:
            time.sleep(0.1)
            now = datetime.datetime.now()
            if (now - start).seconds > args.timeout:
                print('process time out')
                os.kill(s.pid,signal.SIGKILL)
                os.waitpid(-1,os.WNOHANG)
                return None
    return True


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
        if(args.s):
            pickprob = 0.5
        else:
            pickprob = random.uniform(args.density,1)
        output+=chr(ord('a')+i)+'\n'
        for j in range(0,n):
            chosen = random.randint(0,n)
            for k in range(0,n):
                if k==chosen:
                    output += str(1) + ' '
                else:
                    output += toform(bisect.bisect([pickprob,1],random.random())) + ' '
            output+='\n'
        output+='\n'
    return output

if(args.output):
    out = open(args.output,'w')
if args.a:
    fr=2
else:
    fr=args.n

for i in range(fr,args.n+1):
    for x in range(0,args.repeat):
        f = open(tempFile,'w')
        f.write(gen(i,args.m))
        f.close()

        s = subprocess.Popen(['time','-f "%U"','./'+args.acme1,tempFile],
                             stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        if maybeTimeout(s):
            (acme_out,time_out)=s.communicate()
            monsize = re.search(r'has (\d+)',acme_out).group(1)
        
            print 'We picked a monoid of size ' + monsize
            x1=time_out.replace('"','').strip()
            print 'Acme++ took ' + x1
    
            s = subprocess.Popen(['time','-f "%U"','./'+args.acme2,'-mma',tempFile,'-silent'],
                             stdout=subprocess.PIPE,stderr=subprocess.PIPE)
            if maybeTimeout(s):
                (acme_out,time_out)=s.communicate()
                if time_out.find('Stack_overflow')!=-1:     
                    print 'AcmeML had a stack overflow'
                    x2=str(-1)
                else:
                    x2=time_out.replace('"','').strip()
                    print 'AcmeML took ' + x2
                if(args.output):
                    out.write(str(i)+' '+monsize+' '+x1+' '+x2+'\n')
out.close()
