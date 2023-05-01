#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <nlohmann/json-schema.hpp>
#include "json.hpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;


SUITE(Samples)
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
    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }

    static void validate_json(const std::string samples)
    {
        try
        {
            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(samples))
            {
                auto path = dir_entry.path();
                if (path.string().ends_with(".json")) {
                    std::vector<std::string> v(path.begin(), path.end());

                    auto validator_path = path.parent_path().string() + ".json";
                    validator_path = ReplaceAll(validator_path, "samples", "schemas");
                    
                    try
                    {
                        std::ifstream f(validator_path);
                        json shape_schema = json::parse(f);

                        json_validator validator(loader); // create validator
                        try
                        {
                            validator.set_root_schema(shape_schema); // insert root-schema
                        }
                        catch (const std::exception & e)
                        {
                            std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
                            CHECK(false); // fails
                            return;
                        }

                        std::ifstream json_file(dir_entry.path());
                        // std::cout << dir_entry.path() << std::endl;
                        auto jf = json::parse(json_file);
                        try
                        {
                            validator.validate(jf); // validate the document - uses the default throwing error-handler
                        }
                        catch (const std::exception & e)
                        {
                            std::cerr << "Validation failed, here is why: " << e.what() << "\n";
                            CHECK(false); // fails
                            return;
                        }

                    }
                    catch (std::exception & e)
                    {
                        std::cerr << "Could not open and parse " << validator_path << ": " << e.what() << "\n";
                        CHECK(false); // fails
                        return;
                    }
                }
            }
        }
        catch (std::exception & e)
        {
            std::cerr << "Could not open and parse " << samples << ": " << e.what() << "\n";
            CHECK(false); // fails
            return;
        }
    }

    TEST(AllSamples)
    {
        auto samples_file_path = mas_path + "samples/";
        validate_json(samples_file_path);
    }
}


SUITE(Data)
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
                CHECK(false); // fails
                return;
            }

            try
            {
                // std::cout << database << std::endl;
                std::ifstream ndjson_file(database);
                std::string myline;

                while (std::getline(ndjson_file, myline))
                {
                    // std::cout << myline << std::endl;
                    json jf = json::parse(myline);

                    try
                    {
                        validator.validate(jf); // validate the document - uses the default throwing error-handler
                    }
                    catch (const std::exception & e)
                    {
                        std::cerr << "Validation failed, here is why: " << e.what() << "\n";
                        CHECK(false); // fails
                        break;
                    }
                }
            }
            catch (std::exception & e)
            {
                std::cerr << "Could not open and parse " << database << ": " << e.what() << "\n";
                CHECK(false); // fails
                return;
            }
        }
        catch (std::exception & e)
        {
            std::cerr << "Could not open and parse " << validator << ": " << e.what() << "\n";
            CHECK(false); // fails
            return;
        }
    }

    TEST(Shapes)
    {
        auto data_file_path = mas_path + "data/shapes.ndjson";
        auto schema_file_path = mas_path + "schemas/magnetic/core/shape.json";
        validate_ndjson(schema_file_path, data_file_path);
    }

    TEST(Bobbins)
    {
        auto data_file_path = mas_path + "data/bobbins.ndjson";
        auto schema_file_path = mas_path + "schemas/magnetic/bobbin.json";
        validate_ndjson(schema_file_path, data_file_path);
    }

    TEST(Materials)
    {
        auto data_file_path = mas_path + "data/materials.ndjson";
        auto schema_file_path = mas_path + "schemas/magnetic/core/material.json";
        validate_ndjson(schema_file_path, data_file_path);
    }

    TEST(Wires)
    {
        auto data_file_path = mas_path + "data/wires.ndjson";
        auto schema_file_path = mas_path + "schemas/magnetic/wire.json";
        validate_ndjson(schema_file_path, data_file_path);
    }
}
