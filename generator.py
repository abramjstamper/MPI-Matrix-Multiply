#!/usr/bin/python3
import random

outputFileName = "a.txt"
output = ""
col = 256;
row = 256;

for i in range(row):
  for j in range(col):
    output = output + str(random.randint(0,9)) + " "
  output = output + "\n"

file = open(outputFileName, "w")

file.write(output)

file.close()

print(outputFileName + " has been created")
