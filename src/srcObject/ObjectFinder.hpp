#ifndef OBJECTFINDER_HPP
#define OBJECTFINDER_HPP

#include "ObjectFinder.h"

void ObjectFinder2D::findTracer2D
(std::vector<Tracer2D>& tr2d_list, Image const& img, double max_intensity, double min_intensity, double r_px)
{
    // Judge whether a given point is local maximum intensity or not
    //  skip first and last rows and columns   
    Tracer2D tr2d;
    tr2d._r_px = r_px;
    for (int row = 1; row < img.getDimRow()-1; row ++)
    {
        for (int col = 1; col < img.getDimCol()-1; col ++)
        {
            if (img(row, col) > max_intensity)
            {
                // the intensity is out of range 
                std::cerr << "ObjectFinder2D::findTracer2D error: "
                          << "The intensity at (row,col)=(" << row << "," << col << ") is "
                          << img(row, col) << ". " 
                          << "It is larger than max intensity " 
                          << max_intensity << std::endl;
                throw error_range;
            }

            if (img(row, col) >= min_intensity && myMATH::isLocalMax(img, row, col))
            {
                // use Gaussian distribution to find the maximum intensity
                //  => particle center position

                // Note: row => y 
                //       col => x 
                int x1 = (col - 1);
                int x2 =  col;     
                int x3 = (col + 1);
                int y1 = (row - 1);
                int y2 =  row;     
                int y3 = (row + 1);

                double ln_z1 = 0.0;
                double ln_z2 = 0.0;
                double ln_z3 = 0.0;

                // find the col value (coordinate: x)
                ln_z1 = img(y2, x1) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x1));
                ln_z2 = img(y2, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x2));
                ln_z3 = img(y2, x3) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x3));
    
                double xc = -0.5 * (  (ln_z1 * double((x2 * x2) - (x3 * x3))) 
                                    - (ln_z2 * double((x1 * x1) - (x3 * x3))) 
                                    + (ln_z3 * double((x1 * x1) - (x2 * x2))) ) 
                                 / (  (ln_z1 * double(x3 - x2)) 
                                    - (ln_z3 * double(x1 - x2)) 
                                    + (ln_z2 * double(x1 - x3)) );
                if (!std::isfinite(xc)) 
                {
                    continue;
                }

                // find the row value (coordinate: y)
                ln_z1 = img(y1, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y1, x2));
                ln_z3 = img(y3, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y3, x2));

                double yc = -0.5 * (  (ln_z1 * double((y2 * y2) - (y3 * y3))) 
                                    - (ln_z2 * double((y1 * y1) - (y3 * y3))) 
                                    + (ln_z3 * double((y1 * y1) - (y2 * y2))) ) 
                                 / (  (ln_z1 * double(y3 - y2)) 
                                    - (ln_z3 * double(y1 - y2)) 
                                    + (ln_z2 * double(y1 - y3)) );
                if (!std::isfinite(yc)) 
                {
                    continue;
                }

                tr2d._pt_center[0] = xc;
                tr2d._pt_center[1] = yc;
                tr2d_list.push_back(tr2d);
            }
        }
    }
}

