#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;


SUITE(DataTest)
{
    std::string file_path = __FILE__;
    auto mas_path = file_path.substr(0, file_path.rfind("/")).append("/../");

    static void loader(const json_uri & uri, json & schema)
    {
        std::string filename = mas_path + uri.path();
        std::ifstream lf(filename);
        if (!lf.good())
            throw std::invalid_argument("could not open " + uri.url() + " tried with " + filename);
        try
        {
            lf >> schema;
        }
        catch (const std::exception & e)
        {
            throw e;
        }
    }

    static void validate_ndjson(const std::string validator, const std::string database)
    {
        try
        {
            std::ifstream f(validator);
            json shape_schema = json::parse(f);

            json_validator validator(loader); // create validator
            try
            {
                validator.set_root_schema(shape_schema); // insert root-schema
            }
            catch (const std::exception & e)
            {
                std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
                return;
            }

            try
            {
                std::ifstream ndjson_file(database);
                std::string myline;
                while (std::getline(ndjson_file, myline))
                {
                    json jf = json::parse(myline);

                    try
                    {
                        validator.validate(jf); // validate the document - uses the default throwing error-handler
                    }
                    catch (const std::exception & e)
                    {
                        std::cerr << "Validation failed, here is why: " << e.what() << "\n";
                        break;
                    }
                }
            }
            catch (std::exception & e)
            {
                std::cerr << "Could not open and parse " << database << ": " << e.what() << "\n";
                return;
            }
        }
        catch (std::exception & e)
        {
            std::cerr << "Could not open and parse " << validator << ": " << e.what() << "\n";
            return;
        }
    }

    TEST(Shapes)
    {
        auto data_file_path = mas_path + "data/shapes.ndjson";
        auto schema_file_path = mas_path + "schemas/core/shape.json";
        validate_ndjson(schema_file_path, data_file_path);
    }

    TEST(Bobbins)
    {
        auto data_file_path = mas_path + "data/bobbins.ndjson";
        auto schema_file_path = mas_path + "schemas/core/bobbin.json";
        validate_ndjson(schema_file_path, data_file_path);
    }

    TEST(Materials)
    {
        auto data_file_path = mas_path + "data/materials.ndjson";
        auto schema_file_path = mas_path + "schemas/core/material.json";
        validate_ndjson(schema_file_path, data_file_path);
    }
}

int main(int, const char *[])
{
    return UnitTest::RunAllTests();
}
