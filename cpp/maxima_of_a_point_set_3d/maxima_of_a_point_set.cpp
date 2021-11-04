#include <iostream>
#include <fstream>    // For input and output files
#include <algorithm>  // Needed for sort()
#include <map>        // Needed for map<double, double> bst
#include <vector>

using namespace std;

class Point {
public:
    void setMaximal() {
        maximal = true;
    }      // Sets maximal to true: The point is maximal

    double getX() const {
        return this->x;
    }

    double getY() const {
        return this->y;
    }

    double getZ() const {
        return this->z;
    }

    bool getMaximal() const {
        return this->maximal;
    }

    bool operator<(const Point &p) const {
        if (this->getZ() == p.getZ() && this->getY() == p.getY()) {
            return this->getX() < p.getX();
        } else if (this->getZ() == p.getZ()) {
            return this->getY() < p.getY();
        } else {
            return this->getZ() < p.getZ();
        }
    }
    // Returns (*this < p)

    friend istream &operator>>(istream &ISObj, Point &p) {
        char c;
        ISObj >> p.x >> c >> p.y >> c >> p.z;
        return ISObj;
    }
    // Inputs a single point: Its x, y and z coords

    friend ostream &operator<<(ostream &OSObj, const Point &p) {
        return OSObj << p.getX() << ", " << p.getY() << ", " << p.getZ();
    }
    // Outputs a single point: Its x, y and z coords

private:
    double x = 0, y = 0, z = 0;  // Coords of the point
    bool maximal = false;        // Whether the point is maximal
};

typedef map<double, double, std::greater<double> > mapdd;

void inputPoints(ifstream &infile, Point *points, int *numPoints);

void findMaximal(Point *points, int numPoints, int *maxNum);

void printMaximal(ofstream &outfile, Point *points, int numPoints, int maxNum);


int main(int argc, char *argv[]) {
    if (argc != 3) {  // Error check
        cerr << "Usage: ./a.out infile.txt out.txt\n";
        exit(1);
    }
    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);

    for (int i = 1; i <= 10; i++)  // Iterate on 10 sets of points
    {
        int numPoints = 0;  // Number of points in the i-th set
        infile >> numPoints;
        Point points[numPoints]; // points[0..(numPoints-1)]
        inputPoints(infile, points, &numPoints);   // Reads the i-th input set
        sort(points, points + numPoints);          // Sorts points[0..(numPoints-1)]
        int maxNum = 0;     // Number of maximal points in the i-th set
        findMaximal(points, numPoints, &maxNum);   // Finds the maximal points
        outfile << "Output for " << i << "-th Set of Points\n";
        printMaximal(outfile, points, numPoints, maxNum);
    }

    infile.close();
    outfile.close();
    return 0;
}

void inputPoints(ifstream &infile, Point *points, int *numPoints)
// Inputs all the points into points[]
{
    for (int i = 0; i < *numPoints; i++) {
        infile >> points[i];
    }
}

void printMaximal(ofstream &outfile, Point *points, int numPoints, int maxNum)
// Outputs the maximal elements in points[]
{
    outfile << "Input Size = " << numPoints << endl;
    outfile << "MaxNum = " << maxNum << endl << endl;
    outfile << "Maxima(S) (x, y, z)\n";

    for (int i = 0; i < numPoints; ++i) {
        if (points[i].getMaximal()) {
            outfile << i << "  " << points[i] << endl;
        }
    }
    outfile << "\n--------------------------------------------\n\n";
}

void findMaximal(Point *points, int numPoints, int *maxNum)
// Finds the maximal elements in points[0..(numPoints-1)]
{
    mapdd bst;  // Keeps the 2-d maxima (in the (x,y)-plane)
    // of the points seen so far.
    // Keeps (key=y_coord, value=x_coord) pairs.
    //bst[1.0] = 0.0;  // Inserts (key 1.0, value 0.0) into bst
    *maxNum = 1;
    bst[points[numPoints - 1].getY()] = points[numPoints - 1].getX();
    points[numPoints - 1].setMaximal();

    double xcoord, ycoord;
    for (int i = numPoints - 2; i >= 0; i--) {
        xcoord = points[i].getX();
        ycoord = points[i].getY();
        bool dominated = false;
        for (auto it: bst) {
            if (ycoord < it.first && xcoord < it.second) {
                dominated = true;
                break;
            }
        }
        if (!dominated) {
            bst[ycoord] = xcoord;
            points[i].setMaximal();
            *maxNum = *maxNum + 1;
        }
    }
}
