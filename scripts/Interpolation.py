from math import *
from tkinter import *

def Function(x):
    return sin(x / (180 / 3.14)) * 250

#def DrawFunction(
root = Tk()

canv = Canvas(root, width = 500, height = 500, bg = "lightblue")
canv.create_line(250, 500, 250, 0, width = 2, arrow = LAST)
canv.create_line(0, 250, 500, 250, width = 2, arrow = LAST)

prevX = -250
prevY = Function(prevX);
for x in range(-250, 250):
    y = Function(x)
    canv.create_line(prevX + 250, prevY + 250, x + 250, y + 250)
    prevX = x
    prevY = y

canv.pack()
root.mainloop()
