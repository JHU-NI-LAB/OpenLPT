#ifndef TRACK_HPP
#define TRACK_HPP

#include "Track.h"

template<class T3D>
Track<T3D>::Track(const T3D& obj3d, int t) : _n_obj3d(1) 
{ 
    _obj3d_list.push_back(obj3d); 
    _t_list.push_back(t); 
}

template<class T3D>
Track<T3D>::Track(const Track& track) 
    : _obj3d_list(track._obj3d_list), _t_list(track._t_list), _occluded(track._occluded), _n_obj3d(track._n_obj3d), _active(track._active) {}


template<class T3D>
void Track<T3D>::addNext(const T3D& obj, int t)
{
    _obj3d_list.push_back(obj);
    _t_list.push_back(t);
    _n_obj3d ++;
}

template<class T3D>
void Track<T3D>::addNext(const Track& track)
{
    _obj3d_list.insert(_obj3d_list.end(), track._obj3d_list.begin(), track._obj3d_list.end());
    _t_list.insert(_t_list.end(), track._t_list.begin(), track._t_list.end());
    _n_obj3d = _obj3d_list.size();
}

template<>
void Track<Tracer3D>::saveTrack(std::ofstream& output, int track_id, float fps, int n_cam_all)
{    
    for (int i = 0; i < _n_obj3d; i ++)
    {
        output << track_id << "," << _t_list[i]/fps << ",";

        // print 2d info
        _obj3d_list[i].saveObject3D(output, n_cam_all);
    }
}

template<class T3D>
Track<T3D>& Track<T3D>::operator=(const Track<T3D>& track)
{
    _obj3d_list = track._obj3d_list;
    _t_list = track._t_list;
    _occluded = track._occluded;
    _n_obj3d = track._n_obj3d;
    _active = track._active;
    return *this;
}

template<class T3D>
void Track<T3D>::predictNext(T3D& obj3d)
{
    if (typeid(T3D) == typeid(Tracer3D))
    {
        predLMSWiener(obj3d);
    }
    else
    {
        std::cerr << "Error: predictNext not implemented for this type" << std::endl;
    }
}

template<class T3D>
void Track<T3D>::predLMSWiener(T3D& obj3d)
{
    int n_obj3d = _obj3d_list.size();
    int order = 0;

    if (n_obj3d < 2)
    {
        std::cerr << "Track<T3D>::predLMSWiener error at line " << __LINE__ << ": no enough points to predict" << std::endl;
        return;
    }
    else if (n_obj3d < 6)
    {
        order = n_obj3d - 1;
    }
    else 
    {
        order = 5;
    }

    // predict at each direction
    std::vector<double> series (order+1, 0);
    std::vector<double> filter_param (order, 0);
    for (int i = 0; i < 3; i ++)
    {
        for (int j = 0; j < order+1; j ++)
        {
            series[j] = _obj3d_list[n_obj3d-1-order + j]._pt_center[i];
        }
        
        // Wiener filter does badly near zero
        // Make a shift to avoid zero-plane prediction
        bool shift_label = false;
        double shift = 10;
        if (std::fabs(series[order]) < 1)
        {
            shift_label = true;
            for (int j = 0; j < order+1; j ++)
            {
                series[j] += shift;
            }
        }
        
        // calculate the step
        double sum = 0;
        for (int j = 0; j < order; j ++)
        {
            sum += (series[j] * series[j]);
        }
        double step = 1 / sum;

        // initialize the filter param
        std::fill(filter_param.begin(), filter_param.end(), 0);
        double prediction = 0;
        double error = series[order] - prediction;

        int iter = 0;
        while (std::fabs(error) > SMALLNUMBER && iter < WIENER_MAX_ITER)
        {
            // Update parameters and calculate the prediction using the new filter param
            prediction = 0;
            for (int j = 0; j < order; j ++)
            {
                filter_param[j] += (step * series[j] * error);
                prediction += (filter_param[j] * series[j]);
            }

            error = series[order] - prediction;
            iter ++;
        }

        prediction = 0;
        for (int j = 0; j < order; j ++)
        {
            prediction += (filter_param[j] * series[j+1]);
        }
        if (shift_label)
        {
            prediction -= shift;
        }
        obj3d._pt_center[i] = prediction;
    }  

}

#endif