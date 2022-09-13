#include <UnitTest++.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <nlohmann/json-schema.hpp>
#include "json.hpp"
#include "Core.cpp"
#include <magic_enum.hpp>
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

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_55_21_N97_additive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000353 * numberStacks, 0.000353 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.124, 0.124 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 4.4e-05 * numberStacks, 4.4e-05 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.00035 * numberStacks, 0.00035 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.037, 0.037 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.01015, 0.01015 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0172, 0.0172 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00935, 0.00935 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00935, 0.00935 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(E_55_28_21)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_28_21_3C95_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_55_28_21_3C95_additive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000353 * numberStacks, 0.000353 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.124, 0.124 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 4.4e-05 * numberStacks, 4.4e-05 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.00035 * numberStacks, 0.00035 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.037, 0.037 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.01015, 0.01015 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0172, 0.0172 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00935, 0.00935 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00935, 0.00935 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.021 * numberStacks, 0.021 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(E_19_8_5)
    {
        auto coreFilePath = masPath + "samples/core/core_E_19_8_5_N87_substractive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_19_8_5_N87_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000225 * numberStacks, 0.0000225 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0396, 0.0396 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000891 * numberStacks, 0.000000891 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000221 * numberStacks, 0.0000221 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0114, 0.0114 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00475, 0.00475 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0048, 0.0048 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0048 * numberStacks, 0.0048 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00235, 0.00235 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0048 * numberStacks, 0.0048 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00235, 0.00235 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0048 * numberStacks, 0.0048 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(ETD_39_20_13)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_ETD_39_20_13_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ETD 39/20/13";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_ETD_39_20_13_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000125 * numberStacks, 0.000125 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0922, 0.0922 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000011500 * numberStacks, 0.000011500 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000123 * numberStacks, 0.000123 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0282, 0.0282 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00825, 0.00825 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0128, 0.0128 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0128 * numberStacks, 0.0128 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0048, 0.0048 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0128 * numberStacks, 0.0128 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0048, 0.0048 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0128 * numberStacks, 0.0128 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(ETD_19_14_8)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_ETD_19_14_8_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ETD 19/14/8";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_ETD_19_14_8_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000441 * numberStacks, 0.0000441 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0553, 0.0553 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000002440 * numberStacks, 0.000002440 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000395 * numberStacks, 0.0000395 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0184, 0.0184 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0034, 0.0034 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0076, 0.0076 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0076 * numberStacks, 0.0076 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00255, 0.00255 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0076 * numberStacks, 0.0076 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00255, 0.00255 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0076 * numberStacks, 0.0076 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(ETD_54_28_19)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_ETD_54_28_19_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ETD 54/28/19";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_ETD_54_28_19_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000280 * numberStacks, 0.000280 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.127, 0.127 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000035600 * numberStacks, 0.000035600 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000280 * numberStacks, 0.000280 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0396, 0.0396 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0104, 0.0104 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0193, 0.0193 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0193 * numberStacks, 0.0193 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0072, 0.0072 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0193 * numberStacks, 0.0193 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0072, 0.0072 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0193 * numberStacks, 0.0193 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(ER_54_18_18)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_ER_54_18_18_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ER 54/18/18";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_ER_54_18_18_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000256 * numberStacks, 0.000256 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.090, 0.090 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000023000 * numberStacks, 0.000023000 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000252 * numberStacks, 0.000252 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0216, 0.0216 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.01025, 0.01025 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0183, 0.0183 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0183 * numberStacks, 0.0183 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.007, 0.007 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0183 * numberStacks, 0.0183 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.007, 0.007 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0183 * numberStacks, 0.0183 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(ER_18_3_10)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_ER_18_3_10_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ER 18/3/10";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_ER_18_3_10_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000302 * numberStacks, 0.0000302 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0221, 0.0221 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000667 * numberStacks, 0.000000667 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000301 * numberStacks, 0.0000301 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0031, 0.0031 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0047, 0.0047 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0062, 0.0062 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0062 * numberStacks, 0.0062 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0012, 0.0012 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.01 * numberStacks, 0.01 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0012, 0.0012 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.01 * numberStacks, 0.01 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(E_102_20_38)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_E_102_20_38_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ELP 102/20/38";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 2;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_102_20_38_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000538 * numberStacks, 0.000538 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.1476, 0.1476 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000079410 * numberStacks, 0.000079410 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0005245 * numberStacks, 0.0005245 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0266, 0.0266 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.036, 0.036 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.014, 0.014 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0375 * numberStacks, 0.0375 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.008, 0.008 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0375 * numberStacks, 0.0375 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.008, 0.008 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0375 * numberStacks, 0.0375 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(E_14_3_5_5)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_E_14_3.5_5_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "ELP 14/3.5/5";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 3;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_14_3.5_5_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000143 * numberStacks, 0.0000143 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0207, 0.0207 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000296 * numberStacks, 0.000000296 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000139 * numberStacks, 0.0000139 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.004, 0.004 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.004, 0.004 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.003, 0.003 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.005 * numberStacks, 0.005 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0015, 0.0015 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.005 * numberStacks, 0.005 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0015, 0.0015 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.005 * numberStacks, 0.005 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(EL_25_4_3)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_E_25_4.3_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EL 25/4.3";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_25_4.3_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000856 * numberStacks, 0.0000856 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.030, 0.030 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000002570 * numberStacks, 0.000002570 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000083 * numberStacks, 0.000083 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.004, 0.004 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.007255, 0.007255 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.00632, 0.00632 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.01454 * numberStacks, 0.01454 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.002085, 0.002085 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.002085, 0.002085 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::OBLONG);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(EL_11_2)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_E_11_2_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EL 11/2";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_11_2_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000165 * numberStacks, 0.0000165 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0137, 0.0137 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000226 * numberStacks, 0.000000226 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000159 * numberStacks, 0.0000159 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.002, 0.002 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.003195, 0.003195 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.00278, 0.00278 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0064 * numberStacks, 0.0064 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.000915, 0.000915 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0088 * numberStacks, 0.0088 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.000915, 0.000915 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0088 * numberStacks, 0.0088 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::OBLONG);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(EC_70)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EC_70_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EC 70";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EC_70_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000280 * numberStacks, 0.000280 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.144, 0.144 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000040420 * numberStacks, 0.000040420 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000211 * numberStacks, 0.000211 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0455, 0.0455 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0141, 0.0141 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0164, 0.0164 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0164 * numberStacks, 0.0164 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.013, 0.013 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0164 * numberStacks, 0.0164 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.013, 0.013 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0164 * numberStacks, 0.0164 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EFD_10_5_3)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EFD_10_5_3_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EFD 10/5/3";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EFD_10_5_3_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000072 * numberStacks, 0.0000072 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0231, 0.0231 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000166 * numberStacks, 0.000000166 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000065 * numberStacks, 0.0000065 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0075, 0.0075 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00155, 0.00155 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.00455, 0.00455 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.00145 * numberStacks, 0.00145 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.001425, 0.001425 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0027 * numberStacks, 0.0027 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.001425, 0.001425 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0027 * numberStacks, 0.0027 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(EFD_30_15_9)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EFD_30_15_9_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EFD 30/15/9";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EFD_30_15_9_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000069 * numberStacks, 0.000069 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.068, 0.068 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000004690 * numberStacks, 0.000004690 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000069 * numberStacks, 0.000069 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0224, 0.0224 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0039, 0.0039 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0146, 0.0146 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0049 * numberStacks, 0.0049 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0038, 0.0038 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0091 * numberStacks, 0.0091 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0038, 0.0038 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0091 * numberStacks, 0.0091 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(EQ_30_8_20)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EQ_30_8_20_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EQ 30/8/20";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EQ_30_8_20_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000108 * numberStacks, 0.000108 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.046, 0.046 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000004970 * numberStacks, 0.000004970 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000095 * numberStacks, 0.000095 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0106, 0.0106 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0075, 0.0075 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.011, 0.011 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.011 * numberStacks, 0.011 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.002, 0.002 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.002, 0.002 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EPX_10)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EPX_10_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EPX 10";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EPX_10_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000159 * numberStacks, 0.0000159 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0217, 0.0217 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000345 * numberStacks, 0.000000345 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000132 * numberStacks, 0.0000132 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0072, 0.0072 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.002825, 0.002825 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 3u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.00345, 0.00345 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.00485 * numberStacks, 0.00485 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.00785 * numberStacks, 0.00785 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.00785 * numberStacks, 0.00785 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::OBLONG);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EPX_7)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EPX_7_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EPX 7";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EPX_7_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000172 * numberStacks, 0.0000172 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0157, 0.0157 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000270 * numberStacks, 0.000000270 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000139 * numberStacks, 0.0000139 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0045, 0.0045 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0019, 0.0019 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0034, 0.0034 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0057 * numberStacks, 0.0057 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0014 * numberStacks, 0.0014 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::OBLONG);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EPO_13)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EPO_13_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EPO 13";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EPO_13_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000193 * numberStacks, 0.0000193 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0258, 0.0258 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000498 * numberStacks, 0.000000498 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000149 * numberStacks, 0.0000149 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.009, 0.009 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0026, 0.0026 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 3u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0045, 0.0045 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0045 * numberStacks, 0.0045 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0074 * numberStacks, 0.0074 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.0074 * numberStacks, 0.0074 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(LP_42_25_15_8)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_LP_42_25_15_8_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "LP 42/25/15.8";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_LP_42_25_15_8_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000206 * numberStacks, 0.000206 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0901, 0.0901 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000018560 * numberStacks, 0.000018560 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000206 * numberStacks, 0.000206 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0228, 0.0228 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0095, 0.0095 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 3u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0162, 0.0162 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0162 * numberStacks, 0.0162 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.025 * numberStacks, 0.025 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.025 * numberStacks, 0.025 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EP_7)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EP_7_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EP 7";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EP_7_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000103 * numberStacks, 0.0000103 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0157, 0.0157 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000162 * numberStacks, 0.000000162 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000085 * numberStacks, 0.0000085 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.005, 0.005 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0019, 0.0019 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0034, 0.0034 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0034 * numberStacks, 0.0034 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.048, 0.048 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.00095 * numberStacks, 0.00095 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(EP_20)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_EP_20_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "EP 20";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_EP_20_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000078 * numberStacks, 0.000078 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.040, 0.040 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000003120 * numberStacks, 0.000003120 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000060 * numberStacks, 0.000060 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.014, 0.014 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00355, 0.00355 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.009, 0.009 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.009 * numberStacks, 0.009 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.129, 0.129 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.00275 * numberStacks, 0.00275 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(RM_14)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_RM_14_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "RM 14";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_RM_14_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000200 * numberStacks, 0.000200 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.070, 0.070 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000014000 * numberStacks, 0.000014000 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000170 * numberStacks, 0.000170 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0208, 0.0208 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.007, 0.007 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.015, 0.015 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.015 * numberStacks, 0.015 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0066 * numberStacks, 0.0066 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.0066 * numberStacks, 0.0066 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(RM_7LP)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_RM_7LP_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "RM 7LP";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_RM_7LP_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000040 * numberStacks, 0.000040 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0235, 0.0235 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000001190 * numberStacks, 0.000001190 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000323 * numberStacks, 0.0000323 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0047, 0.0047 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00375, 0.00375 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.00725, 0.00725 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.00725 * numberStacks, 0.00725 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.002775 * numberStacks, 0.002775 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.002775 * numberStacks, 0.002775 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(PQ_20_16)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_PQ_20_16_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "PQ 20/16";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_PQ_20_16_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000632 * numberStacks, 0.0000632 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0372, 0.0372 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000002360 * numberStacks, 0.000002360 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000544 * numberStacks, 0.0000544 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.01030, 0.01030 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0046, 0.0046 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0088, 0.0088 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0088 * numberStacks, 0.0088 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.014 * numberStacks, 0.014 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.014 * numberStacks, 0.014 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(PQ_107_87)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_PQ_107_87_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "PQ 107/87";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_PQ_107_87_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.001428 * numberStacks, 0.001428 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.204, 0.204 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000290600 * numberStacks, 0.000290600 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.001320 * numberStacks, 0.001320 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.056, 0.056 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.02635, 0.02635 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.041, 0.041 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.041 * numberStacks, 0.041 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.070 * numberStacks, 0.070 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_depth(), 0.070 * numberStacks, 0.070 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(PM_114_93)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_PM_114_93_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "PM 114/93";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_PM_114_93_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.001720 * numberStacks, 0.001720 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.200, 0.200 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000344000 * numberStacks, 0.000344000 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.001380 * numberStacks, 0.001380 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.063, 0.063 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0225, 0.0225 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.043, 0.043 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.043 * numberStacks, 0.043 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.013 * numberStacks, 0.013 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.013 * numberStacks, 0.013 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(P_150_30)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_P_150_30_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "P 150/30";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_P_150_30_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.003580 * numberStacks, 0.003580 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.160, 0.160 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.00056600 * numberStacks, 0.00056600 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.002800 * numberStacks, 0.002800 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.030, 0.030 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0325, 0.0325 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.065, 0.065 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.065 * numberStacks, 0.065 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.010 * numberStacks, 0.010 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.010 * numberStacks, 0.00515 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(P_11_7)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_P_11_7_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "P 11/7";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_P_11_7_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.0000162 * numberStacks, 0.0000162 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.0155, 0.0155 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000251 * numberStacks, 0.000000251 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.0000132 * numberStacks, 0.0000132 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.00440, 0.00440 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.00215, 0.00215 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0047, 0.0047 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0047 * numberStacks, 0.0047 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00105 * numberStacks, 0.00105 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00105 * numberStacks, 0.00105 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(P_7_4)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_P_7_4_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "P 7/4";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_P_7_4_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000007 * numberStacks, 0.000007 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.010, 0.010 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000000070 * numberStacks, 0.000000070 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000006 * numberStacks, 0.000006 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0028, 0.0028 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0014, 0.0014 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.003, 0.003 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.003 * numberStacks, 0.003 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.00075 * numberStacks, 0.00075 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[2].get_width(), 0.00075 * numberStacks, 0.00075 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
        CHECK(core.get_processed_description()->get_columns()[2].get_shape() == OpenMagnetics::ColumnShape::IRREGULAR);
    }

    TEST(U_79_129_31)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_U_79_129_31_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "U 79/129/31";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_U_79_129_31_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000693 * numberStacks, 0.000693 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.309, 0.309 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000214220 * numberStacks, 0.000214220 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000693 * numberStacks, 0.000693 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.085, 0.085 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.034, 0.034 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.022, 0.022 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0315 * numberStacks, 0.0315 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.022 * numberStacks, 0.022 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.0315 * numberStacks, 0.0315 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(U_26_22_16)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_U_26_22_16_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "U 26/22/16";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_U_26_22_16_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000131 * numberStacks, 0.000131 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.098, 0.098 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000012800 * numberStacks, 0.000012800 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000129 * numberStacks, 0.000129 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.026, 0.026 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.009, 0.009 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0084, 0.0084 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.016 * numberStacks, 0.016 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.0084 * numberStacks, 0.0084 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.016 * numberStacks, 0.016 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(UR_48_39_17)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_UR_48_39_17_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "UR 48/39/17";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_UR_48_39_17_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000215 * numberStacks, 0.000215 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.186, 0.186 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000039990 * numberStacks, 0.000039990 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000215 * numberStacks, 0.000215 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0538, 0.0538 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0174, 0.0174 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.017, 0.017 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.017 * numberStacks, 0.017 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.013 * numberStacks, 0.013 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.017 * numberStacks, 0.017 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(UR_70_33_17)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_UR_70_33_17_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "UR 70/33/17";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_UR_70_33_17_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000214 * numberStacks, 0.000214 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.197, 0.197 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000043800 * numberStacks, 0.000043800 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000214 * numberStacks, 0.000214 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.0381, 0.0381 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.035, 0.035 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.01725, 0.01725 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.01725 * numberStacks, 0.01725 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.01725 * numberStacks, 0.01725 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.01725 * numberStacks, 0.01725 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::ROUND);
    }

    TEST(UR_55_39_36)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_UR_55_39_36_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "UR 55/38/36";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_UR_55_39_36_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000418 * numberStacks, 0.000418 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.188, 0.188 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000078570 * numberStacks, 0.000078570 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000418 * numberStacks, 0.000418 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.051, 0.051 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0196, 0.0196 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.0235, 0.0235 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.0235 * numberStacks, 0.0235 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.012 * numberStacks, 0.012 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.036 * numberStacks, 0.036 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    }

    TEST(UR_64_40_20)
    {
        json coreJson;
        coreJson["functionalDescription"] = json();
        coreJson["functionalDescription"]["name"] = "core_UR_64_40_20_N97_substractive";
        coreJson["functionalDescription"]["type"] = "two-piece set";
        coreJson["functionalDescription"]["material"] = "N97";
        coreJson["functionalDescription"]["shape"] = "UR 64/40/20";
        coreJson["functionalDescription"]["gapping"] = json::array();
        coreJson["functionalDescription"]["numberStacks"] = 1;
        OpenMagnetics::Core core(coreJson);
        double numberStacks = coreJson["functionalDescription"]["numberStacks"];

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_UR_64_40_20_N97_substractive");
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000290 * numberStacks, 0.000290 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.210, 0.210 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.000061000 * numberStacks, 0.000061000 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000290 * numberStacks, 0.000290 * numberStacks * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.053, 0.053 * 0.2);
        CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0232, 0.0232 * 0.2);
        CHECK_EQUAL(core.get_processed_description()->get_columns().size(), 2u);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.020, 0.020 * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.020 * numberStacks, 0.020 * numberStacks * 0.2);
        CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::ROUND);
        CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::ROUND);
    }

    // TEST(UI_93_76_20)
    // {
    //     json coreJson;
    //     coreJson["functionalDescription"] = json();
    //     coreJson["functionalDescription"]["name"] = "core_UI_93_76_20_N97_substractive";
    //     coreJson["functionalDescription"]["type"] = "piece and place set";
    //     coreJson["functionalDescription"]["material"] = "N97";
    //     coreJson["functionalDescription"]["shape"] = "UI 93/76/20";
    //     coreJson["functionalDescription"]["gapping"] = json::array();
    //     coreJson["functionalDescription"]["numberStacks"] = 1;
    //     OpenMagnetics::Core core(coreJson);
    //     double numberStacks = coreJson["functionalDescription"]["numberStacks"];

    //     CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_UI_93_76_20_N97_substractive");
    //     CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_area(), 0.000560 * numberStacks, 0.000560 * numberStacks * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_length(), 0.258, 0.258 * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_effective_volume(), 0.00014400 * numberStacks, 0.00014400 * numberStacks * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_effective_parameters().get_minimum_area(), 0.000560 * numberStacks, 0.000560 * numberStacks * 0.2);
    //     CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_height()), 0.048, 0.048 * 0.2);
    //     CHECK_CLOSE(*(core.get_processed_description()->get_winding_windows()[0].get_width()), 0.0346, 0.0346 * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_width(), 0.028, 0.028 * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_columns()[0].get_depth(), 0.20 * numberStacks, 0.20 * numberStacks * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_width(), 0.028 * numberStacks, 0.028 * numberStacks * 0.2);
    //     CHECK_CLOSE(core.get_processed_description()->get_columns()[1].get_depth(), 0.20 * numberStacks, 0.20 * numberStacks * 0.2);
    //     CHECK(core.get_processed_description()->get_columns()[0].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    //     CHECK(core.get_processed_description()->get_columns()[1].get_shape() == OpenMagnetics::ColumnShape::RECTANGULAR);
    // }
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

        auto geometrical_description = *(core.get_geometrical_description());

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_19_8_5_N87_substractive");
        CHECK_EQUAL(geometrical_description.size(), 2u);
        CHECK(geometrical_description[0].get_machining() != nullptr);
        CHECK(geometrical_description[1].get_machining() == nullptr);
        CHECK(geometrical_description[0].get_type() == OpenMagnetics::GeometricalDescriptionType::HALF_SET);
        CHECK(geometrical_description[1].get_type() == OpenMagnetics::GeometricalDescriptionType::HALF_SET);
    }

    TEST(E_55_21)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        OpenMagnetics::Core core(coreJson);

        auto geometrical_description = *(core.get_geometrical_description());

        CHECK_EQUAL(*(core.get_mutable_functional_description().get_name()), "core_E_55_21_N97_additive");
        CHECK_EQUAL(geometrical_description.size(), 6u);
        CHECK(geometrical_description[0].get_machining() == nullptr);
        CHECK(geometrical_description[1].get_machining() == nullptr);
        CHECK(geometrical_description[2].get_machining() == nullptr);
        CHECK(geometrical_description[3].get_machining() == nullptr);
        CHECK(geometrical_description[4].get_machining() == nullptr);
        CHECK(geometrical_description[5].get_machining() == nullptr);
        CHECK(geometrical_description[4].get_type() == OpenMagnetics::GeometricalDescriptionType::SPACER);
        CHECK(geometrical_description[5].get_type() == OpenMagnetics::GeometricalDescriptionType::SPACER);
    }
}

