import matplotlib.pyplot as plt
import csv
import glob

class PerformacePlots:
    def __init__(self,file):
        for row in csv.reader(file, delimiter=';', quotechar='|'):
            print(', '.join(row))
        #load a file
        self.x
        self.y
        self.legends
        self.title

    def CommVsEdist(self):
        for i in range(len(x)):
            plt.plot(self.x, self.setsize, label ='total set size')
        plt.legend(loc=2)
        plt.xlabel("Edit Distance Upper Bound")
        plt.ylabel("Size")
        plt.title("200 length random string with k = "+str(3))
        plt.grid()
        plt.show()

if __name__=="__main__":
    for filename in glob.glob('*.txt'):
        with open(filename) as csvfile:
            PerformacePlots(filename)
