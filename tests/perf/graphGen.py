import matplotlib.pyplot as plt
import csv
import glob

class PerformacePlots:
    def __init__(self,file,filename):
        self.x = []
        self.y = []
        self.legends = []
        self.title = filename
        
        f = self.graphRead(file)
        self.f2g(f)
        self.CommVsEdist()

        #load a file
    def f2g(self,f):
        for row in f:
            if row[0] == "String Size" and len(self.x[0])>0:
                self.x.append(self.x[0])
                self.y.append([long(row[1])]*len(self.x[0]))
                self.legends.append(row[0])
                break
                
            
            self.x.append([long(num) for num in row[1].split(',')[:-1]])
            self.y.append([long(num) for num in row[2].split(',')[:-1]])
            self.legends.append(row[0])

    def CommVsEdist(self):
        for i in range(len(self.x)):
            plt.plot(self.x[i], self.y[i], label =self.legends[i])
        plt.legend(loc=2)
        plt.xlabel("Edit Distance Upper Bound (edits)")
        plt.ylabel("Comm Cost (bits)")
        plt.title(self.title)
        plt.grid()
        plt.show()

    def graphRead(self,file):
        CnR =[]
        for row in csv.reader(file, delimiter=';'):
            CnR.append(row)
        return CnR

if __name__=="__main__":
    for filename in glob.glob('*.txt'):
        
        with open(filename) as csvfile:
            PerformacePlots(csvfile, filename)
