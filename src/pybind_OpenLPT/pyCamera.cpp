#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "myMATH.h"
#include "Matrix.h"
#include "Camera.h"

namespace py = pybind11;


void init_Camera(py::module &m) 
{
    py::class_<PinholeParam>(m, "PinholeParam")
        .def(py::init<>())
        .def_readwrite("n_row", &PinholeParam::n_row)
        .def_readwrite("n_col", &PinholeParam::n_col)
        .def_readwrite("cam_mtx", &PinholeParam::cam_mtx)
        .def_readwrite("is_distorted", &PinholeParam::is_distorted)
        .def_readwrite("n_dist_coeff", &PinholeParam::n_dist_coeff)
        .def_readwrite("dist_coeff", &PinholeParam::dist_coeff)
        .def_readwrite("r_mtx", &PinholeParam::r_mtx)
        .def_readwrite("t_vec", &PinholeParam::t_vec)
        .def_readwrite("r_mtx_inv", &PinholeParam::r_mtx_inv)
        .def_readwrite("t_vec_inv", &PinholeParam::t_vec_inv)
        .doc() = "PinholeParam struct";

    py::enum_<RefPlane>(m, "RefPlane")
        .value("REF_X", RefPlane::REF_X)
        .value("REF_Y", RefPlane::REF_Y)
        .value("REF_Z", RefPlane::REF_Z)
        .export_values();

    py::class_<PolyParam>(m, "PolyParam")
        .def(py::init<>())
        .def_readwrite("n_row", &PolyParam::n_row)
        .def_readwrite("n_col", &PolyParam::n_col)
        .def_readwrite("ref_plane", &PolyParam::ref_plane)
        .def_readwrite("plane", &PolyParam::plane)
        .def_readwrite("n_coeff", &PolyParam::n_coeff)
        .def_readwrite("u_coeffs", &PolyParam::u_coeffs)
        .def_readwrite("du_coeffs", &PolyParam::du_coeffs)
        .def_readwrite("v_coeffs", &PolyParam::v_coeffs)
        .def_readwrite("dv_coeffs", &PolyParam::dv_coeffs)
        .doc() = "PolyParam struct";

    py::enum_<CameraType>(m, "CameraType")
        .value("PINHOLE", CameraType::PINHOLE)
        .value("POLYNOMIAL", CameraType::POLYNOMIAL)
        .export_values();

    py::class_<Camera>(m, "Camera")
        .def_readwrite("_type", &Camera::_type)
        .def_readwrite("_pinhole_param", &Camera::_pinhole_param)
        .def_readwrite("_poly_param", &Camera::_poly_param)
        .def(py::init<>())
        .def(py::init<const Camera&>())
        .def(py::init<std::istream&>())
        .def(py::init<std::string>())
        .def("loadParameters", [](Camera &self, std::istream& is) {
            self.loadParameters(is);
        })
        .def("loadParameters", [](Camera &self, std::string filename) {
            self.loadParameters(filename);
        })
        .def("saveParameters", &Camera::saveParameters)
        .def("rmtxTorvec", &Camera::rmtxTorvec)
        .def("getNRow", &Camera::getNRow)
        .def("getNCol", &Camera::getNCol)
        .def("project", &Camera::project)
        .def("worldToUndistImg", &Camera::worldToUndistImg)
        .def("distort", &Camera::distort)
        .def("polyProject", &Camera::polyProject)
        .def("lineOfSight", &Camera::lineOfSight)
        .def("undistort", &Camera::undistort)
        .def("pinholeLine", &Camera::pinholeLine)
        .def("polyImgToWorld", &Camera::polyImgToWorld)
        .def("polyLineOfSight", &Camera::polyLineOfSight)
        .doc() = "Camera class";

    py::class_<CamList>(m, "CamList")
        .def(py::init<>())
        .def_readwrite("cam_list", &CamList::cam_list)
        .def_readwrite("intensity_max", &CamList::intensity_max)
        .def_readwrite("useid_list", &CamList::useid_list)
        .doc() = "CamList struct";
}