SUITE(functionalDescription)
{
    std::string filePath = __FILE__;
    auto masPath = filePath.substr(0, filePath.rfind("/")).append("/../");

    TEST(E_55_21_all_gaps_residual)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        coreJson["functionalDescription"]["gapping"][0] = coreJson["functionalDescription"]["gapping"][1];

        OpenMagnetics::Core core(coreJson);

        auto function_description = core.get_functional_description();

        CHECK_EQUAL(function_description.get_gapping().size(), 3u);
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[1].get_type());
        CHECK(*(function_description.get_gapping()[0].get_shape()) == *(function_description.get_gapping()[1].get_shape()));
        CHECK(*(function_description.get_gapping()[0].get_distance_closest_normal_surface()) == *(function_description.get_gapping()[1].get_distance_closest_normal_surface()));
        CHECK(function_description.get_gapping()[0].get_length() == function_description.get_gapping()[1].get_length());
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.2);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == 0);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[1].get_coordinates())[0] == -(*function_description.get_gapping()[2].get_coordinates())[0]);
    }

    TEST(E_55_21_central_gap)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);

        OpenMagnetics::Core core(coreJson);

        auto function_description = core.get_functional_description();

        CHECK_EQUAL(function_description.get_gapping().size(), 3u);
        CHECK(function_description.get_gapping()[0].get_type() != function_description.get_gapping()[1].get_type());
        CHECK(*(function_description.get_gapping()[0].get_shape()) == *(function_description.get_gapping()[1].get_shape()));
        CHECK(*(function_description.get_gapping()[0].get_distance_closest_normal_surface()) != *(function_description.get_gapping()[1].get_distance_closest_normal_surface()));
        CHECK(function_description.get_gapping()[0].get_length() != function_description.get_gapping()[1].get_length());
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.2);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == 0);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] != 0);
        CHECK((*function_description.get_gapping()[1].get_coordinates())[0] == -(*function_description.get_gapping()[2].get_coordinates())[0]);
    }

    TEST(E_55_21_gap_all_columns)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        coreJson["functionalDescription"]["gapping"][1] = coreJson["functionalDescription"]["gapping"][0];
        coreJson["functionalDescription"]["gapping"][2] = coreJson["functionalDescription"]["gapping"][0];

        OpenMagnetics::Core core(coreJson);

        auto function_description = core.get_functional_description();

        CHECK_EQUAL(function_description.get_gapping().size(), 3u);
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[1].get_type());
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[2].get_type());
        CHECK(*(function_description.get_gapping()[0].get_shape()) == *(function_description.get_gapping()[1].get_shape()));
        CHECK(*(function_description.get_gapping()[0].get_distance_closest_normal_surface()) == *(function_description.get_gapping()[1].get_distance_closest_normal_surface()));
        CHECK(function_description.get_gapping()[0].get_length() == function_description.get_gapping()[1].get_length());
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.2);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == 0);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[1].get_coordinates())[0] == -(*function_description.get_gapping()[2].get_coordinates())[0]);
    }

    TEST(E_55_21_central_distributed_gap_even)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        coreJson["functionalDescription"]["gapping"].push_back(coreJson["functionalDescription"]["gapping"][0]);

        OpenMagnetics::Core core(coreJson);

        auto function_description = core.get_functional_description();

        CHECK_EQUAL(function_description.get_gapping().size(), 4u);
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[1].get_type());
        CHECK(function_description.get_gapping()[0].get_type() != function_description.get_gapping()[2].get_type());
        CHECK(*(function_description.get_gapping()[0].get_shape()) == *(function_description.get_gapping()[1].get_shape()));
        CHECK(*(function_description.get_gapping()[0].get_distance_closest_normal_surface()) == *(function_description.get_gapping()[1].get_distance_closest_normal_surface()));
        CHECK(*(function_description.get_gapping()[0].get_distance_closest_normal_surface()) != *(function_description.get_gapping()[2].get_distance_closest_normal_surface()));
        CHECK(function_description.get_gapping()[0].get_length() == function_description.get_gapping()[1].get_length());
        CHECK(function_description.get_gapping()[0].get_length() != function_description.get_gapping()[2].get_length());
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()), 0.2);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[2].get_area()) * 2, 0.2);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == (*function_description.get_gapping()[1].get_coordinates())[0]);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] == -(*function_description.get_gapping()[1].get_coordinates())[1]);
        CHECK((*function_description.get_gapping()[2].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[2].get_coordinates())[0] == -(*function_description.get_gapping()[3].get_coordinates())[0]);
    }

    TEST(E_55_21_central_distributed_gap_odd)
    {
        auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
        std::ifstream json_file(coreFilePath);

        auto coreJson = json::parse(json_file);
        coreJson["functionalDescription"]["gapping"].push_back(coreJson["functionalDescription"]["gapping"][0]);
        coreJson["functionalDescription"]["gapping"].push_back(coreJson["functionalDescription"]["gapping"][0]);

        OpenMagnetics::Core core(coreJson);

        auto function_description = core.get_functional_description();

        CHECK_EQUAL(function_description.get_gapping().size(), 5u);
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[1].get_type());
        CHECK(function_description.get_gapping()[0].get_type() == function_description.get_gapping()[2].get_type());
        CHECK(function_description.get_gapping()[0].get_type() != function_description.get_gapping()[3].get_type());
        CHECK(*(function_description.get_gapping()[0].get_shape()) == *(function_description.get_gapping()[1].get_shape()));
        CHECK(*(function_description.get_gapping()[1].get_distance_closest_normal_surface()) > *(function_description.get_gapping()[0].get_distance_closest_normal_surface()));
        CHECK(*(function_description.get_gapping()[1].get_distance_closest_normal_surface()) > *(function_description.get_gapping()[2].get_distance_closest_normal_surface()));
        CHECK(*(function_description.get_gapping()[1].get_distance_closest_normal_surface()) < *(function_description.get_gapping()[3].get_distance_closest_normal_surface()));
        CHECK(function_description.get_gapping()[0].get_length() == function_description.get_gapping()[1].get_length());
        CHECK(function_description.get_gapping()[0].get_length() == function_description.get_gapping()[2].get_length());
        CHECK(function_description.get_gapping()[0].get_length() != function_description.get_gapping()[3].get_length());
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()), 0.2);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[2].get_area()), 0.2);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[3].get_area()) * 2, 0.2);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == (*function_description.get_gapping()[1].get_coordinates())[0]);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == (*function_description.get_gapping()[2].get_coordinates())[0]);
        CHECK((*function_description.get_gapping()[1].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] == -(*function_description.get_gapping()[2].get_coordinates())[1]);
        CHECK((*function_description.get_gapping()[3].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[3].get_coordinates())[0] == -(*function_description.get_gapping()[4].get_coordinates())[0]);
    }
}