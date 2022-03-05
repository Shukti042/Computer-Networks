import numpy as np
from matplotlib import pyplot as plt
import sys
area=np.zeros(5)
thrput=np.zeros(5)
avgdelay=np.zeros(5)
delratio=np.zeros(5)
dropratio=np.zeros(5)
with open('connection.txt','r') as file: 
    
    # reading each line   
    j=0
    for line in file: 
        # reading each word    
        i=0    
        for word in line.split(): 
            # displaying the words            
            if i==0:
                area[j]=float(word)
            elif i==1:
                thrput[j]=float(word)
            elif i==2:
                avgdelay[j]=float(word)
            elif i==3:
                delratio[j]=float(word)
            else:
                dropratio[j]=float(word)
            i=i+1
        j=j+1
base=""
savename=""
xlabel=""
if sys.argv[1]=="0":
    xlabel="Area"
    base="Area Size (m)"
    savename="Area"
elif sys.argv[1]=="1":
    xlabel="Number of Nodes"
    base="Number of Nodes"
    savename="Nodes"
elif sys.argv[1]=="2":
    xlabel="Number of Flows"
    base="Number of Flows"
    savename="Flows"

plt.title(base+" VS Throughput (bits/sec)")
plt.xlabel(xlabel)
plt.ylabel("Throughput") 
plt.plot(area,thrput)
plt.savefig(savename+"VSThroughput.png")
plt.clf()

plt.title(base+" VS Average Delay (seconds)")
plt.xlabel(xlabel)
plt.ylabel("Average Delay") 
plt.plot(area,avgdelay)
plt.savefig(savename+"VSAvgDelay.png")
plt.clf()

plt.title(base+" VS Delivery ratio")
plt.xlabel(xlabel)
plt.ylabel("Delivery ratio") 
plt.plot(area,delratio)
plt.savefig(savename+"VSDeliveryRatio.png")
plt.clf()

plt.title(base+" VS Drop ratio")
plt.xlabel(xlabel)
plt.ylabel("Drop ratio") 
plt.plot(area,dropratio)
plt.savefig(savename+"VSDropRatio.png")
plt.clf()