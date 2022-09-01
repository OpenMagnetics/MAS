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
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.01);
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
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.01);
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
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()) * 2, 0.01);
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
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()), 0.01);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[2].get_area()) * 2, 0.01);
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
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[1].get_area()), 0.01);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[2].get_area()), 0.01);
        CHECK_CLOSE(*(function_description.get_gapping()[0].get_area()), *(function_description.get_gapping()[3].get_area()) * 2, 0.01);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == (*function_description.get_gapping()[1].get_coordinates())[0]);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[0] == (*function_description.get_gapping()[2].get_coordinates())[0]);
        CHECK((*function_description.get_gapping()[1].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[0].get_coordinates())[1] == -(*function_description.get_gapping()[2].get_coordinates())[1]);
        CHECK((*function_description.get_gapping()[3].get_coordinates())[1] == 0);
        CHECK((*function_description.get_gapping()[3].get_coordinates())[0] == -(*function_description.get_gapping()[4].get_coordinates())[0]);
    }
}