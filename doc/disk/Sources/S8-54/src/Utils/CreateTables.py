import os
from math import *

def CalcNormal(num, numPoints):
    return 1.0

def CalcBlackmann(num, numPoints):
    alpha = 0.16
    a0 = (1.0 - alpha) / 2
    a1 = 0.5
    a2 = alpha / 2
    return a0 - a1 * cos(2 * pi * num / (numPoints - 1)) + a2 * cos(4 * 3.1415926 * num / (numPoints - 1))

def CalcHann(num, numPoints):
    return 0.5 * (1.0 - cos(2.0 * 3.1415926 * num / (numPoints - 1)))

def CalcHamming(num, numPoints):
    return 0.53836 - 0.46164 * cos(2 * 3.1415926 * num / (numPoints - 1))

def WriteTable(nameFile, nameTable, numElements, function):
    nameFile.write("static const float ")
    nameFile.write(nameTable)
    nameFile.write(str(numElements))
    nameFile.write("[")
    nameFile.write(str(numElements))
    nameFile.write("] =\n")
    nameFile.write("{\n");
    for i in range(numElements - 1):
        file.write("\t")
        file.write(str(function(i, numElements)))
        file.write("f,\n")
    file.write("\t")
    file.write(str(function(numElements - 1, numElements)))
    file.write("\n};\n\n")

##### Start here #####
file = open("TablesWindow.h", "w")

names = ["koeffsNorm", "koeffsBlack", "koeffsHann", "koeffsHamming"]
functions = [CalcNormal, CalcBlackmann, CalcHann, CalcHamming]
points = [256, 512, 1024]

for i in range(4):
    for p in points:
        WriteTable(file, names[i], p, functions[i])

file.close();


file = open("TablesLog.h", "w")

file.write("static const float Log10[10001] =\n{\n\t")

file.write(str(20.0 * log10(1e-100)))
file.write(",\n")
for i in range(9999):
    file.write("\t")
    file.write(str(20.0 * log10((i + 1) / 10000.0)))
    file.write(",\n")
file.write("\t")
file.write(str(20.0 * log10(1.0)))
file.write("\n};\n")

file.close();
