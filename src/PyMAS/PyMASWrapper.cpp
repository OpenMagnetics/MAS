#include "pybind11_json.hpp"
#include <pybind11/pybind11.h>
#include "Core.cpp"
#include "json.hpp"

using json = nlohmann::json;


json get_piece_data(json shapeData){
    OpenMagnetics::CoreShape coreShape(shapeData);
    auto corePiece = OpenMagnetics::CorePiece::factory(coreShape);
    return corePiece;
}


json get_core_data(json coreData){
    OpenMagnetics::Core core(coreData);
    return core;
}


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;

PYBIND11_MODULE(PyMAS, m) {
    m.def("get_piece_data", &get_piece_data, "Returns the processed data from a core piece given its dimensions");
    m.def("get_core_data", &get_core_data, "Returns the processed data from a core");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
