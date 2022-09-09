#include <fstream>
#include <numbers>
#include <iostream>
#include <cmath>
#include <vector>
#include <filesystem>
#include <streambuf>
#include <nlohmann/json-schema.hpp>
#include <CoreTemplate.hpp>
#include <magic_enum.hpp>
#include "json.hpp"
#include "Utils.cpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;


namespace OpenMagnetics {
    using nlohmann::json;
    using ColumnShape = ShapeEnum;

    const double residualGap = 0.00001;

    enum class DimensionalValues : int { MAXIMUM, NOMINAL, MINIMUM };

    template <OpenMagnetics::DimensionalValues preferredValue> 
    double resolve_dimensional_values(OpenMagnetics::Dimension dimensionValue)
    {
        double doubleValue;
        if (std::holds_alternative<OpenMagnetics::Utils>(dimensionValue)) {
            switch (preferredValue){
                case OpenMagnetics::DimensionalValues::MAXIMUM:
                    if (std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum());
                    break;
                case OpenMagnetics::DimensionalValues::NOMINAL:
                    if (std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum() != nullptr && std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum() != nullptr)
                        doubleValue = (*(std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum()) + *(std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum())) / 2;
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum());
                    break;
                case OpenMagnetics::DimensionalValues::MINIMUM:
                    if (std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_minimum());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_nominal());
                    else if (std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum() != nullptr)
                        doubleValue = *(std::get<OpenMagnetics::Utils>(dimensionValue).get_maximum());
                    break;
                default: throw std::runtime_error("Unknown type of dimension, options are {MAXIMUM, NOMINAL, MINIMUM}");

            }
        }
        else {
            doubleValue = std::get<double>(dimensionValue);

        }
        return doubleValue;
    }

    class CorePiece {
        private:
            std::vector<ColumnElement> columns;
            double depth;
            double height;
            double width;
            CoreShape shape;
            WindingWindowElement winding_window;
            EffectiveParameters partial_effective_parameters;
        protected:
            using dimensions_map = std::shared_ptr<std::map<std::string, Dimension>>;
        public:
            virtual std::tuple<double, double, double> get_shape_constants() = 0;
            virtual void process_columns() = 0;
            virtual void process_winding_window() = 0;
            virtual void process_extra_data() = 0;

            void flatten_dimensions()
            {
                for ( auto &dimension : *get_mutable_shape().get_dimensions() ) {
                    dimension.second = resolve_dimensional_values<OpenMagnetics::DimensionalValues::NOMINAL>(dimension.second);
                }
            }

            /**
             * List of columns in the piece
             */
            const std::vector<ColumnElement> & get_columns() const { return columns; }
            std::vector<ColumnElement> & get_mutable_columns() { return columns; }
            void set_columns(const std::vector<ColumnElement> & value) { this->columns = value; }

            /**
             * Total depth of the piece
             */
            const double & get_depth() const { return depth; }
            void set_depth(const double & value) { this->depth = value; }

            /**
             * Total height of the piece
             */
            const double & get_height() const { return height; }
            void set_height(const double & value) { this->height = value; }

            /**
             * Total width of the piece
             */
            const double & get_width() const { return width; }
            void set_width(const double & value) { this->width = value; }

            /**
             * List of winding windows, all elements in the list must be of the same type
             */
            const WindingWindowElement & get_winding_window() const { return winding_window; }
            WindingWindowElement & get_mutable_winding_window() { return winding_window; }
            void set_winding_window(const WindingWindowElement & value) { this->winding_window = value; }

            const CoreShape get_shape() const { return shape; }
            CoreShape get_mutable_shape() { return shape; }
            void set_shape(CoreShape value) { this->shape = value; }

            const EffectiveParameters & get_partial_effective_parameters() const { return partial_effective_parameters; }
            EffectiveParameters & get_mutable_partial_effective_parameters() { return partial_effective_parameters; }
            void set_partial_effective_parameters(const EffectiveParameters & value) { this->partial_effective_parameters = value; }


            static std::shared_ptr<CorePiece> factory(CoreShape shape);

            void process()
            {
                flatten_dimensions();
                process_winding_window();
                process_columns();
                process_extra_data();

                auto [c1, c2, minimumArea] = get_shape_constants();
                json pieceEffectiveParameters;
                pieceEffectiveParameters["effectiveLength"] = pow(c1, 2) / c2;
                pieceEffectiveParameters["effectiveArea"] = c1 / c2;
                pieceEffectiveParameters["effectiveVolume"] = pow(c1, 3) / pow(c2, 2);
                pieceEffectiveParameters["minimumArea"] = minimumArea;
                set_partial_effective_parameters(pieceEffectiveParameters);
            }
    };

    class E: public CorePiece
    {
        public:
            void process_extra_data()
            {
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["C"]));
            }

            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(jsonMainColumn["width"].get<double>() * jsonMainColumn["depth"].get<double>());
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["area"] = roundFloat<6>(jsonLateralColumn["width"].get<double>() * jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths;
                std::vector<double> areas;

                double h = std::get<double>(dimensions["B"]) - std::get<double>(dimensions["D"]);
                double q = std::get<double>(dimensions["C"]);
                double s = std::get<double>(dimensions["F"]) / 2;
                double p = (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2;


                lengths.push_back(std::get<double>(dimensions["D"]));
                lengths.push_back((std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2);
                lengths.push_back(std::get<double>(dimensions["D"]));
                lengths.push_back(std::numbers::pi / 8 * (p + h));
                lengths.push_back(std::numbers::pi / 8 * (s + h));

                areas.push_back(2 * q * p);
                areas.push_back(2 * q * h);
                areas.push_back(2 * s * q);
                areas.push_back((areas[0] + areas[1]) / 2);
                areas.push_back((areas[1] + areas[2]) / 2);

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths.size(); ++i)
                {
                    c1 += lengths[i] / areas[i];
                    c2 += lengths[i] / pow(areas[i], 2);
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class ETD: public E
    {
        public:
            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();
                double tetha;
                double aperture;
                if (dimensions.find("G") == dimensions.end()) {
                    tetha = asin(std::get<double>(dimensions["C"]) / std::get<double>(dimensions["E"]));
                    aperture = std::get<double>(dimensions["E"]) / 2 * cos(tetha);
                } else {
                    aperture = std::get<double>(dimensions["G"]) / 2;
                    tetha = acos(aperture / (std::get<double>(dimensions["E"]) / 2));
                }
                double segmentArea = pow(std::get<double>(dimensions["E"]) / 2, 2) / 2 * (2 * tetha - sin(2 * tetha));
                double area = std::get<double>(dimensions["C"]) * (std::get<double>(dimensions["A"]) / 2 - aperture) - segmentArea;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["width"] = roundFloat<6>(std::get<double>(dimensions["A"]) / 2 - std::get<double>(dimensions["E"]) / 2);
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths;
                std::vector<double> areas;

                double h = std::get<double>(dimensions["B"]) - std::get<double>(dimensions["D"]);
                double q = std::get<double>(dimensions["C"]);
                double s = std::get<double>(dimensions["F"]) / 2;
                double s1 = 0.5959 * s;
                double p = get_lateral_leg_area() / std::get<double>(dimensions["C"]);

                lengths.push_back(std::get<double>(dimensions["D"]));
                lengths.push_back((std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2);
                lengths.push_back(std::get<double>(dimensions["D"]));
                lengths.push_back(std::numbers::pi / 8 * (p + h));
                lengths.push_back(std::numbers::pi / 8 * (2 * s1 + h));

                areas.push_back(2 * q * p);
                areas.push_back(2 * q * h);
                areas.push_back(std::numbers::pi * pow(s, 2));
                areas.push_back((areas[0] + areas[1]) / 2);
                areas.push_back((areas[1] + areas[2]) / 2);

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths.size(); ++i)
                {
                    c1 += lengths[i] / areas[i];
                    c2 += lengths[i] / pow(areas[i], 2);
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class EL: public E
    {
        public:
            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F1"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F1"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::OBLONG;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F1"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F2"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2) + (std::get<double>(dimensions["F2"]) - std::get<double>(dimensions["F1"])) * std::get<double>(dimensions["F1"]));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["area"] = roundFloat<6>(jsonLateralColumn["width"].get<double>() * jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths;
                std::vector<double> areas;

                double a = std::get<double>(dimensions["A"]);
                double b = std::get<double>(dimensions["B"]);
                double c = std::get<double>(dimensions["C"]);
                double d = std::get<double>(dimensions["D"]);
                double e = std::get<double>(dimensions["E"]);
                double f1 = std::get<double>(dimensions["F1"]);
                double f2 = std::get<double>(dimensions["F2"]);
                double r = std::get<double>(dimensions["R"]);
                double a21 = (b - d) * c;
                double a23 = (f2 - f1 + std::numbers::pi * f1 / 2) * (b - d);
                double a3 = 1. / 2 * (1. / 4 * std::numbers::pi * pow(f1, 2) + (f2 - f1) * f1);

                lengths.push_back(d);
                lengths.push_back(e / 2 - f1 / 2);
                lengths.push_back(d);
                lengths.push_back(std::numbers::pi / 8 * (a / 2 - e / 2 + b - d));
                lengths.push_back(std::numbers::pi / 8 * (a3 / f2 + b - d));

                areas.push_back(1. / 2 * (a - e) * c - 4 * (pow(r, 2) - 1. / 4 * std::numbers::pi * pow(r, 2)));
                areas.push_back(1. / 2 * (c + f2 - f1 + std::numbers::pi * f1 / 2) * (b - d));
                areas.push_back(a3);
                areas.push_back((areas[0] + a21) / 2);
                areas.push_back((a23 + areas[2]) / 2);

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths.size(); ++i)
                {
                    c1 += lengths[i] / areas[i] / 2;
                    c2 += lengths[i] / (2 * pow(areas[i], 2)) / 2;
                }
                auto minimumArea = 2 * (*min_element(areas.begin(), areas.end()));

                return {c1, c2, minimumArea};
            }
    };

    class EFD: public EL
    {
        public:
            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F1"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F2"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(jsonMainColumn["width"].get<double>() * jsonMainColumn["depth"].get<double>());
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["area"] = roundFloat<6>(jsonLateralColumn["width"].get<double>() * jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4),
                                                    0,
                                                    roundFloat<6>(-std::get<double>(dimensions["F2"]) / 2 - std::get<double>(dimensions["K"]) + std::get<double>(dimensions["C"]) / 2)};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - (std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 4),
                                                    0,
                                                    roundFloat<6>(-std::get<double>(dimensions["F2"]) / 2 - std::get<double>(dimensions["K"]) + std::get<double>(dimensions["C"]) / 2)};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            void process_extra_data()
            {
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["C"]) + std::max(0., std::get<double>(dimensions["K"])));
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths;
                std::vector<double> areas;

                double a = std::get<double>(dimensions["A"]);
                double b = std::get<double>(dimensions["B"]);
                double c = std::get<double>(dimensions["C"]);
                double d = std::get<double>(dimensions["D"]);
                double e = std::get<double>(dimensions["E"]);
                double f1 = std::get<double>(dimensions["F1"]);
                double f2 = std::get<double>(dimensions["F2"]);
                double k = std::get<double>(dimensions["K"]);
                double q = std::get<double>(dimensions["q"]);

                lengths.push_back(d);
                lengths.push_back((e - f1) / 2);
                lengths.push_back(d);
                lengths.push_back(std::numbers::pi / 8 * ((a - e) / 2 + b - d));
                lengths.push_back(std::numbers::pi / 4 * (f1 / 4 + sqrt(pow((c - f2 - 2 * k) / 2, 2) + pow((b - d) / 2 , 2))));

                areas.push_back(c * (a - e) / 2);
                areas.push_back(c * (b - d));
                areas.push_back((f1 * f2 - 2 * pow(q, 2)) / 2);
                areas.push_back((areas[0] + areas[1]) / 2);
                areas.push_back((areas[1] + areas[2]) / 2);

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths.size(); ++i)
                {
                    c1 += lengths[i] / areas[i] / 2;
                    c2 += lengths[i] / (2 * pow(areas[i], 2)) / 2;
                }
                auto minimumArea = 2 * (*min_element(areas.begin(), areas.end()));

                return {c1, c2, minimumArea};
            }
    };

    class ER: public ETD {};

    class PLANAR_ER: public ETD {};

    class PLANAR_E: public E {};

    class PLANAR_EL: public EL {};

    class EC: public ETD {
        public:
            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();
                double tetha = asin(std::get<double>(dimensions["C"]) / std::get<double>(dimensions["E"]));
                double aperture = std::get<double>(dimensions["E"]) / 2 * cos(tetha);
                double segmentArea = pow(std::get<double>(dimensions["E"]) / 2, 2) / 2 * (2 * tetha - sin(2 * tetha));
                double clipHoleArea =  std::numbers::pi * pow(std::get<double>(dimensions["s"]), 2) / 2;
                double area = std::get<double>(dimensions["C"]) * (std::get<double>(dimensions["A"]) / 2 - aperture) - segmentArea - clipHoleArea;
                return area;
            }
    };

    class EQ: public ETD {};

    class EP: public E
    {
        public:
            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();

                double baseArea;
                double windingArea;
                double apertureArea;
                double k;
                if (dimensions.find("K") == dimensions.end()) {
                    k = std::get<double>(dimensions["F"]) / 2;
                } else {
                    k = std::get<double>(dimensions["K"]);
                }
                if (dimensions.find("G") == dimensions.end()) {
                    baseArea = std::get<double>(dimensions["A"]) * std::get<double>(dimensions["C"]);
                    windingArea = k * std::get<double>(dimensions["E"]) + 1 / 2 * std::numbers::pi * pow(std::get<double>(dimensions["E"]) / 2, 2);
                    apertureArea = 0;
                } else {
                    double aperture = std::get<double>(dimensions["G"]) / 2;
                    double tetha = asin(aperture / (std::get<double>(dimensions["E"]) / 2));
                    double segmentArea = (pow(std::get<double>(dimensions["E"]) / 2, 2) / 2 * (2 * tetha - sin(2 * tetha))) / 2;
                    double apertureMaximumDepth = std::get<double>(dimensions["C"]) - k - std::get<double>(dimensions["E"]) / 2 * cos(tetha);
                    apertureArea = aperture * apertureMaximumDepth - segmentArea;
                    baseArea = std::get<double>(dimensions["A"]) / 2 * std::get<double>(dimensions["C"]);
                    windingArea = k * std::get<double>(dimensions["E"]) / 2 + 1 / 4 * std::numbers::pi * pow(std::get<double>(dimensions["E"]) / 2, 2);

                }
                double area = baseArea - windingArea - apertureArea;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                if (dimensions.find("G") == dimensions.end()) {
                    jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]) - std::get<double>(dimensions["E"]) / 2 - std::get<double>(dimensions["K"]));
                    jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                    jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                    jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                    jsonLateralColumn["coordinates"] = {0, roundFloat<6>(std::get<double>(dimensions["C"]) - jsonLateralColumn["depth"].get<double>() / 2), 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                } else {
                    jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                    jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                    jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                    jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                    jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                    jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                }
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths_areas;
                std::vector<double> lengths_areas_2;
                std::vector<double> areas;

                double h1 = 2 * std::get<double>(dimensions["B"]);
                double h2 = 2 * std::get<double>(dimensions["D"]);
                double d1 = std::get<double>(dimensions["E"]);
                double d2 = std::get<double>(dimensions["F"]);
                double a = std::get<double>(dimensions["A"]);
                double b = std::get<double>(dimensions["C"]);
                double k;
                if (dimensions.find("K") == dimensions.end()) {
                    k = std::get<double>(dimensions["F"]) / 2;
                } else {
                    k = std::get<double>(dimensions["K"]);
                }
                double pi = std::numbers::pi;
                double a1 = a * b  - pi * pow(d1, 2) / 8 - d1 * k;
                double a3 = pi * pow(d2 / 2, 2) + (k - d2 / 2) * d2;
                double alpha = atan(std::get<double>(dimensions["E"]) / 2 / k);
                double gamma = sqrt(((pi - alpha) * pow(d1, 2) + 2 * a1) / (4 * (pi - alpha)));
                double l4 = pi / 2 * (gamma - d1 / 2 + (h1 - h2) / 4);
                double a4 = 1. / 2 * (a * b - pi / 8 * pow(d1, 2) - d1 * d2 / 2 + (pi - alpha) * d1 * (h1 / 2 - h2 / 2));
                double l5 = pi / 2 * (0.29289 * (d2 / 2 + k) / 2 + (h1 - h2) / 4);
                double a5 = pi / 2 * (pow((d2 / 2 + k), 2) / 4 + (d2 / 2 + k) / 2 * (h1 - h2));

                areas.push_back(a1);
                areas.push_back(a3);
                areas.push_back(a4);
                areas.push_back(a5);

                lengths_areas.push_back(h2 / a1);
                lengths_areas.push_back(2 / (pi - alpha) / (h1 - h2) * log(d1 / (d2 / 2 + k)));
                lengths_areas.push_back(h2 / a3);
                lengths_areas.push_back(l4 / a4);
                lengths_areas.push_back(l5 / a5);

                lengths_areas_2.push_back(h2 / pow(a1, 2));
                lengths_areas_2.push_back(4 * (d1 - (d2 / 2 + k)) / pow(pi - alpha, 2) / pow(h1 - h2, 2) / d1 / (d2 / 2 + k));
                lengths_areas_2.push_back(h2 / pow(a3, 2));
                lengths_areas_2.push_back(l4 / pow(a4, 2));
                lengths_areas_2.push_back(l5 / pow(a5, 2));

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths_areas.size(); ++i)
                {
                    c1 += lengths_areas[i] / 2;
                    c2 += lengths_areas_2[i] / 2;
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class LP: public EP {
        public:
            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }
    };

    class EPX: public EP
    {
        public:
            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::OBLONG;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"])) / 2 + roundFloat<6>(std::get<double>(dimensions["K"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2) + (std::get<double>(dimensions["K"]) - std::get<double>(dimensions["F"]) / 2) * std::get<double>(dimensions["F"]));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                if (dimensions.find("G") == dimensions.end()) {
                    jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]) - std::get<double>(dimensions["E"]) / 2 - std::get<double>(dimensions["K"]));
                    jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                    jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                    jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                    jsonLateralColumn["coordinates"] = {0, roundFloat<6>(std::get<double>(dimensions["C"]) - jsonLateralColumn["depth"].get<double>() / 2), 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                } else {
                    jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["C"]));
                    jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                    jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                    jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                    jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                    jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                    jsonWindingWindows.push_back(jsonLateralColumn);
                }
                set_columns(jsonWindingWindows);
            }
    };

    class RM: public CorePiece
    { 
        public:
            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_extra_data()
            {
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["E"]));
            }

            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();

                double d2 = std::get<double>(dimensions["E"]);
                double a = std::get<double>(dimensions["J"]);
                double e = std::get<double>(dimensions["G"]);
                double p = sqrt(2) * std::get<double>(dimensions["J"]) - std::get<double>(dimensions["A"]);
                double pi = std::numbers::pi;
                double alpha = pi / 2;
                double beta = alpha - asin(e / d2);

                double a1 = 1. / 2 * pow(a, 2) * (1 + tan(beta - pi / 4)) - beta / 2 * pow(d2, 2) - 1. / 2 * pow(p, 2);
                double area = a1 / 2;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["depth"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["width"].get<double>());
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                auto familySubtype = *get_shape().get_family_subtype();
                std::vector<double> lengths_areas;
                std::vector<double> lengths_areas_2;
                std::vector<double> areas;

                double d2 = std::get<double>(dimensions["E"]);
                double d3 = std::get<double>(dimensions["F"]);
                double d4 = std::get<double>(dimensions["H"]);
                double a = std::get<double>(dimensions["J"]);
                double c = std::get<double>(dimensions["C"]);
                double e = std::get<double>(dimensions["G"]);
                double h = std::get<double>(dimensions["B"]) - std::get<double>(dimensions["D"]);
                double p = sqrt(2) * std::get<double>(dimensions["J"]) - std::get<double>(dimensions["A"]);
                double b = 0;
                double pi = std::numbers::pi;
                double alpha = pi / 2;
                double gamma = pi / 2;
                double beta = alpha - asin(e / d2);
                double lmin = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                double lmax;
                double a7;
                double a8 = alpha / 8 * (pow(d2, 2) - pow(d3, 2));
                if (familySubtype == "1") {
                    lmax = sqrt(1. / 4 * (pow(d2, 2) + pow(d3, 2)) - 1. / 2 * d2 * d3 * cos(alpha - beta));
                    a7 = 1. / 4 * (beta / 2 * pow(d2, 2) + 1. / 2 * pow(e, 2) * tan(beta) - 1. / 2 * pow(e, 2) * tan(alpha - gamma / 2) - pi / 4 * pow(d3, 2));
                } else if (familySubtype == "2") {
                    lmax = sqrt(1. / 4 * (pow(d2, 2) + pow(d3, 2)) - 1. / 2 * d2 * d3 * cos(alpha - beta)) - b / (2 * sin(gamma / 2));
                    a7 = 1. / 4 * (beta / 2 * pow(d2, 2) - pi / 4 * pow(d3, 2) + 1. / 2 * (pow(b, 2) - pow(e, 2)) * tan(alpha - gamma / 2) + 1. / 2 * pow(e, 2) * tan(beta));
                } else if (familySubtype == "3") {
                    lmax = e / 2 + 1. / 2 * (1 - sin(gamma / 2)) * (d2 - c);
                    a7 = 1. / 4 * (beta / 2 * pow(d2, 2) - pi / 4 * pow(d3, 2) + 1. / 2 * pow(c, 2) * tan(alpha - beta));
                } else if (familySubtype == "4") {
                    lmax = sqrt(1. / 4 * (pow(d2, 2) + pow(d3, 2)) - 1. / 2 * d2 * d3 * cos(alpha - beta));
                    a7 = 1. / 4 * (beta / 2 * pow(d2, 2) + 1. / 2 * d2 * d3 * sin(alpha - beta) + 1. / 2 * pow(c - d3, 2) * tan(gamma / 2) - pi / 4 * pow(d3, 2));
                }

                double f = (lmin + lmax) / (2 * lmin);
                double D = a7 / a8;

                double l1 = 2 * std::get<double>(dimensions["D"]);
                double a1 = 1. / 2 * pow(a, 2) * (1 + tan(beta - pi / 4)) - beta / 2 * pow(d2, 2) - 1. / 2 * pow(p, 2);

                double l3 = 2 * std::get<double>(dimensions["D"]);
                double a3 = pi / 4 * (pow(d3, 2) - pow(d4, 2));

                double l4 =  pi / 4 * (h + a / 2 - d2 / 2 );
                double a4 = 1. / 2 * (a1 + 2 * beta * d2 * h);
                double l5 = pi / 4 * (d3 + h - sqrt(1. / 2 * (pow(d3, 2) + pow(d4, 2))));
                double a5 = 1. / 2 * (pi / 4 * (pow(d3, 2) - pow(d4, 2)) + 2 * alpha * d3 * h);

                areas.push_back(a1);
                areas.push_back(a3);
                areas.push_back(a4);
                areas.push_back(a5);

                lengths_areas.push_back(l1 / a1);
                lengths_areas.push_back(log(d2 / d3) * f / (D * pi * h));
                lengths_areas.push_back(l3 / a3);
                lengths_areas.push_back(l4 / a4);
                lengths_areas.push_back(l5 / a5);

                lengths_areas_2.push_back(l1 / pow(a1, 2));
                lengths_areas_2.push_back((1 / d3 - 1 / d2) * f / pow(D * pi * h, 2));
                lengths_areas_2.push_back(l3 / pow(a3, 2));
                lengths_areas_2.push_back(l4 / pow(a4, 2));
                lengths_areas_2.push_back(l5 / pow(a5, 2));

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths_areas.size(); ++i)
                {
                    c1 += lengths_areas[i] / 2;
                    c2 += lengths_areas_2[i] / 2;
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class PQ: public CorePiece
    { 
        public:
            void process_extra_data()
            {
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["C"]));
            }

            
            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();

                double A = std::get<double>(dimensions["A"]);
                double C = std::get<double>(dimensions["C"]);
                double E = std::get<double>(dimensions["E"]);
                double G = std::get<double>(dimensions["G"]);

                double beta = acos(G / E);
                double I = E * sin(beta);

                double a1 = C * (A - G) - beta * pow(E, 2) / 2 + 1. / 2 * G * I;
                double area = a1 / 2;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["depth"] = std::get<double>(dimensions["C"]);
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["width"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths_areas;
                std::vector<double> lengths_areas_2;
                std::vector<double> areas;

                double A = std::get<double>(dimensions["A"]);
                double B = std::get<double>(dimensions["B"]);
                double C = std::get<double>(dimensions["C"]);
                double D = std::get<double>(dimensions["D"]);
                double E = std::get<double>(dimensions["E"]);
                double F = std::get<double>(dimensions["F"]);
                double G = std::get<double>(dimensions["G"]);
                double J;
                double L;
                if (dimensions.find("J") == dimensions.end()) {
                    J = std::get<double>(dimensions["F"]) / 2;  // Totally made up base on drawings
                    L = F + (C - F) / 3;  // Totally made up base on drawings
                } else {
                    J = std::get<double>(dimensions["J"]);
                    L = std::get<double>(dimensions["L"]);
                }

                double pi = std::numbers::pi;
                double beta = acos(G / E);
                double alpha = atan(L / J);
                double I = E * sin(beta);
                double a7 = 1. / 8 * (beta * pow(E, 2) - alpha * pow(F, 2) + G * L - J * I);
                double a8 = pi / 16 * (pow(E, 2) - pow(F, 2));
                double a9 = 2 * alpha * F * (B - D);
                double a10 = 2 * beta * E * (B - D);
                double lmin = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                double lmax = sqrt(pow(E, 2) + pow(F, 2) - 2 * E * F * cos(alpha - beta)) / 2;
                double f = (lmin + lmax) / (2 * lmin);
                double K = a7 / a8;

                double l1 = 2 * D;
                double a1 = C * (A - G) - beta * pow(E, 2) / 2 + 1. / 2 * G * I;
                double a2 = pi * K * E * F * (B - D) / (E - F) * log(E / F);
                double l2 = f * E * F / (E - F) * pow(log(E / F), 2);

                double l3 = 2 * D;
                double a3 = pi / 4 * (pow(F, 2));

                double l4 = pi / 4 * ((B - D) + A / 2 - E / 2);
                double a4 = 1. / 2 * (a1 + a10);
                double l5 = pi / 4 * ((B - D) + (1 - 1. / sqrt(2)) * F);
                double a5 = 1. / 2 * (a3 + a9);

                areas.push_back(a1);
                areas.push_back(a3);
                areas.push_back(a2);
                areas.push_back(a4);
                areas.push_back(a5);

                lengths_areas.push_back(l1 / a1);
                lengths_areas.push_back(l2 / a2);
                lengths_areas.push_back(l3 / a3);
                lengths_areas.push_back(l4 / a4);
                lengths_areas.push_back(l5 / a5);

                lengths_areas_2.push_back(l1 / pow(a1, 2));
                lengths_areas_2.push_back(l2 / pow(a2, 2));
                lengths_areas_2.push_back(l3 / pow(a3, 2));
                lengths_areas_2.push_back(l4 / pow(a4, 2));
                lengths_areas_2.push_back(l5 / pow(a5, 2));

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths_areas.size(); ++i)
                {
                    c1 += lengths_areas[i] / 2;
                    c2 += lengths_areas_2[i] / 2;
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class PM: public CorePiece
    { 
        public:
            
            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_extra_data()
            {
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["E"]));
            }

            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();

                double d1 = std::get<double>(dimensions["A"]);
                double d2 = std::get<double>(dimensions["E"]);
                double f = std::get<double>(dimensions["G"]);
                double b = std::get<double>(dimensions["b"]);
                double t = std::get<double>(dimensions["t"]);
                double pi = std::numbers::pi;

                double alpha = pi / 2;
                double beta = alpha - asin(f / d2);

                double a1 = beta / 2 * (pow(d1, 2) - pow(d2, 2)) - 2 * b * t;
                double area = a1 / 2;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["depth"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["width"].get<double>());
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                std::vector<double> lengths_areas;
                std::vector<double> lengths_areas_2;
                std::vector<double> areas;
                double pi = std::numbers::pi;

                double d1 = std::get<double>(dimensions["A"]);
                double h1 = 2 * std::get<double>(dimensions["B"]);
                double h2 = 2 * std::get<double>(dimensions["D"]);
                double d2 = std::get<double>(dimensions["E"]);
                double d3 = std::get<double>(dimensions["F"]);
                double f = std::get<double>(dimensions["G"]);
                double d4 = std::get<double>(dimensions["H"]);
                double gamma = std::get<double>(dimensions["alpha"]) / 180 * pi;
                double b = std::get<double>(dimensions["b"]);
                double t = std::get<double>(dimensions["t"]);

                double alpha = pi / 2;
                double beta = alpha - asin(f / d2);
                double lmin = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                double lmax = sqrt(1. / 4* (pow(d2, 2) + pow(d3, 2)) - 1. / 2 * d2 * d3 * cos(alpha - beta));
                double g = (lmin + lmax) / (2 * lmin);
                double a7 = beta / 8 * pow(d2, 2) + 1. / 8 * pow(f, 2) * tan(beta) - 1. / 8 * pow(f, 2) * tan(alpha - gamma / 2) - pi / 16 * pow(d3, 2);
                double a8 = alpha / 8 * (pow(d2, 2) - pow(d3, 2));
                double D = a7 / a8;

                double a1 = beta / 2 * (pow(d1, 2) - pow(d2, 2)) - 2 * b * t;
                double l1 = h2;

                double l3 = h2;
                double a3 = pi / 4 * (pow(d3, 2) - pow(d4, 2));

                double l4 = pi / 8 * (h1 - h2 + d1 - d2);
                double a4 = 1. / 2 * (a1 + 2 * beta * d2 * (h1 - h2));
                double l5 = pi / 4 * (d3 + h1 - h2 - sqrt(1. / 2 * (pow(d3, 2) + pow(d4, 2))));
                double a5 = pi / 8 * (pow(d3, 2) - pow(d4, 2)) + alpha * d3 * (h1 - h2);

                areas.push_back(a1);
                areas.push_back(a3);
                areas.push_back(a4);
                areas.push_back(a5);

                lengths_areas.push_back(l1 / a1);
                lengths_areas.push_back(log(d2 / d3) * g / (D * pi * (h1 - h2) / 2));
                lengths_areas.push_back(l3 / a3);
                lengths_areas.push_back(l4 / a4);
                lengths_areas.push_back(l5 / a5);

                lengths_areas_2.push_back(l1 / pow(a1, 2));
                lengths_areas_2.push_back((1 / d3 - 1 / d2) * g / pow(D * pi * (h1 - h2) / 2, 2));
                lengths_areas_2.push_back(l3 / pow(a3, 2));
                lengths_areas_2.push_back(l4 / pow(a4, 2));
                lengths_areas_2.push_back(l5 / pow(a5, 2));

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths_areas.size(); ++i)
                {
                    c1 += lengths_areas[i] / 2;
                    c2 += lengths_areas_2[i] / 2;
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };

    class P: public CorePiece
    { 
        public:
            void process_winding_window()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>(dimensions["D"]);
                jsonWindingWindow["width"] = (std::get<double>(dimensions["E"]) - std::get<double>(dimensions["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>(dimensions["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_extra_data()
            { 
                auto dimensions = *get_shape().get_dimensions();
                set_width(std::get<double>(dimensions["A"]));
                set_height(std::get<double>(dimensions["B"]));
                set_depth(std::get<double>(dimensions["A"]));
            }

            double get_lateral_leg_area() {
                auto dimensions = *get_shape().get_dimensions();
                auto familySubtype = *get_shape().get_family_subtype();
                double pi = std::numbers::pi;
                double d1 = std::get<double>(dimensions["A"]);
                double d2 = std::get<double>(dimensions["E"]);
                double b = std::get<double>(dimensions["G"]);
                double tetha = asin(2 * b / (d1 + d2));
                double n;
                if (familySubtype == "1" || familySubtype == "2") {
                    n = 2;
                } else {
                    n = 0;
                }

                double a1 = 1. / 4 * (pi - n * tetha) * (pow(d1, 2) - pow(d2, 2));
                double area = a1 / 2;
                return area;
            }

            void process_columns()
            {
                auto dimensions = *get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::ROUND;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>(dimensions["F"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonMainColumn["area"] = roundFloat<6>(std::numbers::pi * pow(jsonMainColumn["width"].get<double>() / 2, 2));
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::IRREGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>(dimensions["A"]) - std::get<double>(dimensions["E"])) / 2);
                jsonLateralColumn["area"] = roundFloat<6>(get_lateral_leg_area());
                jsonLateralColumn["depth"] = roundFloat<6>(jsonLateralColumn["area"].get<double>() / jsonLateralColumn["width"].get<double>());
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>(dimensions["D"]));
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>(dimensions["E"]) / 2 + jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>(dimensions["E"]) / 2 - jsonLateralColumn["width"].get<double>() / 2), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = *get_shape().get_dimensions();
                auto familySubtype = *get_shape().get_family_subtype();
                std::vector<double> lengths_areas;
                std::vector<double> lengths_areas_2;
                std::vector<double> areas;
                double pi = std::numbers::pi;

                double r4 = std::get<double>(dimensions["A"]) / 2;
                double r3 = std::get<double>(dimensions["E"]) / 2;
                double r2 = std::get<double>(dimensions["F"]) / 2;
                double r1 = std::get<double>(dimensions["H"]) / 2;
                double h = std::get<double>(dimensions["B"]) - std::get<double>(dimensions["D"]);
                double h2 = 2 * std::get<double>(dimensions["D"]);
                double b = std::get<double>(dimensions["G"]);

                double s1 = r2 - sqrt((pow(r1, 2) + pow(r2, 2)) / 2);
                double s2 = sqrt((pow(r3, 2) + pow(r4, 2)) / 2) - r3;
                double n;
                if (familySubtype == "1" || familySubtype == "2") {
                    n = 2;
                } else {
                    n = 0;
                }

                double k1 = n * b * (r4 - r3);
                double k2 = 1 / (1 - n * b / (2 * pi * r3));
                double k3 = 1 - n * b / (pi * (r3 + r4));

                double a1 = pi * (r4 - r3) * (r4 + r3) - k1;
                double l1 = h2;

                double a3 = pi * (r2 - r1) * (r2 + r1);
                double l3 = h2;

                double l4 = pi / 4 * (2 * s2 + h);
                double a4 = pi / 2 * (pow(r4, 2) - pow(r3, 2) + 2 * r3 * h) * k3;
                double l5 = pi / 4 * (2 * s1 + h);
                double a5 = pi / 2 * (pow(r2, 2) - pow(r1, 2) + 2 * r2 * h);

                areas.push_back(a1);
                areas.push_back(a3);
                areas.push_back(a4);
                areas.push_back(a5);

                lengths_areas.push_back(l1 / a1);
                lengths_areas.push_back(1. / (pi * h) * log(r3 / r2) * k2);
                lengths_areas.push_back(l3 / a3);
                lengths_areas.push_back(l4 / a4);
                lengths_areas.push_back(l5 / a5);

                lengths_areas_2.push_back(l1 / pow(a1, 2));
                lengths_areas_2.push_back(1 / (2 * pow(pi * h, 2)) * (r3 - r2) / (r3 * r2) * k2);
                lengths_areas_2.push_back(l3 / pow(a3, 2));
                lengths_areas_2.push_back(l4 / pow(a4, 2));
                lengths_areas_2.push_back(l5 / pow(a5, 2));

                double c1 = 0, c2 = 0;
                for (size_t i = 0; i < lengths_areas.size(); ++i)
                {
                    c1 += lengths_areas[i] / 2;
                    c2 += lengths_areas_2[i] / 2;
                }
                auto minimumArea = *min_element(areas.begin(), areas.end());

                return {c1, c2, minimumArea};
            }
    };


    std::shared_ptr<CorePiece> CorePiece::factory(CoreShape shape)
    {
        auto family = shape.get_family();
        if (family == CoreShapeFamily::E) {
            std::shared_ptr<CorePiece> piece(new E);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EC) {
            std::shared_ptr<CorePiece> piece(new EC);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EFD) {
            std::shared_ptr<CorePiece> piece(new EFD);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EL) {
            std::shared_ptr<CorePiece> piece(new EL);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EP) {
            std::shared_ptr<CorePiece> piece(new EP);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EPX) {
            std::shared_ptr<CorePiece> piece(new EPX);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::LP) {
            std::shared_ptr<CorePiece> piece(new LP);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::EQ) {
            std::shared_ptr<CorePiece> piece(new EQ);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::ER) {
            std::shared_ptr<CorePiece> piece(new ER);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::ETD) {
            std::shared_ptr<CorePiece> piece(new ETD);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::P) {
            std::shared_ptr<CorePiece> piece(new P);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::PLANAR_E) {
            std::shared_ptr<CorePiece> piece(new PLANAR_E);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::PLANAR_EL) {
            std::shared_ptr<CorePiece> piece(new PLANAR_EL);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::PLANAR_ER) {
            std::shared_ptr<CorePiece> piece(new PLANAR_ER);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::PM) {
            std::shared_ptr<CorePiece> piece(new PM);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::PQ) {
            std::shared_ptr<CorePiece> piece(new PQ);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        else if (family == CoreShapeFamily::RM) {
            std::shared_ptr<CorePiece> piece(new RM);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
        // else if (family == CoreShapeFamily::U) {
        //     std::shared_ptr<CorePiece> piece(new U);
            // piece->set_shape(shape);
            // piece->process();
        //     return piece;
        // }
        else throw std::runtime_error("Unknown shape family, available options are: {E, EC, EFD, EL, EP, EPX, LP, EQ, ER, ETD, P, PLANAR_E, PLANAR_EL, PLANAR_ER, PM, PQ, RM, U}");
    }

    class Core:public CoreTemplate {
        public:
        Core(const json & j) {
            from_json(j, *this);
            process_data();
            process_gap();
        }
        virtual ~Core() = default;

        std::shared_ptr<std::vector<GeometricalDescription>> create_geometrical_description()
        {
            std::vector<GeometricalDescription> geometricalDescription;
            auto numberStacks = *(get_functional_description().get_number_stacks());
            auto gapping = get_functional_description().get_gapping();

            auto corePiece = OpenMagnetics::CorePiece::factory(std::get<OpenMagnetics::CoreShape>(get_functional_description().get_shape()));
            auto corePieceHeight = corePiece->get_height();
            auto corePieceDepth = corePiece->get_depth();

            json jsonSpacer;
            json jsonMachining = json::array();
            json jsonGeometricalDescription;
            double currentDepth = roundFloat<6>((-corePieceDepth * (numberStacks - 1)) / 2);
            double spacerThickness = 0;

            for ( auto &gap : gapping) {
                if (gap.get_type() == OpenMagnetics::GappingType::ADDITIVE){
                    spacerThickness = gap.get_length();
                }
                else if (gap.get_type() == OpenMagnetics::GappingType::SUBTRACTIVE){
                    json aux;
                    aux["length"] = gap.get_length();
                    aux["coordinates"] = gap.get_coordinates();
                    if (aux["coordinates"][0] == nullptr) {
                        aux["coordinates"] = {0, 0, 0};
                    }
                    else {  // Gap coordinates are centered in the gap, while machining coordinates start at the base of the central column surface, therefore, we must correct them
                        aux["coordinates"][1] = aux["coordinates"].at(1).get<double>() - gap.get_length() / 2;
                    }
                    jsonMachining.push_back(aux);
                }

            }

            jsonGeometricalDescription["material"] = get_functional_description().get_material();
            jsonGeometricalDescription["shape"] = std::get<OpenMagnetics::CoreShape>(get_functional_description().get_shape()).get_name();
            switch (get_functional_description().get_type()) {
                case OpenMagnetics::FunctionalDescriptionType::TOROIDAL:
                    jsonGeometricalDescription["type"] = OpenMagnetics::GeometricalDescriptionType::TOROIDAL;

                    //TODO add for toroids
                    break;
                case OpenMagnetics::FunctionalDescriptionType::TWO_PIECE_SET:
                    jsonGeometricalDescription["type"] = OpenMagnetics::GeometricalDescriptionType::HALF_SET;
                    for (auto i = 0; i < numberStacks; ++i) {
                        double currentHeight = roundFloat<6>(spacerThickness / 2 + corePieceHeight);
                        std::vector<double> coordinates = {0, currentHeight, currentDepth};
                        jsonGeometricalDescription["coordinates"] = coordinates;
                        if (jsonMachining.size() > 0) {
                            jsonGeometricalDescription["machining"] = jsonMachining;
                        }
                        geometricalDescription.push_back(GeometricalDescription(jsonGeometricalDescription));

                        if (jsonGeometricalDescription.find("machining") != jsonGeometricalDescription.end())
                        {
                            jsonGeometricalDescription.erase("machining");
                        }

                        currentHeight = -currentHeight;
                        coordinates = {0, currentHeight, currentDepth};
                        jsonGeometricalDescription["coordinates"] = coordinates;
                        geometricalDescription.push_back(GeometricalDescription(jsonGeometricalDescription));

                        currentDepth = roundFloat<6>(currentDepth + corePieceDepth);
                    }

                    if (spacerThickness > 0) {
                        for ( auto &column : corePiece->get_columns()) {
                            if (column.get_type() == OpenMagnetics::ColumnType::LATERAL) {
                                jsonSpacer["type"] = OpenMagnetics::GeometricalDescriptionType::SPACER;
                                jsonSpacer["material"] = "plastic";
                                jsonSpacer["dimensions"] = {column.get_width(), spacerThickness, column.get_depth() * numberStacks};
                                jsonSpacer["coordinates"] = column.get_coordinates();
                                geometricalDescription.push_back(GeometricalDescription(jsonSpacer));
                            }
                        }
                    }
                    break;
                default: throw std::runtime_error("Unknown type of core, options are {TOROIDAL, TWO_PIECE_SET}");
            }

            return std::make_shared<std::vector<GeometricalDescription>>(geometricalDescription);

        }

        std::vector<ColumnElement> find_columns_by_type(ColumnType columnType) {
            std::vector<ColumnElement> foundColumns;
            for (auto &column : get_processed_description()->get_columns()) {
                if (column.get_type() == columnType) {
                    foundColumns.push_back(column);
                }
            }
            return foundColumns;
        }

        std::vector<CoreGap> find_gaps_by_type(GappingType gappingType) {
            std::vector<CoreGap> foundGaps;
            for (auto &gap : get_functional_description().get_gapping()) {
                if (gap.get_type() == gappingType) {
                    foundGaps.push_back(gap);
                }
            }
            return foundGaps;
        }

        void scale_to_stacks(int64_t numberStacks)
        {
            get_processed_description()->get_mutable_effective_parameters().set_effective_area(get_processed_description()->get_effective_parameters().get_effective_area() * numberStacks);
            get_processed_description()->get_mutable_effective_parameters().set_minimum_area(get_processed_description()->get_effective_parameters().get_minimum_area() * numberStacks);
            get_processed_description()->get_mutable_effective_parameters().set_effective_volume(get_processed_description()->get_effective_parameters().get_effective_volume() * numberStacks);
            get_processed_description()->set_depth(get_processed_description()->get_depth() * numberStacks);
            for ( auto &windingWindow : get_processed_description()->get_mutable_columns() ) {
                windingWindow.set_area(windingWindow.get_area() * numberStacks);
                windingWindow.set_depth(windingWindow.get_depth() * numberStacks);
            }
        }

        void process_gap()
        {
            json jsonGap;
            json jsonGapping = json::array();
            auto gapping = get_functional_description().get_gapping();
            double centralColumnGapsHeightOffset;
            double distanceClosestNormalSurface;
            double coreChunkSizePlusGap;
            auto nonResidualGaps = find_gaps_by_type(GappingType::SUBTRACTIVE);
            auto additiveGaps = find_gaps_by_type(GappingType::ADDITIVE);
            nonResidualGaps.insert(nonResidualGaps.end(), additiveGaps.begin(), additiveGaps.end());
            auto residualGaps = find_gaps_by_type(GappingType::RESIDUAL);
            int numberNonResidualGaps = nonResidualGaps.size();
            int numberResidualGaps = residualGaps.size();
            int numberGaps = numberNonResidualGaps + numberResidualGaps;
            int numberColumns = get_processed_description()->get_columns().size();


            // Check if current information is valid
            if (numberNonResidualGaps + numberResidualGaps < numberColumns && (numberNonResidualGaps + numberResidualGaps) > 0) {
                if (!(get_functional_description().get_type() == OpenMagnetics::FunctionalDescriptionType::TOROIDAL && numberColumns == 1)) {
                    throw std::runtime_error("A TWO_PIECE_SET core cannot have less gaps than columns");
                }
            }

            if (numberNonResidualGaps + numberResidualGaps == 0) {
                for (size_t i = 0; i < get_processed_description()->get_columns().size(); ++i) {
                    jsonGap["type"] = GappingType::RESIDUAL;
                    jsonGap["length"] = residualGap;
                    jsonGap["coordinates"] = get_processed_description()->get_columns()[i].get_coordinates();
                    jsonGap["shape"] = get_processed_description()->get_columns()[i].get_shape();
                    jsonGap["distanceClosestNormalSurface"] = get_processed_description()->get_columns()[i].get_height() / 2;
                    jsonGap["area"] = get_processed_description()->get_columns()[i].get_area();
                    jsonGap["sectionDimensions"] = {get_processed_description()->get_columns()[i].get_width(), get_processed_description()->get_columns()[i].get_depth()};
                    jsonGapping.push_back(jsonGap);
                }
            }
            else if ((numberResidualGaps == numberColumns || numberNonResidualGaps == numberColumns) && (numberGaps == numberColumns)) {
                for (size_t i = 0; i < get_processed_description()->get_columns().size(); ++i) {
                    jsonGap["type"] = gapping[i].get_type();
                    jsonGap["length"] = gapping[i].get_length();
                    jsonGap["coordinates"] = get_processed_description()->get_columns()[i].get_coordinates();
                    jsonGap["shape"] = get_processed_description()->get_columns()[i].get_shape();
                    jsonGap["distanceClosestNormalSurface"] = get_processed_description()->get_columns()[i].get_height() / 2;
                    jsonGap["area"] = get_processed_description()->get_columns()[i].get_area();
                    jsonGap["sectionDimensions"] = {get_processed_description()->get_columns()[i].get_width(), get_processed_description()->get_columns()[i].get_depth()};
                    jsonGapping.push_back(jsonGap);
                }
            }
            else {

                auto lateralColumns = find_columns_by_type(ColumnType::LATERAL);
                auto centralColumns = find_columns_by_type(ColumnType::CENTRAL);
                if (numberGaps == numberColumns) {
                    centralColumnGapsHeightOffset = roundFloat<6>(nonResidualGaps[0].get_length() / 2);
                    distanceClosestNormalSurface = roundFloat<6>(centralColumns[0].get_height() / 2 - nonResidualGaps[0].get_length() / 2) ;
                }
                else {
                    coreChunkSizePlusGap = roundFloat<6>(centralColumns[0].get_height() / (nonResidualGaps.size() + 1));
                    centralColumnGapsHeightOffset = roundFloat<6>(-coreChunkSizePlusGap * (nonResidualGaps.size() - 1) / 2);
                    distanceClosestNormalSurface = roundFloat<6>(coreChunkSizePlusGap - nonResidualGaps[0].get_length() / 2);
                }

                for (size_t i = 0; i < nonResidualGaps.size(); ++i) {
                    jsonGap["type"] = nonResidualGaps[i].get_type();
                    jsonGap["length"] = nonResidualGaps[i].get_length();
                    jsonGap["coordinates"] = {centralColumns[0].get_coordinates()[0], centralColumns[0].get_coordinates()[0] + centralColumnGapsHeightOffset, centralColumns[0].get_coordinates()[2]};
                    jsonGap["shape"] = centralColumns[0].get_shape();
                    jsonGap["distanceClosestNormalSurface"] = distanceClosestNormalSurface;
                    jsonGap["area"] = centralColumns[0].get_area();
                    jsonGap["sectionDimensions"] = {centralColumns[0].get_width(), centralColumns[0].get_depth()};
                    jsonGapping.push_back(jsonGap);

                    centralColumnGapsHeightOffset += roundFloat<6>(centralColumns[0].get_height() / (nonResidualGaps.size() + 1));
                    if (i < nonResidualGaps.size() / 2. - 1) {
                        distanceClosestNormalSurface = roundFloat<6>(distanceClosestNormalSurface + coreChunkSizePlusGap);
                    }
                    else if (i > nonResidualGaps.size() / 2. - 1) {
                        distanceClosestNormalSurface = roundFloat<6>(distanceClosestNormalSurface - coreChunkSizePlusGap);
                    }
                }

                for (size_t i = 0; i < lateralColumns.size(); ++i) {
                    jsonGap["type"] = residualGaps[i].get_type();
                    jsonGap["length"] = residualGaps[i].get_length();
                    jsonGap["coordinates"] = lateralColumns[i].get_coordinates();
                    jsonGap["shape"] = lateralColumns[i].get_shape();
                    jsonGap["distanceClosestNormalSurface"] = lateralColumns[i].get_height() / 2;
                    jsonGap["area"] = lateralColumns[i].get_area();
                    jsonGap["sectionDimensions"] = {lateralColumns[i].get_width(), lateralColumns[i].get_depth()};
                    jsonGapping.push_back(jsonGap);
                }
            }

            get_mutable_functional_description().set_gapping(jsonGapping);

        }

        void process_data() { 
            // If the shape is a string, we have to load its dta from the database
            if (std::holds_alternative<std::string>(get_functional_description().get_shape())) {
                auto shape_data = OpenMagnetics::find_data_by_name<OpenMagnetics::CoreShape>(std::get<std::string>(get_functional_description().get_shape()));
                get_mutable_functional_description().set_shape(shape_data);
            }

            auto corePiece = OpenMagnetics::CorePiece::factory(std::get<OpenMagnetics::CoreShape>(get_functional_description().get_shape()));
            ProcessedDescription processedDescription;
            json coreEffectiveParameters;
            json coreWindingWindow;
            auto coreColumns = corePiece->get_columns();
            switch (get_functional_description().get_type()) {
                case OpenMagnetics::FunctionalDescriptionType::TOROIDAL:
                    processedDescription.set_columns(coreColumns);
                    to_json(coreEffectiveParameters, corePiece->get_partial_effective_parameters());
                    processedDescription.set_effective_parameters(coreEffectiveParameters);
                    to_json(coreWindingWindow, corePiece->get_winding_window());
                    processedDescription.get_mutable_winding_windows().push_back(coreWindingWindow);
                    processedDescription.set_depth(corePiece->get_depth());
                    processedDescription.set_height(corePiece->get_height());
                    processedDescription.set_width(corePiece->get_width());
                    break;

                case OpenMagnetics::FunctionalDescriptionType::TWO_PIECE_SET:
                    for ( auto &column : coreColumns) {
                        column.set_height(2 * column.get_height());
                    }
                    processedDescription.set_columns(coreColumns);

                    to_json(coreEffectiveParameters, corePiece->get_partial_effective_parameters());
                    coreEffectiveParameters["effectiveLength"] = 2 * coreEffectiveParameters.at("effectiveLength").get<double>();
                    coreEffectiveParameters["effectiveArea"] = coreEffectiveParameters.at("effectiveArea").get<double>();
                    coreEffectiveParameters["effectiveVolume"] = 2 * coreEffectiveParameters.at("effectiveVolume").get<double>();
                    coreEffectiveParameters["minimumArea"] = coreEffectiveParameters.at("minimumArea").get<double>();
                    processedDescription.set_effective_parameters(coreEffectiveParameters);

                    to_json(coreWindingWindow, corePiece->get_winding_window());
                    coreWindingWindow.at("area") = 2 * coreWindingWindow.at("area").get<double>();
                    coreWindingWindow.at("height") = 2 * coreWindingWindow.at("height").get<double>();
                    processedDescription.get_mutable_winding_windows().push_back(coreWindingWindow);
                    processedDescription.set_depth(corePiece->get_depth());
                    processedDescription.set_height(corePiece->get_height() * 2);
                    processedDescription.set_width(corePiece->get_width());
                    break;
                default: throw std::runtime_error("Unknown type of core, available options are {TOROIDAL, TWO_PIECE_SET}");
            }
            set_processed_description(std::make_shared<ProcessedDescription>(processedDescription));
            scale_to_stacks(*(get_functional_description().get_number_stacks()));

            if (get_geometrical_description() == nullptr) {
                auto geometricalDescription = create_geometrical_description();

                set_geometrical_description(geometricalDescription);
            }
        }
    };
}


// int main(int, const char *[])
// {

//     std::string filePath = __FILE__;
//     auto masPath = filePath.substr(0, filePath.rfind("/")).append("/../");
//     auto coreFilePath = masPath + "samples/core/core_E_55_21_N97_additive.json";
//     std::ifstream json_file(coreFilePath);

//     json coreJson;
//     coreJson["functionalDescription"] = json();
//     coreJson["functionalDescription"]["name"] = "core_PQ_20_16_N97_substractive";
//     coreJson["functionalDescription"]["type"] = "two-piece set";
//     coreJson["functionalDescription"]["material"] = "N97";
//     coreJson["functionalDescription"]["shape"] = "P 59/36";
//     coreJson["functionalDescription"]["gapping"] = json::array();
//     coreJson["functionalDescription"]["numberStacks"] = 1;
//     OpenMagnetics::Core core(coreJson);
//     std::cout << coreJson["functionalDescription"] << "\n";

//     // core.process_data();

//     // auto processed_description = *core.get_processed_description();
//     std::cout << *(core.get_mutable_functional_description().get_name()) << "\n";
//     // std::cout << "effective_area: " << core.get_processed_description()->get_effective_parameters().get_effective_area() << "\n";
//     // std::cout << "effective_length: " << core.get_processed_description()->get_effective_parameters().get_effective_length() << "\n";
//     // std::cout << "effective_volume: " << core.get_processed_description()->get_effective_parameters().get_effective_volume() << "\n";
//     // std::cout << "minimum_area: " << core.get_processed_description()->get_effective_parameters().get_minimum_area() << "\n";
//     // std::cout << "winding window height: " << *(core.get_processed_description()->get_winding_windows()[0].get_height()) << "\n";
//     // std::cout << "winding window depth: " << *(core.get_processed_description()->get_winding_windows()[0].get_width()) << "\n";
//     std::cout << "main column width: " << core.get_processed_description()->get_columns()[0].get_width() << "\n";
//     std::cout << "main column depth: " << core.get_processed_description()->get_columns()[0].get_depth() << "\n";
//     std::cout << "main column height: " << core.get_processed_description()->get_columns()[0].get_height() << "\n";
//     std::cout << "lateral column width: " << core.get_processed_description()->get_columns()[1].get_width() << "\n";
//     std::cout << "lateral column depth: " << core.get_processed_description()->get_columns()[1].get_depth() << "\n";
//     std::cout << "lateral column height: " << core.get_processed_description()->get_columns()[1].get_height() << "\n";
//     // std::cout << "lateral column width: " << core.get_processed_description()->get_columns()[2].get_width() << "\n";
//     // std::cout << "lateral column height: " << core.get_processed_description()->get_columns()[2].get_depth() << "\n";
//     // std::cout << core.get_mutable_functional_description().get_shape() << "\n";
//     // std::cout << effective_parameters.get_effective_volume() << "\n";
//     // json ea;
//     // to_json(ea, core);
//     // std::cout << ea.dump(4) << "\n";

//     // auto ea = *core.get_geometrical_description();
//     // std::cout << "geometrical_description: " << ()[0].get_type() << "\n";
// }
