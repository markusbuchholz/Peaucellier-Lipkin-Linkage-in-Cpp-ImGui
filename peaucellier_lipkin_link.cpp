// Markus Buchholz. 2023
// g++ peaucellier_lipkin_link.cpp -o t -I/usr/include/python3.8 -lpython3.8

#include <iostream>
#include <tuple>
#include <vector>
#include <math.h>
#include <cmath>
#include <iomanip>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

//----------- system dynamic parameters --------------------

float r = 3.0;
float a = 9.0;
float b = 4.0;

//---------------------------------------------------------------

void plot2D(std::vector<float> X, std::vector<float> Y, std::vector<float> arcX, std::vector<float> arcY)
{

    
    plt::title("Peaucellier-Lipkin linkage");
    plt::named_plot("path of linkage", X, Y);
    plt::named_plot("rotation", arcX, arcY);
    plt::xlabel("pos X");
    plt::ylabel("pos Y");
    plt::legend();
    plt::xlabel("pos X");
    plt::ylabel("pos Y");
    plt::show();
}

//---------------------------------------------------------------

std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>> simulation()
{
    std::vector<float> arcx;
    std::vector<float> arcy;
    

    std::vector<float> x;
    std::vector<float> y;
    
    float dt = M_PI / 180.0;
    float Ox = 0.0;
    float Oy = 0.0;
    float Px = Ox - r;
    float Py = Oy;

    float beta = std::acos((float)(a - b) / (2 * r));
    float maxTheta = 2 * beta;
    float maxCy = (a + b) * std::sin(beta);

    float Ax = 0.0;
    float Ay = 0.0;
    float ABx = 0.0;
    float ABy = 0.0;
    float Bx = 0.0;
    float By = 0.0;
    float ADx = 0.0;
    float ADy = 0.0;
    float Dx = 0.0;
    float Dy = 0.0;
    float Cx = 0.0;
    float Cy = 0.0;

    float deg = 67.1;

    for (float t = -M_PI / 6; t < M_PI / 6; t = t + dt)
    {

        //------------------------

        Ax = Ox + r * std::cos(t);
        Ay = Oy + r * std::sin(t);

        float AP = std::sqrt(std::pow(Ax - Px, 2) + std::pow(Ay - Py, 2));
        float alpha = std::acos((float)(b * b + AP * AP - a * a) / (2 * b * AP));

        //------------------------

        auto rotX = [](float x, float y, float tt)
        {
            return (float) x * std::cos(tt) - y * std::sin(tt);
        };

        auto rotY = [](float x, float y, float tt)
        {
            return (float) x * std::sin(tt) + y * std::cos(tt);
        };

        ABx = (float)(b / AP) * rotX(Px - Ax, Py - Ay, alpha);
        ABy = (float)(b / AP) * rotY(Px - Ax, Py - Ay, alpha);

        Bx = Ax + ABx;
        By = Ay + ABy;

        //------------------------
        ADx = (float)(b / AP) * rotX(Px - Ax, Py - Ay, -alpha);
        ADy = (float)(b / AP) * rotY(Px - Ax, Py - Ay, -alpha);

        Dx = Ax + ADx;
        Dy = Ay + ADy;
        //------------------------

        Cx = Ax + ABx + ADx;
        Cy = Ay + ABy + ADy;

        //------------------------
        std::cout << Ax << " : " << ABx << " : " << ADx  <<  " : " << Ax + ABx + ADx << "\n";

        x.push_back(Cx);
        y.push_back(Cy);

        arcx.push_back(Ax);
        arcy.push_back(Ay);
    }
    return std::make_tuple(x, y, arcx, arcy);
}

int main()
{
    auto sim = simulation();
    plot2D(std::get<0>(sim), std::get<1>(sim), std::get<2>(sim), std::get<3>(sim));
}
