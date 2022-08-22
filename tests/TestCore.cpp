#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <nlohmann/json-schema.hpp>
#include "json.hpp"
#include "Core.cpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;
#include <typeinfo>


SUITE(processedDescription)
{
    std::string filePath = __FILE__;
    auto masPath = filePath.substr(0, filePath.rfind("/")).append("/../");

    TEST(E_55_21)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);

        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        core.process_data();
        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_55_21_N97_additive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000353 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.124, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 4.4e-05 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0007, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.037, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.01015, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0172, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.021 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00935, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.021 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00935, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.021 * numberStacks, 0.01);
    }

    TEST(E_55_28_21)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_28_21_3C95_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        core.process_data();
        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_55_28_21_3C95_additive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000353 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.124, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 4.4e-05 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0007, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.037, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.01015, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0172, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.021 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00935, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.021 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00935, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.021 * numberStacks, 0.01);
    }

    TEST(E_19_8_5)
    {
        auto coreFilePath = masPath + "samples/core/core_E_19_8_5_N87_substractive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        core.process_data();
        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_19_8_5_N87_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000225 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0396, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000891 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000221 * numberStacks, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0114, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00475, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0048, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0048 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00235, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0048 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00235, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0048 * numberStacks, 0.01);
    }
}

SUITE(geometricalDescription)
{
    std::string filePath = __FILE__;
    auto masPath = filePath.substr(0, filePath.rfind("/")).append("/../");

    TEST(E_19_8_5)
    {
        auto coreFilePath = masPath + "samples/core/core_E_19_8_5_N87_substractive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        core.process_data();
        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_19_8_5_N87_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000225 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0396, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000891 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000221 * numberStacks, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0114, 0.01);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00475, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0048, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0048 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00235, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0048 * numberStacks, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00235, 0.01);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0048 * numberStacks, 0.01);
    }
}