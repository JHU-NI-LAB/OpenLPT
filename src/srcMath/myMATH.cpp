#include "myMATH.h"

namespace myMATH
{

// Linespace
// n >= 2, including min and max, n_gap = n-1
std::vector<double> linspace (double min, double max, int n)
{
    // both min and max are stored in the vector
    if (n < 2)
    {
        std::cerr << "myMATH::Linspace: Cannot generate line space: "
                  << "n = " << n << ", "
                  << "less than 2!"
                  << std::endl;
        throw error_size;
    }

    double delta = (max - min) / (n - 1);

    std::vector<double> res(n);
    for (int i = 0; i < n; i ++)
    {
        res[i] = (min + delta * i);
    }

    return res;
}

// Trilinear interpolation
double triLinearInterp(AxisLimit const& grid_limit, std::vector<double> const& value, std::vector<double> const& pt_vec)
{
    // [https://en.wikipedia.org/wiki/Trilinear_interpolation]
    // value: 8
    //        c_000 [x_min, y_min, z_min]
    //        c_100 [x_max, y_min, z_min]
    //        c_101 [x_max, y_min, z_max]
    //        c_001 [x_min, y_min, z_max]
    //        c_010 [x_min, y_max, z_min]
    //        c_110 [x_max, y_max, z_min]
    //        c_111 [x_max, y_max, z_max]
    //        c_011 [x_min, y_max, z_max]
    // pt_world: 3 
    //           x, y, z
    
    // Key requirement:
    // If pt_world is outside the limit:
    //  set it as the value on the boundary 

    double x_0 = grid_limit.x_min;
    double x_1 = grid_limit.x_max;
    double y_0 = grid_limit.y_min;
    double y_1 = grid_limit.y_max;
    double z_0 = grid_limit.z_min;
    double z_1 = grid_limit.z_max;

    double c_000 = value[0];
    double c_100 = value[1];
    double c_101 = value[2];
    double c_001 = value[3];
    double c_010 = value[4];
    double c_110 = value[5];
    double c_111 = value[6];
    double c_011 = value[7];

    double x = pt_vec[0];
    double y = pt_vec[1];
    double z = pt_vec[2];

    double x_d = (x - x_0) / (x_1 - x_0);
    double y_d = (y - y_0) / (y_1 - y_0);
    double z_d = (z - z_0) / (z_1 - z_0);

    if (x_d > 1 || x_d < 0 || 
        y_d > 1 || y_d < 0 || 
        z_d > 1 || z_d < 0)
    {
        std::cerr << "myMATH::TriLinearInterp error: out of range" 
                  << "(x,y,z) = (" 
                  << x << ","
                  << y << ","
                  << z << ")"
                  << std::endl;
        throw error_range;
    }

    double c_00 = c_000 * (1 - x_d) + c_100 * x_d;
    double c_01 = c_001 * (1 - x_d) + c_101 * x_d;
    double c_10 = c_010 * (1 - x_d) + c_110 * x_d;
    double c_11 = c_011 * (1 - x_d) + c_111 * x_d;

    double c_0 = c_00 * (1 - y_d) + c_10 * y_d;
    double c_1 = c_01 * (1 - y_d) + c_11 * y_d;

    double c = c_0 * (1 - z_d) + c_1 * z_d;

    return c;
}

// Create unit vector
// unit_vec = (pt2 - pt1) / norm(pt2 - pt1)
Pt3D createUnitVector (Pt3D const& pt1, Pt3D const& pt2)
{
    Pt3D res = pt2 - pt1;
    res /= res.norm();
    return res;
}

// Create unit vector
// unit_vec = (pt2 - pt1) / norm(pt2 - pt1)
Pt2D createUnitVector (Pt2D const& pt1, Pt2D const& pt2)
{
    Pt2D res = pt2 - pt1;
    res /= res.norm();
    return res;
}

// Calculate dot product
double dot (Pt3D const& pt1, Pt3D const& pt2)
{
    double res = 0;
    for (int i = 0; i < 3; i ++)
    {
        res += pt1[i] * pt2[i];
    }
    return res;
}

// Calculate dot product
double dot (Pt2D const& pt1, Pt2D const& pt2)
{
    double res = 0;
    for (int i = 0; i < 2; i ++)
    {
        res += pt1[i] * pt2[i];
    }
    return res;
}

// Calculate the distance between two points
double dist (Pt3D const& pt1, Pt3D const& pt2)
{
    Pt3D res = pt2 - pt1;
    return res.norm();
}

// Calculate the distance between two points
double dist (Pt2D const& pt1, Pt2D const& pt2)
{
    Pt2D res = pt2 - pt1;
    return res.norm();
}

// Calculate the distance between point and line
double dist (Pt3D const& pt, Line3D const& line)
{
    Pt3D diff = pt - line.pt;

    double dist_proj = dot(diff, line.unit_vector);
    double dist = dot(diff, diff) - dist_proj * dist_proj;
    
    if (dist >= 0)
    {
        dist = std::sqrt(dist);
    }
    else if (dist < 0 && dist > -SQRTSMALLNUMBER)
    {
        dist = 0;
    }
    else
    {
        std::cerr << "myMATH::dist: error at line " << __LINE__ << "\n" 
                  << "negative distance " << dist << std::endl;
        pt.print();
        line.pt.print();
        line.unit_vector.print();
        diff.print();
        throw error_range;
    }

    return dist;
}

// Calculate the distance between point and line
double dist (Pt2D const& pt, Line2D const& line)
{
    Pt2D diff = pt - line.pt;

    double dist_proj = dot(diff, line.unit_vector);
    double dist = dot(diff, diff) - std::pow(dist_proj, 2);
    
    if (dist >= 0)
    {
        dist = std::sqrt(dist);
    }
    else if (dist < 0 && dist > -SQRTSMALLNUMBER)
    {
        dist = 0;
    }
    else
    {
        std::cerr << "myMATH::dist: error at line " << __LINE__ << "\n" 
                  << "negative distance " << dist << std::endl;
        pt.print();
        line.pt.print();
        line.unit_vector.print();
        diff.print();
        throw error_range;
    }

    return dist;
}

// Triangulation
void triangulation(Pt3D& pt_world, double& error,
                   std::vector<Line3D> const& line_of_sight_list)
{
    int n = line_of_sight_list.size();
    if (n < 2)
    {
        std::cerr << "myMATH::Triangulation: "
                  << "Cannot triangulate with less than 2 lines of sight!"
                  << std::endl;
        throw error_size;
    }

    Matrix<double> mtx(3, 3, 0);
    Matrix<double> temp(3,3,0);
    Pt3D pt_3d(0,0,0);
    Pt3D pt_ref;
    Pt3D unit_vector;

    for (int i = 0; i < n; i ++)
    {
        pt_ref = line_of_sight_list[i].pt;
        unit_vector = line_of_sight_list[i].unit_vector;

        double nx = unit_vector[0];
        double ny = unit_vector[1];
        double nz = unit_vector[2];

        temp(0,0) = 1-nx*nx; temp(0,1) = -nx*ny;  temp(0,2) = -nx*nz;
        temp(1,0) = -nx*ny;  temp(1,1) = 1-ny*ny; temp(1,2) = -ny*nz;
        temp(2,0) = -nx*nz;  temp(2,1) = -ny*nz;  temp(2,2) = 1-nz*nz;

        pt_3d += temp * pt_ref;
        mtx += temp;
    }

    pt_world = myMATH::inverse(mtx) * pt_3d;

    // calculate errors 
    error = 0;
    Pt3D diff_vec;
    for (int i = 0; i < n; i ++)
    {
        diff_vec = pt_world - line_of_sight_list[i].pt;
        double h = std::pow(diff_vec.norm(),2)
            - std::pow(myMATH::dot(diff_vec, line_of_sight_list[i].unit_vector),2);
            
        if (h < 0 && h >= - SMALLNUMBER)
        {
            h = 0;
        }
        else if (h < - SMALLNUMBER)
        {
            std::cerr << "myMATH::triangulation error: "
                      << "the distance in triangulation is: "
                      << "h = "
                      << h
                      << std::endl;
            throw error_range;
        }
        else
        {
            h = std::sqrt(h);
        }

        error += h;
    }
    error /= n;
}

// Find the cross points of two 2d lines
Pt2D crossPoint (Line2D const& line1, Line2D const& line2)
{
    double den = line1.unit_vector[0] * line2.unit_vector[1] - line1.unit_vector[1] * line2.unit_vector[0];
    if (std::fabs(den) < SMALLNUMBER)
    {
        std::cerr << "myMATH::crossPoint: "
                  << "The two lines are parallel!"
                  << std::endl;
        throw error_range;
    }

    double num = line2.unit_vector[1] * (line2.pt[0] - line1.pt[0]) - line2.unit_vector[0] * (line2.pt[1] - line1.pt[1]);

    Pt2D res(line1.unit_vector);
    res *= num / den;
    res += line1.pt;

    return res;
}

// Polynomial fit
void polyfit (std::vector<double>& coeff, std::vector<double> const& x, std::vector<double> const& y, int order)
{
    if (x.size() != y.size())
    {
        std::cerr << "myMATH::polyfit error at line " << __LINE__ << ":\n"
                  << "x.size() != y.size()"
                  << std::endl;
        throw error_size;
    }

    if (order < 1)
    {
        std::cerr << "myMATH::polyfit error at line " << __LINE__ << ":\n"
                  << "order < 1"
                  << std::endl;
        throw error_range;
    }


    int n = x.size();
    int m = order + 1;
    
    Matrix<double> x_mat(n, m, 0);
    Matrix<double> y_mat(n, 1, 0);
    Matrix<double> a_mat(m, 1, 0);

    for (int i = 0; i < n; i ++)
    {
        for (int j = 0; j < m; j ++)
        {
            x_mat(i,j) = std::pow(x[i], j);
        }
        y_mat(i,0) = y[i];
    }

    // std::string method = m < 4 ? "det" : "gauss";
    std::string method = "gauss";
    a_mat = myMATH::inverse(x_mat.transpose() * x_mat, method) * x_mat.transpose() * y_mat;

    coeff.resize(m);
    for (int i = 0; i < m; i ++)
    {
        coeff[i] = a_mat(i,0);
    }
}

}
