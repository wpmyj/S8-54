# Расчёт таблицы погрешностей

import os
import sys

##############################################
def CalcNormal(volt, range, divider):
    k = 2.5
    if divider:
        k = 3.0
    if volt < 50e-3:
        return k + 2 * range * 10 / volt
    return k + range * 10 / volt

################################################
def CalcMinPercents(percent):
    return percent * 0.8

##################################################
def CalcMaxPercents(percent):
    return percent * 1.5

#################################################
def CalcMinAbsVoltage(volt, percent):
    return (100 - percent) * volt / 100

#####################################################
def CalcMaxAbsVoltage(volt, percent):
    return (100 + percent) * volt / 100

#####################################################
def FormatValue(value):
    value = IncreaseValue(value)
    strng = '%f' % value
    return ('%+9s' % strng)

######################################################
def WriteAbsVoltage(volt, percent):
    file.write(FormatValue(CalcMinAbsVoltage(volt, percent)))
    file.write('-')
    file.write(FormatValue(CalcMaxAbsVoltage(volt, percent)))
    file.write('\t')

####################################################
def WriteToFile(value):
    file.write(str(value))
    file.write('\t')


#*******************************************************************************************************************************
def CalcNormHerz(herz, tBase):
    k = 1.0
    if tBase  <= 0.5e-6:
        k = 2.5
    return k + 10.0 * tBase / (1 / herz)

#*******************************************************************************************************************************
# Увеличивает значение в тысячу раз до тех пор, пока оно не станет больше или равно 0.001
def IncreaseValue(value):
    while value < 0.001:
        value = value * 1e3
    return value

#*******************************************************************************************************************************
# Уменьшает значение в тысячу раз до тех пор, пока оно не станет меньше или равно 1000
def DecreaseValue(value):
    while value > 1000:
        value = value / 1000.0
    return value

def IncreaseValueAbove1(value):
    while value < 1.0:
        value = value * 1e3
    return value

#############################################################
def CalcNormFreq(freq, time):
    return 0.02 + 100.0 / freq / time;

#############################################################
def CalcNormPeriod(period, label, numPers):
    return 0.05 + 100.0 / period / label / numPers;


volts =    [5e-3,  40e-3, 80e-3, 160e-3, 400e-3, 800e-3, 1,      4,      8,     16,    40,    8]
dividers = [False, False, False, False,  False,  False,  False,  False,  False, False, False, True]
ranges =   [2e-3,  5e-3,  10e-3, 20e-3,  50e-3,  100e-3, 200e-3, 500e-3, 1.0,   2.0,   5.0,   1.0]

count = 0;

file = open("5.2.txt", "w")

file.write("********* Table 5.2 *********\n\n")

for volt in volts:
    WriteToFile(volt)

    normPercents = CalcNormal(volt, ranges[count], dividers[count])
    WriteToFile(normPercents)

    minPercents = CalcMinPercents(normPercents)
    WriteToFile(minPercents)

    maxPercents = CalcMaxPercents(normPercents)
    WriteToFile(maxPercents)

    WriteAbsVoltage(volt, normPercents)
    WriteAbsVoltage(volt, minPercents)
    WriteAbsVoltage(volt, maxPercents)
    
    file.write('\n')

    normPercents = CalcNormal(volt * 0.707, ranges[count], dividers[count])
    WriteToFile(normPercents)
    maxPercents = CalcMaxPercents(normPercents)
    WriteToFile(maxPercents)

    WriteAbsVoltage(volt * 0.707, normPercents)
    #WriteAbsVoltage(volt * 0.707, minPercents)
    WriteAbsVoltage(volt * 0.707, maxPercents)

    count = count + 1
    
    file.write('\n\n')

file.close()

file = open("5.3_1.6.txt", "w")

file.write("********* Table 5.3 *********\n\n")

herzs =  [0.1, 1.6, 1.6e3,  25e3, 1.6e6,  16e6,  100e6, 200e6]
tbases = [5,   0.1, 0.1e-3, 5e-6, 0.1e-6, 10e-9, 2e-9,  1e-9]

count = 0

