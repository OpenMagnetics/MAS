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
            virtual void process_winding_window() = 0;
            virtual void process_columns() = 0;
            virtual void process_extra_data() = 0;
            virtual void flatten_dimensions() = 0;

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
            void flatten_dimensions()
            {
                for ( auto &dimension : *get_mutable_shape().get_dimensions() ) {
                    dimension.second = resolve_dimensional_values<OpenMagnetics::DimensionalValues::NOMINAL>(dimension.second);
                }
            }

            void process_winding_window()
            {
                auto dimensions = get_shape().get_dimensions();
                json jsonWindingWindow;
                jsonWindingWindow["height"] = std::get<double>((*dimensions)["D"]);
                jsonWindingWindow["width"] = (std::get<double>((*dimensions)["E"]) - std::get<double>((*dimensions)["F"])) / 2;
                jsonWindingWindow["area"] = jsonWindingWindow["height"].get<double>() * jsonWindingWindow["width"].get<double>();
                jsonWindingWindow["coordinates"] = {std::get<double>((*dimensions)["F"]) / 2, 0};
                set_winding_window(jsonWindingWindow);
            }

            void process_columns()
            {
                auto dimensions = get_shape().get_dimensions();
                json jsonWindingWindows = json::array();
                json jsonMainColumn;
                json jsonLateralColumn;
                jsonMainColumn["type"] = OpenMagnetics::ColumnType::CENTRAL;
                jsonMainColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonMainColumn["width"] = roundFloat<6>(std::get<double>((*dimensions)["F"]));
                jsonMainColumn["depth"] = roundFloat<6>(std::get<double>((*dimensions)["C"]));
                jsonMainColumn["height"] = roundFloat<6>(std::get<double>((*dimensions)["D"]));
                jsonMainColumn["area"] = roundFloat<6>(jsonMainColumn["width"].get<double>() * jsonMainColumn["depth"].get<double>());
                jsonMainColumn["coordinates"] = {0, 0, 0};
                jsonWindingWindows.push_back(jsonMainColumn);
                jsonLateralColumn["type"] = OpenMagnetics::ColumnType::LATERAL;
                jsonLateralColumn["shape"] = OpenMagnetics::ColumnShape::RECTANGULAR;
                jsonLateralColumn["width"] = roundFloat<6>((std::get<double>((*dimensions)["A"]) - std::get<double>((*dimensions)["E"])) / 2);
                jsonLateralColumn["depth"] = roundFloat<6>(std::get<double>((*dimensions)["C"]));
                jsonLateralColumn["height"] = roundFloat<6>(std::get<double>((*dimensions)["D"]));
                jsonLateralColumn["area"] = roundFloat<6>(jsonLateralColumn["width"].get<double>() * jsonLateralColumn["depth"].get<double>());
                jsonLateralColumn["coordinates"] = {roundFloat<6>(std::get<double>((*dimensions)["E"]) / 2 + (std::get<double>((*dimensions)["A"]) - std::get<double>((*dimensions)["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                jsonLateralColumn["coordinates"] = {roundFloat<6>(-std::get<double>((*dimensions)["E"]) / 2 - (std::get<double>((*dimensions)["A"]) - std::get<double>((*dimensions)["E"])) / 4), 0, 0};
                jsonWindingWindows.push_back(jsonLateralColumn);
                set_columns(jsonWindingWindows);
            }

            void process_extra_data()
            {
                auto dimensions = get_shape().get_dimensions();
                set_width(std::get<double>((*dimensions)["A"]));
                set_height(std::get<double>((*dimensions)["B"]));
                set_depth(std::get<double>((*dimensions)["C"]));
            }

            std::tuple<double, double, double> get_shape_constants()
            {
                auto dimensions = get_shape().get_dimensions();
                std::vector<double> lengths;
                std::vector<double> areas;

                double h = std::get<double>((*dimensions)["B"]) - std::get<double>((*dimensions)["D"]);
                double q = std::get<double>((*dimensions)["C"]);
                double s = std::get<double>((*dimensions)["F"]) / 2;
                double p = (std::get<double>((*dimensions)["A"]) - std::get<double>((*dimensions)["E"])) / 2;


                lengths.push_back(std::get<double>((*dimensions)["D"]));
                lengths.push_back((std::get<double>((*dimensions)["E"]) - std::get<double>((*dimensions)["F"])) / 2);
                lengths.push_back(std::get<double>((*dimensions)["D"]));
                lengths.push_back(std::numbers::pi / 8 * (p + h));
                lengths.push_back(std::numbers::pi / 8 * (s + h));

                areas.push_back(2 * q * h);
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

    std::shared_ptr<CorePiece> CorePiece::factory(CoreShape shape)
    {
        auto family = shape.get_family();
        if (family == CoreShapeFamily::E) {
            std::shared_ptr<CorePiece> piece(new E);
            piece->set_shape(shape);
            piece->process();
            return piece;
        }
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
        else throw std::runtime_error("Unknown shape family, available options are: {E, EC, EFD, EL, EP, ER, ETD, P, PLANAR_E, PLANAR_EL, PLANAR_ER, PM, PQ, RM, U}");
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
            if (numberNonResidualGaps + numberResidualGaps < numberColumns) {
                if (!(get_functional_description().get_type() == OpenMagnetics::FunctionalDescriptionType::TOROIDAL && numberColumns == 1)) {
                    throw std::runtime_error("A TWO_PIECE_SET core cannot have less gaps than columns");
                }
            }

            if ((numberResidualGaps == numberColumns || numberNonResidualGaps == numberColumns) && (numberGaps == numberColumns)) {
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

//     auto jf = json::parse(json_file);
//     std::cout << jf["functionalDescription"] << "\n";
//     OpenMagnetics::Core core(jf);
    
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
