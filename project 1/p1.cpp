#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

struct Point {
    long int x; //x-coordinate of the point
    long int y; //y-coordinate of the point
    double polar_angle; //the polar angle with lowest point
    long double distance; //the distance from between the point to lowest point
    bool lowest_point; //return true if it is the lowest point
};

double polar_angle(Point P_0, Point point) {
    long int y = point.y - P_0.y;
    long int x = point.x - P_0.x;

    double rad = atan2(y, x); //calculated polar angle in radius

    return rad * (180 / M_PI);
}


bool compare_polar_angle(const Point &P_0,const Point &P_1) {
    return P_0.polar_angle < P_1.polar_angle;
}

double ccw(const Point &P_1, const Point &P_2, const Point &P_3){
    return ((double)P_2.x - (double)P_1.x)*((double)P_3.y - (double)P_1.y) - ((double)P_2.y - (double)P_1.y)*((double)P_3.x - (double)P_1.x);
}

int main() {
    long int num = 0; //total number of points
    unsigned long int index = 0; //index in the array of the lowest coordinate point
    Point P_0; //lowest coordinate point
    vector<Point> stack; //stack to store the convex hull
    vector<Point> points; //array to store all the points

    stringstream ss; //use to stream input and output 
    string input_line; //string to store input line 

    getline(cin, input_line);
    ss.str(input_line);
    ss >> num; // store the total number of point from the first input line
    ss.clear();

    //read and store all the points in points array
    while (getline(cin, input_line)) {
        ss.str(input_line);
        Point P;
        ss >> P.x >> P.y;
        ss.clear();
        points.push_back(P);

        //find the lowest y - coordinate points
        if (points.size() == 1) {
            points[0].lowest_point = true;
            P_0 = points[0];
        }
        else if (points.back().y < points[index].y) {
            points[index].lowest_point = false;
            points.back().lowest_point = true; //indicate the points is lowest points
            index = points.size() - 1; //set the index of the lowest point in points array
            P_0 = points[index];

        }
        else if (points.back().y == points[index].y) {
            if (points.back().x < points[index].x) {
                //set the points that has the lowest x coordinate as the lowest points, if both y - coordinate is the same
                points[index].lowest_point = false;
                points.back().lowest_point = true; 
                index = points.size() - 1; 
                P_0 = points[index];
            }
        }
    }

    //calculate the polar angle of the all the points 
    //calculate the distance from the point to the lowest point
    for (auto &el:points) {
        el.polar_angle = polar_angle(P_0, el);
        el.distance = sqrt(pow(el.x - P_0.x, 2) + pow(el.y - P_0.y, 2));
    }

    //if P_0 is not at first index, swap the P_0 with the element at the first index
    if (index != 0 && num != 0) {
        swap(points[index], points[0]);
    } 

    if (num != 0) {
        //sort the array base on polar angle compared to the lowest point coordinate
        //sort from smallest to largest angle
        sort(points.begin() + 1, points.end(), compare_polar_angle);

        //remove element that has same coordinate as P_0
        for (unsigned long int i = 1; i < points.size(); ) {
            if (points[i].x == P_0.x && points[i].y == P_0.y) {
                //remove the points with the same coordinate as P_0
                points.erase(points.begin() + (long int)i);
            }
            else {
                i ++;
            }
        }

        //remove points that are have the same polar angle and only keep the point that has furthest distance from lowest point 
        for (int i = 1; i < (int)points.size() - 1; ) {
            //start from index 1 to skip out the first element (P_0)
            if (points[(unsigned long int)i].polar_angle == points[(unsigned long int)i + 1].polar_angle) {
                //remove the points that have same polar angle that are not the furthest 
                if (points[(unsigned long int)i].distance < points[(unsigned long int)i + 1].distance) {
                    points.erase(points.begin() + (long int)i);
                }
                else {
                    points.erase(points.begin() + (long int)i + 1);
                }
            }
            else {
                i ++;
            }
        }

        //graham scan 
        for (auto &el:points) {
            while (stack.size() > 1 && (ccw(stack[stack.size() - 2], stack.back(), el) <= 0)) {
                stack.pop_back(); //pop the last element from the stack
            }
            stack.push_back(el);
        }

    }

    // print out the convex hull to terminal
    for (auto el:stack) {
        cout << el.x << ' ' << el.y << endl;
    }



    return 0;
}