for herz in herzs:
    file.write("%6.1e\t" % herz)

    period = IncreaseValueAbove1(1.0 / herz)
    file.write("%5.1f\t" % period)

    normPercents = CalcNormHerz(herz, tbases[count])
    WriteToFile(normPercents)

    minPercents = CalcMinPercents(normPercents)
    WriteToFile(minPercents)

    maxPercents = CalcMaxPercents(normPercents)
    WriteToFile(maxPercents)

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, normPercents))
    file.write("%7.3f\t\t" % CalcMinAbsVoltage(period, normPercents))

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, minPercents))
    file.write("%7.3f\t\t" % CalcMinAbsVoltage(period, minPercents))

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, maxPercents))
    file.write("%7.3f\t" % CalcMinAbsVoltage(period, maxPercents))

    count = count + 1
    file.write('\n\n')

file.close();


file = open("5.3_1.7.txt", "w")

file.write("********* Table 5.3 *********\n\n")

herzs =  [0.1, 1.7, 1.7e3,  30e3, 1.7e6,  17e6,  100e6, 200e6]
tbases = [5,   0.1, 0.1e-3, 5e-6, 0.1e-6, 10e-9, 2e-9,  1e-9]

count = 0

for herz in herzs:
    file.write("%6.1e\t" % herz)

    period = IncreaseValueAbove1(1.0 / herz)
    file.write("%6.2f\t" % period)

    normPercents = CalcNormHerz(herz, tbases[count])
    WriteToFile(normPercents)

    minPercents = CalcMinPercents(normPercents)
    WriteToFile(minPercents)

    maxPercents = CalcMaxPercents(normPercents)
    WriteToFile(maxPercents)

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, normPercents))
    file.write("%7.3f\t\t" % CalcMinAbsVoltage(period, normPercents))

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, minPercents))
    file.write("%7.3f\t\t" % CalcMinAbsVoltage(period, minPercents))

    file.write("%7.3f-" % CalcMaxAbsVoltage(period, maxPercents))
    file.write("%7.3f\t" % CalcMinAbsVoltage(period, maxPercents))

    count = count + 1
    file.write('\n\n')

file.close();


file = open("5.4.txt", "w")

file.write("********* Table 5.4 *********\n\n")

herzs = [200e6, 10e6,  1e6,   100e3, 10e3,  1e3,   100, 10,    1,   0.1, 0.01]
times =  [0.1,  0.1,   1,     10,    10,    10,    10,  0,     0,   0,   0]
labels =  [0,   100e6, 100e6, 100e6, 100e6, 100e6, 10e6, 10e6, 1e6, 1e6, 1e6]
periods =  [0,  100,   100,   100,   100,   100,   100, 10,    1,   1,   1]


i = 0;
for herz in herzs:
    #WriteToFile(DecreaseValue(herz))

    file.write("%6.0e\t" % herz)

    if times[i] <> 0:

#        file.write("%4.1f\t" % times[i])
        
        numPers = CalcNormFreq(herz, times[i])
        maxPers = numPers * 1.5
        herz = DecreaseValue(herz)
        file.write("%5.3f\t" % numPers)
        file.write("%5.3f\t" % maxPers)

        file.write("%8.3f-" % CalcMinAbsVoltage(herz, numPers))
        file.write("%8.3f\t" % CalcMaxAbsVoltage(herz, numPers))

        file.write("%8.3f-" % CalcMinAbsVoltage(herz, maxPers))
        file.write("%8.3f\t" % CalcMaxAbsVoltage(herz, maxPers))

    if labels[i] <> 0:
        file.write("\n\t")
 #       file.write("%4.0f\t" % DecreaseValue(labels[i]))

        per = 1.0 / herzs[i]
        numPers = CalcNormPeriod(per, labels[i], periods[i])
        maxPers = numPers * 1.5
        file.write("%5.3f\t" % numPers)
        file.write("%5.3f\t" % maxPers)

        per = IncreaseValueAbove1(per)

        file.write("%9.5f-" % CalcMinAbsVoltage(per, numPers))
        file.write("%9.5f\t" % CalcMaxAbsVoltage(per, numPers))

        file.write("%9.5f-" % CalcMinAbsVoltage(per, maxPers))
        file.write("%9.5f\t" % CalcMaxAbsVoltage(per, maxPers))


    i = i + 1
    file.write('\n\n')


file.close()
