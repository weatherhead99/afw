/*
 * This file is part of afw.
 *
 * Developed for the LSST Data Management System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "pybind11/pybind11.h"
#include "pybind11/eigen.h"
#include "pybind11/stl.h"

#include "ndarray/pybind11.h"

#include "lsst/afw/geom/ellipses/Quadrupole.h"

#include "lsst/utils/python.h"
#include "lsst/geom/Angle.h"
#include "lsst/geom/SpherePoint.h"
#include "lsst/geom/Box.h"
#include "lsst/afw/table/Key.h"
#include "lsst/afw/table/Schema.h"
#include "lsst/afw/table/BaseRecord.h"
#include "lsst/afw/table/FunctorKey.h"
#include "lsst/afw/table/aggregates.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace lsst {
namespace afw {
namespace table {

using utils::python::WrapperCollection;

namespace {

// We don't expose base classes (e.g. FunctorKey) to Python, since they're just used to
// define a CRTP interface in C++ and in Python that's just duck-typing.

template <typename T>
using PyPointKey = py::class_<PointKey<T>, std::shared_ptr<PointKey<T>>>;

template <typename T>
using PyPoint3Key = py::class_<Point3Key<T>, std::shared_ptr<Point3Key<T>>>;

template <typename Box>
using PyBoxKey = py::class_<BoxKey<Box>, std::shared_ptr<BoxKey<Box>>>;

using PyCoordKey = py::class_<CoordKey, std::shared_ptr<CoordKey>>;

using PyQuadrupoleKey = py::class_<QuadrupoleKey, std::shared_ptr<QuadrupoleKey>>;

using PyEllipseKey = py::class_<EllipseKey, std::shared_ptr<EllipseKey>>;

template <typename T, int N>
using PyCovarianceMatrixKey =
        py::class_<CovarianceMatrixKey<T, N>, std::shared_ptr<CovarianceMatrixKey<T, N>>>;

template <typename T>
static void declarePointKey(WrapperCollection &wrappers, std::string const &suffix) {
    wrappers.wrapType(
            PyPointKey<T>(wrappers.module, ("Point" + suffix + "Key").c_str()), [](auto &mod, auto &cls) {
                cls.def(py::init<>());
                cls.def(py::init<Key<T> const &, Key<T> const &>(), "x"_a, "y"_a);
                cls.def(py::init<SubSchema const &>());
                cls.def("__eq__", &PointKey<T>::operator==, py::is_operator());
                cls.def("__ne__", &PointKey<T>::operator!=, py::is_operator());
                cls.def("getX", &PointKey<T>::getX);
                cls.def("getY", &PointKey<T>::getY);
                cls.def("isValid", &PointKey<T>::isValid);
                cls.def_static("addFields", &PointKey<T>::addFields, "schema"_a, "name"_a, "doc"_a, "unit"_a);
                cls.def("set", [](PointKey<T> &self, BaseRecord &record,
                                  lsst::geom::Point<T, 2> const &value) { return self.set(record, value); });
                cls.def("get", &PointKey<T>::get);
            });
};

template <typename T>
static void declarePoint3Key(WrapperCollection &wrappers, std::string const &suffix) {
    wrappers.wrapType(
            PyPoint3Key<T>(wrappers.module, ("Point3" + suffix + "Key").c_str()), [](auto &mod, auto &cls) {
                cls.def(py::init<>());
                cls.def(py::init<Key<T> const &, Key<T> const &, Key<T> const &>(), "x"_a, "y"_a, "z"_a);
                cls.def(py::init<SubSchema const &>());
                cls.def("__eq__", &Point3Key<T>::operator==, py::is_operator());
                cls.def("__ne__", &Point3Key<T>::operator!=, py::is_operator());
                cls.def("getX", &Point3Key<T>::getX);
                cls.def("getY", &Point3Key<T>::getY);
                cls.def("isValid", &Point3Key<T>::isValid);
                cls.def_static("addFields", &Point3Key<T>::addFields, "schema"_a, "name"_a, "doc"_a, "unit"_a);
                cls.def("set", [](Point3Key<T> &self, BaseRecord &record,
                                  lsst::geom::Point<T, 3> const &value) { return self.set(record, value); });
                cls.def("get", &Point3Key<T>::get);
            });
};

template <typename Box>
static void declareBoxKey(WrapperCollection &wrappers, std::string const &suffix) {
    wrappers.wrapType(
            PyBoxKey<Box>(wrappers.module, ("Box" + suffix + "Key").c_str()), [](auto &mod, auto &cls) {
                using Element = typename Box::Element;
                cls.def(py::init<>());
                cls.def(py::init<PointKey<Element> const &, PointKey<Element> const &>(), "min"_a, "max"_a);
                cls.def(py::init<SubSchema const &>());
                cls.def("__eq__", &BoxKey<Box>::operator==, py::is_operator());
                cls.def("__ne__", &BoxKey<Box>::operator!=, py::is_operator());
                cls.def("getMin", &BoxKey<Box>::getMin);
                cls.def("getMax", &BoxKey<Box>::getMax);
                cls.def("isValid", &BoxKey<Box>::isValid);
                cls.def_static("addFields", &BoxKey<Box>::addFields, "schema"_a, "name"_a, "doc"_a, "unit"_a);
                cls.def("set", &BoxKey<Box>::set);
                cls.def("get", &BoxKey<Box>::get);

            });
};

static void declareCoordKey(WrapperCollection &wrappers) {
    wrappers.wrapType(PyCoordKey(wrappers.module, "CoordKey"), [](auto &mod, auto &cls) {
        cls.def(py::init<>());
        cls.def(py::init<Key<lsst::geom::Angle>, Key<lsst::geom::Angle>>(), "ra"_a, "dec"_a);
        cls.def(py::init<SubSchema const &>());
        cls.def("__eq__", &CoordKey::operator==, py::is_operator());
        cls.def("__ne__", &CoordKey::operator!=, py::is_operator());
        cls.def_static("addFields", &CoordKey::addFields, "schema"_a, "name"_a, "doc"_a);
        cls.def("getRa", &CoordKey::getRa);
        cls.def("getDec", &CoordKey::getDec);
        cls.def("isValid", &CoordKey::isValid);
        cls.def("get", [](CoordKey &self, BaseRecord const &record) { return self.get(record); });
        cls.def("set", &CoordKey::set);
    });
}

static void declareQuadrupoleKey(WrapperCollection &wrappers) {
    wrappers.wrapType(PyQuadrupoleKey(wrappers.module, "QuadrupoleKey"), [](auto &mod, auto &cls) {
        cls.def(py::init<>());
        cls.def(py::init<Key<double> const &, Key<double> const &, Key<double> const &>(), "ixx"_a, "iyy"_a,
                "ixy"_a);
        cls.def(py::init<SubSchema const &>());
        cls.def("__eq__", &QuadrupoleKey::operator==, py::is_operator());
        cls.def("__nq__", &QuadrupoleKey::operator!=, py::is_operator());
        cls.def_static("addFields", &QuadrupoleKey::addFields, "schema"_a, "name"_a, "doc"_a,
                       "coordType"_a = CoordinateType::PIXEL);
        cls.def("getIxx", &QuadrupoleKey::getIxx);
        cls.def("getIyy", &QuadrupoleKey::getIyy);
        cls.def("getIxy", &QuadrupoleKey::getIxy);
        cls.def("isValid", &QuadrupoleKey::isValid);
        cls.def("set", &QuadrupoleKey::set);
        cls.def("get", &QuadrupoleKey::get);
    });
}

static void declareEllipseKey(WrapperCollection &wrappers) {
    wrappers.wrapType(PyEllipseKey(wrappers.module, "EllipseKey"), [](auto &mod, auto &cls) {
        cls.def(py::init<>());
        cls.def(py::init<QuadrupoleKey const &, PointKey<double> const &>(), "qKey"_a, "pKey"_a);
        cls.def(py::init<SubSchema const &>());
        cls.def("__eq__", &EllipseKey::operator==, py::is_operator());
        cls.def("__nq__", &EllipseKey::operator!=, py::is_operator());
        cls.def_static("addFields", &EllipseKey::addFields, "schema"_a, "name"_a, "doc"_a, "unit"_a);
        cls.def("get", &EllipseKey::get);
        cls.def("set", &EllipseKey::set);
        cls.def("isValid", &EllipseKey::isValid);
        cls.def("getCore", &EllipseKey::getCore);
        cls.def("getCenter", &EllipseKey::getCenter);
    });
}

template <typename T, int N>
static void declareCovarianceMatrixKey(WrapperCollection &wrappers, const ::std::string &suffix) {
    wrappers.wrapType(
            PyCovarianceMatrixKey<T, N>(wrappers.module, ("CovarianceMatrix" + suffix + "Key").c_str()),
            [](auto &mod, auto &cls) {
                using ErrKeyArray = std::vector<Key<T>>;
                using CovarianceKeyArray = std::vector<Key<T>>;
                using NameArray = std::vector<std::string>;

                cls.def(py::init<>());
                // Ordering of the next two ctor declaration matters, as a workaround for DM-8580.
                cls.def(py::init<SubSchema const &, NameArray const &>());
                cls.def(py::init<ErrKeyArray const &, CovarianceKeyArray const &>(), "err"_a,
                        "cov"_a = CovarianceKeyArray());
                cls.def("__eq__", &CovarianceMatrixKey<T, N>::operator==, py::is_operator());
                cls.def("__ne__", &CovarianceMatrixKey<T, N>::operator!=, py::is_operator());
                cls.def_static("addFields",
                               (CovarianceMatrixKey<T, N>(*)(Schema &, std::string const &, NameArray const &,
                                                             std::string const &, bool)) &
                                       CovarianceMatrixKey<T, N>::addFields,
                               "schema"_a, "prefix"_a, "names"_a, "unit"_a, "diagonalOnly"_a = false);
                cls.def_static("addFields",
                               (CovarianceMatrixKey<T, N>(*)(Schema &, std::string const &, NameArray const &,
                                                             NameArray const &, bool)) &
                                       CovarianceMatrixKey<T, N>::addFields,
                               "schema"_a, "prefix"_a, "names"_a, "units"_a, "diagonalOnly"_a = false);
                cls.def("set", [](CovarianceMatrixKey<T, N> &cov, BaseRecord &record,
                                  Eigen::Matrix<T, N, N> const &value) { return cov.set(record, value); });
                cls.def("get", [](CovarianceMatrixKey<T, N> &cov, BaseRecord const &record) {
                    return cov.get(record);
                });
                cls.def("isValid", &CovarianceMatrixKey<T, N>::isValid);
                cls.def("setElement", &CovarianceMatrixKey<T, N>::setElement);
                cls.def("getElement", &CovarianceMatrixKey<T, N>::getElement);
            });
}

}  // namespace

void wrapAggregates(WrapperCollection &wrappers) {
    // TODO: uncomment once afw.geom uses WrapperCollection
    // wrappers.addSignatureDependency("lsst.afw.geom.ellipses");

    wrappers.wrapType(py::enum_<CoordinateType>(wrappers.module, "CoordinateType"), [](auto &mod, auto &enm) {
        enm.value("PIXEL", CoordinateType::PIXEL);
        enm.value("CELESTIAL", CoordinateType::CELESTIAL);
        enm.export_values();
    });

    declarePointKey<double>(wrappers, "2D");
    declarePointKey<int>(wrappers, "2I");
    declarePoint3Key<double>(wrappers, "D");
    declarePoint3Key<int>(wrappers, "I");

    declareBoxKey<lsst::geom::Box2D>(wrappers, "2D");
    declareBoxKey<lsst::geom::Box2I>(wrappers, "2I");

    declareCoordKey(wrappers);
    declareQuadrupoleKey(wrappers);
    declareEllipseKey(wrappers);

    declareCovarianceMatrixKey<float, 2>(wrappers, "2f");
    declareCovarianceMatrixKey<float, 3>(wrappers, "3f");
    declareCovarianceMatrixKey<float, 4>(wrappers, "4f");
    declareCovarianceMatrixKey<float, Eigen::Dynamic>(wrappers, "Xf");
    declareCovarianceMatrixKey<double, 2>(wrappers, "2d");
    declareCovarianceMatrixKey<double, 3>(wrappers, "3d");
    declareCovarianceMatrixKey<double, 4>(wrappers, "4d");
    declareCovarianceMatrixKey<double, Eigen::Dynamic>(wrappers, "Xd");
}

}  // namespace table
}  // namespace afw
}  // namespace lsst
