import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import csv
import glob


class PerformacePlots:
    def __init__(self,file,filename):
        self.x = []
        self.y = []
        self.z = []
        self.legends = []
        self.title = []
        
        f = self.graphRead(file)
        self.f2g(f)
        self.CommVsEdist()

        #load a file
    def f2g(self,f):
        for row in f:
            ind = len(self.title)-1
            if row[0] == "Label":
                self.title.append(row[1])
                self.legends.append(row[2])
                self.x.append([])
                self.y.append([])
                self.z.append([])
            elif row[0]=="X":
                self.x[ind].extend([float(num) for num in row[1].split(',')[:-1]])
            elif row[0]=="Y":
                self.y[ind].extend([float(num) for num in row[1].split(',')[:-1]])
            elif row[0]=="Z":
                self.z[ind].extend([float(num) for num in row[1].split(',')[:-1]])

    def CommVsEdist(self):
        for t_name in set(self.title):
            fig = plt.figure(t_name)
            for i, title in enumerate(self.title):
                if t_name==title and len(self.z[i])==0:
                    plt.plot(self.x[i], self.y[i],'.', label=self.legends[i])
                    plt.legend(loc=2)
                    plt.xlabel("Edit Distance Upper Bound (edits)")
                    plt.ylabel(t_name)
                elif t_name==title and len(self.z[i])>0:
                    ax = fig.add_subplot(111, projection='3d')
                    ax.plot(self.x[i], self.y[i], self.z[i],'.', label=self.legends[i])
                    ax.legend(loc=2)
                    ax.set_xlabel("Edit Distance Upper Bound (edits)")
                    ax.set_ylabel("String Length")
                    ax.set_zlabel(t_name)
            plt.title(t_name)
            plt.grid()
            plt.show()

    def graphRead(self,file):
        CnR =[]
        for row in csv.reader(file, delimiter=':'):
            CnR.append(row)
        return CnR

if __name__=="__main__":
    for filename in glob.glob('*D.txt'):

        with open(filename) as csvfile:
            PerformacePlots(csvfile, filename)