void ObjectFinder2D::findTracer2D
(std::vector<Tracer2D>& tr2d_list, Image const& img, double max_intensity, double min_intensity, double r_px, PixelRange const& region)
{
    // Check region
    if (region.row_min < 0 || region.row_max > img.getDimRow() || region.col_min < 0 || region.col_max > img.getDimCol())
    {
        std::cerr << "ObjectFinder2D::findTracer2D error at line " << __LINE__ << ":"
                  << "The region is out of range. " 
                  << "The region is (" << region.row_min << "," << region.row_max << "," << region.col_min << "," << region.col_max << "). "
                  << "The image size is (nRow,nCol) = (" << img.getDimRow() << "," << img.getDimCol() << ")." << std::endl;
        throw error_range;
    }

    // Judge whether a given point is local maximum intensity or not
    //  skip first and last rows and columns   
    Tracer2D tr2d;
    tr2d._r_px = r_px;
    for (int row = region.row_min+1; row < region.row_max-1; row ++)
    {
        for (int col = region.col_min+1; col < region.col_max-1; col ++)
        {
            if (img(row, col) > max_intensity)
            {
                // the intensity is out of range 
                std::cerr << "ObjectFinder2D::findTracer2D error: "
                          << "The intensity at (row,col)=(" << row << "," << col << ") is "
                          << img(row, col) << ". " 
                          << "It is larger than max intensity " 
                          << max_intensity << std::endl;
                throw error_range;
            }

            if (img(row, col) >= min_intensity && myMATH::isLocalMax(img, row, col))
            {
                // use Gaussian distribution to find the maximum intensity
                //  => particle center position

                // Note: row => y 
                //       col => x 
                int x1 = (col - 1);
                int x2 =  col;     
                int x3 = (col + 1);
                int y1 = (row - 1);
                int y2 =  row;     
                int y3 = (row + 1);

                double ln_z1 = 0.0;
                double ln_z2 = 0.0;
                double ln_z3 = 0.0;

                // find the col value (coordinate: x)
                ln_z1 = img(y2, x1) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x1));
                ln_z2 = img(y2, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x2));
                ln_z3 = img(y2, x3) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y2, x3));
    
                double xc = -0.5 * (  (ln_z1 * double((x2 * x2) - (x3 * x3))) 
                                    - (ln_z2 * double((x1 * x1) - (x3 * x3))) 
                                    + (ln_z3 * double((x1 * x1) - (x2 * x2))) ) 
                                 / (  (ln_z1 * double(x3 - x2)) 
                                    - (ln_z3 * double(x1 - x2)) 
                                    + (ln_z2 * double(x1 - x3)) );
                if (!std::isfinite(xc)) 
                {
                    continue;
                }

                // find the row value (coordinate: y)
                ln_z1 = img(y1, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y1, x2));
                ln_z3 = img(y3, x2) < LOGSMALLNUMBER ? std::log(LOGSMALLNUMBER) : std::log(img(y3, x2));

                double yc = -0.5 * (  (ln_z1 * double((y2 * y2) - (y3 * y3))) 
                                    - (ln_z2 * double((y1 * y1) - (y3 * y3))) 
                                    + (ln_z3 * double((y1 * y1) - (y2 * y2))) ) 
                                 / (  (ln_z1 * double(y3 - y2)) 
                                    - (ln_z3 * double(y1 - y2)) 
                                    + (ln_z2 * double(y1 - y3)) );
                if (!std::isfinite(yc)) 
                {
                    continue;
                }

                tr2d._pt_center[0] = xc;
                tr2d._pt_center[1] = yc;
                tr2d_list.push_back(tr2d);
            }
        }
    }
}

template<class T>
void ObjectFinder2D::findObject2D
(std::vector<T>& obj2d_list, Image const& img, std::vector<double> const& properties)
{
    if (typeid(T) == typeid(Tracer2D))
    {
        obj2d_list.clear();
        findTracer2D(obj2d_list, img, properties[0], properties[1], properties[2]);
    }
    else
    {
        std::cerr << "ObjectFinder error at line " << __LINE__ << ": "
                  << "class " << typeid(T).name()
                  << "is not included in ObjectFinder!" << std::endl;
        throw error_type;
    }
}

template<class T>
void ObjectFinder2D::findObject2D
(std::vector<T>& obj2d_list, Image const& img, std::vector<double> const& properties, PixelRange const& region)
{
    if (typeid(T) == typeid(Tracer2D))
    {
        obj2d_list.clear();
        findTracer2D(obj2d_list, img, properties[0], properties[1], properties[2], region);
    }
    else
    {
        std::cerr << "ObjectFinder error at line " << __LINE__ << ": "
                  << "class " << typeid(T).name()
                  << "is not included in ObjectFinder!" << std::endl;
        throw error_type;
    }
}

#endif 