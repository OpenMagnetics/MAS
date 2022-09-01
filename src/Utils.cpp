#include <nlohmann/json-schema.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <CoreTemplate.hpp>
#include "json.hpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;


namespace OpenMagnetics {
    template <typename T>
    T find_data_by_name(std::string name)
    {
        std::string database;
        if (std::is_same<T, OpenMagnetics::CoreShape>::value) 
            database = "shapes";
        else if (std::is_same<T, OpenMagnetics::CoreMaterial>::value)
            database = "material";
        else throw "Unknown type";

        std::string filePath = __FILE__;
        auto masPath = filePath.substr(0, filePath.rfind("/")).append("/../");
        auto dataFilePath = masPath + "data/" + database + ".ndjson";
        std::ifstream ndjsonFile(dataFilePath);
        std::string myline;
        while (std::getline(ndjsonFile, myline))
        {
            json jf = json::parse(myline);

            if (jf["name"] == name || std::find(jf["aliases"].begin(), jf["aliases"].end(), name) != jf["aliases"].end())
            {
                T datum(jf);
                return datum;
            }
        }

        T datum(json::parse("{}"));
        return datum;
    }

    template <int decimals>
    double roundFloat(double value){
        if (value < 0) 
            return floor(value * pow(10, decimals)) / pow(10, decimals);
        else
            return ceil(value * pow(10, decimals)) / pow(10, decimals);
    }
}