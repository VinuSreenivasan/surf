from string import Template
import re
import os
import sys
import time
import json
import math
import os
import subprocess
import csv

def readResults(fname, evalnum):
    pattern1 = re.compile("START TIME:", re.IGNORECASE)
    pattern2 = re.compile("OUTPUT:", re.IGNORECASE)
    pattern3 = re.compile("END TIME:", re.IGNORECASE)
    pattern4 = re.compile("INPUT:", re.IGNORECASE)
    resDict = {}
    resDict['evalnum'] = evalnum
    resDict['startTime'] = -1
    resDict['endTime'] = -1
    resDict['cost'] = sys.float_info.max
    resDict['x'] = None
    try:
        while True:
            with open(fname, 'rt') as in_file:
                for linenum, line in enumerate(in_file):
                    if pattern1.search(line) is not None:
                        #print(line)
                        str1 = line.rstrip('\n')
                        res = re.findall('START TIME:(.*)', str1)
                        resDict['startTime'] = int(res[0])
                    elif pattern2.search(line) is not None:
                        #print(line)
                        str1 = line.rstrip('\n')
                        res = re.findall('OUTPUT:(.*)', str1)
                        rv = float(res[0])
                        if math.isnan(rv):
                            rv = sys.float_info.max
                        resDict['cost'] = rv
                    elif pattern3.search(line) is not None:
                        #print(line)
                        str1 = line.rstrip('\n')
                        res = re.findall('END TIME:(.*)', str1)
                        resDict['endTime'] = int(res[0])
                    elif pattern4.search(line) is not None:
                        #print(line)
                        #Need to process the inputs properly to create a list
                        str1 = line.rstrip('\n')
                        res = re.findall('INPUT:(.*)', str1)
                        tmpVal = res[0]
                        tmpVal = tmpVal[1:-1]
                        newVal = tmpVal.split(',')
                        newVal = [x.strip() for x in newVal]
                        resDict['x'] = newVal
                if len(resDict.keys()) == 5:
                    key = os.path.basename(fname)
                    resDict['key'] = key
                    resDict['status'] = 0
            if 'endTime' in resDict.keys():
                    break
            time.sleep(5)
    except Exception:
        print('Unexpected error:', sys.exc_info()[0])
    #print(resDict)
    return(resDict)

#Creating a dictionary using x, params 
def commandLine(x, params):
    cmd = {}
    for p, v in zip(params, x):
        cmd[p] = v
    return(cmd)

#Replace the Macros in the source file with the corresponding Pragma values
def generate(cmd, inputfile, outputfile):
    with open(inputfile, "r") as f1:
        buf = f1.readlines()
            
    with open(outputfile, "w") as f2:
        for line in buf:
            flag = 0
            for key, value in cmd.items():
                if key in line:
                    flag = 1
                    if value != "*": #For empty string options
                        f2.write(value)
                    f2.write("\n")
            if flag == 0: #To avoid writing the Marker
                f2.write(line)

def evaluate(x, evalCounter, params, prob_dir, job_dir, tmp_dir, result_dir):
    cmd = commandLine(x, params)
    sourcefile = prob_dir+'/source.c'
    interimfile = tmp_dir+'/%05d.c' % evalCounter
    generate(cmd, sourcefile, interimfile)

    jobfile = job_dir+'/%05d.job' % evalCounter
    outputfile = result_dir+'/%05d.dat' % evalCounter

    filein = open(prob_dir+'/job.tmpl')
    src = Template(filein.read())
    inpstr = str(x)
    #Storing modified source files in the directory
    tmpfile = interimfile
    tmpbinary = tmpfile[:-2]
    d = {'outputfile': outputfile, 'inpstr': inpstr, 'tmpfile': tmpfile, 'tmpbinary': tmpbinary}
    result = src.substitute(d)

    with open(jobfile, "w") as jobFile:
        jobFile.write(result)
    status = subprocess.check_output('chmod +x %s' % jobfile, shell=True)
    status = subprocess.call(' sh %s ' % jobfile, shell=True)
    resDict = readResults(outputfile, evalCounter)
    #print(resDict)

    return(resDict)
