//
//	ObjectInfo.h 
//
//	Base class for various image formats
//	All image format types should inherit from Image 
//
//	Created by Shijie Zhong 07/11/2022 
//

#ifndef OBJECTINFO_H
#define OBJECTINFO_H 

#include <vector>
#include "Matrix.h"
#include "Camera.h"

class ObjectInfo
{
protected:
    Matrix<double> _pt_center; 
    std::vector<Matrix<double>> _match_pos_info; // n_cam * (3*1) [2D, pixel]
    bool _is_tracked = false;
    bool _is_fake = false;
    double _tri_error = 0.0;

public:
    ObjectInfo ()  
    {
        _match_pos_info.reserve(10);
    };
    ObjectInfo (const ObjectInfo& object);
    virtual ~ObjectInfo () {};

    void SetCenterPos (Matrix<double>& pt_center) 
    {
        _pt_center = pt_center;
    };
    Matrix<double> GetCenterPos () const
    {
        return _pt_center;
    };

    void SetMatchPosInfo (std::vector<Matrix<double>>& match_pos_info) 
    {
        _match_pos_info = match_pos_info;
    };
    void SetMatchPosInfo (std::vector<Camera>& cam_list)
    {
        int n_cam = cam_list.size();
        _match_pos_info = std::vector<Matrix<double>> (n_cam, Matrix<double> (3,1));
        for (int cam_id = 0; cam_id < n_cam; cam_id ++)
        {     
            _match_pos_info[cam_id] = cam_list[cam_id].ImgMMToPixel(
                    cam_list[cam_id].WorldToImgMM(
                        _pt_center
                    )
                );
        }
    };
    void ClearMatchPosInfo ()
    {
        _match_pos_info.clear();
    };
    void AddMatchPosInfo (Matrix<double>& pt)
    {
        _match_pos_info.push_back(pt);
    };
    std::vector<Matrix<double>> GetMatchPosInfo () const
    {
        return _match_pos_info;
    };
    Matrix<double> GetMatchPosInfo (int cam_id) const
    {
        int n_cam = _match_pos_info.size();
        if (cam_id >= n_cam)
        {
            std::cerr << "ObjectInfo::GetMatchPosInfo (int cam_id): \n"
                      << "\t" 
                      << "cam_id = " << cam_id << "; "
                      << "is larger than n_cam = " << n_cam << "."
                      << std::endl; 
            throw;
        }

        return _match_pos_info[cam_id];
    };

    void SetError (double error) 
    {
        _tri_error = error;
    };
    double GetError () 
    {
        return _tri_error;
    };

    void SetIsFake (bool is_fake) {_is_fake = is_fake;};
    bool IsFake () const {return _is_fake;};

    void SetIsTrack (bool is_tracked) {_is_tracked = is_tracked;};
    bool IsTrack() const {return _is_tracked;};
};


class TracerInfo : public ObjectInfo
{
private:
    int _r_pixel = 4;

public:
    TracerInfo () {};
    TracerInfo (const TracerInfo& tracer) : ObjectInfo(tracer) {};
    TracerInfo (Matrix<double> pt_center) 
    {
        _pt_center = pt_center;
    };
    ~ TracerInfo () {};

    void AddMatchInfo (TracerInfo const& tracer_info)
    {
        Matrix<double> pt_center = tracer_info.GetCenterPos();
        AddMatchPosInfo(pt_center);
    };

    int GetRadiusPixel () const
    {
        return _r_pixel;
    };
    void SetRadiusPixel (int r_pixel)
    {
        _r_pixel = r_pixel;
    };
};

#endif
