#!/usr/bin/python
import random
import subprocess
import re
import bisect
import datetime
import time
import sys
import os
import signal
tempFile = '/tmp/stamina_test'
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
parser.add_argument('-c','--deterministic',help='generate a deterministic automaton',action='store_true')
parser.add_argument('-d','--density',help='pick density',type=float,default=0)
parser.add_argument('-s',help='simple picking of transitions',action='store_true')
parser.add_argument('stamina', help='path to the Stamina executable')
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
    output = str(n) + '\n' + 'c' + '\n'
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
            chosen = random.randint(0,n-1)
            for k in range(0,n):
                if k==chosen:
                    output += str(1) + ' '
                else:
                    if args.deterministic == 1:
                        output += toform(0) + ' '
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

        s = subprocess.Popen(['time','-f "%U"','./'+args.stamina,tempFile],
                             stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        if maybeTimeout(s):
            (stamina_out,time_out)=s.communicate()
            print stamina_out
            # if(re.match(r'guessed', stamina_out)):
            #     print 'We guessed the unlimited witness'
            # if(re.match(r'good', stamina_out)):
            #     print 'The guess was not good, but we found one anyway'
            # match = re.search(r'height: (\d+)',stamina_out)
            # if(match): 
            #     starheight = match.group(1)
            #     print 'Language with starheight ' + starheight
            x1=time_out.replace('"','').strip()
            print 'Stamina took ' + x1
    
            if(args.output):
                out.write(str(i)+' '+starheight+'\n')
                out.close()
