#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <streambuf>
#include <nlohmann/json-schema.hpp>
#include <CoreTemplate.hpp>
#include "json.hpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;


namespace OpenMagnetics {
    using nlohmann::json;

    class Shape {
        protected:
        EffectiveParameters effective_parameters;

        public:
            Shape() = default;
            ~Shape() = default;
            virtual EffectiveParameters & get_effective_parameters() = 0;
            // virtual int get_winding_window();
            // virtual int get_column_data();

            static Shape *factory(CoreShapeFamily family);
    };

    class E: public Shape
    {
        public:
            std::pair<double, double> get_c1_c2()
            {
                std::pair<double, double> c1_c2;
                return c1_c2;
            }

            EffectiveParameters & get_effective_parameters()
            {
                json json_effective_parameters;
                json_effective_parameters["effectiveArea"] = 1;
                json_effective_parameters["effectiveLength"] = 1;
                json_effective_parameters["effectiveVolume"] = 1;
                json_effective_parameters["minimumArea"] = 1;
                from_json(json_effective_parameters, effective_parameters);
                return effective_parameters;
            }
    };

    template <typename T>
    T find_data_by_name(std::string name)
    {
        std::string database;
        if (std::is_same<T, OpenMagnetics::CoreShape>::value) 
            database = "shapes";
        else if (std::is_same<T, OpenMagnetics::CoreMaterial>::value)
            database = "material";
        else throw "Unknown type";

        std::cout << "name: " << name << "\n";

        std::
        string file_path = __FILE__;
        auto mas_path = file_path.substr(0, file_path.rfind("/")).append("/../");
        auto data_file_path = mas_path + "data/" + database + ".ndjson";
        std::ifstream ndjson_file(database);
        std::string myline;
        std::getline(ndjson_file, myline);
        std::cout << "data_file_path: " << data_file_path << "\n";
        std::cout << "data_file_path2: " << myline << "\n";
        while (std::getline(ndjson_file, myline))
        {
            std::cout << "myline: " << myline << "\n";
            // json jf = json::parse(myline);

            // std::cout << "name: " << jf["name"] << "\n";
            // if (jf["name"] == name) {
            //     T datum(jf);
            //     return datum;
            // }
        }

        std::cout << "Finished" << "\n";
        T datum(json::parse("{}"));
        return datum;
    }

    Shape *Shape::factory(CoreShapeFamily family)
    {
        if (family == CoreShapeFamily::E)
            return new E;
        // else if (family == CoreShapeFamily::EC)
        //     return new EC;
        // else if (family == CoreShapeFamily::EFD)
        //     return new EFD;
        // else if (family == CoreShapeFamily::EL)
        //     return new EL;
        // else if (family == CoreShapeFamily::EP)
        //     return new EP;
        // else if (family == CoreShapeFamily::ER)
        //     return new ER;
        // else if (family == CoreShapeFamily::ETD)
        //     return new ETD;
        // else if (family == CoreShapeFamily::P)
        //     return new P;
        // else if (family == CoreShapeFamily::PLANAR_E)
        //     return new PLANAR_E;
        // else if (family == CoreShapeFamily::PLANAR_EL)
        //     return new PLANAR_EL;
        // else if (family == CoreShapeFamily::PLANAR_ER)
        //     return new PLANAR_ER;
        // else if (family == CoreShapeFamily::PM)
        //     return new PM;
        // else if (family == CoreShapeFamily::PQ)
        //     return new PQ;
        // else if (family == CoreShapeFamily::RM)
        //     return new RM;
        // else if (family == CoreShapeFamily::U)
        //     return new U;
        else throw "Unknown shape";
    }

    class Core:public CoreTemplate {
        public:
        Core() = default;
        virtual ~Core() = default;
        std::shared_ptr<ProcessedDescription> process_data() const { 
            json json_processed_description;
            json_processed_description["ea"] = json::array();
            json_processed_description["ea"].push_back("eaeaea");
            std::cout << "json_processed_description: " << json_processed_description << "\n";
            std::shared_ptr<ProcessedDescription> processed_description;
            return processed_description;
        }
    };
}


int main(int, const char *[])
{
    std::string file_path = __FILE__;
    auto mas_path = file_path.substr(0, file_path.rfind("/")).append("/../");
    auto core_file_path = mas_path + "samples/core/core_PQ_35_35_N97_additive.json";
    std::ifstream json_file(core_file_path);

    auto jf = json::parse(json_file);
    std::cout << jf["functionalDescription"] << "\n";
    OpenMagnetics::Core core(jf);

    core.process_data();

    OpenMagnetics::Shape *e = OpenMagnetics::Shape::factory(OpenMagnetics::CoreShapeFamily::E);
    auto effective_parameters = e->get_effective_parameters();

    // auto processed_description = *core.get_processed_description();
    if (std::holds_alternative<std::string>(core.get_mutable_functional_description().get_shape())) {
        std::cout << "Variant is of type std::string" << std::endl;
        auto name = std::get<std::string>(core.get_mutable_functional_description().get_shape());
        std::cout << "Shape with name: " << name << std::endl;
        auto shape_data = OpenMagnetics::find_data_by_name<OpenMagnetics::CoreShape>(name);
    }
    else{
        std::cout << "Variant is NOT of type std::string" << std::endl;
    }
    std::cout << *(core.get_mutable_functional_description().get_name()) << "\n";
    // std::cout << core.get_mutable_functional_description().get_shape() << "\n";
    std::cout << effective_parameters.get_effective_volume() << "\n";
}
