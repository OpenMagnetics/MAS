//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Mas data = nlohmann::json::parse(jsonString);

#pragma once

#include <optional>
#include <variant>
#include "json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>
#include <unordered_map>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json & j, const std::shared_ptr<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json & j) {
            if (j.is_null()) return std::make_shared<T>(); else return std::make_shared<T>(j.get<T>());
        }
    };
    template <typename T>
    struct adl_serializer<std::optional<T>> {
        static void to_json(json & j, const std::optional<T> & opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::optional<T> from_json(const json & j) {
            if (j.is_null()) return std::make_optional<T>(); else return std::make_optional<T>(j.get<T>());
        }
    };
}
#endif

namespace MAS {
    using nlohmann::json;

    class ClassMemberConstraints {
        private:
        std::optional<int64_t> min_int_value;
        std::optional<int64_t> max_int_value;
        std::optional<double> min_double_value;
        std::optional<double> max_double_value;
        std::optional<size_t> min_length;
        std::optional<size_t> max_length;
        std::optional<std::string> pattern;

        public:
        ClassMemberConstraints(
            std::optional<int64_t> min_int_value,
            std::optional<int64_t> max_int_value,
            std::optional<double> min_double_value,
            std::optional<double> max_double_value,
            std::optional<size_t> min_length,
            std::optional<size_t> max_length,
            std::optional<std::string> pattern
        ) : min_int_value(min_int_value), max_int_value(max_int_value), min_double_value(min_double_value), max_double_value(max_double_value), min_length(min_length), max_length(max_length), pattern(pattern) {}
        ClassMemberConstraints() = default;
        virtual ~ClassMemberConstraints() = default;

        void set_min_int_value(int64_t min_int_value) { this->min_int_value = min_int_value; }
        auto get_min_int_value() const { return min_int_value; }

        void set_max_int_value(int64_t max_int_value) { this->max_int_value = max_int_value; }
        auto get_max_int_value() const { return max_int_value; }

        void set_min_double_value(double min_double_value) { this->min_double_value = min_double_value; }
        auto get_min_double_value() const { return min_double_value; }

        void set_max_double_value(double max_double_value) { this->max_double_value = max_double_value; }
        auto get_max_double_value() const { return max_double_value; }

        void set_min_length(size_t min_length) { this->min_length = min_length; }
        auto get_min_length() const { return min_length; }

        void set_max_length(size_t max_length) { this->max_length = max_length; }
        auto get_max_length() const { return max_length; }

        void set_pattern(const std::string &  pattern) { this->pattern = pattern; }
        auto get_pattern() const { return pattern; }
    };

    class ClassMemberConstraintException : public std::runtime_error {
        public:
        ClassMemberConstraintException(const std::string &  msg) : std::runtime_error(msg) {}
    };

    class ValueTooLowException : public ClassMemberConstraintException {
        public:
        ValueTooLowException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooHighException : public ClassMemberConstraintException {
        public:
        ValueTooHighException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooShortException : public ClassMemberConstraintException {
        public:
        ValueTooShortException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class ValueTooLongException : public ClassMemberConstraintException {
        public:
        ValueTooLongException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    class InvalidPatternException : public ClassMemberConstraintException {
        public:
        InvalidPatternException(const std::string &  msg) : ClassMemberConstraintException(msg) {}
    };

    inline void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, int64_t value) {
        if (c.get_min_int_value() != std::nullopt && value < *c.get_min_int_value()) {
            throw ValueTooLowException ("Value too low for " + name + " (" + std::to_string(value) + "<" + std::to_string(*c.get_min_int_value()) + ")");
        }

        if (c.get_max_int_value() != std::nullopt && value > *c.get_max_int_value()) {
            throw ValueTooHighException ("Value too high for " + name + " (" + std::to_string(value) + ">" + std::to_string(*c.get_max_int_value()) + ")");
        }
    }

    inline void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, double value) {
        if (c.get_min_double_value() != std::nullopt && value < *c.get_min_double_value()) {
            throw ValueTooLowException ("Value too low for " + name + " (" + std::to_string(value) + "<" + std::to_string(*c.get_min_double_value()) + ")");
        }

        if (c.get_max_double_value() != std::nullopt && value > *c.get_max_double_value()) {
            throw ValueTooHighException ("Value too high for " + name + " (" + std::to_string(value) + ">" + std::to_string(*c.get_max_double_value()) + ")");
        }
    }

    inline void CheckConstraint(const std::string &  name, const ClassMemberConstraints & c, const std::string &  value) {
        if (c.get_min_length() != std::nullopt && value.length() < *c.get_min_length()) {
            throw ValueTooShortException ("Value too short for " + name + " (" + std::to_string(value.length()) + "<" + std::to_string(*c.get_min_length()) + ")");
        }

        if (c.get_max_length() != std::nullopt && value.length() > *c.get_max_length()) {
            throw ValueTooLongException ("Value too long for " + name + " (" + std::to_string(value.length()) + ">" + std::to_string(*c.get_max_length()) + ")");
        }

        if (c.get_pattern() != std::nullopt) {
            std::smatch result;
            std::regex_search(value, result, std::regex( *c.get_pattern() ));
            if (result.empty()) {
                throw InvalidPatternException ("Value doesn't match pattern for " + name + " (" + value +" != " + *c.get_pattern() + ")");
            }
        }
    }

    #ifndef NLOHMANN_UNTYPED_MAS_HELPER
    #define NLOHMANN_UNTYPED_MAS_HELPER
    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }
    #endif

    #ifndef NLOHMANN_OPTIONAL_MAS_HELPER
    #define NLOHMANN_OPTIONAL_MAS_HELPER
    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_heap_optional(const json & j, std::string property) {
        return get_heap_optional<T>(j, property.data());
    }
    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, const char * property) {
        auto it = j.find(property);
        if (it != j.end() && !it->is_null()) {
            return j.at(property).get<std::optional<T>>();
        }
        return std::optional<T>();
    }

    template <typename T>
    inline std::optional<T> get_stack_optional(const json & j, std::string property) {
        return get_stack_optional<T>(j, property.data());
    }
    #endif

    /**
     * The input voltage of the boost
     *
     * The input voltage of the buck
     *
     * The input voltage (HV side) of the CLLC converter
     *
     * The input voltage of the DAB converter
     *
     * The input voltage of the flyback
     *
     * The input voltage of the forward
     *
     * The input voltage of the isolatedBuck
     *
     * The input voltage of the isolatedBuckBoost
     *
     * The input voltage of the LLC converter
     *
     * The input voltage of the PSFB converter
     *
     * The input voltage of the pushPull
     *
     * Required values for the altitude
     *
     * Voltage RMS of the main supply to which this transformer is connected to.
     *
     * Required values for the magnetizing inductance
     *
     * Required values for the temperature that the magnetic can reach under operating
     *
     * The maximum thickness of the insulation around the wire, in m
     *
     * The conducting area of the wire, in m². Used for some rectangular shapes where the area
     * is smaller than expected due to rounded corners
     *
     * The conducting diameter of the wire, in m
     *
     * The outer diameter of the wire, in m
     *
     * The conducting height of the wire, in m
     *
     * The conducting width of the wire, in m
     *
     * The outer height of the wire, in m
     *
     * The outer width of the wire, in m
     *
     * The radius of the edge, in case of rectangular wire, in m
     *
     * The outer diameter of the wire. Unit: m.
     *
     * Length of one full twist of the litz bundle along the wire axis. Unit: m. Vendors quote
     * this in mm or as 'lay length'; convert to metres before serialisation. No international
     * standard exists for litz construction; this field captures the most commonly published
     * vendor parameter.
     *
     * Specific heat capacity value according to manufacturer. Unit: J/(kg*K).
     *
     * Thermal conductivity value according to manufacturer. Unit: W/(m*K).
     *
     * DC resistance per winding in Ohms. nominal = typical value, maximum = datasheet max.
     *
     * Inductance per winding in Henries, with tolerance.
     *
     * Leakage inductance in Henries.
     *
     * Body diameter in metres (for cylindrical parts).
     *
     * Body height in metres.
     *
     * Body length in metres.
     *
     * Body width in metres.
     *
     * Operating temperature in degrees Celsius.
     *
     * Value of the leakage inductance between the primary and a secondary winding given by the
     * position in the array
     *
     * Value of the magnetizing inductance. Unit: H. The operating point at which this value
     * applies (frequency, AC test amplitude, DC bias, temperature) is recorded in the optional
     * `measurementCondition` block; if absent, small-signal at 0 A DC bias and the ambient
     * temperature of the operating point is assumed.
     *
     * A dimension of with minimum, nominal, and maximum values
     */
    class DimensionWithTolerance {
        public:
        DimensionWithTolerance() = default;
        virtual ~DimensionWithTolerance() = default;

        private:
        std::optional<bool> exclude_maximum;
        std::optional<bool> exclude_minimum;
        std::optional<double> maximum;
        std::optional<double> minimum;
        std::optional<double> nominal;
        std::optional<std::string> unit;

        public:
        /**
         * True if the maximum value must be excluded from the range
         */
        std::optional<bool> get_exclude_maximum() const { return exclude_maximum; }
        void set_exclude_maximum(std::optional<bool> value) { this->exclude_maximum = value; }

        /**
         * True if the minimum value must be excluded from the range
         */
        std::optional<bool> get_exclude_minimum() const { return exclude_minimum; }
        void set_exclude_minimum(std::optional<bool> value) { this->exclude_minimum = value; }

        /**
         * The maximum value of the dimension
         */
        std::optional<double> get_maximum() const { return maximum; }
        void set_maximum(std::optional<double> value) { this->maximum = value; }

        /**
         * The minimum value of the dimension
         */
        std::optional<double> get_minimum() const { return minimum; }
        void set_minimum(std::optional<double> value) { this->minimum = value; }

        /**
         * The nominal value of the dimension
         */
        std::optional<double> get_nominal() const { return nominal; }
        void set_nominal(std::optional<double> value) { this->nominal = value; }

        /**
         * Optional SI unit string (e.g. 'H', 'F', 'V', 'A', 'Ohm', 'm', 'kg'). When absent, the
         * unit is implied by the field name.
         */
        std::optional<std::string> get_unit() const { return unit; }
        void set_unit(std::optional<std::string> value) { this->unit = value; }
    };

    /**
     * Type of value carried in outputCurrents: which aggregate of the periodic waveform the
     * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
     *
     * Type of value carried in outputVoltages: which aggregate of the periodic waveform the
     * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
     *
     * Type of value carried in outputCurrents. Defaults to dc.
     *
     * Type of value carried in outputVoltages. Defaults to dc.
     */
    enum class OutputSType : int { AVERAGE, DC, PEAK, PEAK_TO_PEAK, RMS };

    /**
     * The description of one boost operating point
     *
     * Base fields common to all topology operating points
     *
     * The description of one buck operating point
     *
     * The description of one forward operating point
     *
     * The description of one isolatedBuck operating point
     *
     * The description of one isolatedBuckBoost operating point
     *
     * The description of one LLC operating point
     *
     * The description of one pushPull operating point
     */
    class TopologyExcitation {
        public:
        TopologyExcitation() = default;
        virtual ~TopologyExcitation() = default;

        private:
        double ambient_temperature;
        std::vector<double> output_currents;
        std::optional<OutputSType> output_currents_type;
        std::vector<double> output_voltages;
        std::optional<OutputSType> output_voltages_type;
        double switching_frequency;

        public:
        /**
         * Ambient temperature of the operating point. Unit: Celsius. See docs/units.md.
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * List of output currents, one per output. Interpreted per outputCurrentsType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_currents() const { return output_currents; }
        std::vector<double> & get_mutable_output_currents() { return output_currents; }
        void set_output_currents(const std::vector<double> & value) { this->output_currents = value; }

        /**
         * Type of value carried in outputCurrents: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_currents_type() const { return output_currents_type; }
        void set_output_currents_type(std::optional<OutputSType> value) { this->output_currents_type = value; }

        /**
         * List of output voltages, one per output. Interpreted per outputVoltagesType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_voltages() const { return output_voltages; }
        std::vector<double> & get_mutable_output_voltages() { return output_voltages; }
        void set_output_voltages(const std::vector<double> & value) { this->output_voltages = value; }

        /**
         * Type of value carried in outputVoltages: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_voltages_type() const { return output_voltages_type; }
        void set_output_voltages_type(std::optional<OutputSType> value) { this->output_voltages_type = value; }

        /**
         * Switching frequency of the operating point. Unit: Hz. See docs/units.md.
         */
        const double & get_switching_frequency() const { return switching_frequency; }
        double & get_mutable_switching_frequency() { return switching_frequency; }
        void set_switching_frequency(const double & value) { this->switching_frequency = value; }
    };

    /**
     * The description of a Boost converter excitation
     */
    class Boost {
        public:
        Boost() = default;
        virtual ~Boost() = default;

        private:
        std::optional<double> current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        std::optional<double> get_current_ripple_ratio() const { return current_ripple_ratio; }
        void set_current_ripple_ratio(std::optional<double> value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the boost
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    /**
     * The description of a Buck converter excitation
     */
    class Buck {
        public:
        Buck() = default;
        virtual ~Buck() = default;

        private:
        std::optional<double> current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        std::optional<double> get_current_ripple_ratio() const { return current_ripple_ratio; }
        void set_current_ripple_ratio(std::optional<double> value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the buck
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    /**
     * The power flow direction.
     *
     * The power flow direction
     */
    enum class CllcPowerFlow : int { FORWARD, REVERSE };

    /**
     * The description of one CLLC operating point
     *
     * The description of one boost operating point
     *
     * Base fields common to all topology operating points
     *
     * The description of one buck operating point
     *
     * The description of one forward operating point
     *
     * The description of one isolatedBuck operating point
     *
     * The description of one isolatedBuckBoost operating point
     *
     * The description of one LLC operating point
     *
     * The description of one pushPull operating point
     */
    class CllcOperatingPoint {
        public:
        CllcOperatingPoint() = default;
        virtual ~CllcOperatingPoint() = default;

        private:
        double ambient_temperature;
        std::vector<double> output_currents;
        std::optional<OutputSType> output_currents_type;
        std::vector<double> output_voltages;
        std::optional<OutputSType> output_voltages_type;
        double switching_frequency;
        CllcPowerFlow power_flow;

        public:
        /**
         * Ambient temperature of the operating point. Unit: Celsius. See docs/units.md.
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * List of output currents, one per output. Interpreted per outputCurrentsType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_currents() const { return output_currents; }
        std::vector<double> & get_mutable_output_currents() { return output_currents; }
        void set_output_currents(const std::vector<double> & value) { this->output_currents = value; }

        /**
         * Type of value carried in outputCurrents: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_currents_type() const { return output_currents_type; }
        void set_output_currents_type(std::optional<OutputSType> value) { this->output_currents_type = value; }

        /**
         * List of output voltages, one per output. Interpreted per outputVoltagesType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_voltages() const { return output_voltages; }
        std::vector<double> & get_mutable_output_voltages() { return output_voltages; }
        void set_output_voltages(const std::vector<double> & value) { this->output_voltages = value; }

        /**
         * Type of value carried in outputVoltages: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_voltages_type() const { return output_voltages_type; }
        void set_output_voltages_type(std::optional<OutputSType> value) { this->output_voltages_type = value; }

        /**
         * Switching frequency of the operating point. Unit: Hz. See docs/units.md.
         */
        const double & get_switching_frequency() const { return switching_frequency; }
        double & get_mutable_switching_frequency() { return switching_frequency; }
        void set_switching_frequency(const double & value) { this->switching_frequency = value; }

        /**
         * The power flow direction.
         */
        const CllcPowerFlow & get_power_flow() const { return power_flow; }
        CllcPowerFlow & get_mutable_power_flow() { return power_flow; }
        void set_power_flow(const CllcPowerFlow & value) { this->power_flow = value; }
    };

    /**
     * The description of a CLLC Bidirectional Resonant converter excitation
     */
    class CllcResonant {
        public:
        CllcResonant() = default;
        virtual ~CllcResonant() = default;

        private:
        std::optional<bool> bidirectional;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        double max_switching_frequency;
        double min_switching_frequency;
        std::vector<CllcOperatingPoint> operating_points;
        std::optional<double> quality_factor;
        std::optional<bool> symmetric_design;

        public:
        /**
         * Whether the converter operates in bidirectional mode
         */
        std::optional<bool> get_bidirectional() const { return bidirectional; }
        void set_bidirectional(std::optional<bool> value) { this->bidirectional = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage (HV side) of the CLLC converter
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum switching frequency for regulation
         */
        const double & get_max_switching_frequency() const { return max_switching_frequency; }
        double & get_mutable_max_switching_frequency() { return max_switching_frequency; }
        void set_max_switching_frequency(const double & value) { this->max_switching_frequency = value; }

        /**
         * The minimum switching frequency for regulation
         */
        const double & get_min_switching_frequency() const { return min_switching_frequency; }
        double & get_mutable_min_switching_frequency() { return min_switching_frequency; }
        void set_min_switching_frequency(const double & value) { this->min_switching_frequency = value; }

        /**
         * A list of operating points
         */
        const std::vector<CllcOperatingPoint> & get_operating_points() const { return operating_points; }
        std::vector<CllcOperatingPoint> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<CllcOperatingPoint> & value) { this->operating_points = value; }

        /**
         * The quality factor of the resonant tank
         */
        std::optional<double> get_quality_factor() const { return quality_factor; }
        void set_quality_factor(std::optional<double> value) { this->quality_factor = value; }

        /**
         * Whether to use symmetric resonant tank design
         */
        std::optional<bool> get_symmetric_design() const { return symmetric_design; }
        void set_symmetric_design(std::optional<bool> value) { this->symmetric_design = value; }
    };

    /**
     * Waveform of the signal to measure
     *
     * Label of the waveform, if applicable. Used for common waveforms
     */
    enum class WaveformLabel : int { BIPOLAR_RECTANGULAR, BIPOLAR_TRIANGULAR, CUSTOM, FLYBACK_PRIMARY, FLYBACK_SECONDARY, FLYBACK_SECONDARY_WITH_DEADTIME, RECTANGULAR, RECTANGULAR_DCM, RECTANGULAR_WITH_DEADTIME, SECONDARY_RECTANGULAR, SECONDARY_RECTANGULAR_WITH_DEADTIME, SINUSOIDAL, TRIANGULAR, TRIANGULAR_WITH_DEADTIME, UNIPOLAR_RECTANGULAR, UNIPOLAR_TRIANGULAR };

    /**
     * The description of a Current Transformer excitation
     */
    class CurrentTransformer {
        public:
        CurrentTransformer() = default;
        virtual ~CurrentTransformer() = default;

        private:
        double ambient_temperature;
        double burden_resistor;
        double diode_voltage_drop;
        double frequency;
        double maximum_duty_cycle;
        double maximum_primary_current_peak;
        WaveformLabel waveform_label;

        public:
        /**
         * The ambient temperature of the operating point
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * The value of the burden resistor in the measuring circuit
         */
        const double & get_burden_resistor() const { return burden_resistor; }
        double & get_mutable_burden_resistor() { return burden_resistor; }
        void set_burden_resistor(const double & value) { this->burden_resistor = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * Frequency of the input
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { this->frequency = value; }

        /**
         * The maximum duty cycle in the input
         */
        const double & get_maximum_duty_cycle() const { return maximum_duty_cycle; }
        double & get_mutable_maximum_duty_cycle() { return maximum_duty_cycle; }
        void set_maximum_duty_cycle(const double & value) { this->maximum_duty_cycle = value; }

        /**
         * The maximum current peak in the input
         */
        const double & get_maximum_primary_current_peak() const { return maximum_primary_current_peak; }
        double & get_mutable_maximum_primary_current_peak() { return maximum_primary_current_peak; }
        void set_maximum_primary_current_peak(const double & value) { this->maximum_primary_current_peak = value; }

        /**
         * Waveform of the signal to measure
         */
        const WaveformLabel & get_waveform_label() const { return waveform_label; }
        WaveformLabel & get_mutable_waveform_label() { return waveform_label; }
        void set_waveform_label(const WaveformLabel & value) { this->waveform_label = value; }
    };

    /**
     * DAB modulation scheme. SPS (only D3), EPS (D1>0, D2=0), DPS (D1=D2>0), TPS (independent).
     * Ref: Huang et al., Energies 11(9):2419 (2018).
     */
    enum class ModulationType : int { DPS, EPS, SPS, TPS };

    /**
     * One DAB operating point. Modulation is parameterised by three phase-shift variables D1,
     * D2, D3 (Huang et al. 2018 convention): D1 and D2 are intra-bridge (leg-to-leg) shifts
     * that control each bridge's effective duty cycle, and D3 is the outer inter-bridge shift
     * that controls power transfer. SPS uses D3 only (D1=D2=0). EPS uses D3 + one inner shift
     * (D1, with D2=0). DPS uses D3 + one symmetric inner shift (D1=D2). TPS uses all three
     * independently.
     *
     * The description of one boost operating point
     *
     * Base fields common to all topology operating points
     *
     * The description of one buck operating point
     *
     * The description of one forward operating point
     *
     * The description of one isolatedBuck operating point
     *
     * The description of one isolatedBuckBoost operating point
     *
     * The description of one LLC operating point
     *
     * The description of one pushPull operating point
     */
    class DabOperatingPoint {
        public:
        DabOperatingPoint() = default;
        virtual ~DabOperatingPoint() = default;

        private:
        double ambient_temperature;
        std::vector<double> output_currents;
        std::optional<OutputSType> output_currents_type;
        std::vector<double> output_voltages;
        std::optional<OutputSType> output_voltages_type;
        double switching_frequency;
        std::optional<double> inner_phase_shift1;
        std::optional<double> inner_phase_shift2;
        std::optional<double> inner_phase_shift3;
        std::optional<ModulationType> modulation_type;

        public:
        /**
         * Ambient temperature of the operating point. Unit: Celsius. See docs/units.md.
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * List of output currents, one per output. Interpreted per outputCurrentsType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_currents() const { return output_currents; }
        std::vector<double> & get_mutable_output_currents() { return output_currents; }
        void set_output_currents(const std::vector<double> & value) { this->output_currents = value; }

        /**
         * Type of value carried in outputCurrents: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_currents_type() const { return output_currents_type; }
        void set_output_currents_type(std::optional<OutputSType> value) { this->output_currents_type = value; }

        /**
         * List of output voltages, one per output. Interpreted per outputVoltagesType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_voltages() const { return output_voltages; }
        std::vector<double> & get_mutable_output_voltages() { return output_voltages; }
        void set_output_voltages(const std::vector<double> & value) { this->output_voltages = value; }

        /**
         * Type of value carried in outputVoltages: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_voltages_type() const { return output_voltages_type; }
        void set_output_voltages_type(std::optional<OutputSType> value) { this->output_voltages_type = value; }

        /**
         * Switching frequency of the operating point. Unit: Hz. See docs/units.md.
         */
        const double & get_switching_frequency() const { return switching_frequency; }
        double & get_mutable_switching_frequency() { return switching_frequency; }
        void set_switching_frequency(const double & value) { this->switching_frequency = value; }

        /**
         * D1 in degrees
         */
        std::optional<double> get_inner_phase_shift1() const { return inner_phase_shift1; }
        void set_inner_phase_shift1(std::optional<double> value) { this->inner_phase_shift1 = value; }

        /**
         * D2 in degrees
         */
        std::optional<double> get_inner_phase_shift2() const { return inner_phase_shift2; }
        void set_inner_phase_shift2(std::optional<double> value) { this->inner_phase_shift2 = value; }

        /**
         * D3 in degrees
         */
        std::optional<double> get_inner_phase_shift3() const { return inner_phase_shift3; }
        void set_inner_phase_shift3(std::optional<double> value) { this->inner_phase_shift3 = value; }

        /**
         * DAB modulation scheme. SPS (only D3), EPS (D1>0, D2=0), DPS (D1=D2>0), TPS (independent).
         * Ref: Huang et al., Energies 11(9):2419 (2018).
         */
        std::optional<ModulationType> get_modulation_type() const { return modulation_type; }
        void set_modulation_type(std::optional<ModulationType> value) { this->modulation_type = value; }
    };

    /**
     * Dual Active Bridge (DAB) DC-DC converter excitation. Modulation parameters follow the
     * standard Triple Phase Shift (TPS) convention: D1 = primary-bridge intra-leg shift, D2 =
     * secondary-bridge intra-leg shift, D3 = inter-bridge (outer) shift. See Huang et al.,
     * Energies 11(9):2419 (2018) and Maniktala & Rosano, 'Power Supplies A to Z', 3rd ed. (Ch.
     * 2).
     */
    class DualActiveBridge {
        public:
        DualActiveBridge() = default;
        virtual ~DualActiveBridge() = default;

        private:
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::vector<DabOperatingPoint> operating_points;
        std::optional<std::vector<double>> per_secondary_leakage;
        std::optional<double> series_inductance;
        std::optional<bool> use_leakage_inductance;

        public:
        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the DAB converter
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * A list of operating points
         */
        const std::vector<DabOperatingPoint> & get_operating_points() const { return operating_points; }
        std::vector<DabOperatingPoint> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<DabOperatingPoint> & value) { this->operating_points = value; }

        /**
         * Optional per-secondary leakage inductance in H, one entry per output. Required for
         * physically meaningful multi-output DAB simulation; if absent, all secondaries share the
         * same series inductance.
         */
        std::optional<std::vector<double>> get_per_secondary_leakage() const { return per_secondary_leakage; }
        void set_per_secondary_leakage(std::optional<std::vector<double>> value) { this->per_secondary_leakage = value; }

        /**
         * The series inductance for power transfer control. If 0, uses transformer leakage
         * inductance
         */
        std::optional<double> get_series_inductance() const { return series_inductance; }
        void set_series_inductance(std::optional<double> value) { this->series_inductance = value; }

        /**
         * Whether to use transformer leakage inductance as the series inductor
         */
        std::optional<bool> get_use_leakage_inductance() const { return use_leakage_inductance; }
        void set_use_leakage_inductance(std::optional<bool> value) { this->use_leakage_inductance = value; }
    };

    /**
     * The conduction mode of the operating point.
     *
     * The conduction mode of the Flyback
     */
    enum class FlybackModes : int { BOUNDARY_MODE_OPERATION, CONTINUOUS_CONDUCTION_MODE, DISCONTINUOUS_CONDUCTION_MODE, QUASI_RESONANT_MODE };

    /**
     * The description of one flyback operating point. Flyback does not require
     * switchingFrequency at the schema level: when omitted, it is inferred from the conduction
     * mode, see docs/inputs.md.
     */
    class FlybackOperatingPoint {
        public:
        FlybackOperatingPoint() = default;
        virtual ~FlybackOperatingPoint() = default;

        private:
        double ambient_temperature;
        std::optional<FlybackModes> mode;
        std::vector<double> output_currents;
        std::optional<OutputSType> output_currents_type;
        std::vector<double> output_voltages;
        std::optional<OutputSType> output_voltages_type;
        std::optional<double> switching_frequency;

        public:
        /**
         * Ambient temperature of the operating point. Unit: Celsius.
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * The conduction mode of the operating point.
         */
        std::optional<FlybackModes> get_mode() const { return mode; }
        void set_mode(std::optional<FlybackModes> value) { this->mode = value; }

        /**
         * List of output currents, one per output. Interpreted per outputCurrentsType (default: dc).
         */
        const std::vector<double> & get_output_currents() const { return output_currents; }
        std::vector<double> & get_mutable_output_currents() { return output_currents; }
        void set_output_currents(const std::vector<double> & value) { this->output_currents = value; }

        /**
         * Type of value carried in outputCurrents. Defaults to dc.
         */
        std::optional<OutputSType> get_output_currents_type() const { return output_currents_type; }
        void set_output_currents_type(std::optional<OutputSType> value) { this->output_currents_type = value; }

        /**
         * List of output voltages, one per output. Interpreted per outputVoltagesType (default: dc).
         */
        const std::vector<double> & get_output_voltages() const { return output_voltages; }
        std::vector<double> & get_mutable_output_voltages() { return output_voltages; }
        void set_output_voltages(const std::vector<double> & value) { this->output_voltages = value; }

        /**
         * Type of value carried in outputVoltages. Defaults to dc.
         */
        std::optional<OutputSType> get_output_voltages_type() const { return output_voltages_type; }
        void set_output_voltages_type(std::optional<OutputSType> value) { this->output_voltages_type = value; }

        /**
         * Switching frequency of the operating point. Unit: Hz. Optional for flyback when mode is
         * supplied.
         */
        std::optional<double> get_switching_frequency() const { return switching_frequency; }
        void set_switching_frequency(std::optional<double> value) { this->switching_frequency = value; }
    };

    /**
     * The description of a Flyback converter excitation
     */
    class Flyback {
        public:
        Flyback() = default;
        virtual ~Flyback() = default;

        private:
        double current_ripple_ratio;
        double diode_voltage_drop;
        double efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_drain_source_voltage;
        std::optional<double> maximum_duty_cycle;
        std::vector<FlybackOperatingPoint> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        const double & get_current_ripple_ratio() const { return current_ripple_ratio; }
        double & get_mutable_current_ripple_ratio() { return current_ripple_ratio; }
        void set_current_ripple_ratio(const double & value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * The target efficiency
         */
        const double & get_efficiency() const { return efficiency; }
        double & get_mutable_efficiency() { return efficiency; }
        void set_efficiency(const double & value) { this->efficiency = value; }

        /**
         * The input voltage of the flyback
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum drain-source voltage in the selected switch
         */
        std::optional<double> get_maximum_drain_source_voltage() const { return maximum_drain_source_voltage; }
        void set_maximum_drain_source_voltage(std::optional<double> value) { this->maximum_drain_source_voltage = value; }

        /**
         * The maximum duty cycle in the selected switch
         */
        std::optional<double> get_maximum_duty_cycle() const { return maximum_duty_cycle; }
        void set_maximum_duty_cycle(std::optional<double> value) { this->maximum_duty_cycle = value; }

        /**
         * A list of operating points
         */
        const std::vector<FlybackOperatingPoint> & get_operating_points() const { return operating_points; }
        std::vector<FlybackOperatingPoint> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<FlybackOperatingPoint> & value) { this->operating_points = value; }
    };

    /**
     * The description of a Forward converter excitation
     */
    class Forward {
        public:
        Forward() = default;
        virtual ~Forward() = default;

        private:
        double current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> duty_cycle;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        const double & get_current_ripple_ratio() const { return current_ripple_ratio; }
        double & get_mutable_current_ripple_ratio() { return current_ripple_ratio; }
        void set_current_ripple_ratio(const double & value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * Duty cycle for the converter, maximum 50%
         */
        std::optional<double> get_duty_cycle() const { return duty_cycle; }
        void set_duty_cycle(std::optional<double> value) { this->duty_cycle = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the forward
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    /**
     * The description of a Isolated Buck / Flybuck converter excitation
     */
    class IsolatedBuck {
        public:
        IsolatedBuck() = default;
        virtual ~IsolatedBuck() = default;

        private:
        std::optional<double> current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        std::optional<double> get_current_ripple_ratio() const { return current_ripple_ratio; }
        void set_current_ripple_ratio(std::optional<double> value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the isolatedBuck
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    /**
     * The description of a Isolated BuckBoost / FlyBuck - Boost converter excitation
     */
    class IsolatedBuckBoost {
        public:
        IsolatedBuckBoost() = default;
        virtual ~IsolatedBuckBoost() = default;

        private:
        std::optional<double> current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        std::optional<double> get_current_ripple_ratio() const { return current_ripple_ratio; }
        void set_current_ripple_ratio(std::optional<double> value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the isolatedBuckBoost
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    /**
     * The type of primary bridge
     *
     * The type of primary bridge for LLC
     */
    enum class LlcBridgeType : int { FULL_BRIDGE, HALF_BRIDGE };

    /**
     * The description of an LLC Resonant converter excitation
     */
    class LlcResonant {
        public:
        LlcResonant() = default;
        virtual ~LlcResonant() = default;

        private:
        std::optional<LlcBridgeType> bridge_type;
        std::optional<double> efficiency;
        std::optional<double> inductance_ratio;
        DimensionWithTolerance input_voltage;
        std::optional<bool> integrated_resonant_inductor;
        double max_switching_frequency;
        double min_switching_frequency;
        std::vector<TopologyExcitation> operating_points;
        std::optional<double> quality_factor;
        std::optional<double> resonant_capacitance;
        std::optional<double> resonant_frequency;
        std::optional<double> series_inductance;

        public:
        /**
         * The type of primary bridge
         */
        std::optional<LlcBridgeType> get_bridge_type() const { return bridge_type; }
        void set_bridge_type(std::optional<LlcBridgeType> value) { this->bridge_type = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The inductance ratio between Lm/Ls
         */
        std::optional<double> get_inductance_ratio() const { return inductance_ratio; }
        void set_inductance_ratio(std::optional<double> value) { this->inductance_ratio = value; }

        /**
         * The input voltage of the LLC converter
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * Whether the resonant inductor is integrated in the transformer (leakage)
         */
        std::optional<bool> get_integrated_resonant_inductor() const { return integrated_resonant_inductor; }
        void set_integrated_resonant_inductor(std::optional<bool> value) { this->integrated_resonant_inductor = value; }

        /**
         * The maximum switching frequency for regulation
         */
        const double & get_max_switching_frequency() const { return max_switching_frequency; }
        double & get_mutable_max_switching_frequency() { return max_switching_frequency; }
        void set_max_switching_frequency(const double & value) { this->max_switching_frequency = value; }

        /**
         * The minimum switching frequency for regulation
         */
        const double & get_min_switching_frequency() const { return min_switching_frequency; }
        double & get_mutable_min_switching_frequency() { return min_switching_frequency; }
        void set_min_switching_frequency(const double & value) { this->min_switching_frequency = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }

        /**
         * The quality factor of the resonant tank
         */
        std::optional<double> get_quality_factor() const { return quality_factor; }
        void set_quality_factor(std::optional<double> value) { this->quality_factor = value; }

        /**
         * Optional explicit resonant capacitor (Cr) value in F. Overrides the Q/Ln/fr derivation
         * when set.
         */
        std::optional<double> get_resonant_capacitance() const { return resonant_capacitance; }
        void set_resonant_capacitance(std::optional<double> value) { this->resonant_capacitance = value; }

        /**
         * The resonant frequency of the tank (optional, will be calculated if not provided)
         */
        std::optional<double> get_resonant_frequency() const { return resonant_frequency; }
        void set_resonant_frequency(std::optional<double> value) { this->resonant_frequency = value; }

        /**
         * Optional explicit resonant inductor (Ls) value in H. Overrides the Q/Ln/fr derivation
         * when set.
         */
        std::optional<double> get_series_inductance() const { return series_inductance; }
        void set_series_inductance(std::optional<double> value) { this->series_inductance = value; }
    };

    /**
     * The description of one PSFB operating point
     *
     * The description of one boost operating point
     *
     * Base fields common to all topology operating points
     *
     * The description of one buck operating point
     *
     * The description of one forward operating point
     *
     * The description of one isolatedBuck operating point
     *
     * The description of one isolatedBuckBoost operating point
     *
     * The description of one LLC operating point
     *
     * The description of one pushPull operating point
     */
    class PsfbOperatingPoint {
        public:
        PsfbOperatingPoint() = default;
        virtual ~PsfbOperatingPoint() = default;

        private:
        double ambient_temperature;
        std::vector<double> output_currents;
        std::optional<OutputSType> output_currents_type;
        std::vector<double> output_voltages;
        std::optional<OutputSType> output_voltages_type;
        double switching_frequency;
        double phase_shift;

        public:
        /**
         * Ambient temperature of the operating point. Unit: Celsius. See docs/units.md.
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { this->ambient_temperature = value; }

        /**
         * List of output currents, one per output. Interpreted per outputCurrentsType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_currents() const { return output_currents; }
        std::vector<double> & get_mutable_output_currents() { return output_currents; }
        void set_output_currents(const std::vector<double> & value) { this->output_currents = value; }

        /**
         * Type of value carried in outputCurrents: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_currents_type() const { return output_currents_type; }
        void set_output_currents_type(std::optional<OutputSType> value) { this->output_currents_type = value; }

        /**
         * List of output voltages, one per output. Interpreted per outputVoltagesType (default:
         * dc). See docs/normative-references.md.
         */
        const std::vector<double> & get_output_voltages() const { return output_voltages; }
        std::vector<double> & get_mutable_output_voltages() { return output_voltages; }
        void set_output_voltages(const std::vector<double> & value) { this->output_voltages = value; }

        /**
         * Type of value carried in outputVoltages: which aggregate of the periodic waveform the
         * number represents. Defaults to dc. See IEV 103-02 (values of a periodic quantity).
         */
        std::optional<OutputSType> get_output_voltages_type() const { return output_voltages_type; }
        void set_output_voltages_type(std::optional<OutputSType> value) { this->output_voltages_type = value; }

        /**
         * Switching frequency of the operating point. Unit: Hz. See docs/units.md.
         */
        const double & get_switching_frequency() const { return switching_frequency; }
        double & get_mutable_switching_frequency() { return switching_frequency; }
        void set_switching_frequency(const double & value) { this->switching_frequency = value; }

        /**
         * Phase shift in degrees
         */
        const double & get_phase_shift() const { return phase_shift; }
        double & get_mutable_phase_shift() { return phase_shift; }
        void set_phase_shift(const double & value) { this->phase_shift = value; }
    };

    /**
     * The type of secondary rectifier
     */
    enum class PsfbRectifierType : int { CENTER_TAPPED, CURRENT_DOUBLER, FULL_BRIDGE };

    /**
     * The description of a Phase Shift Full Bridge (PSFB) converter excitation
     */
    class PhaseShiftFullBridge {
        public:
        PhaseShiftFullBridge() = default;
        virtual ~PhaseShiftFullBridge() = default;

        private:
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_phase_shift;
        std::vector<PsfbOperatingPoint> operating_points;
        std::optional<double> output_inductance;
        std::optional<PsfbRectifierType> rectifier_type;
        std::optional<double> series_inductance;
        std::optional<bool> use_leakage_inductance;

        public:
        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the PSFB converter
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum phase shift as ratio of half period (0-1)
         */
        std::optional<double> get_maximum_phase_shift() const { return maximum_phase_shift; }
        void set_maximum_phase_shift(std::optional<double> value) { this->maximum_phase_shift = value; }

        /**
         * A list of operating points
         */
        const std::vector<PsfbOperatingPoint> & get_operating_points() const { return operating_points; }
        std::vector<PsfbOperatingPoint> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<PsfbOperatingPoint> & value) { this->operating_points = value; }

        /**
         * The output filter inductance
         */
        std::optional<double> get_output_inductance() const { return output_inductance; }
        void set_output_inductance(std::optional<double> value) { this->output_inductance = value; }

        /**
         * The type of secondary rectifier
         */
        std::optional<PsfbRectifierType> get_rectifier_type() const { return rectifier_type; }
        void set_rectifier_type(std::optional<PsfbRectifierType> value) { this->rectifier_type = value; }

        /**
         * The series inductance for ZVS and duty cycle control. If 0, uses transformer leakage
         * inductance
         */
        std::optional<double> get_series_inductance() const { return series_inductance; }
        void set_series_inductance(std::optional<double> value) { this->series_inductance = value; }

        /**
         * Whether to use transformer leakage inductance as the series inductor
         */
        std::optional<bool> get_use_leakage_inductance() const { return use_leakage_inductance; }
        void set_use_leakage_inductance(std::optional<bool> value) { this->use_leakage_inductance = value; }
    };

    /**
     * The description of a Push-Pull excitation
     */
    class PushPull {
        public:
        PushPull() = default;
        virtual ~PushPull() = default;

        private:
        double current_ripple_ratio;
        double diode_voltage_drop;
        std::optional<double> duty_cycle;
        std::optional<double> efficiency;
        DimensionWithTolerance input_voltage;
        std::optional<double> maximum_drain_source_voltage;
        std::optional<double> maximum_switch_current;
        std::vector<TopologyExcitation> operating_points;

        public:
        /**
         * The maximum current ripple allowed in the output
         */
        const double & get_current_ripple_ratio() const { return current_ripple_ratio; }
        double & get_mutable_current_ripple_ratio() { return current_ripple_ratio; }
        void set_current_ripple_ratio(const double & value) { this->current_ripple_ratio = value; }

        /**
         * The voltage drop on the diode
         */
        const double & get_diode_voltage_drop() const { return diode_voltage_drop; }
        double & get_mutable_diode_voltage_drop() { return diode_voltage_drop; }
        void set_diode_voltage_drop(const double & value) { this->diode_voltage_drop = value; }

        /**
         * Duty cycle for the converter, maximum 50%
         */
        std::optional<double> get_duty_cycle() const { return duty_cycle; }
        void set_duty_cycle(std::optional<double> value) { this->duty_cycle = value; }

        /**
         * The target efficiency
         */
        std::optional<double> get_efficiency() const { return efficiency; }
        void set_efficiency(std::optional<double> value) { this->efficiency = value; }

        /**
         * The input voltage of the pushPull
         */
        const DimensionWithTolerance & get_input_voltage() const { return input_voltage; }
        DimensionWithTolerance & get_mutable_input_voltage() { return input_voltage; }
        void set_input_voltage(const DimensionWithTolerance & value) { this->input_voltage = value; }

        /**
         * The maximum drain-source voltage in the selected switch
         */
        std::optional<double> get_maximum_drain_source_voltage() const { return maximum_drain_source_voltage; }
        void set_maximum_drain_source_voltage(std::optional<double> value) { this->maximum_drain_source_voltage = value; }

        /**
         * The maximum current that can go through the selected switch
         */
        std::optional<double> get_maximum_switch_current() const { return maximum_switch_current; }
        void set_maximum_switch_current(std::optional<double> value) { this->maximum_switch_current = value; }

        /**
         * A list of operating points
         */
        const std::vector<TopologyExcitation> & get_operating_points() const { return operating_points; }
        std::vector<TopologyExcitation> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<TopologyExcitation> & value) { this->operating_points = value; }
    };

    class SupportedTopologies {
        public:
        SupportedTopologies() = default;
        virtual ~SupportedTopologies() = default;

        private:
        std::optional<Boost> boost;
        std::optional<Buck> buck;
        std::optional<CllcResonant> cllc_resonant;
        std::optional<CurrentTransformer> current_transformer;
        std::optional<DualActiveBridge> dual_active_bridge;
        std::optional<Flyback> flyback;
        std::optional<Forward> forward;
        std::optional<IsolatedBuck> isolated_buck;
        std::optional<IsolatedBuckBoost> isolated_buck_boost;
        std::optional<LlcResonant> llc_resonant;
        std::optional<PhaseShiftFullBridge> phase_shift_full_bridge;
        std::optional<PushPull> push_pull;

        public:
        std::optional<Boost> get_boost() const { return boost; }
        void set_boost(std::optional<Boost> value) { this->boost = value; }

        std::optional<Buck> get_buck() const { return buck; }
        void set_buck(std::optional<Buck> value) { this->buck = value; }

        std::optional<CllcResonant> get_cllc_resonant() const { return cllc_resonant; }
        void set_cllc_resonant(std::optional<CllcResonant> value) { this->cllc_resonant = value; }

        std::optional<CurrentTransformer> get_current_transformer() const { return current_transformer; }
        void set_current_transformer(std::optional<CurrentTransformer> value) { this->current_transformer = value; }

        std::optional<DualActiveBridge> get_dual_active_bridge() const { return dual_active_bridge; }
        void set_dual_active_bridge(std::optional<DualActiveBridge> value) { this->dual_active_bridge = value; }

        std::optional<Flyback> get_flyback() const { return flyback; }
        void set_flyback(std::optional<Flyback> value) { this->flyback = value; }

        std::optional<Forward> get_forward() const { return forward; }
        void set_forward(std::optional<Forward> value) { this->forward = value; }

        std::optional<IsolatedBuck> get_isolated_buck() const { return isolated_buck; }
        void set_isolated_buck(std::optional<IsolatedBuck> value) { this->isolated_buck = value; }

        std::optional<IsolatedBuckBoost> get_isolated_buck_boost() const { return isolated_buck_boost; }
        void set_isolated_buck_boost(std::optional<IsolatedBuckBoost> value) { this->isolated_buck_boost = value; }

        std::optional<LlcResonant> get_llc_resonant() const { return llc_resonant; }
        void set_llc_resonant(std::optional<LlcResonant> value) { this->llc_resonant = value; }

        std::optional<PhaseShiftFullBridge> get_phase_shift_full_bridge() const { return phase_shift_full_bridge; }
        void set_phase_shift_full_bridge(std::optional<PhaseShiftFullBridge> value) { this->phase_shift_full_bridge = value; }

        std::optional<PushPull> get_push_pull() const { return push_pull; }
        void set_push_pull(std::optional<PushPull> value) { this->push_pull = value; }
    };

    class ConverterInformation {
        public:
        ConverterInformation() = default;
        virtual ~ConverterInformation() = default;

        private:
        std::optional<SupportedTopologies> supported_topologies;

        public:
        std::optional<SupportedTopologies> get_supported_topologies() const { return supported_topologies; }
        void set_supported_topologies(std::optional<SupportedTopologies> value) { this->supported_topologies = value; }
    };

    /**
     * Application of the magnetic, can be Power, Signal Processing, or Interference
     * Suppression
     *
     * List of applications a magnetic material can serve
     */
    enum class Application : int { INTERFERENCE_SUPPRESSION, POWER, SIGNAL_PROCESSING };

    /**
     * Required CTI
     */
    enum class Cti : int { GROUP_I, GROUP_II, GROUP_IIIA, GROUP_IIIB };

    /**
     * Required type of insulation
     *
     * Insulation grade classification as stated in the datasheet (e.g. 'reinforced', 'basic').
     * Aligns with IEC insulationType vocabulary.
     */
    enum class InsulationType : int { BASIC, DOUBLE, FUNCTIONAL, REINFORCED, SUPPLEMENTARY };

    /**
     * Required overvoltage category
     */
    enum class OvervoltageCategory : int { I, II, III, IV };

    /**
     * Required pollution for the magnetic to work under
     */
    enum class PollutionDegree : int { PD1, PD2, PD3, PD4 };

    enum class InsulationStandards : int { IEC_603351, IEC_606641, IEC_615581, IEC_623681 };

    class InsulationRequirements {
        public:
        InsulationRequirements() = default;
        virtual ~InsulationRequirements() = default;

        private:
        std::optional<DimensionWithTolerance> altitude;
        std::optional<Cti> cti;
        std::optional<InsulationType> insulation_type;
        std::optional<DimensionWithTolerance> main_supply_voltage;
        std::optional<OvervoltageCategory> overvoltage_category;
        std::optional<PollutionDegree> pollution_degree;
        std::optional<std::vector<InsulationStandards>> standards;

        public:
        /**
         * Required values for the altitude
         */
        std::optional<DimensionWithTolerance> get_altitude() const { return altitude; }
        void set_altitude(std::optional<DimensionWithTolerance> value) { this->altitude = value; }

        /**
         * Required CTI
         */
        std::optional<Cti> get_cti() const { return cti; }
        void set_cti(std::optional<Cti> value) { this->cti = value; }

        std::optional<InsulationType> get_insulation_type() const { return insulation_type; }
        void set_insulation_type(std::optional<InsulationType> value) { this->insulation_type = value; }

        /**
         * Voltage RMS of the main supply to which this transformer is connected to.
         */
        std::optional<DimensionWithTolerance> get_main_supply_voltage() const { return main_supply_voltage; }
        void set_main_supply_voltage(std::optional<DimensionWithTolerance> value) { this->main_supply_voltage = value; }

        /**
         * Required overvoltage category
         */
        std::optional<OvervoltageCategory> get_overvoltage_category() const { return overvoltage_category; }
        void set_overvoltage_category(std::optional<OvervoltageCategory> value) { this->overvoltage_category = value; }

        /**
         * Required pollution for the magnetic to work under
         */
        std::optional<PollutionDegree> get_pollution_degree() const { return pollution_degree; }
        void set_pollution_degree(std::optional<PollutionDegree> value) { this->pollution_degree = value; }

        /**
         * List of standards that will be taken into account for insulation.
         */
        std::optional<std::vector<InsulationStandards>> get_standards() const { return standards; }
        void set_standards(std::optional<std::vector<InsulationStandards>> value) { this->standards = value; }
    };

    /**
     * Tag to identify windings that are sharing the same ground
     */
    enum class IsolationSide : int { DENARY, DUODENARY, NONARY, OCTONARY, PRIMARY, QUATERNARY, QUINARY, SECONDARY, SENARY, SEPTENARY, TERTIARY, UNDENARY };

    /**
     * Market where the magnetic will end up being used
     */
    enum class Market : int { COMMERCIAL, INDUSTRIAL, MEDICAL, MILITARY, SPACE };

    /**
     * Maximum dimensions, width, height, and depth, for the designed magnetic, in m
     */
    class MaximumDimensions {
        public:
        MaximumDimensions() = default;
        virtual ~MaximumDimensions() = default;

        private:
        std::optional<double> depth;
        std::optional<double> height;
        std::optional<double> width;

        public:
        std::optional<double> get_depth() const { return depth; }
        void set_depth(std::optional<double> value) { this->depth = value; }

        std::optional<double> get_height() const { return height; }
        void set_height(std::optional<double> value) { this->height = value; }

        std::optional<double> get_width() const { return width; }
        void set_width(std::optional<double> value) { this->width = value; }
    };

    /**
     * Data describing one impendance value
     *
     * Impedance value. Uses the same impedancePoint structure as designRequirements.
     */
    class ImpedancePoint {
        public:
        ImpedancePoint() :
            magnitude_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~ImpedancePoint() = default;

        private:
        std::optional<double> imaginary_part;
        double magnitude;
        ClassMemberConstraints magnitude_constraint;
        std::optional<double> phase;
        std::optional<double> real_part;

        public:
        std::optional<double> get_imaginary_part() const { return imaginary_part; }
        void set_imaginary_part(std::optional<double> value) { this->imaginary_part = value; }

        /**
         * Magnitude of the impedance, in Ohm
         */
        const double & get_magnitude() const { return magnitude; }
        double & get_mutable_magnitude() { return magnitude; }
        void set_magnitude(const double & value) { CheckConstraint("magnitude", magnitude_constraint, value); this->magnitude = value; }

        std::optional<double> get_phase() const { return phase; }
        void set_phase(std::optional<double> value) { this->phase = value; }

        std::optional<double> get_real_part() const { return real_part; }
        void set_real_part(std::optional<double> value) { this->real_part = value; }
    };

    /**
     * An impedance value pinned to a specific frequency. The impedance is a structured
     * impedancePoint with magnitude, phase and real/imaginary parts. Bare-magnitude callers
     * populate magnitude only and leave phase / real / imaginary unset.
     */
    class ImpedanceAtFrequency {
        public:
        ImpedanceAtFrequency() :
            frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~ImpedanceAtFrequency() = default;

        private:
        double frequency;
        ClassMemberConstraints frequency_constraint;
        ImpedancePoint impedance;

        public:
        /**
         * Frequency at which the impedance applies. Unit: Hz.
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { CheckConstraint("frequency", frequency_constraint, value); this->frequency = value; }

        const ImpedancePoint & get_impedance() const { return impedance; }
        ImpedancePoint & get_mutable_impedance() { return impedance; }
        void set_impedance(const ImpedancePoint & value) { this->impedance = value; }
    };

    /**
     * Sub application of the magnetic, can be Power Filtering, Transforming, Isolation, Common
     * Mode Noise Filtering, Differential Mode Noise Filtering
     */
    enum class SubApplication : int { COMMON_MODE_NOISE_FILTERING, DIFFERENTIAL_MODE_NOISE_FILTERING, ISOLATION, POWER_FILTERING, TRANSFORMING };

    /**
     * Type of the terminal
     *
     * PCB mounting style. Uses the same connectionType enum as designRequirements.terminalType.
     */
    enum class ConnectionType : int { FLYING_LEAD, PCB_PAD, PIN, SCREW, SMT, THT };

    /**
     * Topology that will use the magnetic
     */
    enum class Topologies : int { ACTIVE_CLAMP_FORWARD_CONVERTER, BOOST_CONVERTER, BUCK_CONVERTER, CLLC_RESONANT_CONVERTER, COMMON_MODE_CHOKE, CUK_CONVERTER, CURRENT_TRANSFORMER, DIFFERENTIAL_MODE_CHOKE, DUAL_ACTIVE_BRIDGE_CONVERTER, FLYBACK_CONVERTER, FULL_BRIDGE_CONVERTER, HALF_BRIDGE_CONVERTER, INVERTING_BUCK_BOOST_CONVERTER, ISOLATED_BUCK_BOOST_CONVERTER, ISOLATED_BUCK_CONVERTER, LLC_RESONANT_CONVERTER, PHASE_SHIFTED_FULL_BRIDGE_CONVERTER, PHASE_SHIFTED_HALF_BRIDGE_CONVERTER, POWER_FACTOR_CORRECTION, PUSH_PULL_CONVERTER, SEPIC, SINGLE_SWITCH_FORWARD_CONVERTER, TWO_SWITCH_FORWARD_CONVERTER, WEINBERG_CONVERTER, ZETA_CONVERTER };

    /**
     * Technology that must be used to create the wiring
     *
     * Type of the group
     */
    enum class WiringTechnology : int { DEPOSITION, PRINTED, STAMPED, WOUND };

    /**
     * Data describing the design requirements
     *
     * The list of requirement that must comply a given magnetic
     */
    class DesignRequirements {
        public:
        DesignRequirements() = default;
        virtual ~DesignRequirements() = default;

        private:
        std::optional<Application> application;
        std::optional<InsulationRequirements> insulation;
        std::optional<std::vector<IsolationSide>> isolation_sides;
        std::optional<std::vector<DimensionWithTolerance>> leakage_inductance;
        DimensionWithTolerance magnetizing_inductance;
        std::optional<Market> market;
        std::optional<MaximumDimensions> maximum_dimensions;
        std::optional<double> maximum_weight;
        std::optional<std::vector<ImpedanceAtFrequency>> minimum_impedance;
        std::optional<std::string> name;
        std::optional<DimensionWithTolerance> operating_temperature;
        std::optional<std::vector<DimensionWithTolerance>> stray_capacitance;
        std::optional<SubApplication> sub_application;
        std::optional<std::vector<ConnectionType>> terminal_type;
        std::optional<Topologies> topology;
        std::vector<DimensionWithTolerance> turns_ratios;
        std::optional<WiringTechnology> wiring_technology;

        public:
        std::optional<Application> get_application() const { return application; }
        void set_application(std::optional<Application> value) { this->application = value; }

        std::optional<InsulationRequirements> get_insulation() const { return insulation; }
        void set_insulation(std::optional<InsulationRequirements> value) { this->insulation = value; }

        /**
         * Isolation side where each winding is connected to.
         */
        std::optional<std::vector<IsolationSide>> get_isolation_sides() const { return isolation_sides; }
        void set_isolation_sides(std::optional<std::vector<IsolationSide>> value) { this->isolation_sides = value; }

        /**
         * Required values for the leakage inductance
         */
        std::optional<std::vector<DimensionWithTolerance>> get_leakage_inductance() const { return leakage_inductance; }
        void set_leakage_inductance(std::optional<std::vector<DimensionWithTolerance>> value) { this->leakage_inductance = value; }

        /**
         * Required values for the magnetizing inductance
         */
        const DimensionWithTolerance & get_magnetizing_inductance() const { return magnetizing_inductance; }
        DimensionWithTolerance & get_mutable_magnetizing_inductance() { return magnetizing_inductance; }
        void set_magnetizing_inductance(const DimensionWithTolerance & value) { this->magnetizing_inductance = value; }

        /**
         * Market where the magnetic will end up being used
         */
        std::optional<Market> get_market() const { return market; }
        void set_market(std::optional<Market> value) { this->market = value; }

        /**
         * Maximum dimensions, width, height, and depth, for the designed magnetic, in m
         */
        std::optional<MaximumDimensions> get_maximum_dimensions() const { return maximum_dimensions; }
        void set_maximum_dimensions(std::optional<MaximumDimensions> value) { this->maximum_dimensions = value; }

        /**
         * Maximum weight for the designed magnetic, in Kg
         */
        std::optional<double> get_maximum_weight() const { return maximum_weight; }
        void set_maximum_weight(std::optional<double> value) { this->maximum_weight = value; }

        /**
         * List of minimum impedance at given frequency in the primary
         */
        std::optional<std::vector<ImpedanceAtFrequency>> get_minimum_impedance() const { return minimum_impedance; }
        void set_minimum_impedance(std::optional<std::vector<ImpedanceAtFrequency>> value) { this->minimum_impedance = value; }

        /**
         * A label that identifies these Design Requirements
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * Required values for the temperature that the magnetic can reach under operating
         */
        std::optional<DimensionWithTolerance> get_operating_temperature() const { return operating_temperature; }
        void set_operating_temperature(std::optional<DimensionWithTolerance> value) { this->operating_temperature = value; }

        /**
         * Required values for the stray capacitance
         */
        std::optional<std::vector<DimensionWithTolerance>> get_stray_capacitance() const { return stray_capacitance; }
        void set_stray_capacitance(std::optional<std::vector<DimensionWithTolerance>> value) { this->stray_capacitance = value; }

        std::optional<SubApplication> get_sub_application() const { return sub_application; }
        void set_sub_application(std::optional<SubApplication> value) { this->sub_application = value; }

        /**
         * Type of the terminal that must be used, per winding
         */
        std::optional<std::vector<ConnectionType>> get_terminal_type() const { return terminal_type; }
        void set_terminal_type(std::optional<std::vector<ConnectionType>> value) { this->terminal_type = value; }

        /**
         * Topology that will use the magnetic
         */
        std::optional<Topologies> get_topology() const { return topology; }
        void set_topology(std::optional<Topologies> value) { this->topology = value; }

        /**
         * Required turns ratios between primary and the rest of windings
         */
        const std::vector<DimensionWithTolerance> & get_turns_ratios() const { return turns_ratios; }
        std::vector<DimensionWithTolerance> & get_mutable_turns_ratios() { return turns_ratios; }
        void set_turns_ratios(const std::vector<DimensionWithTolerance> & value) { this->turns_ratios = value; }

        std::optional<WiringTechnology> get_wiring_technology() const { return wiring_technology; }
        void set_wiring_technology(std::optional<WiringTechnology> value) { this->wiring_technology = value; }
    };

    /**
     * Cooling method for the magnetic component
     *
     * Data describing a natural convection cooling
     *
     * Data describing a forced convection cooling
     *
     * Data describing a heatsink cooling
     *
     * Data describing a cold plate cooling
     */
    class Cooling {
        public:
        Cooling() = default;
        virtual ~Cooling() = default;

        private:
        std::optional<std::string> fluid;
        std::optional<double> temperature;
        std::optional<double> flow_diameter;
        std::optional<std::vector<double>> velocity;
        std::optional<std::vector<double>> dimensions;
        std::optional<double> interface_thermal_resistance;
        std::optional<double> interface_thickness;
        std::optional<double> thermal_resistance;
        std::optional<double> maximum_temperature;

        public:
        /**
         * Name of the fluid used
         */
        std::optional<std::string> get_fluid() const { return fluid; }
        void set_fluid(std::optional<std::string> value) { this->fluid = value; }

        /**
         * Temperature of the fluid, in Celsius. If absent, ambient temperature is assumed.
         *
         * Temperature of the fluid. To be used only if different from ambient temperature
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * Diameter of the fluid flow, normally defined as a fan diameter
         */
        std::optional<double> get_flow_diameter() const { return flow_diameter; }
        void set_flow_diameter(std::optional<double> value) { this->flow_diameter = value; }

        std::optional<std::vector<double>> get_velocity() const { return velocity; }
        void set_velocity(std::optional<std::vector<double>> value) { this->velocity = value; }

        /**
         * Dimensions of the cube defining the heatsink
         *
         * Dimensions of the cube defining the cold plate
         */
        std::optional<std::vector<double>> get_dimensions() const { return dimensions; }
        void set_dimensions(std::optional<std::vector<double>> value) { this->dimensions = value; }

        /**
         * Thermal resistance of the thermal interface used to connect the device to the heatsink.
         * Unit: K/W.
         *
         * Thermal resistance of the thermal interface used to connect the device to the cold plate.
         * Unit: K/W.
         */
        std::optional<double> get_interface_thermal_resistance() const { return interface_thermal_resistance; }
        void set_interface_thermal_resistance(std::optional<double> value) { this->interface_thermal_resistance = value; }

        /**
         * Thickness of the thermal interface used to connect the device to the heatsink, in m
         *
         * Thickness of the thermal interface used to connect the device to the cold plate, in m
         */
        std::optional<double> get_interface_thickness() const { return interface_thickness; }
        void set_interface_thickness(std::optional<double> value) { this->interface_thickness = value; }

        /**
         * Bulk thermal resistance of the heat sink. Unit: K/W.
         *
         * Bulk thermal resistance of the cold plate. Unit: K/W.
         */
        std::optional<double> get_thermal_resistance() const { return thermal_resistance; }
        void set_thermal_resistance(std::optional<double> value) { this->thermal_resistance = value; }

        /**
         * Maximum temperature of the cold plate. Unit: Celsius.
         */
        std::optional<double> get_maximum_temperature() const { return maximum_temperature; }
        void set_maximum_temperature(std::optional<double> value) { this->maximum_temperature = value; }
    };

    /**
     * The description of a magnetic operating conditions
     */
    class OperatingConditions {
        public:
        OperatingConditions() :
            ambient_relative_humidity_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            ambient_temperature_constraint(std::nullopt, std::nullopt, -273.15, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~OperatingConditions() = default;

        private:
        std::optional<double> ambient_relative_humidity;
        ClassMemberConstraints ambient_relative_humidity_constraint;
        double ambient_temperature;
        ClassMemberConstraints ambient_temperature_constraint;
        std::optional<Cooling> cooling;
        std::optional<std::string> name;

        public:
        /**
         * Relative Humidity of the ambient where the magnetic will operate
         */
        std::optional<double> get_ambient_relative_humidity() const { return ambient_relative_humidity; }
        void set_ambient_relative_humidity(std::optional<double> value) { if (value) CheckConstraint("ambient_relative_humidity", ambient_relative_humidity_constraint, *value); this->ambient_relative_humidity = value; }

        /**
         * Temperature of the ambient where the magnetic will operate
         */
        const double & get_ambient_temperature() const { return ambient_temperature; }
        double & get_mutable_ambient_temperature() { return ambient_temperature; }
        void set_ambient_temperature(const double & value) { CheckConstraint("ambient_temperature", ambient_temperature_constraint, value); this->ambient_temperature = value; }

        /**
         * Cooling method for the magnetic component
         */
        std::optional<Cooling> get_cooling() const { return cooling; }
        void set_cooling(std::optional<Cooling> value) { this->cooling = value; }

        /**
         * A label that identifies this Operating Conditions
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }
    };

    /**
     * Data containing the harmonics of the waveform, defined by a list of amplitudes and a list
     * of frequencies
     */
    class Harmonics {
        public:
        Harmonics() = default;
        virtual ~Harmonics() = default;

        private:
        std::vector<double> amplitudes;
        std::vector<double> frequencies;

        public:
        /**
         * List of amplitudes of the harmonics that compose the waveform
         */
        const std::vector<double> & get_amplitudes() const { return amplitudes; }
        std::vector<double> & get_mutable_amplitudes() { return amplitudes; }
        void set_amplitudes(const std::vector<double> & value) { this->amplitudes = value; }

        /**
         * List of frequencies of the harmonics that compose the waveform
         */
        const std::vector<double> & get_frequencies() const { return frequencies; }
        std::vector<double> & get_mutable_frequencies() { return frequencies; }
        void set_frequencies(const std::vector<double> & value) { this->frequencies = value; }
    };

    class Processed {
        public:
        Processed() :
            ac_effective_frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            duty_cycle_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            effective_frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            negative_peak_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            peak_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            peak_to_peak_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            positive_peak_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            rms_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~Processed() = default;

        private:
        std::optional<double> ac_effective_frequency;
        ClassMemberConstraints ac_effective_frequency_constraint;
        std::optional<double> average;
        std::optional<double> dead_time;
        std::optional<double> duty_cycle;
        ClassMemberConstraints duty_cycle_constraint;
        std::optional<double> effective_frequency;
        ClassMemberConstraints effective_frequency_constraint;
        WaveformLabel label;
        std::optional<double> negative_peak;
        ClassMemberConstraints negative_peak_constraint;
        double offset;
        std::optional<double> peak;
        ClassMemberConstraints peak_constraint;
        std::optional<double> peak_to_peak;
        ClassMemberConstraints peak_to_peak_constraint;
        std::optional<double> phase;
        std::optional<double> positive_peak;
        ClassMemberConstraints positive_peak_constraint;
        std::optional<double> rms;
        ClassMemberConstraints rms_constraint;
        std::optional<double> thd;

        public:
        /**
         * Effective (equivalent-sine) frequency of the AC component of the waveform (DC component
         * removed). Unit: Hz.
         */
        std::optional<double> get_ac_effective_frequency() const { return ac_effective_frequency; }
        void set_ac_effective_frequency(std::optional<double> value) { if (value) CheckConstraint("ac_effective_frequency", ac_effective_frequency_constraint, *value); this->ac_effective_frequency = value; }

        /**
         * The average value of the waveform, referred to 0
         */
        std::optional<double> get_average() const { return average; }
        void set_average(std::optional<double> value) { this->average = value; }

        /**
         * The dead time after TOn and Toff, in seconds, if applicable
         */
        std::optional<double> get_dead_time() const { return dead_time; }
        void set_dead_time(std::optional<double> value) { this->dead_time = value; }

        /**
         * The duty cycle of the waveform, if applicable
         */
        std::optional<double> get_duty_cycle() const { return duty_cycle; }
        void set_duty_cycle(std::optional<double> value) { if (value) CheckConstraint("duty_cycle", duty_cycle_constraint, *value); this->duty_cycle = value; }

        /**
         * Effective (equivalent-sine) frequency of the waveform, defined as the frequency of a
         * sinusoid with the same RMS rate of change. Unit: Hz.
         */
        std::optional<double> get_effective_frequency() const { return effective_frequency; }
        void set_effective_frequency(std::optional<double> value) { if (value) CheckConstraint("effective_frequency", effective_frequency_constraint, *value); this->effective_frequency = value; }

        const WaveformLabel & get_label() const { return label; }
        WaveformLabel & get_mutable_label() { return label; }
        void set_label(const WaveformLabel & value) { this->label = value; }

        /**
         * The most-negative value of the waveform (always <= 0 for bipolar signals)
         */
        std::optional<double> get_negative_peak() const { return negative_peak; }
        void set_negative_peak(std::optional<double> value) { if (value) CheckConstraint("negative_peak", negative_peak_constraint, *value); this->negative_peak = value; }

        /**
         * The offset value of the waveform, referred to 0
         */
        const double & get_offset() const { return offset; }
        double & get_mutable_offset() { return offset; }
        void set_offset(const double & value) { this->offset = value; }

        /**
         * The maximum absolute value of the waveform
         */
        std::optional<double> get_peak() const { return peak; }
        void set_peak(std::optional<double> value) { if (value) CheckConstraint("peak", peak_constraint, *value); this->peak = value; }

        /**
         * The peak to peak value of the waveform
         */
        std::optional<double> get_peak_to_peak() const { return peak_to_peak; }
        void set_peak_to_peak(std::optional<double> value) { if (value) CheckConstraint("peak_to_peak", peak_to_peak_constraint, *value); this->peak_to_peak = value; }

        /**
         * The phase of the waveform, in degrees
         */
        std::optional<double> get_phase() const { return phase; }
        void set_phase(std::optional<double> value) { this->phase = value; }

        /**
         * The maximum positive value of the waveform
         */
        std::optional<double> get_positive_peak() const { return positive_peak; }
        void set_positive_peak(std::optional<double> value) { if (value) CheckConstraint("positive_peak", positive_peak_constraint, *value); this->positive_peak = value; }

        /**
         * The RMS value of the waveform
         */
        std::optional<double> get_rms() const { return rms; }
        void set_rms(std::optional<double> value) { if (value) CheckConstraint("rms", rms_constraint, *value); this->rms = value; }

        /**
         * The Total Harmonic Distortion of the waveform, according to
         * https://en.wikipedia.org/wiki/Total_harmonic_distortion
         */
        std::optional<double> get_thd() const { return thd; }
        void set_thd(std::optional<double> value) { this->thd = value; }
    };

    /**
     * Data containing the points that define an arbitrary waveform with equidistant points
     *
     * Data containing the points that define an arbitrary waveform with non-equidistant points
     * paired with their time in the period
     */
    class Waveform {
        public:
        Waveform() :
            number_periods_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~Waveform() = default;

        private:
        std::vector<double> data;
        std::optional<int64_t> number_periods;
        ClassMemberConstraints number_periods_constraint;
        std::optional<WaveformLabel> ancillary_label;
        std::optional<std::vector<double>> time;

        public:
        /**
         * List of values that compose the waveform, at equidistant times form each other
         */
        const std::vector<double> & get_data() const { return data; }
        std::vector<double> & get_mutable_data() { return data; }
        void set_data(const std::vector<double> & value) { this->data = value; }

        /**
         * The number of periods covered by the data
         */
        std::optional<int64_t> get_number_periods() const { return number_periods; }
        void set_number_periods(std::optional<int64_t> value) { if (value) CheckConstraint("number_periods", number_periods_constraint, *value); this->number_periods = value; }

        std::optional<WaveformLabel> get_ancillary_label() const { return ancillary_label; }
        void set_ancillary_label(std::optional<WaveformLabel> value) { this->ancillary_label = value; }

        std::optional<std::vector<double>> get_time() const { return time; }
        void set_time(std::optional<std::vector<double>> value) { this->time = value; }
    };

    /**
     * Excitation of the B field that produced the core losses
     *
     * Structure definining one electromagnetic parameters: current, voltage, magnetic flux
     * density
     */
    class SignalDescriptor {
        public:
        SignalDescriptor() = default;
        virtual ~SignalDescriptor() = default;

        private:
        std::optional<Harmonics> harmonics;
        std::optional<Processed> processed;
        std::optional<Waveform> waveform;

        public:
        /**
         * Data containing the harmonics of the waveform, defined by a list of amplitudes and a list
         * of frequencies
         */
        std::optional<Harmonics> get_harmonics() const { return harmonics; }
        void set_harmonics(std::optional<Harmonics> value) { this->harmonics = value; }

        std::optional<Processed> get_processed() const { return processed; }
        void set_processed(std::optional<Processed> value) { this->processed = value; }

        std::optional<Waveform> get_waveform() const { return waveform; }
        void set_waveform(std::optional<Waveform> value) { this->waveform = value; }
    };

    /**
     * Data describing the excitation of the winding
     *
     * The description of a magnetic operating point
     */
    class OperatingPointExcitation {
        public:
        OperatingPointExcitation() :
            frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~OperatingPointExcitation() = default;

        private:
        std::optional<SignalDescriptor> current;
        double frequency;
        ClassMemberConstraints frequency_constraint;
        std::optional<SignalDescriptor> magnetic_field_strength;
        std::optional<SignalDescriptor> magnetic_flux_density;
        std::optional<SignalDescriptor> magnetizing_current;
        std::optional<std::string> name;
        std::optional<SignalDescriptor> voltage;

        public:
        std::optional<SignalDescriptor> get_current() const { return current; }
        void set_current(std::optional<SignalDescriptor> value) { this->current = value; }

        /**
         * Frequency of the waveform, common for all electromagnetic parameters, in Hz
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { CheckConstraint("frequency", frequency_constraint, value); this->frequency = value; }

        std::optional<SignalDescriptor> get_magnetic_field_strength() const { return magnetic_field_strength; }
        void set_magnetic_field_strength(std::optional<SignalDescriptor> value) { this->magnetic_field_strength = value; }

        std::optional<SignalDescriptor> get_magnetic_flux_density() const { return magnetic_flux_density; }
        void set_magnetic_flux_density(std::optional<SignalDescriptor> value) { this->magnetic_flux_density = value; }

        std::optional<SignalDescriptor> get_magnetizing_current() const { return magnetizing_current; }
        void set_magnetizing_current(std::optional<SignalDescriptor> value) { this->magnetizing_current = value; }

        /**
         * A label that identifies this Operating Point
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        std::optional<SignalDescriptor> get_voltage() const { return voltage; }
        void set_voltage(std::optional<SignalDescriptor> value) { this->voltage = value; }
    };

    /**
     * Data describing one operating point, including the operating conditions and the
     * excitations for all ports
     *
     * Excitation of the current per winding that produced the winding losses.
     */
    class OperatingPoint {
        public:
        OperatingPoint() = default;
        virtual ~OperatingPoint() = default;

        private:
        OperatingConditions conditions;
        std::vector<OperatingPointExcitation> excitations_per_winding;
        std::optional<std::string> name;

        public:
        const OperatingConditions & get_conditions() const { return conditions; }
        OperatingConditions & get_mutable_conditions() { return conditions; }
        void set_conditions(const OperatingConditions & value) { this->conditions = value; }

        const std::vector<OperatingPointExcitation> & get_excitations_per_winding() const { return excitations_per_winding; }
        std::vector<OperatingPointExcitation> & get_mutable_excitations_per_winding() { return excitations_per_winding; }
        void set_excitations_per_winding(const std::vector<OperatingPointExcitation> & value) { this->excitations_per_winding = value; }

        /**
         * Name describing this operating point
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }
    };

    /**
     * The description of the inputs that can be used to design a Magnetic
     */
    class Inputs {
        public:
        Inputs() = default;
        virtual ~Inputs() = default;

        private:
        std::optional<ConverterInformation> converter_information;
        DesignRequirements design_requirements;
        std::vector<OperatingPoint> operating_points;

        public:
        std::optional<ConverterInformation> get_converter_information() const { return converter_information; }
        void set_converter_information(std::optional<ConverterInformation> value) { this->converter_information = value; }

        /**
         * Data describing the design requirements
         */
        const DesignRequirements & get_design_requirements() const { return design_requirements; }
        DesignRequirements & get_mutable_design_requirements() { return design_requirements; }
        void set_design_requirements(const DesignRequirements & value) { this->design_requirements = value; }

        /**
         * Data describing the operating points
         */
        const std::vector<OperatingPoint> & get_operating_points() const { return operating_points; }
        std::vector<OperatingPoint> & get_mutable_operating_points() { return operating_points; }
        void set_operating_points(const std::vector<OperatingPoint> & value) { this->operating_points = value; }
    };

    /**
     * The distributor's price for this part.
     *
     * A monetary value with an explicit currency (ISO 4217 code).
     *
     * The manufacturer's reference price for this part.
     */
    class Cost {
        public:
        Cost() :
            currency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::string("^[A-Z]{3}$")),
            value_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~Cost() = default;

        private:
        std::string currency;
        ClassMemberConstraints currency_constraint;
        double value;
        ClassMemberConstraints value_constraint;

        public:
        /**
         * ISO 4217 three-letter currency code, e.g. EUR, USD, CNY, JPY.
         */
        const std::string & get_currency() const { return currency; }
        std::string & get_mutable_currency() { return currency; }
        void set_currency(const std::string & value) { CheckConstraint("currency", currency_constraint, value); this->currency = value; }

        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { CheckConstraint("value", value_constraint, value); this->value = value; }
    };

    /**
     * Data from the distributor for a given part
     */
    class DistributorInfo {
        public:
        DistributorInfo() = default;
        virtual ~DistributorInfo() = default;

        private:
        std::optional<Cost> cost;
        std::optional<std::string> country;
        std::optional<std::string> distributed_area;
        std::optional<std::string> email;
        std::optional<std::string> link;
        std::string name;
        std::optional<std::string> phone;
        double quantity;
        std::string reference;
        std::optional<std::string> updated_at;

        public:
        /**
         * The distributor's price for this part.
         */
        std::optional<Cost> get_cost() const { return cost; }
        void set_cost(std::optional<Cost> value) { this->cost = value; }

        /**
         * The country of the distributor of the part
         */
        std::optional<std::string> get_country() const { return country; }
        void set_country(std::optional<std::string> value) { this->country = value; }

        /**
         * Geographical area in which the distributor operates.
         */
        std::optional<std::string> get_distributed_area() const { return distributed_area; }
        void set_distributed_area(std::optional<std::string> value) { this->distributed_area = value; }

        /**
         * The distributor's email
         */
        std::optional<std::string> get_email() const { return email; }
        void set_email(std::optional<std::string> value) { this->email = value; }

        /**
         * The distributor's link
         */
        std::optional<std::string> get_link() const { return link; }
        void set_link(std::optional<std::string> value) { this->link = value; }

        /**
         * The name of the distributor of the part
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * The distributor's phone
         */
        std::optional<std::string> get_phone() const { return phone; }
        void set_phone(std::optional<std::string> value) { this->phone = value; }

        /**
         * The number of individual pieces available in the distributor
         */
        const double & get_quantity() const { return quantity; }
        double & get_mutable_quantity() { return quantity; }
        void set_quantity(const double & value) { this->quantity = value; }

        /**
         * The distributor's reference of this part
         */
        const std::string & get_reference() const { return reference; }
        std::string & get_mutable_reference() { return reference; }
        void set_reference(const std::string & value) { this->reference = value; }

        /**
         * The date that this information was updated
         */
        std::optional<std::string> get_updated_at() const { return updated_at; }
        void set_updated_at(std::optional<std::string> value) { this->updated_at = value; }
    };

    class PinWindingConnection {
        public:
        PinWindingConnection() = default;
        virtual ~PinWindingConnection() = default;

        private:
        std::optional<std::string> pin;
        std::optional<std::string> winding;

        public:
        /**
         * The name of the connected pin
         */
        std::optional<std::string> get_pin() const { return pin; }
        void set_pin(std::optional<std::string> value) { this->pin = value; }

        /**
         * The name of the connected winding
         */
        std::optional<std::string> get_winding() const { return winding; }
        void set_winding(std::optional<std::string> value) { this->winding = value; }
    };

    using Dimension = std::variant<DimensionWithTolerance, double>;

    /**
     * Bobbin family, named after the core shape family it is intended to fit. See IEC 62317 /
     * IEC 63093.
     */
    enum class BobbinFamily : int { E, EC, EFD, EL, EP, ER, ETD, P, PM, PQ, RM, T, U };

    /**
     * data for describing one point of dielectric strength
     */
    class DielectricStrengthElement {
        public:
        DielectricStrengthElement() :
            humidity_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~DielectricStrengthElement() = default;

        private:
        std::optional<double> humidity;
        ClassMemberConstraints humidity_constraint;
        std::optional<double> temperature;
        std::optional<double> thickness;
        double value;

        public:
        /**
         * Humidity for the field value, in proportion over 1
         */
        std::optional<double> get_humidity() const { return humidity; }
        void set_humidity(std::optional<double> value) { if (value) CheckConstraint("humidity", humidity_constraint, *value); this->humidity = value; }

        /**
         * Temperature for the field value, in Celsius.
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * Thickness of the material
         */
        std::optional<double> get_thickness() const { return thickness; }
        void set_thickness(std::optional<double> value) { this->thickness = value; }

        /**
         * Dielectric strength value, in V / m
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    /**
     * The production status of a part according to its manufacturer
     */
    enum class Status : int { OBSOLETE, PRODUCTION, PROTOTYPE };

    /**
     * Data from the manufacturer for a given part
     */
    class ManufacturerInfo {
        public:
        ManufacturerInfo() :
            irdi_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::string("^[0-9A-Za-z]{4}(?:-[0-9A-Za-z]+)*#[0-9A-Za-z./-]+#[0-9A-Za-z.-]+$"))
        {}
        virtual ~ManufacturerInfo() = default;

        private:
        std::optional<Cost> cost;
        std::optional<std::string> datasheet_url;
        std::optional<std::string> description;
        std::optional<std::string> family;
        std::optional<std::string> irdi;
        ClassMemberConstraints irdi_constraint;
        std::string name;
        std::optional<std::string> order_code;
        std::optional<std::string> reference;
        std::optional<Status> status;

        public:
        /**
         * The manufacturer's reference price for this part.
         */
        std::optional<Cost> get_cost() const { return cost; }
        void set_cost(std::optional<Cost> value) { this->cost = value; }

        /**
         * The manufacturer's URL to the datasheet of the product
         */
        std::optional<std::string> get_datasheet_url() const { return datasheet_url; }
        void set_datasheet_url(std::optional<std::string> value) { this->datasheet_url = value; }

        /**
         * The description of the part according to its manufacturer
         */
        std::optional<std::string> get_description() const { return description; }
        void set_description(std::optional<std::string> value) { this->description = value; }

        /**
         * The family of a magnetic, as defined by the manufacturer
         */
        std::optional<std::string> get_family() const { return family; }
        void set_family(std::optional<std::string> value) { this->family = value; }

        /**
         * Optional International Registration Data Identifier (per ISO/IEC 11179-6) issued by an
         * authoritative registry such as IEC CDD or ECLASS. When populated, this is the canonical
         * machine-readable identifier for the part class.
         */
        std::optional<std::string> get_irdi() const { return irdi; }
        void set_irdi(std::optional<std::string> value) { if (value) CheckConstraint("irdi", irdi_constraint, *value); this->irdi = value; }

        /**
         * The name of the manufacturer of the part
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * The manufacturer's order code of this part
         */
        std::optional<std::string> get_order_code() const { return order_code; }
        void set_order_code(std::optional<std::string> value) { this->order_code = value; }

        /**
         * The manufacturer's reference of this part
         */
        std::optional<std::string> get_reference() const { return reference; }
        void set_reference(std::optional<std::string> value) { this->reference = value; }

        /**
         * The production status of a part according to its manufacturer
         */
        std::optional<Status> get_status() const { return status; }
        void set_status(std::optional<Status> value) { this->status = value; }
    };

    /**
     * data for describing one point of resistivity
     */
    class ResistivityPoint {
        public:
        ResistivityPoint() = default;
        virtual ~ResistivityPoint() = default;

        private:
        std::optional<double> temperature;
        double value;

        public:
        /**
         * Temperature for the field value, in Celsius.
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * Resistivity value, in Ohm * m
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    enum class TemperatureClassEnum : int { A, B, E, F, H, N, R, THE_200, THE_220, THE_250, Y };

    using TemperatureClassUnion = std::variant<double, TemperatureClassEnum>;

    /**
     * A material for insulation
     */
    class InsulationMaterial {
        public:
        InsulationMaterial() = default;
        virtual ~InsulationMaterial() = default;

        private:
        std::optional<std::vector<std::string>> aliases;
        std::optional<std::string> composition;
        std::vector<DielectricStrengthElement> dielectric_strength;
        std::optional<ManufacturerInfo> manufacturer_info;
        std::optional<double> melting_point;
        std::string name;
        std::optional<double> relative_permittivity;
        std::optional<std::vector<ResistivityPoint>> resistivity;
        std::optional<double> specific_heat;
        std::optional<std::vector<ResistivityPoint>> surface_resistivity;
        std::optional<TemperatureClassUnion> temperature_class;
        std::optional<double> thermal_conductivity;

        public:
        /**
         * Alternative names of the material
         */
        std::optional<std::vector<std::string>> get_aliases() const { return aliases; }
        void set_aliases(std::optional<std::vector<std::string>> value) { this->aliases = value; }

        /**
         * The composition of a insulation material
         */
        std::optional<std::string> get_composition() const { return composition; }
        void set_composition(std::optional<std::string> value) { this->composition = value; }

        const std::vector<DielectricStrengthElement> & get_dielectric_strength() const { return dielectric_strength; }
        std::vector<DielectricStrengthElement> & get_mutable_dielectric_strength() { return dielectric_strength; }
        void set_dielectric_strength(const std::vector<DielectricStrengthElement> & value) { this->dielectric_strength = value; }

        std::optional<ManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<ManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * The melting temperature of the insulation material, in Celsius
         */
        std::optional<double> get_melting_point() const { return melting_point; }
        void set_melting_point(std::optional<double> value) { this->melting_point = value; }

        /**
         * The name of a insulation material
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * The dielectric constant of the insulation material
         */
        std::optional<double> get_relative_permittivity() const { return relative_permittivity; }
        void set_relative_permittivity(std::optional<double> value) { this->relative_permittivity = value; }

        /**
         * Volume resistivity per IEC 60093. Unit: ohm metre (Ohm.m).
         */
        std::optional<std::vector<ResistivityPoint>> get_resistivity() const { return resistivity; }
        void set_resistivity(std::optional<std::vector<ResistivityPoint>> value) { this->resistivity = value; }

        /**
         * Specific heat capacity of the insulation material. Unit: J/(kg*K).
         */
        std::optional<double> get_specific_heat() const { return specific_heat; }
        void set_specific_heat(std::optional<double> value) { this->specific_heat = value; }

        /**
         * Surface resistivity per IEC 60093. Unit: ohm per square (Ohm/sq). Relevant to tracking
         * and creepage assessment per IEC 60112 / IEC 60664-1.
         */
        std::optional<std::vector<ResistivityPoint>> get_surface_resistivity() const { return surface_resistivity; }
        void set_surface_resistivity(std::optional<std::vector<ResistivityPoint>> value) { this->surface_resistivity = value; }

        /**
         * Insulation thermal class per IEC 60085. May be expressed as the IEC letter class (Y, A,
         * E, B, F, H, N, R, 200, 220, 250) or as the numeric maximum continuous operating
         * temperature in Celsius (the form printed on most magnet-wire spools and IEC 60317
         * sub-spec datasheets). The two forms are interchangeable per the IEC 60085 mapping. See
         * docs/normative-references.md.
         */
        std::optional<TemperatureClassUnion> get_temperature_class() const { return temperature_class; }
        void set_temperature_class(std::optional<TemperatureClassUnion> value) { this->temperature_class = value; }

        /**
         * Thermal conductivity of the insulation material. Unit: W/(m*K).
         */
        std::optional<double> get_thermal_conductivity() const { return thermal_conductivity; }
        void set_thermal_conductivity(std::optional<double> value) { this->thermal_conductivity = value; }
    };

    using InsulationMaterialDataOrNameUnion = std::variant<InsulationMaterial, std::string>;

    /**
     * The shape of the pin
     */
    enum class PinShape : int { IRREGULAR, RECTANGULAR, ROUND };

    /**
     * Type of pin
     */
    enum class PinDescriptionType : int { SMD, THT };

    /**
     * Data describing one pin in a bobbin
     */
    class Pin {
        public:
        Pin() = default;
        virtual ~Pin() = default;

        private:
        std::optional<std::vector<double>> coordinates;
        std::vector<double> dimensions;
        std::optional<std::string> name;
        std::optional<std::vector<double>> rotation;
        PinShape shape;
        PinDescriptionType type;

        public:
        /**
         * The coordinates of the center of the pin, referred to the center of the main column
         */
        std::optional<std::vector<double>> get_coordinates() const { return coordinates; }
        void set_coordinates(std::optional<std::vector<double>> value) { this->coordinates = value; }

        /**
         * Dimensions of the rectangle defining the pin
         */
        const std::vector<double> & get_dimensions() const { return dimensions; }
        std::vector<double> & get_mutable_dimensions() { return dimensions; }
        void set_dimensions(const std::vector<double> & value) { this->dimensions = value; }

        /**
         * Name given to the pin
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * The rotation of the pin, default is vertical
         */
        std::optional<std::vector<double>> get_rotation() const { return rotation; }
        void set_rotation(std::optional<std::vector<double>> value) { this->rotation = value; }

        /**
         * The shape of the pin
         */
        const PinShape & get_shape() const { return shape; }
        PinShape & get_mutable_shape() { return shape; }
        void set_shape(const PinShape & value) { this->shape = value; }

        /**
         * Type of pin
         */
        const PinDescriptionType & get_type() const { return type; }
        PinDescriptionType & get_mutable_type() { return type; }
        void set_type(const PinDescriptionType & value) { this->type = value; }
    };

    using Pitch = std::variant<std::vector<double>, double>;

    /**
     * Data describing the pinout of a bobbin
     */
    class Pinout {
        public:
        Pinout() = default;
        virtual ~Pinout() = default;

        private:
        std::optional<double> central_pitch;
        int64_t number_pins;
        std::optional<std::vector<int64_t>> number_pins_per_row;
        std::optional<int64_t> number_rows;
        Pin pin_description;
        Pitch pitch;
        double row_distance;

        public:
        /**
         * The distance between central pins
         */
        std::optional<double> get_central_pitch() const { return central_pitch; }
        void set_central_pitch(std::optional<double> value) { this->central_pitch = value; }

        /**
         * The number of pins
         */
        const int64_t & get_number_pins() const { return number_pins; }
        int64_t & get_mutable_number_pins() { return number_pins; }
        void set_number_pins(const int64_t & value) { this->number_pins = value; }

        /**
         * List of pins per row
         */
        std::optional<std::vector<int64_t>> get_number_pins_per_row() const { return number_pins_per_row; }
        void set_number_pins_per_row(std::optional<std::vector<int64_t>> value) { this->number_pins_per_row = value; }

        /**
         * The number of rows of a bobbin, typically 2
         */
        std::optional<int64_t> get_number_rows() const { return number_rows; }
        void set_number_rows(std::optional<int64_t> value) { this->number_rows = value; }

        const Pin & get_pin_description() const { return pin_description; }
        Pin & get_mutable_pin_description() { return pin_description; }
        void set_pin_description(const Pin & value) { this->pin_description = value; }

        const Pitch & get_pitch() const { return pitch; }
        Pitch & get_mutable_pitch() { return pitch; }
        void set_pitch(const Pitch & value) { this->pitch = value; }

        /**
         * The distance between a row of pins and the center of the bobbin
         */
        const double & get_row_distance() const { return row_distance; }
        double & get_mutable_row_distance() { return row_distance; }
        void set_row_distance(const double & value) { this->row_distance = value; }
    };

    /**
     * Whether the bobbin is a standard catalogue part or a custom design.
     *
     * The type of a magnetic shape
     */
    enum class FunctionalDescriptionType : int { CUSTOM, STANDARD };

    /**
     * Bobbin data described in functional terms, in a form suitable for analytical models.
     */
    class BobbinFunctionalDescription {
        public:
        BobbinFunctionalDescription() = default;
        virtual ~BobbinFunctionalDescription() = default;

        private:
        std::optional<std::vector<PinWindingConnection>> connections;
        std::map<std::string, Dimension> dimensions;
        BobbinFamily family;
        std::optional<std::string> family_subtype;
        std::optional<InsulationMaterialDataOrNameUnion> material;
        std::optional<Pinout> pinout;
        std::string shape;
        FunctionalDescriptionType type;

        public:
        /**
         * List of connections between windings and pins
         */
        std::optional<std::vector<PinWindingConnection>> get_connections() const { return connections; }
        void set_connections(std::optional<std::vector<PinWindingConnection>> value) { this->connections = value; }

        /**
         * Bobbin dimensions. Keys are the dimension labels defined in IEC 62317 / IEC 63093 for the
         * parent core shape.
         */
        const std::map<std::string, Dimension> & get_dimensions() const { return dimensions; }
        std::map<std::string, Dimension> & get_mutable_dimensions() { return dimensions; }
        void set_dimensions(const std::map<std::string, Dimension> & value) { this->dimensions = value; }

        /**
         * Bobbin family, named after the core shape family it is intended to fit. See IEC 62317 /
         * IEC 63093.
         */
        const BobbinFamily & get_family() const { return family; }
        BobbinFamily & get_mutable_family() { return family; }
        void set_family(const BobbinFamily & value) { this->family = value; }

        /**
         * Subtype of the shape, where more than one variant exists in the family.
         */
        std::optional<std::string> get_family_subtype() const { return family_subtype; }
        void set_family_subtype(std::optional<std::string> value) { this->family_subtype = value; }

        std::optional<InsulationMaterialDataOrNameUnion> get_material() const { return material; }
        void set_material(std::optional<InsulationMaterialDataOrNameUnion> value) { this->material = value; }

        std::optional<Pinout> get_pinout() const { return pinout; }
        void set_pinout(std::optional<Pinout> value) { this->pinout = value; }

        /**
         * Name of the core shape this bobbin is matched to.
         */
        const std::string & get_shape() const { return shape; }
        std::string & get_mutable_shape() { return shape; }
        void set_shape(const std::string & value) { this->shape = value; }

        /**
         * Whether the bobbin is a standard catalogue part or a custom design.
         */
        const FunctionalDescriptionType & get_type() const { return type; }
        FunctionalDescriptionType & get_mutable_type() { return type; }
        void set_type(const FunctionalDescriptionType & value) { this->type = value; }
    };

    /**
     * Shape of the column, also used for gaps
     */
    enum class ColumnShape : int { IRREGULAR, OBLONG, RECTANGULAR, ROUND };

    /**
     * Way in which the sections are aligned inside the winding window
     *
     * Way in which the turns are aligned inside the layer
     *
     * Way in which the layers are aligned inside the section
     */
    enum class CoilAlignment : int { CENTERED, INNER_OR_TOP, OUTER_OR_BOTTOM, SPREAD };

    /**
     * Way in which the sections are oriented inside the winding window
     *
     * Way in which the layer is oriented inside the section
     *
     * Way in which the layers are oriented inside the section
     */
    enum class WindingOrientation : int { CONTIGUOUS, OVERLAPPING };

    enum class WindingWindowShape : int { RECTANGULAR, ROUND };

    /**
     * List of rectangular winding windows
     *
     * It is the area between the winding column and the closest lateral column, and it
     * represents the area where all the wires of the magnetic will have to fit, and
     * equivalently, where all the current must circulate once, in the case of inductors, or
     * twice, in the case of transformers
     *
     * List of radial winding windows
     *
     * It is the area between the delimited between a height from the surface of the toroidal
     * core at a given angle, and it represents the area where all the wires of the magnetic
     * will have to fit, and equivalently, where all the current must circulate once, in the
     * case of inductors, or twice, in the case of transformers
     */
    class WindingWindowElement {
        public:
        WindingWindowElement() = default;
        virtual ~WindingWindowElement() = default;

        private:
        std::optional<double> area;
        std::optional<std::vector<double>> coordinates;
        std::optional<double> height;
        std::optional<CoilAlignment> sections_alignment;
        std::optional<WindingOrientation> sections_orientation;
        std::optional<WindingWindowShape> shape;
        std::optional<double> width;
        std::optional<double> angle;
        std::optional<double> radial_height;

        public:
        /**
         * Area of the winding window
         */
        std::optional<double> get_area() const { return area; }
        void set_area(std::optional<double> value) { this->area = value; }

        /**
         * The coordinates of the center of the winding window, referred to the center of the main
         * column. In the case of half-sets, the center will be in the top point, where it would
         * join another half-set
         *
         * The coordinates of the point of the winding window where the middle height touches the
         * main column, referred to the center of the main column. In the case of half-sets, the
         * center will be in the top point, where it would join another half-set
         */
        std::optional<std::vector<double>> get_coordinates() const { return coordinates; }
        void set_coordinates(std::optional<std::vector<double>> value) { this->coordinates = value; }

        /**
         * Vertical height of the winding window
         */
        std::optional<double> get_height() const { return height; }
        void set_height(std::optional<double> value) { this->height = value; }

        /**
         * Way in which the sections are aligned inside the winding window
         */
        std::optional<CoilAlignment> get_sections_alignment() const { return sections_alignment; }
        void set_sections_alignment(std::optional<CoilAlignment> value) { this->sections_alignment = value; }

        /**
         * Way in which the sections are oriented inside the winding window
         */
        std::optional<WindingOrientation> get_sections_orientation() const { return sections_orientation; }
        void set_sections_orientation(std::optional<WindingOrientation> value) { this->sections_orientation = value; }

        /**
         * Shape of the winding window
         */
        std::optional<WindingWindowShape> get_shape() const { return shape; }
        void set_shape(std::optional<WindingWindowShape> value) { this->shape = value; }

        /**
         * Horizontal width of the winding window
         */
        std::optional<double> get_width() const { return width; }
        void set_width(std::optional<double> value) { this->width = value; }

        /**
         * Total angle of the window
         */
        std::optional<double> get_angle() const { return angle; }
        void set_angle(std::optional<double> value) { this->angle = value; }

        /**
         * Radial height of the winding window
         */
        std::optional<double> get_radial_height() const { return radial_height; }
        void set_radial_height(std::optional<double> value) { this->radial_height = value; }
    };

    class CoreBobbinProcessedDescription {
        public:
        CoreBobbinProcessedDescription() :
            column_depth_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            column_thickness_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            column_width_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            wall_thickness_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CoreBobbinProcessedDescription() = default;

        private:
        double column_depth;
        ClassMemberConstraints column_depth_constraint;
        ColumnShape column_shape;
        double column_thickness;
        ClassMemberConstraints column_thickness_constraint;
        std::optional<double> column_width;
        ClassMemberConstraints column_width_constraint;
        std::optional<std::vector<double>> coordinates;
        std::optional<std::vector<Pin>> pins;
        double wall_thickness;
        ClassMemberConstraints wall_thickness_constraint;
        std::vector<WindingWindowElement> winding_windows;

        public:
        /**
         * The depth of the central column wall, including thickness, in the z axis
         */
        const double & get_column_depth() const { return column_depth; }
        double & get_mutable_column_depth() { return column_depth; }
        void set_column_depth(const double & value) { CheckConstraint("column_depth", column_depth_constraint, value); this->column_depth = value; }

        const ColumnShape & get_column_shape() const { return column_shape; }
        ColumnShape & get_mutable_column_shape() { return column_shape; }
        void set_column_shape(const ColumnShape & value) { this->column_shape = value; }

        /**
         * The thickness of the central column wall, where the wire is wound, in the X axis
         */
        const double & get_column_thickness() const { return column_thickness; }
        double & get_mutable_column_thickness() { return column_thickness; }
        void set_column_thickness(const double & value) { CheckConstraint("column_thickness", column_thickness_constraint, value); this->column_thickness = value; }

        /**
         * The width of the central column wall, including thickness, in the x axis
         */
        std::optional<double> get_column_width() const { return column_width; }
        void set_column_width(std::optional<double> value) { if (value) CheckConstraint("column_width", column_width_constraint, *value); this->column_width = value; }

        /**
         * The coordinates of the center of the bobbin central wall, where the wires are wound,
         * referred to the center of the main column.
         */
        std::optional<std::vector<double>> get_coordinates() const { return coordinates; }
        void set_coordinates(std::optional<std::vector<double>> value) { this->coordinates = value; }

        /**
         * List of pins, geometrically defining how and where it is
         */
        std::optional<std::vector<Pin>> get_pins() const { return pins; }
        void set_pins(std::optional<std::vector<Pin>> value) { this->pins = value; }

        /**
         * The thickness of the walls that hold the wire on both sides of the column
         */
        const double & get_wall_thickness() const { return wall_thickness; }
        double & get_mutable_wall_thickness() { return wall_thickness; }
        void set_wall_thickness(const double & value) { CheckConstraint("wall_thickness", wall_thickness_constraint, value); this->wall_thickness = value; }

        /**
         * List of winding windows, all elements in the list must be of the same type
         */
        const std::vector<WindingWindowElement> & get_winding_windows() const { return winding_windows; }
        std::vector<WindingWindowElement> & get_mutable_winding_windows() { return winding_windows; }
        void set_winding_windows(const std::vector<WindingWindowElement> & value) { this->winding_windows = value; }
    };

    /**
     * Description of a bobbin (the insulating former on which one or more windings are
     * arranged). The term is industry-conventional; an IEV entry is being proposed by PSMA. See
     * docs/normative-references.md.
     */
    class Bobbin {
        public:
        Bobbin() = default;
        virtual ~Bobbin() = default;

        private:
        std::optional<std::vector<DistributorInfo>> distributors_info;
        std::optional<BobbinFunctionalDescription> functional_description;
        std::optional<ManufacturerInfo> manufacturer_info;
        std::optional<std::string> name;
        std::optional<CoreBobbinProcessedDescription> processed_description;

        public:
        /**
         * List of distributors of this bobbin.
         */
        std::optional<std::vector<DistributorInfo>> get_distributors_info() const { return distributors_info; }
        void set_distributors_info(std::optional<std::vector<DistributorInfo>> value) { this->distributors_info = value; }

        /**
         * Bobbin data described in functional terms, in a form suitable for analytical models.
         */
        std::optional<BobbinFunctionalDescription> get_functional_description() const { return functional_description; }
        void set_functional_description(std::optional<BobbinFunctionalDescription> value) { this->functional_description = value; }

        std::optional<ManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<ManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * Name of the bobbin.
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        std::optional<CoreBobbinProcessedDescription> get_processed_description() const { return processed_description; }
        void set_processed_description(std::optional<CoreBobbinProcessedDescription> value) { this->processed_description = value; }
    };

    using BobbinDataOrNameUnionElement = std::variant<Bobbin, std::string>;

    using BobbinDataOrNameUnion = std::variant<std::vector<BobbinDataOrNameUnionElement>, Bobbin, std::string>;

    /**
     * Direction of the current in the connection.
     */
    enum class Direction : int { INPUT, OUTPUT };

    /**
     * Connection of a wire to a terminal.
     */
    class ConnectionElement {
        public:
        ConnectionElement() = default;
        virtual ~ConnectionElement() = default;

        private:
        std::optional<Direction> direction;
        std::optional<double> length;
        std::optional<int64_t> metric;
        std::optional<std::string> pin_name;
        std::optional<ConnectionType> type;

        public:
        /**
         * Direction of the current in the connection.
         */
        std::optional<Direction> get_direction() const { return direction; }
        void set_direction(std::optional<Direction> value) { this->direction = value; }

        /**
         * Length of the connection, from the exit of the last turn to the terminal. Unit: m.
         */
        std::optional<double> get_length() const { return length; }
        void set_length(std::optional<double> value) { this->length = value; }

        /**
         * Metric of the terminal, if applicable.
         */
        std::optional<int64_t> get_metric() const { return metric; }
        void set_metric(std::optional<int64_t> value) { this->metric = value; }

        /**
         * Name of the pin where the wire is connected, if applicable.
         */
        std::optional<std::string> get_pin_name() const { return pin_name; }
        void set_pin_name(std::optional<std::string> value) { this->pin_name = value; }

        std::optional<ConnectionType> get_type() const { return type; }
        void set_type(std::optional<ConnectionType> value) { this->type = value; }
    };

    /**
     * The type of the coating
     */
    enum class InsulationWireCoatingType : int { BARE, ENAMELLED, EXTRUDED, INSULATED, SERVED, TAPED };

    /**
     * A coating for a wire
     */
    class InsulationWireCoating {
        public:
        InsulationWireCoating() = default;
        virtual ~InsulationWireCoating() = default;

        private:
        std::optional<double> breakdown_voltage;
        std::optional<int64_t> grade;
        std::optional<InsulationMaterialDataOrNameUnion> material;
        std::optional<int64_t> number_layers;
        std::optional<double> temperature_rating;
        std::optional<DimensionWithTolerance> thickness;
        std::optional<double> thickness_layers;
        std::optional<InsulationWireCoatingType> type;

        public:
        /**
         * The minimum voltage that causes a portion of an insulator to experience electrical
         * breakdown and become electrically conductive, in V
         */
        std::optional<double> get_breakdown_voltage() const { return breakdown_voltage; }
        void set_breakdown_voltage(std::optional<double> value) { this->breakdown_voltage = value; }

        /**
         * The grade of the insulation around the wire
         */
        std::optional<int64_t> get_grade() const { return grade; }
        void set_grade(std::optional<int64_t> value) { this->grade = value; }

        std::optional<InsulationMaterialDataOrNameUnion> get_material() const { return material; }
        void set_material(std::optional<InsulationMaterialDataOrNameUnion> value) { this->material = value; }

        /**
         * The number of layers of the insulation around the wire
         */
        std::optional<int64_t> get_number_layers() const { return number_layers; }
        void set_number_layers(std::optional<int64_t> value) { this->number_layers = value; }

        /**
         * The maximum temperature that the wire coating can withstand
         */
        std::optional<double> get_temperature_rating() const { return temperature_rating; }
        void set_temperature_rating(std::optional<double> value) { this->temperature_rating = value; }

        /**
         * The maximum thickness of the insulation around the wire, in m
         */
        std::optional<DimensionWithTolerance> get_thickness() const { return thickness; }
        void set_thickness(std::optional<DimensionWithTolerance> value) { this->thickness = value; }

        /**
         * The thickness of the layers of the insulation around the wire, in m
         */
        std::optional<double> get_thickness_layers() const { return thickness_layers; }
        void set_thickness_layers(std::optional<double> value) { this->thickness_layers = value; }

        /**
         * The type of the coating
         */
        std::optional<InsulationWireCoatingType> get_type() const { return type; }
        void set_type(std::optional<InsulationWireCoatingType> value) { this->type = value; }
    };

    using CoatingDataOrNameUnion = std::variant<InsulationWireCoating, std::string>;

    /**
     * data for describing the resistivity of a wire
     */
    class Resistivity {
        public:
        Resistivity() = default;
        virtual ~Resistivity() = default;

        private:
        double reference_temperature;
        double reference_value;
        double temperature_coefficient;

        public:
        /**
         * Temperature reference value, in Celsius
         */
        const double & get_reference_temperature() const { return reference_temperature; }
        double & get_mutable_reference_temperature() { return reference_temperature; }
        void set_reference_temperature(const double & value) { this->reference_temperature = value; }

        /**
         * Resistivity reference value, in Ohm * m
         */
        const double & get_reference_value() const { return reference_value; }
        double & get_mutable_reference_value() { return reference_value; }
        void set_reference_value(const double & value) { this->reference_value = value; }

        /**
         * Temperature coefficient value, alpha, in 1 / Celsius
         */
        const double & get_temperature_coefficient() const { return temperature_coefficient; }
        double & get_mutable_temperature_coefficient() { return temperature_coefficient; }
        void set_temperature_coefficient(const double & value) { this->temperature_coefficient = value; }
    };

    /**
     * data for describing one point of thermal conductivity
     */
    class ThermalConductivityElement {
        public:
        ThermalConductivityElement() = default;
        virtual ~ThermalConductivityElement() = default;

        private:
        double temperature;
        double value;

        public:
        /**
         * Temperature for the field value, in Celsius.
         */
        const double & get_temperature() const { return temperature; }
        double & get_mutable_temperature() { return temperature; }
        void set_temperature(const double & value) { this->temperature = value; }

        /**
         * Thermal conductivity value. Unit: W/(m*K).
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    /**
     * A material for wire
     */
    class WireMaterial {
        public:
        WireMaterial() = default;
        virtual ~WireMaterial() = default;

        private:
        std::string name;
        double permeability;
        Resistivity resistivity;
        std::optional<std::vector<ThermalConductivityElement>> thermal_conductivity;

        public:
        /**
         * The name of a wire material
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * The permeability of a wire material
         */
        const double & get_permeability() const { return permeability; }
        double & get_mutable_permeability() { return permeability; }
        void set_permeability(const double & value) { this->permeability = value; }

        const Resistivity & get_resistivity() const { return resistivity; }
        Resistivity & get_mutable_resistivity() { return resistivity; }
        void set_resistivity(const Resistivity & value) { this->resistivity = value; }

        std::optional<std::vector<ThermalConductivityElement>> get_thermal_conductivity() const { return thermal_conductivity; }
        void set_thermal_conductivity(std::optional<std::vector<ThermalConductivityElement>> value) { this->thermal_conductivity = value; }
    };

    using WireMaterialDataOrNameUnion = std::variant<WireMaterial, std::string>;

    /**
     * The standard of wire
     */
    enum class WireStandard : int { IEC_60317, IPC_6012, NEMA_MW_1000_C };

    /**
     * The type of wire
     */
    enum class WireType : int { FOIL, LITZ, PLANAR, RECTANGULAR, ROUND };

    /**
     * The description of a solid round magnet wire
     *
     * The description of a basic magnet wire
     */
    class WireRound {
        public:
        WireRound() = default;
        virtual ~WireRound() = default;

        private:
        DimensionWithTolerance conducting_diameter;
        std::optional<WireMaterialDataOrNameUnion> material;
        std::optional<DimensionWithTolerance> outer_diameter;
        std::optional<CoatingDataOrNameUnion> coating;
        std::optional<DimensionWithTolerance> conducting_area;
        std::optional<ManufacturerInfo> manufacturer_info;
        std::optional<std::string> name;
        std::optional<int64_t> number_conductors;
        std::optional<WireStandard> standard;
        std::optional<std::string> standard_name;
        WireType type;

        public:
        /**
         * The conducting diameter of the wire, in m
         */
        const DimensionWithTolerance & get_conducting_diameter() const { return conducting_diameter; }
        DimensionWithTolerance & get_mutable_conducting_diameter() { return conducting_diameter; }
        void set_conducting_diameter(const DimensionWithTolerance & value) { this->conducting_diameter = value; }

        std::optional<WireMaterialDataOrNameUnion> get_material() const { return material; }
        void set_material(std::optional<WireMaterialDataOrNameUnion> value) { this->material = value; }

        /**
         * The outer diameter of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_outer_diameter() const { return outer_diameter; }
        void set_outer_diameter(std::optional<DimensionWithTolerance> value) { this->outer_diameter = value; }

        std::optional<CoatingDataOrNameUnion> get_coating() const { return coating; }
        void set_coating(std::optional<CoatingDataOrNameUnion> value) { this->coating = value; }

        /**
         * The conducting area of the wire, in m². Used for some rectangular shapes where the area
         * is smaller than expected due to rounded corners
         */
        std::optional<DimensionWithTolerance> get_conducting_area() const { return conducting_area; }
        void set_conducting_area(std::optional<DimensionWithTolerance> value) { this->conducting_area = value; }

        std::optional<ManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<ManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * The name of wire
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * The number of conductors in the wire
         */
        std::optional<int64_t> get_number_conductors() const { return number_conductors; }
        void set_number_conductors(std::optional<int64_t> value) { this->number_conductors = value; }

        /**
         * The standard of wire
         */
        std::optional<WireStandard> get_standard() const { return standard; }
        void set_standard(std::optional<WireStandard> value) { this->standard = value; }

        /**
         * Name according to the standard of wire
         */
        std::optional<std::string> get_standard_name() const { return standard_name; }
        void set_standard_name(std::optional<std::string> value) { this->standard_name = value; }

        const WireType & get_type() const { return type; }
        WireType & get_mutable_type() { return type; }
        void set_type(const WireType & value) { this->type = value; }
    };

    using Strand = std::variant<WireRound, std::string>;

    /**
     * The description of a solid round magnet wire
     *
     * The description of a basic magnet wire
     *
     * The description of a solid foil magnet wire
     *
     * The description of a solid rectangular magnet wire
     *
     * The description of a stranded litz magnet wire
     *
     * The description of a solid planar magnet wire
     */
    class Wire {
        public:
        Wire() = default;
        virtual ~Wire() = default;

        private:
        std::optional<DimensionWithTolerance> conducting_diameter;
        std::optional<WireMaterialDataOrNameUnion> material;
        std::optional<DimensionWithTolerance> outer_diameter;
        std::optional<CoatingDataOrNameUnion> coating;
        std::optional<DimensionWithTolerance> conducting_area;
        std::optional<ManufacturerInfo> manufacturer_info;
        std::optional<std::string> name;
        std::optional<int64_t> number_conductors;
        std::optional<WireStandard> standard;
        std::optional<std::string> standard_name;
        WireType type;
        std::optional<DimensionWithTolerance> conducting_height;
        std::optional<DimensionWithTolerance> conducting_width;
        std::optional<DimensionWithTolerance> outer_height;
        std::optional<DimensionWithTolerance> outer_width;
        std::optional<DimensionWithTolerance> edge_radius;
        std::optional<Strand> strand;
        std::optional<DimensionWithTolerance> twist_pitch;

        public:
        /**
         * The conducting diameter of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_conducting_diameter() const { return conducting_diameter; }
        void set_conducting_diameter(std::optional<DimensionWithTolerance> value) { this->conducting_diameter = value; }

        std::optional<WireMaterialDataOrNameUnion> get_material() const { return material; }
        void set_material(std::optional<WireMaterialDataOrNameUnion> value) { this->material = value; }

        /**
         * The outer diameter of the wire, in m
         *
         * The outer diameter of the wire. Unit: m.
         */
        std::optional<DimensionWithTolerance> get_outer_diameter() const { return outer_diameter; }
        void set_outer_diameter(std::optional<DimensionWithTolerance> value) { this->outer_diameter = value; }

        std::optional<CoatingDataOrNameUnion> get_coating() const { return coating; }
        void set_coating(std::optional<CoatingDataOrNameUnion> value) { this->coating = value; }

        /**
         * The conducting area of the wire, in m². Used for some rectangular shapes where the area
         * is smaller than expected due to rounded corners
         */
        std::optional<DimensionWithTolerance> get_conducting_area() const { return conducting_area; }
        void set_conducting_area(std::optional<DimensionWithTolerance> value) { this->conducting_area = value; }

        std::optional<ManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<ManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * The name of wire
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * The number of conductors in the wire
         */
        std::optional<int64_t> get_number_conductors() const { return number_conductors; }
        void set_number_conductors(std::optional<int64_t> value) { this->number_conductors = value; }

        /**
         * The standard of wire
         */
        std::optional<WireStandard> get_standard() const { return standard; }
        void set_standard(std::optional<WireStandard> value) { this->standard = value; }

        /**
         * Name according to the standard of wire
         */
        std::optional<std::string> get_standard_name() const { return standard_name; }
        void set_standard_name(std::optional<std::string> value) { this->standard_name = value; }

        const WireType & get_type() const { return type; }
        WireType & get_mutable_type() { return type; }
        void set_type(const WireType & value) { this->type = value; }

        /**
         * The conducting height of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_conducting_height() const { return conducting_height; }
        void set_conducting_height(std::optional<DimensionWithTolerance> value) { this->conducting_height = value; }

        /**
         * The conducting width of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_conducting_width() const { return conducting_width; }
        void set_conducting_width(std::optional<DimensionWithTolerance> value) { this->conducting_width = value; }

        /**
         * The outer height of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_outer_height() const { return outer_height; }
        void set_outer_height(std::optional<DimensionWithTolerance> value) { this->outer_height = value; }

        /**
         * The outer width of the wire, in m
         */
        std::optional<DimensionWithTolerance> get_outer_width() const { return outer_width; }
        void set_outer_width(std::optional<DimensionWithTolerance> value) { this->outer_width = value; }

        /**
         * The radius of the edge, in case of rectangular wire, in m
         */
        std::optional<DimensionWithTolerance> get_edge_radius() const { return edge_radius; }
        void set_edge_radius(std::optional<DimensionWithTolerance> value) { this->edge_radius = value; }

        /**
         * The wire used as strands
         */
        std::optional<Strand> get_strand() const { return strand; }
        void set_strand(std::optional<Strand> value) { this->strand = value; }

        /**
         * Length of one full twist of the litz bundle along the wire axis. Unit: m. Vendors quote
         * this in mm or as 'lay length'; convert to metres before serialisation. No international
         * standard exists for litz construction; this field captures the most commonly published
         * vendor parameter.
         */
        std::optional<DimensionWithTolerance> get_twist_pitch() const { return twist_pitch; }
        void set_twist_pitch(std::optional<DimensionWithTolerance> value) { this->twist_pitch = value; }
    };

    using WireDataOrNameUnion = std::variant<Wire, std::string>;

    /**
     * One winding (assembly of interconnected turns and/or coils intended for common operation,
     * per IEV 151-13-17). Examples: primary, secondary, auxiliary.
     */
    class CoilFunctionalDescription {
        public:
        CoilFunctionalDescription() = default;
        virtual ~CoilFunctionalDescription() = default;

        private:
        std::optional<std::vector<ConnectionElement>> connections;
        IsolationSide isolation_side;
        std::string name;
        int64_t number_parallels;
        int64_t number_turns;
        WireDataOrNameUnion wire;
        std::optional<std::vector<std::string>> wound_with;

        public:
        /**
         * Array of all the pins this winding is connected to.
         */
        std::optional<std::vector<ConnectionElement>> get_connections() const { return connections; }
        void set_connections(std::optional<std::vector<ConnectionElement>> value) { this->connections = value; }

        const IsolationSide & get_isolation_side() const { return isolation_side; }
        IsolationSide & get_mutable_isolation_side() { return isolation_side; }
        void set_isolation_side(const IsolationSide & value) { this->isolation_side = value; }

        /**
         * Name of the winding (e.g. primary, secondary).
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Number of parallel-connected conductors making up one electrical turn of the winding.
         */
        const int64_t & get_number_parallels() const { return number_parallels; }
        int64_t & get_mutable_number_parallels() { return number_parallels; }
        void set_number_parallels(const int64_t & value) { this->number_parallels = value; }

        /**
         * Number of turns (per IEV 151-13-14) in the winding.
         */
        const int64_t & get_number_turns() const { return number_turns; }
        int64_t & get_mutable_number_turns() { return number_turns; }
        void set_number_turns(const int64_t & value) { this->number_turns = value; }

        const WireDataOrNameUnion & get_wire() const { return wire; }
        WireDataOrNameUnion & get_mutable_wire() { return wire; }
        void set_wire(const WireDataOrNameUnion & value) { this->wire = value; }

        /**
         * List of winding names that are wound together with this winding.
         */
        std::optional<std::vector<std::string>> get_wound_with() const { return wound_with; }
        void set_wound_with(std::optional<std::vector<std::string>> value) { this->wound_with = value; }
    };

    /**
     * System in which dimension and coordinates are in
     */
    enum class CoordinateSystem : int { CARTESIAN, CYLINDRICAL, POLAR };

    /**
     * One part of a winding, described by the proportion of each parallel that is contained
     * here.
     */
    class PartialWinding {
        public:
        PartialWinding() = default;
        virtual ~PartialWinding() = default;

        private:
        std::optional<std::vector<ConnectionElement>> connections;
        std::vector<double> parallels_proportion;
        std::string winding;

        public:
        /**
         * Two-element array representing the input and output connections of this partial winding.
         */
        std::optional<std::vector<ConnectionElement>> get_connections() const { return connections; }
        void set_connections(std::optional<std::vector<ConnectionElement>> value) { this->connections = value; }

        /**
         * Per-parallel proportion of turns contained in this part.
         */
        const std::vector<double> & get_parallels_proportion() const { return parallels_proportion; }
        std::vector<double> & get_mutable_parallels_proportion() { return parallels_proportion; }
        void set_parallels_proportion(const std::vector<double> & value) { this->parallels_proportion = value; }

        /**
         * Name of the winding that this part belongs to.
         */
        const std::string & get_winding() const { return winding; }
        std::string & get_mutable_winding() { return winding; }
        void set_winding(const std::string & value) { this->winding = value; }
    };

    /**
     * Data describing one group in a magnetic, which can include several sections. Ideally this
     * is used for PCB or different winding windows
     */
    class Group {
        public:
        Group() = default;
        virtual ~Group() = default;

        private:
        std::vector<double> coordinates;
        std::optional<CoordinateSystem> coordinate_system;
        std::vector<double> dimensions;
        std::string name;
        std::vector<PartialWinding> partial_windings;
        WindingOrientation sections_orientation;
        WiringTechnology type;

        public:
        /**
         * The coordinates of the center of the section, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * System in which dimension and coordinates are in
         */
        std::optional<CoordinateSystem> get_coordinate_system() const { return coordinate_system; }
        void set_coordinate_system(std::optional<CoordinateSystem> value) { this->coordinate_system = value; }

        /**
         * Dimensions of the rectangle defining the group
         */
        const std::vector<double> & get_dimensions() const { return dimensions; }
        std::vector<double> & get_mutable_dimensions() { return dimensions; }
        void set_dimensions(const std::vector<double> & value) { this->dimensions = value; }

        /**
         * Name given to the group
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * List of partial windings in this group
         */
        const std::vector<PartialWinding> & get_partial_windings() const { return partial_windings; }
        std::vector<PartialWinding> & get_mutable_partial_windings() { return partial_windings; }
        void set_partial_windings(const std::vector<PartialWinding> & value) { this->partial_windings = value; }

        /**
         * Way in which the sections are oriented inside the winding window
         */
        const WindingOrientation & get_sections_orientation() const { return sections_orientation; }
        WindingOrientation & get_mutable_sections_orientation() { return sections_orientation; }
        void set_sections_orientation(const WindingOrientation & value) { this->sections_orientation = value; }

        /**
         * Type of the group
         */
        const WiringTechnology & get_type() const { return type; }
        WiringTechnology & get_mutable_type() { return type; }
        void set_type(const WiringTechnology & value) { this->type = value; }
    };

    /**
     * Type of the layer
     *
     * Type of the section
     */
    enum class ElectricalType : int { CONDUCTION, INSULATION, SHIELDING };

    /**
     * Defines if the layer is wound by consecutive turns or parallels
     *
     * Defines if the section is wound by consecutive turns or parallels
     */
    enum class WindingStyle : int { WIND_BY_CONSECUTIVE_PARALLELS, WIND_BY_CONSECUTIVE_TURNS };

    /**
     * Data describing one layer in a magnetic
     */
    class Layer {
        public:
        Layer() :
            filling_factor_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~Layer() = default;

        private:
        std::optional<std::vector<std::vector<double>>> additional_coordinates;
        std::vector<double> coordinates;
        std::optional<CoordinateSystem> coordinate_system;
        std::vector<double> dimensions;
        std::optional<double> filling_factor;
        ClassMemberConstraints filling_factor_constraint;
        std::optional<InsulationMaterialDataOrNameUnion> insulation_material;
        std::string name;
        WindingOrientation orientation;
        std::vector<PartialWinding> partial_windings;
        std::optional<std::string> section;
        std::optional<CoilAlignment> turns_alignment;
        ElectricalType type;
        std::optional<WindingStyle> winding_style;

        public:
        /**
         * List of additional coordinates of the center of the layer, referred to the center of the
         * main column, in case the layer is not symmetrical, as in toroids
         */
        std::optional<std::vector<std::vector<double>>> get_additional_coordinates() const { return additional_coordinates; }
        void set_additional_coordinates(std::optional<std::vector<std::vector<double>>> value) { this->additional_coordinates = value; }

        /**
         * The coordinates of the center of the layer, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * System in which dimension and coordinates are in
         */
        std::optional<CoordinateSystem> get_coordinate_system() const { return coordinate_system; }
        void set_coordinate_system(std::optional<CoordinateSystem> value) { this->coordinate_system = value; }

        /**
         * Dimensions of the rectangle defining the layer
         */
        const std::vector<double> & get_dimensions() const { return dimensions; }
        std::vector<double> & get_mutable_dimensions() { return dimensions; }
        void set_dimensions(const std::vector<double> & value) { this->dimensions = value; }

        /**
         * How much space in this layer is used by wires compared to the total
         */
        std::optional<double> get_filling_factor() const { return filling_factor; }
        void set_filling_factor(std::optional<double> value) { if (value) CheckConstraint("filling_factor", filling_factor_constraint, *value); this->filling_factor = value; }

        /**
         * In case of insulating layer, the material used
         */
        std::optional<InsulationMaterialDataOrNameUnion> get_insulation_material() const { return insulation_material; }
        void set_insulation_material(std::optional<InsulationMaterialDataOrNameUnion> value) { this->insulation_material = value; }

        /**
         * Name given to the layer
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Way in which the layer is oriented inside the section
         */
        const WindingOrientation & get_orientation() const { return orientation; }
        WindingOrientation & get_mutable_orientation() { return orientation; }
        void set_orientation(const WindingOrientation & value) { this->orientation = value; }

        /**
         * List of partial windings in this layer
         */
        const std::vector<PartialWinding> & get_partial_windings() const { return partial_windings; }
        std::vector<PartialWinding> & get_mutable_partial_windings() { return partial_windings; }
        void set_partial_windings(const std::vector<PartialWinding> & value) { this->partial_windings = value; }

        /**
         * The name of the section that this layer belongs to
         */
        std::optional<std::string> get_section() const { return section; }
        void set_section(std::optional<std::string> value) { this->section = value; }

        /**
         * Way in which the turns are aligned inside the layer
         */
        std::optional<CoilAlignment> get_turns_alignment() const { return turns_alignment; }
        void set_turns_alignment(std::optional<CoilAlignment> value) { this->turns_alignment = value; }

        /**
         * Type of the layer
         */
        const ElectricalType & get_type() const { return type; }
        ElectricalType & get_mutable_type() { return type; }
        void set_type(const ElectricalType & value) { this->type = value; }

        /**
         * Defines if the layer is wound by consecutive turns or parallels
         */
        std::optional<WindingStyle> get_winding_style() const { return winding_style; }
        void set_winding_style(std::optional<WindingStyle> value) { this->winding_style = value; }
    };

    /**
     * Data describing the information about the margin of a section
     */
    class MarginInfo {
        public:
        MarginInfo() = default;
        virtual ~MarginInfo() = default;

        private:
        double bottom_or_right_width;
        std::optional<InsulationMaterialDataOrNameUnion> insulation_material;
        double layer_thickness;
        int64_t number_layers;
        double top_or_left_width;

        public:
        /**
         * Width of the margin in the bottom or right side of the section, along where the clearance
         * would happen. Also the width of the tape to implement it.
         */
        const double & get_bottom_or_right_width() const { return bottom_or_right_width; }
        double & get_mutable_bottom_or_right_width() { return bottom_or_right_width; }
        void set_bottom_or_right_width(const double & value) { this->bottom_or_right_width = value; }

        /**
         * In case of insulating layer, the material used
         */
        std::optional<InsulationMaterialDataOrNameUnion> get_insulation_material() const { return insulation_material; }
        void set_insulation_material(std::optional<InsulationMaterialDataOrNameUnion> value) { this->insulation_material = value; }

        /**
         * Thickness of the layers used to implement the margin.
         */
        const double & get_layer_thickness() const { return layer_thickness; }
        double & get_mutable_layer_thickness() { return layer_thickness; }
        void set_layer_thickness(const double & value) { this->layer_thickness = value; }

        /**
         * Number of layers to implement the margin
         */
        const int64_t & get_number_layers() const { return number_layers; }
        int64_t & get_mutable_number_layers() { return number_layers; }
        void set_number_layers(const int64_t & value) { this->number_layers = value; }

        /**
         * Width of the margin in the top or left side of the section, along where the clearance
         * would happen. Also the width of the tape to implement it.
         */
        const double & get_top_or_left_width() const { return top_or_left_width; }
        double & get_mutable_top_or_left_width() { return top_or_left_width; }
        void set_top_or_left_width(const double & value) { this->top_or_left_width = value; }
    };

    using Margin = std::variant<std::vector<double>, MarginInfo>;

    /**
     * Data describing one section in a magnetic
     */
    class Section {
        public:
        Section() :
            filling_factor_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            number_layers_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~Section() = default;

        private:
        std::vector<double> coordinates;
        std::optional<CoordinateSystem> coordinate_system;
        std::vector<double> dimensions;
        std::optional<double> filling_factor;
        ClassMemberConstraints filling_factor_constraint;
        std::optional<std::string> group;
        std::optional<CoilAlignment> layers_alignment;
        WindingOrientation layers_orientation;
        std::optional<Margin> margin;
        std::string name;
        std::optional<double> number_layers;
        ClassMemberConstraints number_layers_constraint;
        std::vector<PartialWinding> partial_windings;
        ElectricalType type;
        std::optional<WindingStyle> winding_style;

        public:
        /**
         * The coordinates of the center of the section, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * System in which dimension and coordinates are in
         */
        std::optional<CoordinateSystem> get_coordinate_system() const { return coordinate_system; }
        void set_coordinate_system(std::optional<CoordinateSystem> value) { this->coordinate_system = value; }

        /**
         * Dimensions of the rectangle defining the section
         */
        const std::vector<double> & get_dimensions() const { return dimensions; }
        std::vector<double> & get_mutable_dimensions() { return dimensions; }
        void set_dimensions(const std::vector<double> & value) { this->dimensions = value; }

        /**
         * How much space in this section is used by wires compared to the total
         */
        std::optional<double> get_filling_factor() const { return filling_factor; }
        void set_filling_factor(std::optional<double> value) { if (value) CheckConstraint("filling_factor", filling_factor_constraint, *value); this->filling_factor = value; }

        /**
         * The name of the group that this section belongs to
         */
        std::optional<std::string> get_group() const { return group; }
        void set_group(std::optional<std::string> value) { this->group = value; }

        /**
         * Way in which the layers are aligned inside the section
         */
        std::optional<CoilAlignment> get_layers_alignment() const { return layers_alignment; }
        void set_layers_alignment(std::optional<CoilAlignment> value) { this->layers_alignment = value; }

        /**
         * Way in which the layers are oriented inside the section
         */
        const WindingOrientation & get_layers_orientation() const { return layers_orientation; }
        WindingOrientation & get_mutable_layers_orientation() { return layers_orientation; }
        void set_layers_orientation(const WindingOrientation & value) { this->layers_orientation = value; }

        /**
         * Defines the distance at the extremes of the section reserved for margin tape. Two-element
         * array, from 'inner or top' to 'outer or bottom'.
         */
        std::optional<Margin> get_margin() const { return margin; }
        void set_margin(std::optional<Margin> value) { this->margin = value; }

        /**
         * Name given to the winding
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Optional field to force how many layers must fit in a section
         */
        std::optional<double> get_number_layers() const { return number_layers; }
        void set_number_layers(std::optional<double> value) { if (value) CheckConstraint("number_layers", number_layers_constraint, *value); this->number_layers = value; }

        /**
         * List of partial windings in this section
         */
        const std::vector<PartialWinding> & get_partial_windings() const { return partial_windings; }
        std::vector<PartialWinding> & get_mutable_partial_windings() { return partial_windings; }
        void set_partial_windings(const std::vector<PartialWinding> & value) { this->partial_windings = value; }

        /**
         * Type of the section
         */
        const ElectricalType & get_type() const { return type; }
        ElectricalType & get_mutable_type() { return type; }
        void set_type(const ElectricalType & value) { this->type = value; }

        /**
         * Defines if the section is wound by consecutive turns or parallels
         */
        std::optional<WindingStyle> get_winding_style() const { return winding_style; }
        void set_winding_style(std::optional<WindingStyle> value) { this->winding_style = value; }
    };

    enum class TurnCrossSectionalShape : int { OVAL, RECTANGULAR, ROUND };

    /**
     * Way in which the turn is wound
     */
    enum class TurnOrientation : int { CLOCKWISE, COUNTER_CLOCKWISE };

    /**
     * Data describing one turn in a magnetic
     */
    class Turn {
        public:
        Turn() = default;
        virtual ~Turn() = default;

        private:
        std::optional<std::vector<std::vector<double>>> additional_coordinates;
        std::optional<double> angle;
        std::vector<double> coordinates;
        std::optional<CoordinateSystem> coordinate_system;
        std::optional<TurnCrossSectionalShape> cross_sectional_shape;
        std::optional<std::vector<double>> dimensions;
        std::optional<std::string> layer;
        double length;
        std::string name;
        std::optional<TurnOrientation> orientation;
        int64_t parallel;
        std::optional<double> rotation;
        std::optional<std::string> section;
        std::string winding;

        public:
        /**
         * List of additional coordinates of the center of the turn, referred to the center of the
         * main column, in case the turn is not symmetrical, as in toroids
         */
        std::optional<std::vector<std::vector<double>>> get_additional_coordinates() const { return additional_coordinates; }
        void set_additional_coordinates(std::optional<std::vector<std::vector<double>>> value) { this->additional_coordinates = value; }

        /**
         * The angle that the turn does, useful for partial turns, in degrees
         */
        std::optional<double> get_angle() const { return angle; }
        void set_angle(std::optional<double> value) { this->angle = value; }

        /**
         * The coordinates of the center of the turn, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * System in which dimension and coordinates are in
         */
        std::optional<CoordinateSystem> get_coordinate_system() const { return coordinate_system; }
        void set_coordinate_system(std::optional<CoordinateSystem> value) { this->coordinate_system = value; }

        std::optional<TurnCrossSectionalShape> get_cross_sectional_shape() const { return cross_sectional_shape; }
        void set_cross_sectional_shape(std::optional<TurnCrossSectionalShape> value) { this->cross_sectional_shape = value; }

        /**
         * Dimensions of the rectangle defining the turn
         */
        std::optional<std::vector<double>> get_dimensions() const { return dimensions; }
        void set_dimensions(std::optional<std::vector<double>> value) { this->dimensions = value; }

        /**
         * The name of the layer that this turn belongs to
         */
        std::optional<std::string> get_layer() const { return layer; }
        void set_layer(std::optional<std::string> value) { this->layer = value; }

        /**
         * The length of the turn, referred from the center of its cross section, in m
         */
        const double & get_length() const { return length; }
        double & get_mutable_length() { return length; }
        void set_length(const double & value) { this->length = value; }

        /**
         * Name given to the turn
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Way in which the turn is wound
         */
        std::optional<TurnOrientation> get_orientation() const { return orientation; }
        void set_orientation(std::optional<TurnOrientation> value) { this->orientation = value; }

        /**
         * The index of the parallel that this turn belongs to
         */
        const int64_t & get_parallel() const { return parallel; }
        int64_t & get_mutable_parallel() { return parallel; }
        void set_parallel(const int64_t & value) { this->parallel = value; }

        /**
         * Rotation of the rectangle defining the turn, in degrees
         */
        std::optional<double> get_rotation() const { return rotation; }
        void set_rotation(std::optional<double> value) { this->rotation = value; }

        /**
         * The name of the section that this turn belongs to
         */
        std::optional<std::string> get_section() const { return section; }
        void set_section(std::optional<std::string> value) { this->section = value; }

        /**
         * The name of the winding that this turn belongs to
         */
        const std::string & get_winding() const { return winding; }
        std::string & get_mutable_winding() { return winding; }
        void set_winding(const std::string & value) { this->winding = value; }
    };

    /**
     * Data describing the coil
     *
     * The description of a magnetic coil
     */
    class Coil {
        public:
        Coil() = default;
        virtual ~Coil() = default;

        private:
        BobbinDataOrNameUnion bobbin;
        std::vector<CoilFunctionalDescription> functional_description;
        std::optional<std::vector<Group>> groups_description;
        std::optional<std::vector<Layer>> layers_description;
        std::optional<std::vector<Section>> sections_description;
        std::optional<std::vector<Turn>> turns_description;

        public:
        /**
         * Bobbin(s) for this coil. Scalar (single Bobbin object or name) describes a single bobbin
         * (typically around the centre column). Array describes per-column bobbins for multi-column
         * magnetics (e.g. 3-phase transformers). Convention A: bobbins[i] is mounted on
         * core.columns[i] (index 0 = centre/main column).
         */
        const BobbinDataOrNameUnion & get_bobbin() const { return bobbin; }
        BobbinDataOrNameUnion & get_mutable_bobbin() { return bobbin; }
        void set_bobbin(const BobbinDataOrNameUnion & value) { this->bobbin = value; }

        /**
         * Coil data described in functional terms (the windings, per IEV 151-13-17), in a form
         * suitable for purely magnetic analytical models.
         */
        const std::vector<CoilFunctionalDescription> & get_functional_description() const { return functional_description; }
        std::vector<CoilFunctionalDescription> & get_mutable_functional_description() { return functional_description; }
        void set_functional_description(const std::vector<CoilFunctionalDescription> & value) { this->functional_description = value; }

        /**
         * Coil data at the group level. A group may define a PCB or distinct winding windows.
         */
        std::optional<std::vector<Group>> get_groups_description() const { return groups_description; }
        void set_groups_description(std::optional<std::vector<Group>> value) { this->groups_description = value; }

        /**
         * The data from the coil at the layer level, in a way that can be used by more advanced
         * analytical and finite element models
         */
        std::optional<std::vector<Layer>> get_layers_description() const { return layers_description; }
        void set_layers_description(std::optional<std::vector<Layer>> value) { this->layers_description = value; }

        /**
         * Coil data at the section level, suitable for more advanced analytical and finite-element
         * models.
         */
        std::optional<std::vector<Section>> get_sections_description() const { return sections_description; }
        void set_sections_description(std::optional<std::vector<Section>> value) { this->sections_description = value; }

        /**
         * The data from the coil at the turn level, in a way that can be used by the most advanced
         * analytical and finite element models
         */
        std::optional<std::vector<Turn>> get_turns_description() const { return turns_description; }
        void set_turns_description(std::optional<std::vector<Turn>> value) { this->turns_description = value; }
    };

    /**
     * The coating of the core
     */
    enum class Coating : int { EPOXY, PARYLENE };

    /**
     * The type of a gap
     */
    enum class GapType : int { ADDITIVE, RESIDUAL, SUBTRACTIVE };

    /**
     * A gap for the magnetic cores
     */
    class CoreGap {
        public:
        CoreGap() :
            distance_closest_normal_surface_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            distance_closest_parallel_surface_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CoreGap() = default;

        private:
        std::optional<double> area;
        std::optional<std::vector<double>> coordinates;
        std::optional<double> distance_closest_normal_surface;
        ClassMemberConstraints distance_closest_normal_surface_constraint;
        std::optional<double> distance_closest_parallel_surface;
        ClassMemberConstraints distance_closest_parallel_surface_constraint;
        double length;
        std::optional<std::vector<double>> section_dimensions;
        std::optional<ColumnShape> shape;
        GapType type;

        public:
        /**
         * Geometrical area of the gap
         */
        std::optional<double> get_area() const { return area; }
        void set_area(std::optional<double> value) { this->area = value; }

        /**
         * The coordinates of the center of the gap, referred to the center of the main column.
         */
        std::optional<std::vector<double>> get_coordinates() const { return coordinates; }
        void set_coordinates(std::optional<std::vector<double>> value) { this->coordinates = value; }

        /**
         * The distance where the closest perpendicular surface is. This usually is half the winding
         * height
         */
        std::optional<double> get_distance_closest_normal_surface() const { return distance_closest_normal_surface; }
        void set_distance_closest_normal_surface(std::optional<double> value) { if (value) CheckConstraint("distance_closest_normal_surface", distance_closest_normal_surface_constraint, *value); this->distance_closest_normal_surface = value; }

        /**
         * The distance where the closest parallel surface is. This usually is the opposite side of
         * the winnding window
         */
        std::optional<double> get_distance_closest_parallel_surface() const { return distance_closest_parallel_surface; }
        void set_distance_closest_parallel_surface(std::optional<double> value) { if (value) CheckConstraint("distance_closest_parallel_surface", distance_closest_parallel_surface_constraint, *value); this->distance_closest_parallel_surface = value; }

        /**
         * The length of the gap
         */
        const double & get_length() const { return length; }
        double & get_mutable_length() { return length; }
        void set_length(const double & value) { this->length = value; }

        /**
         * Dimension of the section normal to the magnetic flux
         */
        std::optional<std::vector<double>> get_section_dimensions() const { return section_dimensions; }
        void set_section_dimensions(std::optional<std::vector<double>> value) { this->section_dimensions = value; }

        std::optional<ColumnShape> get_shape() const { return shape; }
        void set_shape(std::optional<ColumnShape> value) { this->shape = value; }

        /**
         * The type of a gap
         */
        const GapType & get_type() const { return type; }
        GapType & get_mutable_type() { return type; }
        void set_type(const GapType & value) { this->type = value; }
    };

    /**
     * One point of the BH cycle.
     */
    class BhCycleElement {
        public:
        BhCycleElement() = default;
        virtual ~BhCycleElement() = default;

        private:
        double magnetic_field;
        double magnetic_flux_density;
        double temperature;

        public:
        /**
         * Magnetic field strength value. Unit: A/m.
         */
        const double & get_magnetic_field() const { return magnetic_field; }
        double & get_mutable_magnetic_field() { return magnetic_field; }
        void set_magnetic_field(const double & value) { this->magnetic_field = value; }

        /**
         * Magnetic flux density value. Unit: T.
         */
        const double & get_magnetic_flux_density() const { return magnetic_flux_density; }
        double & get_mutable_magnetic_flux_density() { return magnetic_flux_density; }
        void set_magnetic_flux_density(const double & value) { this->magnetic_flux_density = value; }

        /**
         * Temperature for the field value, in Celsius.
         */
        const double & get_temperature() const { return temperature; }
        double & get_mutable_temperature() { return temperature; }
        void set_temperature(const double & value) { this->temperature = value; }
    };

    /**
     * List of mass losses points
     *
     * Mass-specific losses at a given point of magnetic flux density, frequency and
     * temperature. Unit: W/kg.
     */
    class MassLossesPoint {
        public:
        MassLossesPoint() = default;
        virtual ~MassLossesPoint() = default;

        private:
        OperatingPointExcitation magnetic_flux_density;
        std::string origin;
        double temperature;
        double value;

        public:
        const OperatingPointExcitation & get_magnetic_flux_density() const { return magnetic_flux_density; }
        OperatingPointExcitation & get_mutable_magnetic_flux_density() { return magnetic_flux_density; }
        void set_magnetic_flux_density(const OperatingPointExcitation & value) { this->magnetic_flux_density = value; }

        /**
         * origin of the data
         */
        const std::string & get_origin() const { return origin; }
        std::string & get_mutable_origin() { return origin; }
        void set_origin(const std::string & value) { this->origin = value; }

        /**
         * Temperature value, in Celsius.
         */
        const double & get_temperature() const { return temperature; }
        double & get_mutable_temperature() { return temperature; }
        void set_temperature(const double & value) { this->temperature = value; }

        /**
         * Mass-specific losses value. Unit: W/kg.
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    enum class MassCoreLossesMethodType : int { MAGNETEC };

    /**
     * Magnetec method for estimating mass losses
     */
    class MagnetecCoreLossesMethodData {
        public:
        MagnetecCoreLossesMethodData() = default;
        virtual ~MagnetecCoreLossesMethodData() = default;

        private:
        MassCoreLossesMethodType method;

        public:
        /**
         * Name of this method
         */
        const MassCoreLossesMethodType & get_method() const { return method; }
        MassCoreLossesMethodType & get_mutable_method() { return method; }
        void set_method(const MassCoreLossesMethodType & value) { this->method = value; }
    };

    using MassLossesMethod = std::variant<std::vector<MassLossesPoint>, MagnetecCoreLossesMethodData>;

    /**
     * The composition of a magnetic material
     */
    enum class MaterialType : int { AMORPHOUS, ELECTRICAL_STEEL, FERRITE, NANOCRYSTALLINE, POWDER };

    /**
     * The composition of a magnetic material
     */
    enum class MaterialComposition : int { CARBONYL_IRON, FE_MO, FE_NI, FE_NI_MO, FE_SI, FE_SI_AL, IRON, MG_ZN, MN_ZN, NI_ZN, PROPRIETARY };

    /**
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the frequency, as factor = a + b * f + c * pow(f, 2) + d * pow(f, 3) + e * pow(f, 4)
     *
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the frequency, as factor = 1 / (a + b * pow(f, c) ) + d
     */
    class FrequencyFactor {
        public:
        FrequencyFactor() = default;
        virtual ~FrequencyFactor() = default;

        private:
        double a;
        double b;
        double c;
        double d;
        std::optional<double> e;

        public:
        const double & get_a() const { return a; }
        double & get_mutable_a() { return a; }
        void set_a(const double & value) { this->a = value; }

        const double & get_b() const { return b; }
        double & get_mutable_b() { return b; }
        void set_b(const double & value) { this->b = value; }

        const double & get_c() const { return c; }
        double & get_mutable_c() { return c; }
        void set_c(const double & value) { this->c = value; }

        const double & get_d() const { return d; }
        double & get_mutable_d() { return d; }
        void set_d(const double & value) { this->d = value; }

        std::optional<double> get_e() const { return e; }
        void set_e(std::optional<double> value) { this->e = value; }
    };

    /**
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the H DC bias, as factor = a + b * pow(H, c)
     *
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the H DC bias, as factor = a + b * pow(H, c) + d
     *
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the H DC bias, as factor = 1 / (a + b * pow(H, c))
     */
    class MagneticFieldDcBiasFactor {
        public:
        MagneticFieldDcBiasFactor() = default;
        virtual ~MagneticFieldDcBiasFactor() = default;

        private:
        double a;
        double b;
        double c;
        std::optional<double> d;

        public:
        const double & get_a() const { return a; }
        double & get_mutable_a() { return a; }
        void set_a(const double & value) { this->a = value; }

        const double & get_b() const { return b; }
        double & get_mutable_b() { return b; }
        void set_b(const double & value) { this->b = value; }

        const double & get_c() const { return c; }
        double & get_mutable_c() { return c; }
        void set_c(const double & value) { this->c = value; }

        std::optional<double> get_d() const { return d; }
        void set_d(std::optional<double> value) { this->d = value; }
    };

    /**
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the B field, as factor = = 1 / ( 1 / ( a + b * pow(B,c)) + 1 / (d * pow(B, e) ) + 1 / f )
     */
    class MagneticFluxDensityFactor {
        public:
        MagneticFluxDensityFactor() = default;
        virtual ~MagneticFluxDensityFactor() = default;

        private:
        double a;
        double b;
        double c;
        double d;
        double e;
        double f;

        public:
        const double & get_a() const { return a; }
        double & get_mutable_a() { return a; }
        void set_a(const double & value) { this->a = value; }

        const double & get_b() const { return b; }
        double & get_mutable_b() { return b; }
        void set_b(const double & value) { this->b = value; }

        const double & get_c() const { return c; }
        double & get_mutable_c() { return c; }
        void set_c(const double & value) { this->c = value; }

        const double & get_d() const { return d; }
        double & get_mutable_d() { return d; }
        void set_d(const double & value) { this->d = value; }

        const double & get_e() const { return e; }
        double & get_mutable_e() { return e; }
        void set_e(const double & value) { this->e = value; }

        const double & get_f() const { return f; }
        double & get_mutable_f() { return f; }
        void set_f(const double & value) { this->f = value; }
    };

    enum class InitialPermeabilitModifierMethod : int { FAIR_RITE, MAGNETICS, MICROMETALS, POCO, TDG };

    /**
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the temperature, as factor = a + b * T + c * pow(T, 2) + d * pow(T, 3) + e * pow(T, 4)
     *
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the temperature, as either factor = a * (T -20) * 0.0001 or factor = (a + c * T + e *
     * pow(T, 2)) / (1 + b * T + d * pow(T, 2))
     *
     * Field with the coefficients used to calculate how much the permeability decreases with
     * the temperature, as either factor = a
     */
    class TemperatureFactor {
        public:
        TemperatureFactor() = default;
        virtual ~TemperatureFactor() = default;

        private:
        double a;
        std::optional<double> b;
        std::optional<double> c;
        std::optional<double> d;
        std::optional<double> e;

        public:
        const double & get_a() const { return a; }
        double & get_mutable_a() { return a; }
        void set_a(const double & value) { this->a = value; }

        std::optional<double> get_b() const { return b; }
        void set_b(std::optional<double> value) { this->b = value; }

        std::optional<double> get_c() const { return c; }
        void set_c(std::optional<double> value) { this->c = value; }

        std::optional<double> get_d() const { return d; }
        void set_d(std::optional<double> value) { this->d = value; }

        std::optional<double> get_e() const { return e; }
        void set_e(std::optional<double> value) { this->e = value; }
    };

    /**
     * Object where keys are shape families for which this permeability is valid. If missing,
     * the variant is valid for all shapes
     *
     * Coefficients given by Magnetics in order to calculate the permeability of their cores
     *
     * Coefficients given by Micrometals in order to calculate the permeability of their cores
     *
     * Coefficients given by Fair-Rite in order to calculate the permeability of their
     * materials
     *
     * Coefficients given by Poco in order to calculate the permeability of their materials
     *
     * Coefficients given by TDG in order to calculate the permeability of their materials
     */
    class InitialPermeabilitModifier {
        public:
        InitialPermeabilitModifier() = default;
        virtual ~InitialPermeabilitModifier() = default;

        private:
        std::optional<FrequencyFactor> frequency_factor;
        std::optional<MagneticFieldDcBiasFactor> magnetic_field_dc_bias_factor;
        std::optional<InitialPermeabilitModifierMethod> method;
        std::optional<TemperatureFactor> temperature_factor;
        std::optional<MagneticFluxDensityFactor> magnetic_flux_density_factor;

        public:
        /**
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the frequency, as factor = a + b * f + c * pow(f, 2) + d * pow(f, 3) + e * pow(f, 4)
         *
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the frequency, as factor = 1 / (a + b * pow(f, c) ) + d
         */
        std::optional<FrequencyFactor> get_frequency_factor() const { return frequency_factor; }
        void set_frequency_factor(std::optional<FrequencyFactor> value) { this->frequency_factor = value; }

        /**
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the H DC bias, as factor = a + b * pow(H, c)
         *
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the H DC bias, as factor = a + b * pow(H, c) + d
         *
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the H DC bias, as factor = 1 / (a + b * pow(H, c))
         */
        std::optional<MagneticFieldDcBiasFactor> get_magnetic_field_dc_bias_factor() const { return magnetic_field_dc_bias_factor; }
        void set_magnetic_field_dc_bias_factor(std::optional<MagneticFieldDcBiasFactor> value) { this->magnetic_field_dc_bias_factor = value; }

        /**
         * Name of this method
         */
        std::optional<InitialPermeabilitModifierMethod> get_method() const { return method; }
        void set_method(std::optional<InitialPermeabilitModifierMethod> value) { this->method = value; }

        /**
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the temperature, as factor = a + b * T + c * pow(T, 2) + d * pow(T, 3) + e * pow(T, 4)
         *
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the temperature, as either factor = a * (T -20) * 0.0001 or factor = (a + c * T + e *
         * pow(T, 2)) / (1 + b * T + d * pow(T, 2))
         *
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the temperature, as either factor = a
         */
        std::optional<TemperatureFactor> get_temperature_factor() const { return temperature_factor; }
        void set_temperature_factor(std::optional<TemperatureFactor> value) { this->temperature_factor = value; }

        /**
         * Field with the coefficients used to calculate how much the permeability decreases with
         * the B field, as factor = = 1 / ( 1 / ( a + b * pow(B,c)) + 1 / (d * pow(B, e) ) + 1 / f )
         */
        std::optional<MagneticFluxDensityFactor> get_magnetic_flux_density_factor() const { return magnetic_flux_density_factor; }
        void set_magnetic_flux_density_factor(std::optional<MagneticFluxDensityFactor> value) { this->magnetic_flux_density_factor = value; }
    };

    /**
     * data for describing one point of permeability
     */
    class PermeabilityPoint {
        public:
        PermeabilityPoint() = default;
        virtual ~PermeabilityPoint() = default;

        private:
        std::optional<double> frequency;
        std::optional<double> magnetic_field_dc_bias;
        std::optional<double> magnetic_flux_density_peak;
        std::optional<std::map<std::string, InitialPermeabilitModifier>> modifiers;
        std::optional<double> temperature;
        std::optional<double> tolerance;
        double value;

        public:
        /**
         * Frequency of the Magnetic field, in Hz
         */
        std::optional<double> get_frequency() const { return frequency; }
        void set_frequency(std::optional<double> value) { this->frequency = value; }

        /**
         * DC bias in the magnetic field, in A/m
         */
        std::optional<double> get_magnetic_field_dc_bias() const { return magnetic_field_dc_bias; }
        void set_magnetic_field_dc_bias(std::optional<double> value) { this->magnetic_field_dc_bias = value; }

        /**
         * magnetic flux density peak for the field value, in T
         */
        std::optional<double> get_magnetic_flux_density_peak() const { return magnetic_flux_density_peak; }
        void set_magnetic_flux_density_peak(std::optional<double> value) { this->magnetic_flux_density_peak = value; }

        /**
         * Per-manufacturer factor blocks applied to the base permeability value to account for DC
         * bias, frequency and temperature dependence.
         */
        std::optional<std::map<std::string, InitialPermeabilitModifier>> get_modifiers() const { return modifiers; }
        void set_modifiers(std::optional<std::map<std::string, InitialPermeabilitModifier>> value) { this->modifiers = value; }

        /**
         * temperature for the field value, in Celsius
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * tolerance for the field value
         */
        std::optional<double> get_tolerance() const { return tolerance; }
        void set_tolerance(std::optional<double> value) { this->tolerance = value; }

        /**
         * Permeability value
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    using Permeability = std::variant<std::vector<PermeabilityPoint>, PermeabilityPoint>;

    /**
     * Complex relative permeability (mu' + j*mu''), used for high-frequency loss modelling.
     */
    class ComplexPermeabilityData {
        public:
        ComplexPermeabilityData() = default;
        virtual ~ComplexPermeabilityData() = default;

        private:
        Permeability imaginary;
        Permeability real;

        public:
        const Permeability & get_imaginary() const { return imaginary; }
        Permeability & get_mutable_imaginary() { return imaginary; }
        void set_imaginary(const Permeability & value) { this->imaginary = value; }

        const Permeability & get_real() const { return real; }
        Permeability & get_mutable_real() { return real; }
        void set_real(const Permeability & value) { this->real = value; }
    };

    /**
     * Relative permeability of the magnetic material, broken down by which kind of permeability
     * was measured. At minimum `initial` is required for materials that have one (essentially
     * all ferrites and tape-wound alloys). Each entry uses permeabilityPoint, which pins
     * frequency, temperature, AC peak flux density and DC bias per measurement.
     */
    class Permeabilities {
        public:
        Permeabilities() = default;
        virtual ~Permeabilities() = default;

        private:
        std::optional<Permeability> amplitude;
        std::optional<ComplexPermeabilityData> complex;
        std::optional<Permeability> incremental;
        Permeability initial;
        std::optional<Permeability> reversible;

        public:
        /**
         * Amplitude relative permeability mu_a. The secant slope of the BH curve at a specified
         * peak AC flux density on a defined geometry. Each point pins B_peak, frequency and
         * temperature.
         */
        std::optional<Permeability> get_amplitude() const { return amplitude; }
        void set_amplitude(std::optional<Permeability> value) { this->amplitude = value; }

        /**
         * Complex relative permeability (mu' + j*mu''), used for high-frequency loss modelling.
         */
        std::optional<ComplexPermeabilityData> get_complex() const { return complex; }
        void set_complex(std::optional<ComplexPermeabilityData> value) { this->complex = value; }

        /**
         * Incremental relative permeability mu_delta. The slope of a small AC excursion
         * superimposed on a non-zero DC bias H. Each point pins magneticFieldDcBias and the AC
         * magneticFluxDensityPeak. Required to model inductors operating under DC bias (PFC chokes,
         * output filters, flyback primaries).
         */
        std::optional<Permeability> get_incremental() const { return incremental; }
        void set_incremental(std::optional<Permeability> value) { this->incremental = value; }

        /**
         * Initial relative permeability mu_i. Measured at near-zero AC excitation (e.g. B < 0.25
         * mT) and low frequency (typically <= 10 kHz) at the material's reference temperature, per
         * the IEC 60401-3 catalogue convention.
         */
        const Permeability & get_initial() const { return initial; }
        Permeability & get_mutable_initial() { return initial; }
        void set_initial(const Permeability & value) { this->initial = value; }

        /**
         * Reversible relative permeability mu_rev. The limit of mu_delta as the AC excursion tends
         * to zero. Tabulated vs magneticFieldDcBias; used for small-signal AC analysis around a DC
         * operating point.
         */
        std::optional<Permeability> get_reversible() const { return reversible; }
        void set_reversible(std::optional<Permeability> value) { this->reversible = value; }
    };

    /**
     * Manufacturer recommended operating conditions for this material
     */
    class CoreMaterialRecommendations {
        public:
        CoreMaterialRecommendations() :
            minimum_frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CoreMaterialRecommendations() = default;

        private:
        std::optional<double> maximum_frequency;
        std::optional<double> maximum_magnetic_flux_density;
        std::optional<double> maximum_operating_temperature;
        std::optional<double> minimum_frequency;
        ClassMemberConstraints minimum_frequency_constraint;
        std::optional<std::vector<std::string>> typical_applications;
        std::optional<std::vector<std::string>> typical_topologies;

        public:
        /**
         * Maximum recommended operating frequency in Hz
         */
        std::optional<double> get_maximum_frequency() const { return maximum_frequency; }
        void set_maximum_frequency(std::optional<double> value) { this->maximum_frequency = value; }

        /**
         * Maximum recommended AC flux density, in T
         */
        std::optional<double> get_maximum_magnetic_flux_density() const { return maximum_magnetic_flux_density; }
        void set_maximum_magnetic_flux_density(std::optional<double> value) { this->maximum_magnetic_flux_density = value; }

        /**
         * Maximum recommended operating temperature in Celsius
         */
        std::optional<double> get_maximum_operating_temperature() const { return maximum_operating_temperature; }
        void set_maximum_operating_temperature(std::optional<double> value) { this->maximum_operating_temperature = value; }

        /**
         * Minimum recommended operating frequency in Hz
         */
        std::optional<double> get_minimum_frequency() const { return minimum_frequency; }
        void set_minimum_frequency(std::optional<double> value) { if (value) CheckConstraint("minimum_frequency", minimum_frequency_constraint, *value); this->minimum_frequency = value; }

        /**
         * Typical applications per manufacturer
         */
        std::optional<std::vector<std::string>> get_typical_applications() const { return typical_applications; }
        void set_typical_applications(std::optional<std::vector<std::string>> value) { this->typical_applications = value; }

        /**
         * Recommended converter topologies
         */
        std::optional<std::vector<std::string>> get_typical_topologies() const { return typical_topologies; }
        void set_typical_topologies(std::optional<std::vector<std::string>> value) { this->typical_topologies = value; }
    };

    /**
     * The type of a magnetic material
     */
    enum class CoreMaterialType : int { COMMERCIAL, CUSTOM };

    /**
     * data for describing the volumetric losses at a given point of magnetic flux density,
     * frequency and temperature
     *
     * List of volumetric losses points
     */
    class VolumetricLossesPoint {
        public:
        VolumetricLossesPoint() = default;
        virtual ~VolumetricLossesPoint() = default;

        private:
        OperatingPointExcitation magnetic_flux_density;
        std::string origin;
        double temperature;
        double value;

        public:
        const OperatingPointExcitation & get_magnetic_flux_density() const { return magnetic_flux_density; }
        OperatingPointExcitation & get_mutable_magnetic_flux_density() { return magnetic_flux_density; }
        void set_magnetic_flux_density(const OperatingPointExcitation & value) { this->magnetic_flux_density = value; }

        /**
         * Origin of the data (datasheet, measurement, simulation, fitted).
         */
        const std::string & get_origin() const { return origin; }
        std::string & get_mutable_origin() { return origin; }
        void set_origin(const std::string & value) { this->origin = value; }

        /**
         * temperature value, in Celsius
         */
        const double & get_temperature() const { return temperature; }
        double & get_mutable_temperature() { return temperature; }
        void set_temperature(const double & value) { this->temperature = value; }

        /**
         * Volumetric losses value. Unit: W/m^3.
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    /**
     * List of coefficients for taking into account the excess losses and the dependencies of
     * the resistivity
     */
    class RoshenAdditionalCoefficients {
        public:
        RoshenAdditionalCoefficients() = default;
        virtual ~RoshenAdditionalCoefficients() = default;

        private:
        double excess_losses_coefficient;
        double resistivity_frequency_coefficient;
        double resistivity_magnetic_flux_density_coefficient;
        double resistivity_offset;
        double resistivity_temperature_coefficient;

        public:
        const double & get_excess_losses_coefficient() const { return excess_losses_coefficient; }
        double & get_mutable_excess_losses_coefficient() { return excess_losses_coefficient; }
        void set_excess_losses_coefficient(const double & value) { this->excess_losses_coefficient = value; }

        const double & get_resistivity_frequency_coefficient() const { return resistivity_frequency_coefficient; }
        double & get_mutable_resistivity_frequency_coefficient() { return resistivity_frequency_coefficient; }
        void set_resistivity_frequency_coefficient(const double & value) { this->resistivity_frequency_coefficient = value; }

        const double & get_resistivity_magnetic_flux_density_coefficient() const { return resistivity_magnetic_flux_density_coefficient; }
        double & get_mutable_resistivity_magnetic_flux_density_coefficient() { return resistivity_magnetic_flux_density_coefficient; }
        void set_resistivity_magnetic_flux_density_coefficient(const double & value) { this->resistivity_magnetic_flux_density_coefficient = value; }

        const double & get_resistivity_offset() const { return resistivity_offset; }
        double & get_mutable_resistivity_offset() { return resistivity_offset; }
        void set_resistivity_offset(const double & value) { this->resistivity_offset = value; }

        const double & get_resistivity_temperature_coefficient() const { return resistivity_temperature_coefficient; }
        double & get_mutable_resistivity_temperature_coefficient() { return resistivity_temperature_coefficient; }
        void set_resistivity_temperature_coefficient(const double & value) { this->resistivity_temperature_coefficient = value; }
    };

    /**
     * Data for describing the loss factor at a given frequency and temperature
     */
    class LossFactorPoint {
        public:
        LossFactorPoint() = default;
        virtual ~LossFactorPoint() = default;

        private:
        std::optional<double> frequency;
        std::optional<double> temperature;
        double value;

        public:
        /**
         * Frequency of the field, in Hz
         */
        std::optional<double> get_frequency() const { return frequency; }
        void set_frequency(std::optional<double> value) { this->frequency = value; }

        /**
         * temperature for the value, in Celsius
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * Loss Factor value
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    enum class VolumetricCoreLossesMethodType : int { LOSS_FACTOR, MAGNETICS, MICROMETALS, POCO, ROSHEN, STEINMETZ, TDG };

    class SteinmetzCoreLossesMethodRangeDatum {
        public:
        SteinmetzCoreLossesMethodRangeDatum() = default;
        virtual ~SteinmetzCoreLossesMethodRangeDatum() = default;

        private:
        double alpha;
        double beta;
        std::optional<double> ct0;
        std::optional<double> ct1;
        std::optional<double> ct2;
        double k;
        std::optional<double> maximum_frequency;
        std::optional<double> minimum_frequency;

        public:
        /**
         * frequency power coefficient alpha
         */
        const double & get_alpha() const { return alpha; }
        double & get_mutable_alpha() { return alpha; }
        void set_alpha(const double & value) { this->alpha = value; }

        /**
         * magnetic flux density power coefficient beta
         */
        const double & get_beta() const { return beta; }
        double & get_mutable_beta() { return beta; }
        void set_beta(const double & value) { this->beta = value; }

        /**
         * Constant temperature coefficient ct0
         */
        std::optional<double> get_ct0() const { return ct0; }
        void set_ct0(std::optional<double> value) { this->ct0 = value; }

        /**
         * Proportional negative temperature coefficient ct1
         */
        std::optional<double> get_ct1() const { return ct1; }
        void set_ct1(std::optional<double> value) { this->ct1 = value; }

        /**
         * Square temperature coefficient ct2
         */
        std::optional<double> get_ct2() const { return ct2; }
        void set_ct2(std::optional<double> value) { this->ct2 = value; }

        /**
         * Proportional coefficient k
         */
        const double & get_k() const { return k; }
        double & get_mutable_k() { return k; }
        void set_k(const double & value) { this->k = value; }

        /**
         * maximum frequency for which the coefficients are valid, in Hz
         */
        std::optional<double> get_maximum_frequency() const { return maximum_frequency; }
        void set_maximum_frequency(std::optional<double> value) { this->maximum_frequency = value; }

        /**
         * minimum frequency for which the coefficients are valid, in Hz
         */
        std::optional<double> get_minimum_frequency() const { return minimum_frequency; }
        void set_minimum_frequency(std::optional<double> value) { this->minimum_frequency = value; }
    };

    /**
     * Steinmetz coefficients for estimating volumetric losses in a given frequency range. The
     * model is P = k * f^alpha * B^beta. By normative convention in MAS, P is in W/m^3, f in Hz
     * and B in T (peak); k therefore has units of W/(m^3 * Hz^alpha * T^beta). Datasheets that
     * publish coefficients in scaled units (kW/m^3, mW/cm^3, kHz, mT) must be converted before
     * serialisation.
     *
     * Roshen coefficients for estimating volumetric losses
     *
     * Micrometals method for estimating volumetric losses
     *
     * Magnetics method for estimating volumetric losses
     *
     * Poco method for estimating volumetric losses
     *
     * TDG method for estimating volumetric losses
     *
     * Loss factor method for estimating volumetric losses
     */
    class CoreLossesMethodData {
        public:
        CoreLossesMethodData() = default;
        virtual ~CoreLossesMethodData() = default;

        private:
        VolumetricCoreLossesMethodType method;
        std::optional<std::vector<SteinmetzCoreLossesMethodRangeDatum>> ranges;
        std::optional<RoshenAdditionalCoefficients> coefficients;
        std::optional<std::vector<VolumetricLossesPoint>> reference_volumetric_losses;
        std::optional<double> a;
        std::optional<double> b;
        std::optional<double> c;
        std::optional<double> d;
        std::optional<std::vector<LossFactorPoint>> factors;

        public:
        /**
         * Name of this method
         */
        const VolumetricCoreLossesMethodType & get_method() const { return method; }
        VolumetricCoreLossesMethodType & get_mutable_method() { return method; }
        void set_method(const VolumetricCoreLossesMethodType & value) { this->method = value; }

        std::optional<std::vector<SteinmetzCoreLossesMethodRangeDatum>> get_ranges() const { return ranges; }
        void set_ranges(std::optional<std::vector<SteinmetzCoreLossesMethodRangeDatum>> value) { this->ranges = value; }

        /**
         * List of coefficients for taking into account the excess losses and the dependencies of
         * the resistivity
         */
        std::optional<RoshenAdditionalCoefficients> get_coefficients() const { return coefficients; }
        void set_coefficients(std::optional<RoshenAdditionalCoefficients> value) { this->coefficients = value; }

        /**
         * List of reference volumetric losses used to estimate excess eddy current losses
         */
        std::optional<std::vector<VolumetricLossesPoint>> get_reference_volumetric_losses() const { return reference_volumetric_losses; }
        void set_reference_volumetric_losses(std::optional<std::vector<VolumetricLossesPoint>> value) { this->reference_volumetric_losses = value; }

        std::optional<double> get_a() const { return a; }
        void set_a(std::optional<double> value) { this->a = value; }

        std::optional<double> get_b() const { return b; }
        void set_b(std::optional<double> value) { this->b = value; }

        std::optional<double> get_c() const { return c; }
        void set_c(std::optional<double> value) { this->c = value; }

        std::optional<double> get_d() const { return d; }
        void set_d(std::optional<double> value) { this->d = value; }

        std::optional<std::vector<LossFactorPoint>> get_factors() const { return factors; }
        void set_factors(std::optional<std::vector<LossFactorPoint>> value) { this->factors = value; }
    };

    using VolumetricLossesMethod = std::variant<std::vector<VolumetricLossesPoint>, CoreLossesMethodData>;

    /**
     * A material for the magnetic cores
     */
    class CoreMaterial {
        public:
        CoreMaterial() = default;
        virtual ~CoreMaterial() = default;

        private:
        std::optional<std::vector<std::string>> alternatives;
        std::optional<std::vector<Application>> application;
        std::optional<std::vector<BhCycleElement>> bh_cycle;
        std::optional<std::vector<BhCycleElement>> coercive_force;
        std::optional<std::string> commercial_name;
        std::optional<double> curie_temperature;
        std::optional<double> density;
        std::optional<std::string> family;
        std::optional<DimensionWithTolerance> heat_capacity;
        std::optional<DimensionWithTolerance> heat_conductivity;
        ManufacturerInfo manufacturer_info;
        std::optional<std::map<std::string, std::vector<MassLossesMethod>>> mass_losses;
        MaterialType material;
        std::optional<MaterialComposition> material_composition;
        std::string name;
        Permeabilities permeability;
        std::optional<CoreMaterialRecommendations> recommendations;
        std::optional<std::vector<BhCycleElement>> remanence;
        std::vector<ResistivityPoint> resistivity;
        std::vector<BhCycleElement> saturation;
        CoreMaterialType type;
        std::map<std::string, std::vector<VolumetricLossesMethod>> volumetric_losses;

        public:
        /**
         * A list of alternative materials that could replace this one
         */
        std::optional<std::vector<std::string>> get_alternatives() const { return alternatives; }
        void set_alternatives(std::optional<std::vector<std::string>> value) { this->alternatives = value; }

        std::optional<std::vector<Application>> get_application() const { return application; }
        void set_application(std::optional<std::vector<Application>> value) { this->application = value; }

        std::optional<std::vector<BhCycleElement>> get_bh_cycle() const { return bh_cycle; }
        void set_bh_cycle(std::optional<std::vector<BhCycleElement>> value) { this->bh_cycle = value; }

        /**
         * BH Cycle points where the magnetic flux density is 0
         */
        std::optional<std::vector<BhCycleElement>> get_coercive_force() const { return coercive_force; }
        void set_coercive_force(std::optional<std::vector<BhCycleElement>> value) { this->coercive_force = value; }

        /**
         * The name of a magnetic material together its manufacturer
         */
        std::optional<std::string> get_commercial_name() const { return commercial_name; }
        void set_commercial_name(std::optional<std::string> value) { this->commercial_name = value; }

        /**
         * The temperature at which this material loses all ferromagnetism. Unit: Celsius.
         */
        std::optional<double> get_curie_temperature() const { return curie_temperature; }
        void set_curie_temperature(std::optional<double> value) { this->curie_temperature = value; }

        /**
         * Density value according to manufacturer, in kg/m3
         */
        std::optional<double> get_density() const { return density; }
        void set_density(std::optional<double> value) { this->density = value; }

        /**
         * The family of a magnetic material according to its manufacturer
         */
        std::optional<std::string> get_family() const { return family; }
        void set_family(std::optional<std::string> value) { this->family = value; }

        /**
         * Specific heat capacity value according to manufacturer. Unit: J/(kg*K).
         */
        std::optional<DimensionWithTolerance> get_heat_capacity() const { return heat_capacity; }
        void set_heat_capacity(std::optional<DimensionWithTolerance> value) { this->heat_capacity = value; }

        /**
         * Thermal conductivity value according to manufacturer. Unit: W/(m*K).
         */
        std::optional<DimensionWithTolerance> get_heat_conductivity() const { return heat_conductivity; }
        void set_heat_conductivity(std::optional<DimensionWithTolerance> value) { this->heat_conductivity = value; }

        const ManufacturerInfo & get_manufacturer_info() const { return manufacturer_info; }
        ManufacturerInfo & get_mutable_manufacturer_info() { return manufacturer_info; }
        void set_manufacturer_info(const ManufacturerInfo & value) { this->manufacturer_info = value; }

        /**
         * Mass-specific core losses. Values throughout this block are in watts per kilogram (W/kg).
         * Note: this is the per-mass form used for tape-wound, amorphous and nanocrystalline
         * materials; the per-volume form lives in `volumetricLosses` and uses W/m^3. The two are
         * NOT interchangeable without the material density. See docs/units.md and
         * docs/normative-references.md.
         */
        std::optional<std::map<std::string, std::vector<MassLossesMethod>>> get_mass_losses() const { return mass_losses; }
        void set_mass_losses(std::optional<std::map<std::string, std::vector<MassLossesMethod>>> value) { this->mass_losses = value; }

        /**
         * The composition of a magnetic material
         */
        const MaterialType & get_material() const { return material; }
        MaterialType & get_mutable_material() { return material; }
        void set_material(const MaterialType & value) { this->material = value; }

        /**
         * The composition of a magnetic material
         */
        std::optional<MaterialComposition> get_material_composition() const { return material_composition; }
        void set_material_composition(std::optional<MaterialComposition> value) { this->material_composition = value; }

        /**
         * The name of a magnetic material
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * Relative permeability of the magnetic material, broken down by which kind of permeability
         * was measured. At minimum `initial` is required for materials that have one (essentially
         * all ferrites and tape-wound alloys). Each entry uses permeabilityPoint, which pins
         * frequency, temperature, AC peak flux density and DC bias per measurement.
         */
        const Permeabilities & get_permeability() const { return permeability; }
        Permeabilities & get_mutable_permeability() { return permeability; }
        void set_permeability(const Permeabilities & value) { this->permeability = value; }

        /**
         * Manufacturer recommended operating conditions for this material
         */
        std::optional<CoreMaterialRecommendations> get_recommendations() const { return recommendations; }
        void set_recommendations(std::optional<CoreMaterialRecommendations> value) { this->recommendations = value; }

        /**
         * BH Cycle points where the magnetic field is 0
         */
        std::optional<std::vector<BhCycleElement>> get_remanence() const { return remanence; }
        void set_remanence(std::optional<std::vector<BhCycleElement>> value) { this->remanence = value; }

        /**
         * Resistivity value according to manufacturer
         */
        const std::vector<ResistivityPoint> & get_resistivity() const { return resistivity; }
        std::vector<ResistivityPoint> & get_mutable_resistivity() { return resistivity; }
        void set_resistivity(const std::vector<ResistivityPoint> & value) { this->resistivity = value; }

        /**
         * BH curve points characterising the saturation flux density of the material. By
         * convention, saturation is reported at the field strength at which the relative
         * differential permeability has fallen to approximately 10 % of the initial permeability.
         * See docs/normative-references.md.
         */
        const std::vector<BhCycleElement> & get_saturation() const { return saturation; }
        std::vector<BhCycleElement> & get_mutable_saturation() { return saturation; }
        void set_saturation(const std::vector<BhCycleElement> & value) { this->saturation = value; }

        /**
         * The type of a magnetic material
         */
        const CoreMaterialType & get_type() const { return type; }
        CoreMaterialType & get_mutable_type() { return type; }
        void set_type(const CoreMaterialType & value) { this->type = value; }

        /**
         * Volumetric core losses. Values throughout this block are in watts per cubic metre
         * (W/m^3). Coefficient-based methods (Steinmetz, Roshen, ...) assume the same unit system:
         * P in W/m^3, f in Hz, B in T (peak). See docs/units.md and docs/normative-references.md.
         */
        const std::map<std::string, std::vector<VolumetricLossesMethod>> & get_volumetric_losses() const { return volumetric_losses; }
        std::map<std::string, std::vector<VolumetricLossesMethod>> & get_mutable_volumetric_losses() { return volumetric_losses; }
        void set_volumetric_losses(const std::map<std::string, std::vector<VolumetricLossesMethod>> & value) { this->volumetric_losses = value; }
    };

    using CoreMaterialDataOrNameUnion = std::variant<CoreMaterial, std::string>;

    /**
     * The family of a magnetic shape
     */
    enum class CoreShapeFamily : int { C, DRUM, E, EC, EFD, EI, EL, ELP, EP, EPX, EQ, ER, ETD, H, LP, P, PLANAR_E, PLANAR_EL, PLANAR_ER, PM, PQ, PQI, RM, ROD, T, U, UI, UR, UT };

    /**
     * Describes if the magnetic circuit of the shape is open, and can be combined with others;
     * or closed, and has to be used by itself
     */
    enum class MagneticCircuit : int { CLOSED, OPEN };

    /**
     * A shape for the magnetic cores
     */
    class CoreShape {
        public:
        CoreShape() = default;
        virtual ~CoreShape() = default;

        private:
        std::optional<std::vector<std::string>> aliases;
        std::optional<std::map<std::string, Dimension>> dimensions;
        CoreShapeFamily family;
        std::optional<std::string> family_subtype;
        std::optional<MagneticCircuit> magnetic_circuit;
        std::optional<std::string> name;
        FunctionalDescriptionType type;

        public:
        /**
         * Alternative names of a magnetic shape
         */
        std::optional<std::vector<std::string>> get_aliases() const { return aliases; }
        void set_aliases(std::optional<std::vector<std::string>> value) { this->aliases = value; }

        /**
         * The dimensions of a magnetic shape. Keys are the dimension labels defined in IEC 62317
         * (and the modernised IEC 63093 series for planar cores).
         */
        std::optional<std::map<std::string, Dimension>> get_dimensions() const { return dimensions; }
        void set_dimensions(std::optional<std::map<std::string, Dimension>> value) { this->dimensions = value; }

        /**
         * The family of a magnetic shape
         */
        const CoreShapeFamily & get_family() const { return family; }
        CoreShapeFamily & get_mutable_family() { return family; }
        void set_family(const CoreShapeFamily & value) { this->family = value; }

        /**
         * The subtype of the shape, in case there are more than one
         */
        std::optional<std::string> get_family_subtype() const { return family_subtype; }
        void set_family_subtype(std::optional<std::string> value) { this->family_subtype = value; }

        /**
         * Describes if the magnetic circuit of the shape is open, and can be combined with others;
         * or closed, and has to be used by itself
         */
        std::optional<MagneticCircuit> get_magnetic_circuit() const { return magnetic_circuit; }
        void set_magnetic_circuit(std::optional<MagneticCircuit> value) { this->magnetic_circuit = value; }

        /**
         * The name of a magnetic shape
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * The type of a magnetic shape
         */
        const FunctionalDescriptionType & get_type() const { return type; }
        FunctionalDescriptionType & get_mutable_type() { return type; }
        void set_type(const FunctionalDescriptionType & value) { this->type = value; }
    };

    using CoreShapeDataOrNameUnion = std::variant<CoreShape, std::string>;

    /**
     * The type of core
     */
    enum class CoreType : int { CLOSED_SHAPE, PIECE_AND_PLATE, TOROIDAL, TWO_PIECE_SET };

    /**
     * The data from the core based on its function, in a way that can be used by analytical
     * models.
     */
    class CoreFunctionalDescription {
        public:
        CoreFunctionalDescription() :
            number_stacks_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CoreFunctionalDescription() = default;

        private:
        std::optional<Coating> coating;
        std::vector<CoreGap> gapping;
        CoreMaterialDataOrNameUnion material;
        std::optional<int64_t> number_stacks;
        ClassMemberConstraints number_stacks_constraint;
        CoreShapeDataOrNameUnion shape;
        CoreType type;

        public:
        /**
         * The coating of the core
         */
        std::optional<Coating> get_coating() const { return coating; }
        void set_coating(std::optional<Coating> value) { this->coating = value; }

        /**
         * The lists of gaps in the magnetic core
         */
        const std::vector<CoreGap> & get_gapping() const { return gapping; }
        std::vector<CoreGap> & get_mutable_gapping() { return gapping; }
        void set_gapping(const std::vector<CoreGap> & value) { this->gapping = value; }

        const CoreMaterialDataOrNameUnion & get_material() const { return material; }
        CoreMaterialDataOrNameUnion & get_mutable_material() { return material; }
        void set_material(const CoreMaterialDataOrNameUnion & value) { this->material = value; }

        /**
         * The number of stacked cores
         */
        std::optional<int64_t> get_number_stacks() const { return number_stacks; }
        void set_number_stacks(std::optional<int64_t> value) { if (value) CheckConstraint("number_stacks", number_stacks_constraint, *value); this->number_stacks = value; }

        const CoreShapeDataOrNameUnion & get_shape() const { return shape; }
        CoreShapeDataOrNameUnion & get_mutable_shape() { return shape; }
        void set_shape(const CoreShapeDataOrNameUnion & value) { this->shape = value; }

        /**
         * The type of core
         */
        const CoreType & get_type() const { return type; }
        CoreType & get_mutable_type() { return type; }
        void set_type(const CoreType & value) { this->type = value; }
    };

    /**
     * Data describing the machining applied to a piece
     */
    class Machining {
        public:
        Machining() = default;
        virtual ~Machining() = default;

        private:
        std::vector<double> coordinates;
        double length;

        public:
        /**
         * The coordinates of the start of the machining, referred to the top of the main column of
         * the piece
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * Length of the machining
         */
        const double & get_length() const { return length; }
        double & get_mutable_length() { return length; }
        void set_length(const double & value) { this->length = value; }
    };

    /**
     * The type of piece
     *
     * The type of spacer
     */
    enum class CoreGeometricalDescriptionElementType : int { CLOSED, HALF_SET, PLATE, SHEET, SPACER, TOROIDAL };

    /**
     * The data from the core based on its geometrical description, in a way that can be used by
     * CAD models.
     *
     * Data describing the a piece of a core
     *
     * Data describing the spacer used to separate cores in additive gaps
     */
    class CoreGeometricalDescriptionElement {
        public:
        CoreGeometricalDescriptionElement() = default;
        virtual ~CoreGeometricalDescriptionElement() = default;

        private:
        std::vector<double> coordinates;
        std::optional<std::vector<Machining>> machining;
        std::optional<CoreMaterialDataOrNameUnion> material;
        std::optional<std::vector<double>> rotation;
        std::optional<CoreShapeDataOrNameUnion> shape;
        CoreGeometricalDescriptionElementType type;
        std::optional<std::vector<double>> dimensions;
        std::optional<InsulationMaterialDataOrNameUnion> insulation_material;

        public:
        /**
         * The coordinates of the top of the piece, referred to the center of the main column
         *
         * The coordinates of the center of the gap, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        std::optional<std::vector<Machining>> get_machining() const { return machining; }
        void set_machining(std::optional<std::vector<Machining>> value) { this->machining = value; }

        std::optional<CoreMaterialDataOrNameUnion> get_material() const { return material; }
        void set_material(std::optional<CoreMaterialDataOrNameUnion> value) { this->material = value; }

        /**
         * The rotation of the top of the piece from its original state, referred to the center of
         * the main column
         */
        std::optional<std::vector<double>> get_rotation() const { return rotation; }
        void set_rotation(std::optional<std::vector<double>> value) { this->rotation = value; }

        std::optional<CoreShapeDataOrNameUnion> get_shape() const { return shape; }
        void set_shape(std::optional<CoreShapeDataOrNameUnion> value) { this->shape = value; }

        /**
         * The type of piece
         *
         * The type of spacer
         */
        const CoreGeometricalDescriptionElementType & get_type() const { return type; }
        CoreGeometricalDescriptionElementType & get_mutable_type() { return type; }
        void set_type(const CoreGeometricalDescriptionElementType & value) { this->type = value; }

        /**
         * Dimensions of the cube defining the spacer
         */
        std::optional<std::vector<double>> get_dimensions() const { return dimensions; }
        void set_dimensions(std::optional<std::vector<double>> value) { this->dimensions = value; }

        /**
         * Material of the spacer
         */
        std::optional<InsulationMaterialDataOrNameUnion> get_insulation_material() const { return insulation_material; }
        void set_insulation_material(std::optional<InsulationMaterialDataOrNameUnion> value) { this->insulation_material = value; }
    };

    /**
     * Name of the column
     */
    enum class ColumnType : int { CENTRAL, LATERAL };

    /**
     * Data describing a column of the core
     */
    class ColumnElement {
        public:
        ColumnElement() = default;
        virtual ~ColumnElement() = default;

        private:
        double area;
        std::vector<double> coordinates;
        double depth;
        double height;
        std::optional<double> minimum_depth;
        std::optional<double> minimum_width;
        ColumnShape shape;
        ColumnType type;
        double width;

        public:
        /**
         * Area of the section column, normal to the magnetic flux direction
         */
        const double & get_area() const { return area; }
        double & get_mutable_area() { return area; }
        void set_area(const double & value) { this->area = value; }

        /**
         * The coordinates of the center of the column, referred to the center of the main column.
         * In the case of half-sets, the center will be in the top point, where it would join
         * another half-set
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * Depth of the column
         */
        const double & get_depth() const { return depth; }
        double & get_mutable_depth() { return depth; }
        void set_depth(const double & value) { this->depth = value; }

        /**
         * Height of the column
         */
        const double & get_height() const { return height; }
        double & get_mutable_height() { return height; }
        void set_height(const double & value) { this->height = value; }

        /**
         * Minimum depth of the column, if irregular
         */
        std::optional<double> get_minimum_depth() const { return minimum_depth; }
        void set_minimum_depth(std::optional<double> value) { this->minimum_depth = value; }

        /**
         * Minimum width of the column, if irregular
         */
        std::optional<double> get_minimum_width() const { return minimum_width; }
        void set_minimum_width(std::optional<double> value) { this->minimum_width = value; }

        const ColumnShape & get_shape() const { return shape; }
        ColumnShape & get_mutable_shape() { return shape; }
        void set_shape(const ColumnShape & value) { this->shape = value; }

        /**
         * Name of the column
         */
        const ColumnType & get_type() const { return type; }
        ColumnType & get_mutable_type() { return type; }
        void set_type(const ColumnType & value) { this->type = value; }

        /**
         * Width of the column
         */
        const double & get_width() const { return width; }
        double & get_mutable_width() { return width; }
        void set_width(const double & value) { this->width = value; }
    };

    /**
     * Effective data of the magnetic core
     */
    class EffectiveParameters {
        public:
        EffectiveParameters() = default;
        virtual ~EffectiveParameters() = default;

        private:
        double effective_area;
        double effective_length;
        double effective_volume;
        double minimum_area;

        public:
        /**
         * This is the equivalent section that the magnetic flux traverses, because the shape of the
         * core is not uniform and its section changes along the path
         */
        const double & get_effective_area() const { return effective_area; }
        double & get_mutable_effective_area() { return effective_area; }
        void set_effective_area(const double & value) { this->effective_area = value; }

        /**
         * This is the equivalent length that the magnetic flux travels through the core.
         */
        const double & get_effective_length() const { return effective_length; }
        double & get_mutable_effective_length() { return effective_length; }
        void set_effective_length(const double & value) { this->effective_length = value; }

        /**
         * This is the product of the effective length by the effective area, and represents the
         * equivalent volume that is magnetized by the field
         */
        const double & get_effective_volume() const { return effective_volume; }
        double & get_mutable_effective_volume() { return effective_volume; }
        void set_effective_volume(const double & value) { this->effective_volume = value; }

        /**
         * This is the minimum area seen by the magnetic flux along its path
         */
        const double & get_minimum_area() const { return minimum_area; }
        double & get_mutable_minimum_area() { return minimum_area; }
        void set_minimum_area(const double & value) { this->minimum_area = value; }
    };

    /**
     * The data from the core after been processed, and ready to use by the analytical models
     */
    class CoreProcessedDescription {
        public:
        CoreProcessedDescription() = default;
        virtual ~CoreProcessedDescription() = default;

        private:
        std::vector<ColumnElement> columns;
        double depth;
        EffectiveParameters effective_parameters;
        double height;
        std::optional<double> thermal_resistance;
        double width;
        std::vector<WindingWindowElement> winding_windows;

        public:
        /**
         * List of columns in the core
         */
        const std::vector<ColumnElement> & get_columns() const { return columns; }
        std::vector<ColumnElement> & get_mutable_columns() { return columns; }
        void set_columns(const std::vector<ColumnElement> & value) { this->columns = value; }

        /**
         * Total depth of the core
         */
        const double & get_depth() const { return depth; }
        double & get_mutable_depth() { return depth; }
        void set_depth(const double & value) { this->depth = value; }

        const EffectiveParameters & get_effective_parameters() const { return effective_parameters; }
        EffectiveParameters & get_mutable_effective_parameters() { return effective_parameters; }
        void set_effective_parameters(const EffectiveParameters & value) { this->effective_parameters = value; }

        /**
         * Total height of the core
         */
        const double & get_height() const { return height; }
        double & get_mutable_height() { return height; }
        void set_height(const double & value) { this->height = value; }

        /**
         * Parameter describing steady state temperature rise versus dissipated power within a given
         * device.
         */
        std::optional<double> get_thermal_resistance() const { return thermal_resistance; }
        void set_thermal_resistance(std::optional<double> value) { this->thermal_resistance = value; }

        /**
         * Total width of the core
         */
        const double & get_width() const { return width; }
        double & get_mutable_width() { return width; }
        void set_width(const double & value) { this->width = value; }

        /**
         * List of winding windows, all elements in the list must be of the same type
         */
        const std::vector<WindingWindowElement> & get_winding_windows() const { return winding_windows; }
        std::vector<WindingWindowElement> & get_mutable_winding_windows() { return winding_windows; }
        void set_winding_windows(const std::vector<WindingWindowElement> & value) { this->winding_windows = value; }
    };

    /**
     * Data describing the magnetic core.
     *
     * The description of a magnetic core
     */
    class MagneticCore {
        public:
        MagneticCore() = default;
        virtual ~MagneticCore() = default;

        private:
        std::optional<std::vector<DistributorInfo>> distributors_info;
        CoreFunctionalDescription functional_description;
        std::optional<std::vector<CoreGeometricalDescriptionElement>> geometrical_description;
        std::optional<ManufacturerInfo> manufacturer_info;
        std::optional<std::string> name;
        std::optional<CoreProcessedDescription> processed_description;

        public:
        /**
         * The lists of distributors of the magnetic core
         */
        std::optional<std::vector<DistributorInfo>> get_distributors_info() const { return distributors_info; }
        void set_distributors_info(std::optional<std::vector<DistributorInfo>> value) { this->distributors_info = value; }

        /**
         * The data from the core based on its function, in a way that can be used by analytical
         * models.
         */
        const CoreFunctionalDescription & get_functional_description() const { return functional_description; }
        CoreFunctionalDescription & get_mutable_functional_description() { return functional_description; }
        void set_functional_description(const CoreFunctionalDescription & value) { this->functional_description = value; }

        /**
         * List with data from the core based on its geometrical description, in a way that can be
         * used by CAD models.
         */
        std::optional<std::vector<CoreGeometricalDescriptionElement>> get_geometrical_description() const { return geometrical_description; }
        void set_geometrical_description(std::optional<std::vector<CoreGeometricalDescriptionElement>> value) { this->geometrical_description = value; }

        std::optional<ManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<ManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * The name of core
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * The data from the core after been processed, and ready to use by the analytical models
         */
        std::optional<CoreProcessedDescription> get_processed_description() const { return processed_description; }
        void set_processed_description(std::optional<CoreProcessedDescription> value) { this->processed_description = value; }
    };

    /**
     * Application parameters published by the manufacturer — describes the circuit this
     * component was designed for, not the component's intrinsic electrical properties.
     *
     * Application parameters published by the manufacturer in the datasheet. These describe the
     * circuit the component was designed for, not the component's intrinsic electrical
     * properties.
     */
    class MagneticDatasheetApplication {
        public:
        MagneticDatasheetApplication() :
            switching_frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~MagneticDatasheetApplication() = default;

        private:
        std::optional<double> auxiliary_voltage;
        std::optional<Dimension> input_voltage;
        std::optional<std::vector<double>> output_currents;
        std::optional<std::vector<double>> output_voltages;
        std::optional<double> switching_frequency;
        ClassMemberConstraints switching_frequency_constraint;

        public:
        /**
         * Auxiliary winding voltage in Volts.
         */
        std::optional<double> get_auxiliary_voltage() const { return auxiliary_voltage; }
        void set_auxiliary_voltage(std::optional<double> value) { this->auxiliary_voltage = value; }

        /**
         * Input voltage range for which this component is designed, in Volts.
         */
        std::optional<Dimension> get_input_voltage() const { return input_voltage; }
        void set_input_voltage(std::optional<Dimension> value) { this->input_voltage = value; }

        /**
         * Output currents per secondary winding in Amperes.
         */
        std::optional<std::vector<double>> get_output_currents() const { return output_currents; }
        void set_output_currents(std::optional<std::vector<double>> value) { this->output_currents = value; }

        /**
         * Output voltages per secondary winding in Volts.
         */
        std::optional<std::vector<double>> get_output_voltages() const { return output_voltages; }
        void set_output_voltages(std::optional<std::vector<double>> value) { this->output_voltages = value; }

        /**
         * Nominal switching frequency in Hz for which this component is designed.
         */
        std::optional<double> get_switching_frequency() const { return switching_frequency; }
        void set_switching_frequency(std::optional<double> value) { if (value) CheckConstraint("switching_frequency", switching_frequency_constraint, *value); this->switching_frequency = value; }
    };

    /**
     * Packaging and supply-chain information from the datasheet or product page.
     *
     * Packaging and supply-chain information.
     */
    class Business {
        public:
        Business() = default;
        virtual ~Business() = default;

        private:
        std::optional<std::string> packaging;

        public:
        /**
         * Packaging format (e.g. 'Tape & Reel', 'Bulk', 'Tray').
         */
        std::optional<std::string> get_packaging() const { return packaging; }
        void set_packaging(std::optional<std::string> value) { this->packaging = value; }
    };

    /**
     * Common-mode filter performance figures from the datasheet.
     *
     * Common-mode filter performance figures.
     */
    class CommonModeFilter {
        public:
        CommonModeFilter() :
            cut_off_frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CommonModeFilter() = default;

        private:
        std::optional<double> attenuation;
        std::optional<std::string> attenuation_test_condition;
        std::optional<double> cut_off_frequency;
        ClassMemberConstraints cut_off_frequency_constraint;

        public:
        /**
         * Common-mode insertion-loss / attenuation value from the datasheet in dB.
         */
        std::optional<double> get_attenuation() const { return attenuation; }
        void set_attenuation(std::optional<double> value) { this->attenuation = value; }

        /**
         * Test condition string for the attenuation figure (e.g. '100 MHz, 50 Ω').
         */
        std::optional<std::string> get_attenuation_test_condition() const { return attenuation_test_condition; }
        void set_attenuation_test_condition(std::optional<std::string> value) { this->attenuation_test_condition = value; }

        /**
         * -3 dB cut-off frequency of the common-mode filter in Hz.
         */
        std::optional<double> get_cut_off_frequency() const { return cut_off_frequency; }
        void set_cut_off_frequency(std::optional<double> value) { if (value) CheckConstraint("cut_off_frequency", cut_off_frequency_constraint, *value); this->cut_off_frequency = value; }
    };

    class DcResistance {
        public:
        DcResistance() = default;
        virtual ~DcResistance() = default;

        private:
        double resistance;
        std::string winding;

        public:
        const double & get_resistance() const { return resistance; }
        double & get_mutable_resistance() { return resistance; }
        void set_resistance(const double & value) { this->resistance = value; }

        const std::string & get_winding() const { return winding; }
        std::string & get_mutable_winding() { return winding; }
        void set_winding(const std::string & value) { this->winding = value; }
    };

    class DatasheetImpedancePoint {
        public:
        DatasheetImpedancePoint() :
            frequency_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~DatasheetImpedancePoint() = default;

        private:
        double frequency;
        ClassMemberConstraints frequency_constraint;
        ImpedancePoint impedance;

        public:
        /**
         * Frequency in Hz.
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { CheckConstraint("frequency", frequency_constraint, value); this->frequency = value; }

        /**
         * Impedance value. Uses the same impedancePoint structure as designRequirements.
         */
        const ImpedancePoint & get_impedance() const { return impedance; }
        ImpedancePoint & get_mutable_impedance() { return impedance; }
        void set_impedance(const ImpedancePoint & value) { this->impedance = value; }
    };

    /**
     * Electrical characteristics as stated in the datasheet.
     */
    class Electrical {
        public:
        Electrical() = default;
        virtual ~Electrical() = default;

        private:
        std::optional<CommonModeFilter> common_mode_filter;
        std::optional<double> coupling_coefficient;
        std::optional<DimensionWithTolerance> dc_resistance;
        std::optional<std::vector<DcResistance>> dc_resistances;
        std::optional<std::vector<DatasheetImpedancePoint>> impedance_points;
        std::optional<DimensionWithTolerance> inductance;
        std::optional<double> insulation_resistance;
        std::optional<double> insulation_test_voltage_ac;
        std::optional<DimensionWithTolerance> leakage_inductance;
        std::optional<double> maximum_impedance;
        std::optional<double> rated_current;
        std::optional<double> rated_voltage_ac;
        std::optional<double> rated_voltage_dc;
        std::optional<double> saturation_current_peak;
        std::optional<double> self_resonant_frequency;
        std::optional<double> turns_ratio;

        public:
        /**
         * Common-mode filter performance figures from the datasheet.
         */
        std::optional<CommonModeFilter> get_common_mode_filter() const { return common_mode_filter; }
        void set_common_mode_filter(std::optional<CommonModeFilter> value) { this->common_mode_filter = value; }

        /**
         * Magnetic coupling coefficient k (0–1).
         */
        std::optional<double> get_coupling_coefficient() const { return coupling_coefficient; }
        void set_coupling_coefficient(std::optional<double> value) { this->coupling_coefficient = value; }

        /**
         * DC resistance per winding in Ohms. nominal = typical value, maximum = datasheet max.
         */
        std::optional<DimensionWithTolerance> get_dc_resistance() const { return dc_resistance; }
        void set_dc_resistance(std::optional<DimensionWithTolerance> value) { this->dc_resistance = value; }

        /**
         * DC resistance per winding for multi-winding parts. Use dcResistance for single-winding
         * parts.
         */
        std::optional<std::vector<DcResistance>> get_dc_resistances() const { return dc_resistances; }
        void set_dc_resistances(std::optional<std::vector<DcResistance>> value) { this->dc_resistances = value; }

        /**
         * Impedance vs. frequency points from the datasheet. Uses the same impedanceAtFrequency
         * structure as designRequirements.minimumImpedance.
         */
        std::optional<std::vector<DatasheetImpedancePoint>> get_impedance_points() const { return impedance_points; }
        void set_impedance_points(std::optional<std::vector<DatasheetImpedancePoint>> value) { this->impedance_points = value; }

        /**
         * Inductance per winding in Henries, with tolerance.
         */
        std::optional<DimensionWithTolerance> get_inductance() const { return inductance; }
        void set_inductance(std::optional<DimensionWithTolerance> value) { this->inductance = value; }

        /**
         * Minimum insulation resistance between windings in Ohms.
         */
        std::optional<double> get_insulation_resistance() const { return insulation_resistance; }
        void set_insulation_resistance(std::optional<double> value) { this->insulation_resistance = value; }

        /**
         * Hi-pot / dielectric withstand test voltage (AC RMS) in Volts.
         */
        std::optional<double> get_insulation_test_voltage_ac() const { return insulation_test_voltage_ac; }
        void set_insulation_test_voltage_ac(std::optional<double> value) { this->insulation_test_voltage_ac = value; }

        /**
         * Leakage inductance in Henries.
         */
        std::optional<DimensionWithTolerance> get_leakage_inductance() const { return leakage_inductance; }
        void set_leakage_inductance(std::optional<DimensionWithTolerance> value) { this->leakage_inductance = value; }

        /**
         * Peak / maximum impedance magnitude in Ohms (frequency not specified in datasheet).
         */
        std::optional<double> get_maximum_impedance() const { return maximum_impedance; }
        void set_maximum_impedance(std::optional<double> value) { this->maximum_impedance = value; }

        /**
         * Rated DC current per winding in Amperes.
         */
        std::optional<double> get_rated_current() const { return rated_current; }
        void set_rated_current(std::optional<double> value) { this->rated_current = value; }

        /**
         * Maximum rated AC voltage (RMS) between windings in Volts.
         */
        std::optional<double> get_rated_voltage_ac() const { return rated_voltage_ac; }
        void set_rated_voltage_ac(std::optional<double> value) { this->rated_voltage_ac = value; }

        /**
         * Maximum rated DC voltage between any two terminals in Volts.
         */
        std::optional<double> get_rated_voltage_dc() const { return rated_voltage_dc; }
        void set_rated_voltage_dc(std::optional<double> value) { this->rated_voltage_dc = value; }

        /**
         * Peak saturation current in Amperes (I_sat from datasheet).
         */
        std::optional<double> get_saturation_current_peak() const { return saturation_current_peak; }
        void set_saturation_current_peak(std::optional<double> value) { this->saturation_current_peak = value; }

        /**
         * Self-resonant frequency in Hz.
         */
        std::optional<double> get_self_resonant_frequency() const { return self_resonant_frequency; }
        void set_self_resonant_frequency(std::optional<double> value) { this->self_resonant_frequency = value; }

        /**
         * Turns ratio (e.g. 100 means 1:100) for coupled inductors / transformers.
         */
        std::optional<double> get_turns_ratio() const { return turns_ratio; }
        void set_turns_ratio(std::optional<double> value) { this->turns_ratio = value; }
    };

    /**
     * Physical dimensions and mounting style from the datasheet.
     *
     * Physical dimensions and mounting style.
     */
    class Mechanical {
        public:
        Mechanical() = default;
        virtual ~Mechanical() = default;

        private:
        std::optional<DimensionWithTolerance> diameter;
        std::optional<DimensionWithTolerance> height;
        std::optional<DimensionWithTolerance> length;
        std::optional<ConnectionType> mounting;
        std::optional<DimensionWithTolerance> width;

        public:
        /**
         * Body diameter in metres (for cylindrical parts).
         */
        std::optional<DimensionWithTolerance> get_diameter() const { return diameter; }
        void set_diameter(std::optional<DimensionWithTolerance> value) { this->diameter = value; }

        /**
         * Body height in metres.
         */
        std::optional<DimensionWithTolerance> get_height() const { return height; }
        void set_height(std::optional<DimensionWithTolerance> value) { this->height = value; }

        /**
         * Body length in metres.
         */
        std::optional<DimensionWithTolerance> get_length() const { return length; }
        void set_length(std::optional<DimensionWithTolerance> value) { this->length = value; }

        /**
         * PCB mounting style. Uses the same connectionType enum as designRequirements.terminalType.
         */
        std::optional<ConnectionType> get_mounting() const { return mounting; }
        void set_mounting(std::optional<ConnectionType> value) { this->mounting = value; }

        /**
         * Body width in metres.
         */
        std::optional<DimensionWithTolerance> get_width() const { return width; }
        void set_width(std::optional<DimensionWithTolerance> value) { this->width = value; }
    };

    /**
     * Basic part identification from the datasheet.
     *
     * Basic part identification.
     */
    class Part {
        public:
        Part() = default;
        virtual ~Part() = default;

        private:
        std::optional<bool> automotive;
        std::optional<std::string> case_code;
        std::optional<std::string> description;
        std::optional<std::string> family;
        std::optional<InsulationType> insulation_grade;
        std::optional<std::string> match_code;
        std::optional<std::string> material;
        std::optional<int64_t> number_of_windings;
        std::optional<std::string> part_number;
        std::optional<bool> shielded;
        std::optional<std::string> winding_style;

        public:
        /**
         * True if the part is qualified for automotive applications (AEC-Q200 or equivalent).
         */
        std::optional<bool> get_automotive() const { return automotive; }
        void set_automotive(std::optional<bool> value) { this->automotive = value; }

        /**
         * Case or package code / size reference (e.g. '0805', 'SMD-4P').
         */
        std::optional<std::string> get_case_code() const { return case_code; }
        void set_case_code(std::optional<std::string> value) { this->case_code = value; }

        /**
         * Human-readable product description as given in the datasheet or catalogue.
         */
        std::optional<std::string> get_description() const { return description; }
        void set_description(std::optional<std::string> value) { this->description = value; }

        /**
         * Product family or series name (e.g. WE-CMB, WE-CNSW).
         */
        std::optional<std::string> get_family() const { return family; }
        void set_family(std::optional<std::string> value) { this->family = value; }

        /**
         * Insulation grade classification as stated in the datasheet (e.g. 'reinforced', 'basic').
         * Aligns with IEC insulationType vocabulary.
         */
        std::optional<InsulationType> get_insulation_grade() const { return insulation_grade; }
        void set_insulation_grade(std::optional<InsulationType> value) { this->insulation_grade = value; }

        /**
         * Internal match / order code used by the manufacturer.
         */
        std::optional<std::string> get_match_code() const { return match_code; }
        void set_match_code(std::optional<std::string> value) { this->match_code = value; }

        /**
         * Core material designation as given in the datasheet (e.g. NiZn, MnZn).
         */
        std::optional<std::string> get_material() const { return material; }
        void set_material(std::optional<std::string> value) { this->material = value; }

        /**
         * Total number of electrically independent windings.
         */
        std::optional<int64_t> get_number_of_windings() const { return number_of_windings; }
        void set_number_of_windings(std::optional<int64_t> value) { this->number_of_windings = value; }

        /**
         * Manufacturer part number.
         */
        std::optional<std::string> get_part_number() const { return part_number; }
        void set_part_number(std::optional<std::string> value) { this->part_number = value; }

        /**
         * True if the component has a shielded construction.
         */
        std::optional<bool> get_shielded() const { return shielded; }
        void set_shielded(std::optional<bool> value) { this->shielded = value; }

        /**
         * Winding construction style (e.g. bifilar, sectional, trifilar).
         */
        std::optional<std::string> get_winding_style() const { return winding_style; }
        void set_winding_style(std::optional<std::string> value) { this->winding_style = value; }
    };

    /**
     * Operating temperature range from the datasheet.
     *
     * Operating temperature range.
     */
    class Thermal {
        public:
        Thermal() = default;
        virtual ~Thermal() = default;

        private:
        std::optional<DimensionWithTolerance> operating_temperature;
        std::optional<double> temperature_rise;
        std::optional<double> thermal_resistance;

        public:
        /**
         * Operating temperature in degrees Celsius.
         */
        std::optional<DimensionWithTolerance> get_operating_temperature() const { return operating_temperature; }
        void set_operating_temperature(std::optional<DimensionWithTolerance> value) { this->operating_temperature = value; }

        /**
         * Maximum temperature rise above ambient. Unit: K (numerically equivalent to a Celsius
         * difference).
         */
        std::optional<double> get_temperature_rise() const { return temperature_rise; }
        void set_temperature_rise(std::optional<double> value) { this->temperature_rise = value; }

        /**
         * Thermal resistance. Unit: K/W (numerically equivalent to °C/W).
         */
        std::optional<double> get_thermal_resistance() const { return thermal_resistance; }
        void set_thermal_resistance(std::optional<double> value) { this->thermal_resistance = value; }
    };

    /**
     * All values extracted directly from the component datasheet, organised by domain.
     */
    class DatasheetInfo {
        public:
        DatasheetInfo() = default;
        virtual ~DatasheetInfo() = default;

        private:
        std::optional<MagneticDatasheetApplication> application;
        std::optional<Business> business;
        std::optional<Electrical> electrical;
        std::optional<Mechanical> mechanical;
        std::optional<Part> part;
        std::optional<Thermal> thermal;

        public:
        /**
         * Application parameters published by the manufacturer — describes the circuit this
         * component was designed for, not the component's intrinsic electrical properties.
         */
        std::optional<MagneticDatasheetApplication> get_application() const { return application; }
        void set_application(std::optional<MagneticDatasheetApplication> value) { this->application = value; }

        /**
         * Packaging and supply-chain information from the datasheet or product page.
         */
        std::optional<Business> get_business() const { return business; }
        void set_business(std::optional<Business> value) { this->business = value; }

        /**
         * Electrical characteristics as stated in the datasheet.
         */
        std::optional<Electrical> get_electrical() const { return electrical; }
        void set_electrical(std::optional<Electrical> value) { this->electrical = value; }

        /**
         * Physical dimensions and mounting style from the datasheet.
         */
        std::optional<Mechanical> get_mechanical() const { return mechanical; }
        void set_mechanical(std::optional<Mechanical> value) { this->mechanical = value; }

        /**
         * Basic part identification from the datasheet.
         */
        std::optional<Part> get_part() const { return part; }
        void set_part(std::optional<Part> value) { this->part = value; }

        /**
         * Operating temperature range from the datasheet.
         */
        std::optional<Thermal> get_thermal() const { return thermal; }
        void set_thermal(std::optional<Thermal> value) { this->thermal = value; }
    };

    /**
     * Manufacturer information for the magnetic. Extends the shared manufacturerInfo with a
     * datasheetInfo block for catalogue-level data.
     *
     * Data from the manufacturer for a given part
     */
    class MagneticManufacturerInfo {
        public:
        MagneticManufacturerInfo() :
            irdi_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::string("^[0-9A-Za-z]{4}(?:-[0-9A-Za-z]+)*#[0-9A-Za-z./-]+#[0-9A-Za-z.-]+$"))
        {}
        virtual ~MagneticManufacturerInfo() = default;

        private:
        std::optional<Cost> cost;
        std::optional<std::string> datasheet_url;
        std::optional<std::string> description;
        std::optional<std::string> family;
        std::optional<std::string> irdi;
        ClassMemberConstraints irdi_constraint;
        std::string name;
        std::optional<std::string> order_code;
        std::optional<std::string> reference;
        std::optional<Status> status;
        std::optional<DatasheetInfo> datasheet_info;

        public:
        /**
         * The manufacturer's reference price for this part.
         */
        std::optional<Cost> get_cost() const { return cost; }
        void set_cost(std::optional<Cost> value) { this->cost = value; }

        /**
         * The manufacturer's URL to the datasheet of the product
         */
        std::optional<std::string> get_datasheet_url() const { return datasheet_url; }
        void set_datasheet_url(std::optional<std::string> value) { this->datasheet_url = value; }

        /**
         * The description of the part according to its manufacturer
         */
        std::optional<std::string> get_description() const { return description; }
        void set_description(std::optional<std::string> value) { this->description = value; }

        /**
         * The family of a magnetic, as defined by the manufacturer
         */
        std::optional<std::string> get_family() const { return family; }
        void set_family(std::optional<std::string> value) { this->family = value; }

        /**
         * Optional International Registration Data Identifier (per ISO/IEC 11179-6) issued by an
         * authoritative registry such as IEC CDD or ECLASS. When populated, this is the canonical
         * machine-readable identifier for the part class.
         */
        std::optional<std::string> get_irdi() const { return irdi; }
        void set_irdi(std::optional<std::string> value) { if (value) CheckConstraint("irdi", irdi_constraint, *value); this->irdi = value; }

        /**
         * The name of the manufacturer of the part
         */
        const std::string & get_name() const { return name; }
        std::string & get_mutable_name() { return name; }
        void set_name(const std::string & value) { this->name = value; }

        /**
         * The manufacturer's order code of this part
         */
        std::optional<std::string> get_order_code() const { return order_code; }
        void set_order_code(std::optional<std::string> value) { this->order_code = value; }

        /**
         * The manufacturer's reference of this part
         */
        std::optional<std::string> get_reference() const { return reference; }
        void set_reference(std::optional<std::string> value) { this->reference = value; }

        /**
         * The production status of a part according to its manufacturer
         */
        std::optional<Status> get_status() const { return status; }
        void set_status(std::optional<Status> value) { this->status = value; }

        /**
         * All values extracted directly from the component datasheet, organised by domain.
         */
        std::optional<DatasheetInfo> get_datasheet_info() const { return datasheet_info; }
        void set_datasheet_info(std::optional<DatasheetInfo> value) { this->datasheet_info = value; }
    };

    /**
     * The description of a magnetic
     */
    class Magnetic {
        public:
        Magnetic() = default;
        virtual ~Magnetic() = default;

        private:
        Coil coil;
        MagneticCore core;
        std::optional<std::vector<DistributorInfo>> distributors_info;
        std::optional<MagneticManufacturerInfo> manufacturer_info;
        std::optional<std::vector<double>> rotation;

        public:
        /**
         * Data describing the coil
         */
        const Coil & get_coil() const { return coil; }
        Coil & get_mutable_coil() { return coil; }
        void set_coil(const Coil & value) { this->coil = value; }

        /**
         * Data describing the magnetic core.
         */
        const MagneticCore & get_core() const { return core; }
        MagneticCore & get_mutable_core() { return core; }
        void set_core(const MagneticCore & value) { this->core = value; }

        /**
         * The lists of distributors of the magnetic
         */
        std::optional<std::vector<DistributorInfo>> get_distributors_info() const { return distributors_info; }
        void set_distributors_info(std::optional<std::vector<DistributorInfo>> value) { this->distributors_info = value; }

        /**
         * Manufacturer information for the magnetic. Extends the shared manufacturerInfo with a
         * datasheetInfo block for catalogue-level data.
         */
        std::optional<MagneticManufacturerInfo> get_manufacturer_info() const { return manufacturer_info; }
        void set_manufacturer_info(std::optional<MagneticManufacturerInfo> value) { this->manufacturer_info = value; }

        /**
         * The rotation of the magnetic, by default the winding column goes vertical
         */
        std::optional<std::vector<double>> get_rotation() const { return rotation; }
        void set_rotation(std::optional<std::vector<double>> value) { this->rotation = value; }
    };

    /**
     * Optional declaration of which MAS conformance class this document targets. Class A —
     * Inductor Basic. Class B — Transformer (multi-winding, insulation, isolated topologies).
     * Class C — Full (all schemas, all topologies). See docs/conformance.md and
     * schemas/conformance/.
     */
    enum class MasConformance : int { A, B, C };

    /**
     * Origin of the value of the result
     */
    enum class ResultOrigin : int { MANUFACTURER, MEASUREMENT, SIMULATION };

    /**
     * Data describing the output core losses
     *
     * Data describing the core losses and the intermediate inputs used to calculate them
     */
    class CoreLossesOutput {
        public:
        CoreLossesOutput() :
            eddy_current_core_losses_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            hysteresis_core_losses_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~CoreLossesOutput() = default;

        private:
        double core_losses;
        std::optional<double> eddy_current_core_losses;
        ClassMemberConstraints eddy_current_core_losses_constraint;
        std::optional<double> hysteresis_core_losses;
        ClassMemberConstraints hysteresis_core_losses_constraint;
        std::optional<SignalDescriptor> magnetic_flux_density;
        std::optional<double> mass_losses;
        std::string method_used;
        ResultOrigin origin;
        double temperature;
        std::optional<double> volumetric_losses;

        public:
        /**
         * Total core losses. Unit: W.
         */
        const double & get_core_losses() const { return core_losses; }
        double & get_mutable_core_losses() { return core_losses; }
        void set_core_losses(const double & value) { this->core_losses = value; }

        /**
         * Part of the core losses due to eddy currents. Unit: W.
         */
        std::optional<double> get_eddy_current_core_losses() const { return eddy_current_core_losses; }
        void set_eddy_current_core_losses(std::optional<double> value) { if (value) CheckConstraint("eddy_current_core_losses", eddy_current_core_losses_constraint, *value); this->eddy_current_core_losses = value; }

        /**
         * Part of the core losses due to hysteresis. Unit: W.
         */
        std::optional<double> get_hysteresis_core_losses() const { return hysteresis_core_losses; }
        void set_hysteresis_core_losses(std::optional<double> value) { if (value) CheckConstraint("hysteresis_core_losses", hysteresis_core_losses_constraint, *value); this->hysteresis_core_losses = value; }

        /**
         * Excitation of the B field that produced the core losses
         */
        std::optional<SignalDescriptor> get_magnetic_flux_density() const { return magnetic_flux_density; }
        void set_magnetic_flux_density(std::optional<SignalDescriptor> value) { this->magnetic_flux_density = value; }

        /**
         * Mass-specific value of the core losses. Unit: W/kg.
         */
        std::optional<double> get_mass_losses() const { return mass_losses; }
        void set_mass_losses(std::optional<double> value) { this->mass_losses = value; }

        /**
         * Model used to calculate the core losses in the case of simulation, or method used to
         * measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * temperature in the core that produced the core losses
         */
        const double & get_temperature() const { return temperature; }
        double & get_mutable_temperature() { return temperature; }
        void set_temperature(const double & value) { this->temperature = value; }

        /**
         * Volumetric value of the core losses. Unit: W/m^3.
         */
        std::optional<double> get_volumetric_losses() const { return volumetric_losses; }
        void set_volumetric_losses(std::optional<double> value) { this->volumetric_losses = value; }
    };

    class ComplexMatrixAtFrequency {
        public:
        ComplexMatrixAtFrequency() = default;
        virtual ~ComplexMatrixAtFrequency() = default;

        private:
        double frequency;
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> magnitude;
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> phase;

        public:
        /**
         * Frequency of the matrix
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { this->frequency = value; }

        const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_magnitude() const { return magnitude; }
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_mutable_magnitude() { return magnitude; }
        void set_magnitude(const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & value) { this->magnitude = value; }

        const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_phase() const { return phase; }
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_mutable_phase() { return phase; }
        void set_phase(const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & value) { this->phase = value; }
    };

    class ScalarMatrixAtFrequency {
        public:
        ScalarMatrixAtFrequency() = default;
        virtual ~ScalarMatrixAtFrequency() = default;

        private:
        double frequency;
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> magnitude;

        public:
        /**
         * Frequency of the matrix
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { this->frequency = value; }

        const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_magnitude() const { return magnitude; }
        std::map<std::string, std::map<std::string, DimensionWithTolerance>> & get_mutable_magnitude() { return magnitude; }
        void set_magnitude(const std::map<std::string, std::map<std::string, DimensionWithTolerance>> & value) { this->magnitude = value; }
    };

    /**
     * Data describing the output impedance
     *
     * Data describing the impedance and the intermediate inputs used to calculate it
     */
    class ImpedanceOutput {
        public:
        ImpedanceOutput() = default;
        virtual ~ImpedanceOutput() = default;

        private:
        std::optional<std::vector<ComplexMatrixAtFrequency>> impedance_matrix;
        std::vector<ScalarMatrixAtFrequency> inductance_matrix;
        std::string method_used;
        ResultOrigin origin;
        std::vector<ScalarMatrixAtFrequency> resistance_matrix;

        public:
        /**
         * List of impedance matrix per frequency
         */
        std::optional<std::vector<ComplexMatrixAtFrequency>> get_impedance_matrix() const { return impedance_matrix; }
        void set_impedance_matrix(std::optional<std::vector<ComplexMatrixAtFrequency>> value) { this->impedance_matrix = value; }

        /**
         * List of inductance matrix per frequency
         */
        const std::vector<ScalarMatrixAtFrequency> & get_inductance_matrix() const { return inductance_matrix; }
        std::vector<ScalarMatrixAtFrequency> & get_mutable_inductance_matrix() { return inductance_matrix; }
        void set_inductance_matrix(const std::vector<ScalarMatrixAtFrequency> & value) { this->inductance_matrix = value; }

        /**
         * Model used to calculate the impedance in the case of simulation, or method used to
         * measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * List of resistance matrix per frequency
         */
        const std::vector<ScalarMatrixAtFrequency> & get_resistance_matrix() const { return resistance_matrix; }
        std::vector<ScalarMatrixAtFrequency> & get_mutable_resistance_matrix() { return resistance_matrix; }
        void set_resistance_matrix(const std::vector<ScalarMatrixAtFrequency> & value) { this->resistance_matrix = value; }
    };

    /**
     * Data describing the leakage inductance and the intermediate inputs used to calculate them
     */
    class LeakageInductanceOutput {
        public:
        LeakageInductanceOutput() = default;
        virtual ~LeakageInductanceOutput() = default;

        private:
        std::vector<DimensionWithTolerance> leakage_inductance_per_winding;
        std::string method_used;
        ResultOrigin origin;

        public:
        const std::vector<DimensionWithTolerance> & get_leakage_inductance_per_winding() const { return leakage_inductance_per_winding; }
        std::vector<DimensionWithTolerance> & get_mutable_leakage_inductance_per_winding() { return leakage_inductance_per_winding; }
        void set_leakage_inductance_per_winding(const std::vector<DimensionWithTolerance> & value) { this->leakage_inductance_per_winding = value; }

        /**
         * Model used to calculate the leakage inductance in the case of simulation, or method used
         * to measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }
    };

    /**
     * Operating point at which `magnetizingInductance` was measured or computed. All fields
     * optional; absence implies the conventional small-signal default. See
     * docs/normative-references.md.
     */
    class InductanceMeasurementCondition {
        public:
        InductanceMeasurementCondition() :
            current_rms_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~InductanceMeasurementCondition() = default;

        private:
        std::optional<double> current_rms;
        ClassMemberConstraints current_rms_constraint;
        std::optional<double> dc_bias_current;
        std::optional<double> frequency;
        std::optional<double> temperature;
        std::optional<double> voltage_rms;

        public:
        /**
         * RMS test current through the winding. Unit: A.
         */
        std::optional<double> get_current_rms() const { return current_rms; }
        void set_current_rms(std::optional<double> value) { if (value) CheckConstraint("current_rms", current_rms_constraint, *value); this->current_rms = value; }

        /**
         * DC bias current through the winding at which this inductance was measured. Unit: A.
         */
        std::optional<double> get_dc_bias_current() const { return dc_bias_current; }
        void set_dc_bias_current(std::optional<double> value) { this->dc_bias_current = value; }

        /**
         * Test frequency. Unit: Hz.
         */
        std::optional<double> get_frequency() const { return frequency; }
        void set_frequency(std::optional<double> value) { this->frequency = value; }

        /**
         * Temperature at which this inductance was measured. Unit: Celsius.
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * RMS test voltage applied across the winding (small-signal characterisation). Unit: V.
         */
        std::optional<double> get_voltage_rms() const { return voltage_rms; }
        void set_voltage_rms(std::optional<double> value) { this->voltage_rms = value; }
    };

    /**
     * Data describing the reluctance of an air gap
     */
    class AirGapReluctanceOutput {
        public:
        AirGapReluctanceOutput() = default;
        virtual ~AirGapReluctanceOutput() = default;

        private:
        double fringing_factor;
        double maximum_storable_magnetic_energy;
        std::string method_used;
        ResultOrigin origin;
        double reluctance;

        public:
        /**
         * Value of the Fringing Factor
         */
        const double & get_fringing_factor() const { return fringing_factor; }
        double & get_mutable_fringing_factor() { return fringing_factor; }
        void set_fringing_factor(const double & value) { this->fringing_factor = value; }

        /**
         * Value of the maximum magnetic energy storable in the gap
         */
        const double & get_maximum_storable_magnetic_energy() const { return maximum_storable_magnetic_energy; }
        double & get_mutable_maximum_storable_magnetic_energy() { return maximum_storable_magnetic_energy; }
        void set_maximum_storable_magnetic_energy(const double & value) { this->maximum_storable_magnetic_energy = value; }

        /**
         * Model used to calculate the magnetizing inductance in the case of simulation, or method
         * used to measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * Value of the reluctance of the gap
         */
        const double & get_reluctance() const { return reluctance; }
        double & get_mutable_reluctance() { return reluctance; }
        void set_reluctance(const double & value) { this->reluctance = value; }
    };

    /**
     * Data describing the magnetizing inductance and the intermediate inputs used to calculate
     * them
     */
    class MagnetizingInductanceOutput {
        public:
        MagnetizingInductanceOutput() = default;
        virtual ~MagnetizingInductanceOutput() = default;

        private:
        double core_reluctance;
        std::optional<double> gapping_reluctance;
        DimensionWithTolerance magnetizing_inductance;
        std::optional<double> maximum_fringing_factor;
        std::optional<double> maximum_magnetic_energy_core;
        std::optional<double> maximum_storable_magnetic_energy_gapping;
        std::optional<InductanceMeasurementCondition> measurement_condition;
        std::string method_used;
        ResultOrigin origin;
        std::optional<std::vector<AirGapReluctanceOutput>> reluctance_per_gap;
        std::optional<double> ungapped_core_reluctance;

        public:
        /**
         * Value of the reluctance of the core
         */
        const double & get_core_reluctance() const { return core_reluctance; }
        double & get_mutable_core_reluctance() { return core_reluctance; }
        void set_core_reluctance(const double & value) { this->core_reluctance = value; }

        /**
         * Value of the reluctance of the gaps
         */
        std::optional<double> get_gapping_reluctance() const { return gapping_reluctance; }
        void set_gapping_reluctance(std::optional<double> value) { this->gapping_reluctance = value; }

        /**
         * Value of the magnetizing inductance. Unit: H. The operating point at which this value
         * applies (frequency, AC test amplitude, DC bias, temperature) is recorded in the optional
         * `measurementCondition` block; if absent, small-signal at 0 A DC bias and the ambient
         * temperature of the operating point is assumed.
         */
        const DimensionWithTolerance & get_magnetizing_inductance() const { return magnetizing_inductance; }
        DimensionWithTolerance & get_mutable_magnetizing_inductance() { return magnetizing_inductance; }
        void set_magnetizing_inductance(const DimensionWithTolerance & value) { this->magnetizing_inductance = value; }

        /**
         * Maximum value of the fringing of the gaps
         */
        std::optional<double> get_maximum_fringing_factor() const { return maximum_fringing_factor; }
        void set_maximum_fringing_factor(std::optional<double> value) { this->maximum_fringing_factor = value; }

        /**
         * Value of the maximum magnetic energy storable in the core
         */
        std::optional<double> get_maximum_magnetic_energy_core() const { return maximum_magnetic_energy_core; }
        void set_maximum_magnetic_energy_core(std::optional<double> value) { this->maximum_magnetic_energy_core = value; }

        /**
         * Value of the maximum magnetic energy storable in the gaps
         */
        std::optional<double> get_maximum_storable_magnetic_energy_gapping() const { return maximum_storable_magnetic_energy_gapping; }
        void set_maximum_storable_magnetic_energy_gapping(std::optional<double> value) { this->maximum_storable_magnetic_energy_gapping = value; }

        /**
         * Operating point at which `magnetizingInductance` was measured or computed. All fields
         * optional; absence implies the conventional small-signal default. See
         * docs/normative-references.md.
         */
        std::optional<InductanceMeasurementCondition> get_measurement_condition() const { return measurement_condition; }
        void set_measurement_condition(std::optional<InductanceMeasurementCondition> value) { this->measurement_condition = value; }

        /**
         * Model used to calculate the magnetizing inductance in the case of simulation, or method
         * used to measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * Reluctance per gap in the magnetic core
         */
        std::optional<std::vector<AirGapReluctanceOutput>> get_reluctance_per_gap() const { return reluctance_per_gap; }
        void set_reluctance_per_gap(std::optional<std::vector<AirGapReluctanceOutput>> value) { this->reluctance_per_gap = value; }

        /**
         * Value of the reluctance of the core
         */
        std::optional<double> get_ungapped_core_reluctance() const { return ungapped_core_reluctance; }
        void set_ungapped_core_reluctance(std::optional<double> value) { this->ungapped_core_reluctance = value; }
    };

    /**
     * Data describing the output inductance
     *
     * Data describing the inductance
     */
    class InductanceOutput {
        public:
        InductanceOutput() = default;
        virtual ~InductanceOutput() = default;

        private:
        std::optional<std::vector<ScalarMatrixAtFrequency>> coupling_coefficients_matrix;
        std::optional<std::vector<ScalarMatrixAtFrequency>> inductance_matrix;
        std::optional<LeakageInductanceOutput> leakage_inductance;
        MagnetizingInductanceOutput magnetizing_inductance;

        public:
        /**
         * List of coupling coefficients matrix per frequency
         */
        std::optional<std::vector<ScalarMatrixAtFrequency>> get_coupling_coefficients_matrix() const { return coupling_coefficients_matrix; }
        void set_coupling_coefficients_matrix(std::optional<std::vector<ScalarMatrixAtFrequency>> value) { this->coupling_coefficients_matrix = value; }

        /**
         * List of inductance matrix per frequency
         */
        std::optional<std::vector<ScalarMatrixAtFrequency>> get_inductance_matrix() const { return inductance_matrix; }
        void set_inductance_matrix(std::optional<std::vector<ScalarMatrixAtFrequency>> value) { this->inductance_matrix = value; }

        std::optional<LeakageInductanceOutput> get_leakage_inductance() const { return leakage_inductance; }
        void set_leakage_inductance(std::optional<LeakageInductanceOutput> value) { this->leakage_inductance = value; }

        const MagnetizingInductanceOutput & get_magnetizing_inductance() const { return magnetizing_inductance; }
        MagnetizingInductanceOutput & get_mutable_magnetizing_inductance() { return magnetizing_inductance; }
        void set_magnetizing_inductance(const MagnetizingInductanceOutput & value) { this->magnetizing_inductance = value; }
    };

    /**
     * Type of the voltage
     *
     * Type of the voltage.
     */
    enum class VoltageType : int { AC, DC };

    /**
     * Data describing the output insulation that the magnetic has
     *
     * List of voltages that the magnetic can withstand
     */
    class DielectricVoltage {
        public:
        DielectricVoltage() = default;
        virtual ~DielectricVoltage() = default;

        private:
        std::optional<double> duration;
        std::optional<std::string> method_used;
        ResultOrigin origin;
        double voltage;
        VoltageType voltage_type;

        public:
        /**
         * Duration of the voltage test, or undefined if the field is not present
         */
        std::optional<double> get_duration() const { return duration; }
        void set_duration(std::optional<double> value) { this->duration = value; }

        /**
         * Model used to calculate the voltage in the case of simulation, or method used to measure
         * it
         */
        std::optional<std::string> get_method_used() const { return method_used; }
        void set_method_used(std::optional<std::string> value) { this->method_used = value; }

        /**
         * Origin of the value of the result
         */
        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * Voltage that the magnetic withstands
         */
        const double & get_voltage() const { return voltage; }
        double & get_mutable_voltage() { return voltage; }
        void set_voltage(const double & value) { this->voltage = value; }

        /**
         * Type of the voltage
         */
        const VoltageType & get_voltage_type() const { return voltage_type; }
        VoltageType & get_mutable_voltage_type() { return voltage_type; }
        void set_voltage_type(const VoltageType & value) { this->voltage_type = value; }
    };

    /**
     * Data describing the output insulation coordination that the magnetic has
     *
     * List of voltages that the magnetic can withstand
     */
    class InsulationCoordinationOutput {
        public:
        InsulationCoordinationOutput() :
            clearance_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            creepage_distance_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
            distance_through_insulation_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt)
        {}
        virtual ~InsulationCoordinationOutput() = default;

        private:
        double clearance;
        ClassMemberConstraints clearance_constraint;
        double creepage_distance;
        ClassMemberConstraints creepage_distance_constraint;
        double distance_through_insulation;
        ClassMemberConstraints distance_through_insulation_constraint;
        double withstand_voltage;
        std::optional<double> withstand_voltage_duration;
        std::optional<VoltageType> withstand_voltage_type;

        public:
        /**
         * Clearance required for this magnetic. Unit: m.
         */
        const double & get_clearance() const { return clearance; }
        double & get_mutable_clearance() { return clearance; }
        void set_clearance(const double & value) { CheckConstraint("clearance", clearance_constraint, value); this->clearance = value; }

        /**
         * Creepage distance required for this magnetic. Unit: m.
         */
        const double & get_creepage_distance() const { return creepage_distance; }
        double & get_mutable_creepage_distance() { return creepage_distance; }
        void set_creepage_distance(const double & value) { CheckConstraint("creepage_distance", creepage_distance_constraint, value); this->creepage_distance = value; }

        /**
         * Distance through insulation required for this magnetic. Unit: m.
         */
        const double & get_distance_through_insulation() const { return distance_through_insulation; }
        double & get_mutable_distance_through_insulation() { return distance_through_insulation; }
        void set_distance_through_insulation(const double & value) { CheckConstraint("distance_through_insulation", distance_through_insulation_constraint, value); this->distance_through_insulation = value; }

        /**
         * Voltage that the magnetic withstands. Unit: V.
         */
        const double & get_withstand_voltage() const { return withstand_voltage; }
        double & get_mutable_withstand_voltage() { return withstand_voltage; }
        void set_withstand_voltage(const double & value) { this->withstand_voltage = value; }

        /**
         * Duration of the voltage test. Unit: s. Absent if the test specification does not require
         * a fixed duration.
         */
        std::optional<double> get_withstand_voltage_duration() const { return withstand_voltage_duration; }
        void set_withstand_voltage_duration(std::optional<double> value) { this->withstand_voltage_duration = value; }

        /**
         * Type of the voltage.
         */
        std::optional<VoltageType> get_withstand_voltage_type() const { return withstand_voltage_type; }
        void set_withstand_voltage_type(std::optional<VoltageType> value) { this->withstand_voltage_type = value; }
    };

    class SixCapacitorNetworkPerWinding {
        public:
        SixCapacitorNetworkPerWinding() = default;
        virtual ~SixCapacitorNetworkPerWinding() = default;

        private:
        double c1;
        double c2;
        double c3;
        double c4;
        double c5;
        double c6;

        public:
        const double & get_c1() const { return c1; }
        double & get_mutable_c1() { return c1; }
        void set_c1(const double & value) { this->c1 = value; }

        const double & get_c2() const { return c2; }
        double & get_mutable_c2() { return c2; }
        void set_c2(const double & value) { this->c2 = value; }

        const double & get_c3() const { return c3; }
        double & get_mutable_c3() { return c3; }
        void set_c3(const double & value) { this->c3 = value; }

        const double & get_c4() const { return c4; }
        double & get_mutable_c4() { return c4; }
        void set_c4(const double & value) { this->c4 = value; }

        const double & get_c5() const { return c5; }
        double & get_mutable_c5() { return c5; }
        void set_c5(const double & value) { this->c5 = value; }

        const double & get_c6() const { return c6; }
        double & get_mutable_c6() { return c6; }
        void set_c6(const double & value) { this->c6 = value; }
    };

    class TripoleCapacitancePerWinding {
        public:
        TripoleCapacitancePerWinding() = default;
        virtual ~TripoleCapacitancePerWinding() = default;

        private:
        double c1;
        double c2;
        double c3;

        public:
        const double & get_c1() const { return c1; }
        double & get_mutable_c1() { return c1; }
        void set_c1(const double & value) { this->c1 = value; }

        const double & get_c2() const { return c2; }
        double & get_mutable_c2() { return c2; }
        void set_c2(const double & value) { this->c2 = value; }

        const double & get_c3() const { return c3; }
        double & get_mutable_c3() { return c3; }
        void set_c3(const double & value) { this->c3 = value; }
    };

    /**
     * Data describing the output stray capacitance
     *
     * Data describing the stray capacitance and the intermediate inputs used to calculate them
     */
    class StrayCapacitanceOutput {
        public:
        StrayCapacitanceOutput() = default;
        virtual ~StrayCapacitanceOutput() = default;

        private:
        std::optional<std::map<std::string, std::map<std::string, double>>> capacitance_among_turns;
        std::optional<std::map<std::string, std::map<std::string, double>>> capacitance_among_windings;
        std::optional<std::map<std::string, std::map<std::string, ScalarMatrixAtFrequency>>> capacitance_matrix;
        std::optional<std::map<std::string, std::map<std::string, double>>> electric_energy_among_turns;
        std::optional<std::vector<ScalarMatrixAtFrequency>> maxwell_capacitance_matrix;
        std::string method_used;
        ResultOrigin origin;
        std::optional<std::map<std::string, std::map<std::string, SixCapacitorNetworkPerWinding>>> six_capacitor_network_per_winding;
        std::optional<std::map<std::string, std::map<std::string, TripoleCapacitancePerWinding>>> tripole_capacitance_per_winding;
        std::optional<std::vector<double>> voltage_divider_end_per_turn;
        std::optional<std::vector<double>> voltage_divider_start_per_turn;
        std::optional<std::map<std::string, std::map<std::string, double>>> voltage_drop_among_turns;
        std::optional<std::vector<double>> voltage_per_turn;

        public:
        /**
         * Capacitance among all pair of adjacent turns
         */
        std::optional<std::map<std::string, std::map<std::string, double>>> get_capacitance_among_turns() const { return capacitance_among_turns; }
        void set_capacitance_among_turns(std::optional<std::map<std::string, std::map<std::string, double>>> value) { this->capacitance_among_turns = value; }

        /**
         * Capacitance among all windings
         */
        std::optional<std::map<std::string, std::map<std::string, double>>> get_capacitance_among_windings() const { return capacitance_among_windings; }
        void set_capacitance_among_windings(std::optional<std::map<std::string, std::map<std::string, double>>> value) { this->capacitance_among_windings = value; }

        /**
         * List of capacitance matrix per frequency
         */
        std::optional<std::map<std::string, std::map<std::string, ScalarMatrixAtFrequency>>> get_capacitance_matrix() const { return capacitance_matrix; }
        void set_capacitance_matrix(std::optional<std::map<std::string, std::map<std::string, ScalarMatrixAtFrequency>>> value) { this->capacitance_matrix = value; }

        /**
         * Electric energy among all pair of adjacent turns
         */
        std::optional<std::map<std::string, std::map<std::string, double>>> get_electric_energy_among_turns() const { return electric_energy_among_turns; }
        void set_electric_energy_among_turns(std::optional<std::map<std::string, std::map<std::string, double>>> value) { this->electric_energy_among_turns = value; }

        /**
         * List of Maxwell capacitance matrix per frequency
         */
        std::optional<std::vector<ScalarMatrixAtFrequency>> get_maxwell_capacitance_matrix() const { return maxwell_capacitance_matrix; }
        void set_maxwell_capacitance_matrix(std::optional<std::vector<ScalarMatrixAtFrequency>> value) { this->maxwell_capacitance_matrix = value; }

        /**
         * Model used to calculate the stray capacitance in the case of simulation, or method used
         * to measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        /**
         * Origin of the value of the result
         */
        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * Network of six equivalent capacitors that describe the capacitance between two given
         * windings
         */
        std::optional<std::map<std::string, std::map<std::string, SixCapacitorNetworkPerWinding>>> get_six_capacitor_network_per_winding() const { return six_capacitor_network_per_winding; }
        void set_six_capacitor_network_per_winding(std::optional<std::map<std::string, std::map<std::string, SixCapacitorNetworkPerWinding>>> value) { this->six_capacitor_network_per_winding = value; }

        /**
         * The three values of a three input electrostatic multipole that describe the capacitance
         * between two given windings
         */
        std::optional<std::map<std::string, std::map<std::string, TripoleCapacitancePerWinding>>> get_tripole_capacitance_per_winding() const { return tripole_capacitance_per_winding; }
        void set_tripole_capacitance_per_winding(std::optional<std::map<std::string, std::map<std::string, TripoleCapacitancePerWinding>>> value) { this->tripole_capacitance_per_winding = value; }

        /**
         * Voltage divider at the end of the physical turn
         */
        std::optional<std::vector<double>> get_voltage_divider_end_per_turn() const { return voltage_divider_end_per_turn; }
        void set_voltage_divider_end_per_turn(std::optional<std::vector<double>> value) { this->voltage_divider_end_per_turn = value; }

        /**
         * Voltage divider at the start of the physical turn
         */
        std::optional<std::vector<double>> get_voltage_divider_start_per_turn() const { return voltage_divider_start_per_turn; }
        void set_voltage_divider_start_per_turn(std::optional<std::vector<double>> value) { this->voltage_divider_start_per_turn = value; }

        /**
         * Voltage drop among all pair of adjacent turns
         */
        std::optional<std::map<std::string, std::map<std::string, double>>> get_voltage_drop_among_turns() const { return voltage_drop_among_turns; }
        void set_voltage_drop_among_turns(std::optional<std::map<std::string, std::map<std::string, double>>> value) { this->voltage_drop_among_turns = value; }

        /**
         * Voltage at the beginning of the physical turn
         */
        std::optional<std::vector<double>> get_voltage_per_turn() const { return voltage_per_turn; }
        void set_voltage_per_turn(std::optional<std::vector<double>> value) { this->voltage_per_turn = value; }
    };

    class TemperaturePoint {
        public:
        TemperaturePoint() = default;
        virtual ~TemperaturePoint() = default;

        private:
        std::vector<double> coordinates;
        double value;

        public:
        /**
         * The coordinates of the temperature point, referred to the center of the main column
         */
        const std::vector<double> & get_coordinates() const { return coordinates; }
        std::vector<double> & get_mutable_coordinates() { return coordinates; }
        void set_coordinates(const std::vector<double> & value) { this->coordinates = value; }

        /**
         * temperature at the point, in Celsius
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    /**
     * Data describing the output temperature
     *
     * Data describing the temperature and the intermediate inputs used to calculate them
     */
    class TemperatureOutput {
        public:
        TemperatureOutput() = default;
        virtual ~TemperatureOutput() = default;

        private:
        std::optional<double> bulk_thermal_resistance;
        std::optional<double> initial_temperature;
        double maximum_temperature;
        std::string method_used;
        ResultOrigin origin;
        std::optional<TemperaturePoint> temperature_point;

        public:
        /**
         * Bulk thermal resistance of the whole magnetic. Unit: K/W.
         */
        std::optional<double> get_bulk_thermal_resistance() const { return bulk_thermal_resistance; }
        void set_bulk_thermal_resistance(std::optional<double> value) { this->bulk_thermal_resistance = value; }

        /**
         * Temperature of the magnetic before it started working. If missing ambient temperature
         * must be assumed
         */
        std::optional<double> get_initial_temperature() const { return initial_temperature; }
        void set_initial_temperature(std::optional<double> value) { this->initial_temperature = value; }

        /**
         * maximum temperature reached
         */
        const double & get_maximum_temperature() const { return maximum_temperature; }
        double & get_mutable_maximum_temperature() { return maximum_temperature; }
        void set_maximum_temperature(const double & value) { this->maximum_temperature = value; }

        /**
         * Model used to calculate the temperature in the case of simulation, or method used to
         * measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        std::optional<TemperaturePoint> get_temperature_point() const { return temperature_point; }
        void set_temperature_point(std::optional<TemperaturePoint> value) { this->temperature_point = value; }
    };

    /**
     * List of value of the winding ohmic losses
     */
    class OhmicLosses {
        public:
        OhmicLosses() = default;
        virtual ~OhmicLosses() = default;

        private:
        double losses;
        std::optional<std::string> method_used;
        ResultOrigin origin;

        public:
        /**
         * Value of the losses
         */
        const double & get_losses() const { return losses; }
        double & get_mutable_losses() { return losses; }
        void set_losses(const double & value) { this->losses = value; }

        /**
         * Model used to calculate the magnetizing inductance in the case of simulation, or method
         * used to measure it
         */
        std::optional<std::string> get_method_used() const { return method_used; }
        void set_method_used(std::optional<std::string> value) { this->method_used = value; }

        /**
         * Origin of the value of the result
         */
        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }
    };

    /**
     * List of value of the winding proximity losses per harmonic
     *
     * Data describing the losses due to either DC, skin effect, or proximity effect; in a given
     * element, which can be winding, section, layer or physical turn
     *
     * List of value of the winding skin losses per harmonic
     */
    class WindingLossElement {
        public:
        WindingLossElement() = default;
        virtual ~WindingLossElement() = default;

        private:
        std::vector<double> harmonic_frequencies;
        std::vector<double> losses_per_harmonic;
        std::string method_used;
        ResultOrigin origin;

        public:
        /**
         * List of frequencies of the harmonics that are producing losses
         */
        const std::vector<double> & get_harmonic_frequencies() const { return harmonic_frequencies; }
        std::vector<double> & get_mutable_harmonic_frequencies() { return harmonic_frequencies; }
        void set_harmonic_frequencies(const std::vector<double> & value) { this->harmonic_frequencies = value; }

        /**
         * Losses produced by each harmonic
         */
        const std::vector<double> & get_losses_per_harmonic() const { return losses_per_harmonic; }
        std::vector<double> & get_mutable_losses_per_harmonic() { return losses_per_harmonic; }
        void set_losses_per_harmonic(const std::vector<double> & value) { this->losses_per_harmonic = value; }

        /**
         * Model used to calculate the magnetizing inductance in the case of simulation, or method
         * used to measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }
    };

    class WindingLossesPerElement {
        public:
        WindingLossesPerElement() = default;
        virtual ~WindingLossesPerElement() = default;

        private:
        std::optional<std::string> name;
        std::optional<OhmicLosses> ohmic_losses;
        std::optional<WindingLossElement> proximity_effect_losses;
        std::optional<WindingLossElement> skin_effect_losses;

        public:
        /**
         * Name of the element
         */
        std::optional<std::string> get_name() const { return name; }
        void set_name(std::optional<std::string> value) { this->name = value; }

        /**
         * List of value of the winding ohmic losses
         */
        std::optional<OhmicLosses> get_ohmic_losses() const { return ohmic_losses; }
        void set_ohmic_losses(std::optional<OhmicLosses> value) { this->ohmic_losses = value; }

        /**
         * List of value of the winding proximity losses per harmonic
         */
        std::optional<WindingLossElement> get_proximity_effect_losses() const { return proximity_effect_losses; }
        void set_proximity_effect_losses(std::optional<WindingLossElement> value) { this->proximity_effect_losses = value; }

        /**
         * List of value of the winding skin losses per harmonic
         */
        std::optional<WindingLossElement> get_skin_effect_losses() const { return skin_effect_losses; }
        void set_skin_effect_losses(std::optional<WindingLossElement> value) { this->skin_effect_losses = value; }
    };

    /**
     * Data describing the output winding losses
     *
     * Data describing the winding losses and the intermediate inputs used to calculate them
     */
    class WindingLossesOutput {
        public:
        WindingLossesOutput() = default;
        virtual ~WindingLossesOutput() = default;

        private:
        std::optional<std::vector<double>> current_divider_per_turn;
        std::optional<OperatingPoint> current_per_winding;
        std::optional<std::vector<double>> dc_resistance_per_turn;
        std::optional<std::vector<double>> dc_resistance_per_winding;
        std::string method_used;
        ResultOrigin origin;
        std::optional<std::vector<ScalarMatrixAtFrequency>> resistance_matrix;
        std::optional<double> temperature;
        double winding_losses;
        std::optional<std::vector<WindingLossesPerElement>> winding_losses_per_layer;
        std::optional<std::vector<WindingLossesPerElement>> winding_losses_per_section;
        std::optional<std::vector<WindingLossesPerElement>> winding_losses_per_turn;
        std::optional<std::vector<WindingLossesPerElement>> winding_losses_per_winding;

        public:
        /**
         * Excitation of the current per physical turn that produced the winding losses
         */
        std::optional<std::vector<double>> get_current_divider_per_turn() const { return current_divider_per_turn; }
        void set_current_divider_per_turn(std::optional<std::vector<double>> value) { this->current_divider_per_turn = value; }

        /**
         * Excitation of the current per winding that produced the winding losses.
         */
        std::optional<OperatingPoint> get_current_per_winding() const { return current_per_winding; }
        void set_current_per_winding(std::optional<OperatingPoint> value) { this->current_per_winding = value; }

        /**
         * List of DC resistance per turn
         */
        std::optional<std::vector<double>> get_dc_resistance_per_turn() const { return dc_resistance_per_turn; }
        void set_dc_resistance_per_turn(std::optional<std::vector<double>> value) { this->dc_resistance_per_turn = value; }

        /**
         * List of DC resistance per winding
         */
        std::optional<std::vector<double>> get_dc_resistance_per_winding() const { return dc_resistance_per_winding; }
        void set_dc_resistance_per_winding(std::optional<std::vector<double>> value) { this->dc_resistance_per_winding = value; }

        /**
         * Model used to calculate the winding losses in the case of simulation, or method used to
         * measure it
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        /**
         * List of resistance matrix per frequency
         */
        std::optional<std::vector<ScalarMatrixAtFrequency>> get_resistance_matrix() const { return resistance_matrix; }
        void set_resistance_matrix(std::optional<std::vector<ScalarMatrixAtFrequency>> value) { this->resistance_matrix = value; }

        /**
         * temperature in the winding that produced the winding losses
         */
        std::optional<double> get_temperature() const { return temperature; }
        void set_temperature(std::optional<double> value) { this->temperature = value; }

        /**
         * Value of the winding losses
         */
        const double & get_winding_losses() const { return winding_losses; }
        double & get_mutable_winding_losses() { return winding_losses; }
        void set_winding_losses(const double & value) { this->winding_losses = value; }

        std::optional<std::vector<WindingLossesPerElement>> get_winding_losses_per_layer() const { return winding_losses_per_layer; }
        void set_winding_losses_per_layer(std::optional<std::vector<WindingLossesPerElement>> value) { this->winding_losses_per_layer = value; }

        std::optional<std::vector<WindingLossesPerElement>> get_winding_losses_per_section() const { return winding_losses_per_section; }
        void set_winding_losses_per_section(std::optional<std::vector<WindingLossesPerElement>> value) { this->winding_losses_per_section = value; }

        std::optional<std::vector<WindingLossesPerElement>> get_winding_losses_per_turn() const { return winding_losses_per_turn; }
        void set_winding_losses_per_turn(std::optional<std::vector<WindingLossesPerElement>> value) { this->winding_losses_per_turn = value; }

        std::optional<std::vector<WindingLossesPerElement>> get_winding_losses_per_winding() const { return winding_losses_per_winding; }
        void set_winding_losses_per_winding(std::optional<std::vector<WindingLossesPerElement>> value) { this->winding_losses_per_winding = value; }
    };

    /**
     * Data describing the value of a field in a 2D or 3D space
     */
    class FieldPoint {
        public:
        FieldPoint() = default;
        virtual ~FieldPoint() = default;

        private:
        std::optional<std::string> label;
        std::vector<double> point;
        std::optional<double> rotation;
        std::optional<int64_t> turn_index;
        std::optional<double> turn_length;
        double value;

        public:
        /**
         * If this point has some special significance, can be identified with this label
         */
        std::optional<std::string> get_label() const { return label; }
        void set_label(std::optional<std::string> value) { this->label = value; }

        /**
         * The coordinates of the point of the field
         */
        const std::vector<double> & get_point() const { return point; }
        std::vector<double> & get_mutable_point() { return point; }
        void set_point(const std::vector<double> & value) { this->point = value; }

        /**
         * Rotation of the rectangle defining the turn, in degrees
         */
        std::optional<double> get_rotation() const { return rotation; }
        void set_rotation(std::optional<double> value) { this->rotation = value; }

        /**
         * If this field point is inside of a wire, this is the index of the turn
         */
        std::optional<int64_t> get_turn_index() const { return turn_index; }
        void set_turn_index(std::optional<int64_t> value) { this->turn_index = value; }

        /**
         * If this field point is inside of a wire, this is the length of the turn
         */
        std::optional<double> get_turn_length() const { return turn_length; }
        void set_turn_length(std::optional<double> value) { this->turn_length = value; }

        /**
         * Value of the field at this point.
         */
        const double & get_value() const { return value; }
        double & get_mutable_value() { return value; }
        void set_value(const double & value) { this->value = value; }
    };

    /**
     * Data describing a field in a 2D or 3D space
     */
    class Field {
        public:
        Field() = default;
        virtual ~Field() = default;

        private:
        std::vector<FieldPoint> data;
        double frequency;

        public:
        /**
         * Value of the magnetizing inductance
         */
        const std::vector<FieldPoint> & get_data() const { return data; }
        std::vector<FieldPoint> & get_mutable_data() { return data; }
        void set_data(const std::vector<FieldPoint> & value) { this->data = value; }

        /**
         * Value of the field at this point
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { this->frequency = value; }
    };

    /**
     * Data describing the output current density field
     *
     * Data describing the current in the different chunks used in field calculation
     */
    class WindingWindowCurrentDensityFieldOutput {
        public:
        WindingWindowCurrentDensityFieldOutput() = default;
        virtual ~WindingWindowCurrentDensityFieldOutput() = default;

        private:
        std::vector<Field> field_per_frequency;
        std::string method_used;
        ResultOrigin origin;
        std::vector<WireDataOrNameUnion> wires;

        public:
        const std::vector<Field> & get_field_per_frequency() const { return field_per_frequency; }
        std::vector<Field> & get_mutable_field_per_frequency() { return field_per_frequency; }
        void set_field_per_frequency(const std::vector<Field> & value) { this->field_per_frequency = value; }

        /**
         * Model used to calculate the current field
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }

        const std::vector<WireDataOrNameUnion> & get_wires() const { return wires; }
        std::vector<WireDataOrNameUnion> & get_mutable_wires() { return wires; }
        void set_wires(const std::vector<WireDataOrNameUnion> & value) { this->wires = value; }
    };

    /**
     * Data describing the output current field
     *
     * Data describing the current in the different chunks used in field calculation
     */
    class WindingWindowCurrentFieldOutput {
        public:
        WindingWindowCurrentFieldOutput() = default;
        virtual ~WindingWindowCurrentFieldOutput() = default;

        private:
        std::vector<Field> field_per_frequency;
        std::string method_used;
        ResultOrigin origin;

        public:
        const std::vector<Field> & get_field_per_frequency() const { return field_per_frequency; }
        std::vector<Field> & get_mutable_field_per_frequency() { return field_per_frequency; }
        void set_field_per_frequency(const std::vector<Field> & value) { this->field_per_frequency = value; }

        /**
         * Model used to calculate the current field
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }
    };

    /**
     * Data describing the complex value of a field in a 2D or 3D space
     */
    class ComplexFieldPoint {
        public:
        ComplexFieldPoint() = default;
        virtual ~ComplexFieldPoint() = default;

        private:
        double imaginary;
        std::optional<std::string> label;
        std::vector<double> point;
        double real;
        std::optional<int64_t> turn_index;
        std::optional<double> turn_length;

        public:
        /**
         * Imaginary value of the field at this point.
         */
        const double & get_imaginary() const { return imaginary; }
        double & get_mutable_imaginary() { return imaginary; }
        void set_imaginary(const double & value) { this->imaginary = value; }

        /**
         * If this point has some special significance, can be identified with this label
         */
        std::optional<std::string> get_label() const { return label; }
        void set_label(std::optional<std::string> value) { this->label = value; }

        /**
         * The coordinates of the point of the field
         */
        const std::vector<double> & get_point() const { return point; }
        std::vector<double> & get_mutable_point() { return point; }
        void set_point(const std::vector<double> & value) { this->point = value; }

        /**
         * Real value of the field at this point
         */
        const double & get_real() const { return real; }
        double & get_mutable_real() { return real; }
        void set_real(const double & value) { this->real = value; }

        /**
         * If this field point is inside of a wire, this is the index of the turn
         */
        std::optional<int64_t> get_turn_index() const { return turn_index; }
        void set_turn_index(std::optional<int64_t> value) { this->turn_index = value; }

        /**
         * If this field point is inside of a wire, this is the length of the turn
         */
        std::optional<double> get_turn_length() const { return turn_length; }
        void set_turn_length(std::optional<double> value) { this->turn_length = value; }
    };

    /**
     * Data describing a field in a 2D or 3D space
     */
    class ComplexField {
        public:
        ComplexField() = default;
        virtual ~ComplexField() = default;

        private:
        std::vector<ComplexFieldPoint> data;
        double frequency;

        public:
        /**
         * Value of the magnetizing inductance
         */
        const std::vector<ComplexFieldPoint> & get_data() const { return data; }
        std::vector<ComplexFieldPoint> & get_mutable_data() { return data; }
        void set_data(const std::vector<ComplexFieldPoint> & value) { this->data = value; }

        /**
         * Value of the field at this point
         */
        const double & get_frequency() const { return frequency; }
        double & get_mutable_frequency() { return frequency; }
        void set_frequency(const double & value) { this->frequency = value; }
    };

    /**
     * Data describing the output magnetic strength field
     */
    class WindingWindowMagneticStrengthFieldOutput {
        public:
        WindingWindowMagneticStrengthFieldOutput() = default;
        virtual ~WindingWindowMagneticStrengthFieldOutput() = default;

        private:
        std::vector<ComplexField> field_per_frequency;
        std::string method_used;
        ResultOrigin origin;

        public:
        const std::vector<ComplexField> & get_field_per_frequency() const { return field_per_frequency; }
        std::vector<ComplexField> & get_mutable_field_per_frequency() { return field_per_frequency; }
        void set_field_per_frequency(const std::vector<ComplexField> & value) { this->field_per_frequency = value; }

        /**
         * Model used to calculate the magnetic strength field
         */
        const std::string & get_method_used() const { return method_used; }
        std::string & get_mutable_method_used() { return method_used; }
        void set_method_used(const std::string & value) { this->method_used = value; }

        const ResultOrigin & get_origin() const { return origin; }
        ResultOrigin & get_mutable_origin() { return origin; }
        void set_origin(const ResultOrigin & value) { this->origin = value; }
    };

    /**
     * The description of the outputs that result of simulating a Magnetic
     */
    class Outputs {
        public:
        Outputs() = default;
        virtual ~Outputs() = default;

        private:
        std::optional<CoreLossesOutput> core_losses;
        std::optional<ImpedanceOutput> impedance;
        std::optional<InductanceOutput> inductance;
        std::optional<std::vector<DielectricVoltage>> insulation;
        std::optional<InsulationCoordinationOutput> insulation_coordination;
        std::optional<std::vector<StrayCapacitanceOutput>> stray_capacitance;
        std::optional<TemperatureOutput> temperature;
        std::optional<WindingLossesOutput> winding_losses;
        std::optional<WindingWindowCurrentDensityFieldOutput> winding_window_current_density_field;
        std::optional<WindingWindowCurrentFieldOutput> winding_window_current_field;
        std::optional<WindingWindowMagneticStrengthFieldOutput> winding_window_magnetic_strength_field;

        public:
        /**
         * Data describing the output core losses
         */
        std::optional<CoreLossesOutput> get_core_losses() const { return core_losses; }
        void set_core_losses(std::optional<CoreLossesOutput> value) { this->core_losses = value; }

        /**
         * Data describing the output impedance
         */
        std::optional<ImpedanceOutput> get_impedance() const { return impedance; }
        void set_impedance(std::optional<ImpedanceOutput> value) { this->impedance = value; }

        /**
         * Data describing the output inductance
         */
        std::optional<InductanceOutput> get_inductance() const { return inductance; }
        void set_inductance(std::optional<InductanceOutput> value) { this->inductance = value; }

        /**
         * Data describing the output insulation that the magnetic has
         */
        std::optional<std::vector<DielectricVoltage>> get_insulation() const { return insulation; }
        void set_insulation(std::optional<std::vector<DielectricVoltage>> value) { this->insulation = value; }

        /**
         * Data describing the output insulation coordination that the magnetic has
         */
        std::optional<InsulationCoordinationOutput> get_insulation_coordination() const { return insulation_coordination; }
        void set_insulation_coordination(std::optional<InsulationCoordinationOutput> value) { this->insulation_coordination = value; }

        /**
         * Data describing the output stray capacitance
         */
        std::optional<std::vector<StrayCapacitanceOutput>> get_stray_capacitance() const { return stray_capacitance; }
        void set_stray_capacitance(std::optional<std::vector<StrayCapacitanceOutput>> value) { this->stray_capacitance = value; }

        /**
         * Data describing the output temperature
         */
        std::optional<TemperatureOutput> get_temperature() const { return temperature; }
        void set_temperature(std::optional<TemperatureOutput> value) { this->temperature = value; }

        /**
         * Data describing the output winding losses
         */
        std::optional<WindingLossesOutput> get_winding_losses() const { return winding_losses; }
        void set_winding_losses(std::optional<WindingLossesOutput> value) { this->winding_losses = value; }

        /**
         * Data describing the output current density field
         */
        std::optional<WindingWindowCurrentDensityFieldOutput> get_winding_window_current_density_field() const { return winding_window_current_density_field; }
        void set_winding_window_current_density_field(std::optional<WindingWindowCurrentDensityFieldOutput> value) { this->winding_window_current_density_field = value; }

        /**
         * Data describing the output current field
         */
        std::optional<WindingWindowCurrentFieldOutput> get_winding_window_current_field() const { return winding_window_current_field; }
        void set_winding_window_current_field(std::optional<WindingWindowCurrentFieldOutput> value) { this->winding_window_current_field = value; }

        /**
         * Data describing the output magnetic strength field
         */
        std::optional<WindingWindowMagneticStrengthFieldOutput> get_winding_window_magnetic_strength_field() const { return winding_window_magnetic_strength_field; }
        void set_winding_window_magnetic_strength_field(std::optional<WindingWindowMagneticStrengthFieldOutput> value) { this->winding_window_magnetic_strength_field = value; }
    };

    /**
     * All the data structure used in the Magnetic Agnostic Structure
     */
    class Mas {
        public:
        Mas() :
            mas_version_constraint(std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::string("^\\d+\\.\\d+\\.\\d+(?:-[0-9A-Za-z.-]+)?$"))
        {}
        virtual ~Mas() = default;

        private:
        Inputs inputs;
        Magnetic magnetic;
        std::optional<MasConformance> mas_conformance;
        std::optional<std::string> mas_version;
        ClassMemberConstraints mas_version_constraint;
        std::vector<Outputs> outputs;

        public:
        /**
         * The description of the inputs that can be used to design a Magnetic
         */
        const Inputs & get_inputs() const { return inputs; }
        Inputs & get_mutable_inputs() { return inputs; }
        void set_inputs(const Inputs & value) { this->inputs = value; }

        /**
         * The description of a magnetic
         */
        const Magnetic & get_magnetic() const { return magnetic; }
        Magnetic & get_mutable_magnetic() { return magnetic; }
        void set_magnetic(const Magnetic & value) { this->magnetic = value; }

        /**
         * Optional declaration of which MAS conformance class this document targets. Class A —
         * Inductor Basic. Class B — Transformer (multi-winding, insulation, isolated topologies).
         * Class C — Full (all schemas, all topologies). See docs/conformance.md and
         * schemas/conformance/.
         */
        std::optional<MasConformance> get_mas_conformance() const { return mas_conformance; }
        void set_mas_conformance(std::optional<MasConformance> value) { this->mas_conformance = value; }

        /**
         * SemVer version of the MAS specification this document conforms to (MAJOR.MINOR.PATCH).
         * Optional in 0.x; will become required in 1.0.
         */
        std::optional<std::string> get_mas_version() const { return mas_version; }
        void set_mas_version(std::optional<std::string> value) { if (value) CheckConstraint("mas_version", mas_version_constraint, *value); this->mas_version = value; }

        /**
         * The description of the outputs that are produced after designing a Magnetic
         */
        const std::vector<Outputs> & get_outputs() const { return outputs; }
        std::vector<Outputs> & get_mutable_outputs() { return outputs; }
        void set_outputs(const std::vector<Outputs> & value) { this->outputs = value; }
    };
}

namespace MAS {
void from_json(const json & j, DimensionWithTolerance & x);
void to_json(json & j, const DimensionWithTolerance & x);

void from_json(const json & j, TopologyExcitation & x);
void to_json(json & j, const TopologyExcitation & x);

void from_json(const json & j, Boost & x);
void to_json(json & j, const Boost & x);

void from_json(const json & j, Buck & x);
void to_json(json & j, const Buck & x);

void from_json(const json & j, CllcOperatingPoint & x);
void to_json(json & j, const CllcOperatingPoint & x);

void from_json(const json & j, CllcResonant & x);
void to_json(json & j, const CllcResonant & x);

void from_json(const json & j, CurrentTransformer & x);
void to_json(json & j, const CurrentTransformer & x);

void from_json(const json & j, DabOperatingPoint & x);
void to_json(json & j, const DabOperatingPoint & x);

void from_json(const json & j, DualActiveBridge & x);
void to_json(json & j, const DualActiveBridge & x);

void from_json(const json & j, FlybackOperatingPoint & x);
void to_json(json & j, const FlybackOperatingPoint & x);

void from_json(const json & j, Flyback & x);
void to_json(json & j, const Flyback & x);

void from_json(const json & j, Forward & x);
void to_json(json & j, const Forward & x);

void from_json(const json & j, IsolatedBuck & x);
void to_json(json & j, const IsolatedBuck & x);

void from_json(const json & j, IsolatedBuckBoost & x);
void to_json(json & j, const IsolatedBuckBoost & x);

void from_json(const json & j, LlcResonant & x);
void to_json(json & j, const LlcResonant & x);

void from_json(const json & j, PsfbOperatingPoint & x);
void to_json(json & j, const PsfbOperatingPoint & x);

void from_json(const json & j, PhaseShiftFullBridge & x);
void to_json(json & j, const PhaseShiftFullBridge & x);

void from_json(const json & j, PushPull & x);
void to_json(json & j, const PushPull & x);

void from_json(const json & j, SupportedTopologies & x);
void to_json(json & j, const SupportedTopologies & x);

void from_json(const json & j, ConverterInformation & x);
void to_json(json & j, const ConverterInformation & x);

void from_json(const json & j, InsulationRequirements & x);
void to_json(json & j, const InsulationRequirements & x);

void from_json(const json & j, MaximumDimensions & x);
void to_json(json & j, const MaximumDimensions & x);

void from_json(const json & j, ImpedancePoint & x);
void to_json(json & j, const ImpedancePoint & x);

void from_json(const json & j, ImpedanceAtFrequency & x);
void to_json(json & j, const ImpedanceAtFrequency & x);

void from_json(const json & j, DesignRequirements & x);
void to_json(json & j, const DesignRequirements & x);

void from_json(const json & j, Cooling & x);
void to_json(json & j, const Cooling & x);

void from_json(const json & j, OperatingConditions & x);
void to_json(json & j, const OperatingConditions & x);

void from_json(const json & j, Harmonics & x);
void to_json(json & j, const Harmonics & x);

void from_json(const json & j, Processed & x);
void to_json(json & j, const Processed & x);

void from_json(const json & j, Waveform & x);
void to_json(json & j, const Waveform & x);

void from_json(const json & j, SignalDescriptor & x);
void to_json(json & j, const SignalDescriptor & x);

void from_json(const json & j, OperatingPointExcitation & x);
void to_json(json & j, const OperatingPointExcitation & x);

void from_json(const json & j, OperatingPoint & x);
void to_json(json & j, const OperatingPoint & x);

void from_json(const json & j, Inputs & x);
void to_json(json & j, const Inputs & x);

void from_json(const json & j, Cost & x);
void to_json(json & j, const Cost & x);

void from_json(const json & j, DistributorInfo & x);
void to_json(json & j, const DistributorInfo & x);

void from_json(const json & j, PinWindingConnection & x);
void to_json(json & j, const PinWindingConnection & x);

void from_json(const json & j, DielectricStrengthElement & x);
void to_json(json & j, const DielectricStrengthElement & x);

void from_json(const json & j, ManufacturerInfo & x);
void to_json(json & j, const ManufacturerInfo & x);

void from_json(const json & j, ResistivityPoint & x);
void to_json(json & j, const ResistivityPoint & x);

void from_json(const json & j, InsulationMaterial & x);
void to_json(json & j, const InsulationMaterial & x);

void from_json(const json & j, Pin & x);
void to_json(json & j, const Pin & x);

void from_json(const json & j, Pinout & x);
void to_json(json & j, const Pinout & x);

void from_json(const json & j, BobbinFunctionalDescription & x);
void to_json(json & j, const BobbinFunctionalDescription & x);

void from_json(const json & j, WindingWindowElement & x);
void to_json(json & j, const WindingWindowElement & x);

void from_json(const json & j, CoreBobbinProcessedDescription & x);
void to_json(json & j, const CoreBobbinProcessedDescription & x);

void from_json(const json & j, Bobbin & x);
void to_json(json & j, const Bobbin & x);

void from_json(const json & j, ConnectionElement & x);
void to_json(json & j, const ConnectionElement & x);

void from_json(const json & j, InsulationWireCoating & x);
void to_json(json & j, const InsulationWireCoating & x);

void from_json(const json & j, Resistivity & x);
void to_json(json & j, const Resistivity & x);

void from_json(const json & j, ThermalConductivityElement & x);
void to_json(json & j, const ThermalConductivityElement & x);

void from_json(const json & j, WireMaterial & x);
void to_json(json & j, const WireMaterial & x);

void from_json(const json & j, WireRound & x);
void to_json(json & j, const WireRound & x);

void from_json(const json & j, Wire & x);
void to_json(json & j, const Wire & x);

void from_json(const json & j, CoilFunctionalDescription & x);
void to_json(json & j, const CoilFunctionalDescription & x);

void from_json(const json & j, PartialWinding & x);
void to_json(json & j, const PartialWinding & x);

void from_json(const json & j, Group & x);
void to_json(json & j, const Group & x);

void from_json(const json & j, Layer & x);
void to_json(json & j, const Layer & x);

void from_json(const json & j, MarginInfo & x);
void to_json(json & j, const MarginInfo & x);

void from_json(const json & j, Section & x);
void to_json(json & j, const Section & x);

void from_json(const json & j, Turn & x);
void to_json(json & j, const Turn & x);

void from_json(const json & j, Coil & x);
void to_json(json & j, const Coil & x);

void from_json(const json & j, CoreGap & x);
void to_json(json & j, const CoreGap & x);

void from_json(const json & j, BhCycleElement & x);
void to_json(json & j, const BhCycleElement & x);

void from_json(const json & j, MassLossesPoint & x);
void to_json(json & j, const MassLossesPoint & x);

void from_json(const json & j, MagnetecCoreLossesMethodData & x);
void to_json(json & j, const MagnetecCoreLossesMethodData & x);

void from_json(const json & j, FrequencyFactor & x);
void to_json(json & j, const FrequencyFactor & x);

void from_json(const json & j, MagneticFieldDcBiasFactor & x);
void to_json(json & j, const MagneticFieldDcBiasFactor & x);

void from_json(const json & j, MagneticFluxDensityFactor & x);
void to_json(json & j, const MagneticFluxDensityFactor & x);

void from_json(const json & j, TemperatureFactor & x);
void to_json(json & j, const TemperatureFactor & x);

void from_json(const json & j, InitialPermeabilitModifier & x);
void to_json(json & j, const InitialPermeabilitModifier & x);

void from_json(const json & j, PermeabilityPoint & x);
void to_json(json & j, const PermeabilityPoint & x);

void from_json(const json & j, ComplexPermeabilityData & x);
void to_json(json & j, const ComplexPermeabilityData & x);

void from_json(const json & j, Permeabilities & x);
void to_json(json & j, const Permeabilities & x);

void from_json(const json & j, CoreMaterialRecommendations & x);
void to_json(json & j, const CoreMaterialRecommendations & x);

void from_json(const json & j, VolumetricLossesPoint & x);
void to_json(json & j, const VolumetricLossesPoint & x);

void from_json(const json & j, RoshenAdditionalCoefficients & x);
void to_json(json & j, const RoshenAdditionalCoefficients & x);

void from_json(const json & j, LossFactorPoint & x);
void to_json(json & j, const LossFactorPoint & x);

void from_json(const json & j, SteinmetzCoreLossesMethodRangeDatum & x);
void to_json(json & j, const SteinmetzCoreLossesMethodRangeDatum & x);

void from_json(const json & j, CoreLossesMethodData & x);
void to_json(json & j, const CoreLossesMethodData & x);

void from_json(const json & j, CoreMaterial & x);
void to_json(json & j, const CoreMaterial & x);

void from_json(const json & j, CoreShape & x);
void to_json(json & j, const CoreShape & x);

void from_json(const json & j, CoreFunctionalDescription & x);
void to_json(json & j, const CoreFunctionalDescription & x);

void from_json(const json & j, Machining & x);
void to_json(json & j, const Machining & x);

void from_json(const json & j, CoreGeometricalDescriptionElement & x);
void to_json(json & j, const CoreGeometricalDescriptionElement & x);

void from_json(const json & j, ColumnElement & x);
void to_json(json & j, const ColumnElement & x);

void from_json(const json & j, EffectiveParameters & x);
void to_json(json & j, const EffectiveParameters & x);

void from_json(const json & j, CoreProcessedDescription & x);
void to_json(json & j, const CoreProcessedDescription & x);

void from_json(const json & j, MagneticCore & x);
void to_json(json & j, const MagneticCore & x);

void from_json(const json & j, MagneticDatasheetApplication & x);
void to_json(json & j, const MagneticDatasheetApplication & x);

void from_json(const json & j, Business & x);
void to_json(json & j, const Business & x);

void from_json(const json & j, CommonModeFilter & x);
void to_json(json & j, const CommonModeFilter & x);

void from_json(const json & j, DcResistance & x);
void to_json(json & j, const DcResistance & x);

void from_json(const json & j, DatasheetImpedancePoint & x);
void to_json(json & j, const DatasheetImpedancePoint & x);

void from_json(const json & j, Electrical & x);
void to_json(json & j, const Electrical & x);

void from_json(const json & j, Mechanical & x);
void to_json(json & j, const Mechanical & x);

void from_json(const json & j, Part & x);
void to_json(json & j, const Part & x);

void from_json(const json & j, Thermal & x);
void to_json(json & j, const Thermal & x);

void from_json(const json & j, DatasheetInfo & x);
void to_json(json & j, const DatasheetInfo & x);

void from_json(const json & j, MagneticManufacturerInfo & x);
void to_json(json & j, const MagneticManufacturerInfo & x);

void from_json(const json & j, Magnetic & x);
void to_json(json & j, const Magnetic & x);

void from_json(const json & j, CoreLossesOutput & x);
void to_json(json & j, const CoreLossesOutput & x);

void from_json(const json & j, ComplexMatrixAtFrequency & x);
void to_json(json & j, const ComplexMatrixAtFrequency & x);

void from_json(const json & j, ScalarMatrixAtFrequency & x);
void to_json(json & j, const ScalarMatrixAtFrequency & x);

void from_json(const json & j, ImpedanceOutput & x);
void to_json(json & j, const ImpedanceOutput & x);

void from_json(const json & j, LeakageInductanceOutput & x);
void to_json(json & j, const LeakageInductanceOutput & x);

void from_json(const json & j, InductanceMeasurementCondition & x);
void to_json(json & j, const InductanceMeasurementCondition & x);

void from_json(const json & j, AirGapReluctanceOutput & x);
void to_json(json & j, const AirGapReluctanceOutput & x);

void from_json(const json & j, MagnetizingInductanceOutput & x);
void to_json(json & j, const MagnetizingInductanceOutput & x);

void from_json(const json & j, InductanceOutput & x);
void to_json(json & j, const InductanceOutput & x);

void from_json(const json & j, DielectricVoltage & x);
void to_json(json & j, const DielectricVoltage & x);

void from_json(const json & j, InsulationCoordinationOutput & x);
void to_json(json & j, const InsulationCoordinationOutput & x);

void from_json(const json & j, SixCapacitorNetworkPerWinding & x);
void to_json(json & j, const SixCapacitorNetworkPerWinding & x);

void from_json(const json & j, TripoleCapacitancePerWinding & x);
void to_json(json & j, const TripoleCapacitancePerWinding & x);

void from_json(const json & j, StrayCapacitanceOutput & x);
void to_json(json & j, const StrayCapacitanceOutput & x);

void from_json(const json & j, TemperaturePoint & x);
void to_json(json & j, const TemperaturePoint & x);

void from_json(const json & j, TemperatureOutput & x);
void to_json(json & j, const TemperatureOutput & x);

void from_json(const json & j, OhmicLosses & x);
void to_json(json & j, const OhmicLosses & x);

void from_json(const json & j, WindingLossElement & x);
void to_json(json & j, const WindingLossElement & x);

void from_json(const json & j, WindingLossesPerElement & x);
void to_json(json & j, const WindingLossesPerElement & x);

void from_json(const json & j, WindingLossesOutput & x);
void to_json(json & j, const WindingLossesOutput & x);

void from_json(const json & j, FieldPoint & x);
void to_json(json & j, const FieldPoint & x);

void from_json(const json & j, Field & x);
void to_json(json & j, const Field & x);

void from_json(const json & j, WindingWindowCurrentDensityFieldOutput & x);
void to_json(json & j, const WindingWindowCurrentDensityFieldOutput & x);

void from_json(const json & j, WindingWindowCurrentFieldOutput & x);
void to_json(json & j, const WindingWindowCurrentFieldOutput & x);

void from_json(const json & j, ComplexFieldPoint & x);
void to_json(json & j, const ComplexFieldPoint & x);

void from_json(const json & j, ComplexField & x);
void to_json(json & j, const ComplexField & x);

void from_json(const json & j, WindingWindowMagneticStrengthFieldOutput & x);
void to_json(json & j, const WindingWindowMagneticStrengthFieldOutput & x);

void from_json(const json & j, Outputs & x);
void to_json(json & j, const Outputs & x);

void from_json(const json & j, Mas & x);
void to_json(json & j, const Mas & x);

void from_json(const json & j, OutputSType & x);
void to_json(json & j, const OutputSType & x);

void from_json(const json & j, CllcPowerFlow & x);
void to_json(json & j, const CllcPowerFlow & x);

void from_json(const json & j, WaveformLabel & x);
void to_json(json & j, const WaveformLabel & x);

void from_json(const json & j, ModulationType & x);
void to_json(json & j, const ModulationType & x);

void from_json(const json & j, FlybackModes & x);
void to_json(json & j, const FlybackModes & x);

void from_json(const json & j, LlcBridgeType & x);
void to_json(json & j, const LlcBridgeType & x);

void from_json(const json & j, PsfbRectifierType & x);
void to_json(json & j, const PsfbRectifierType & x);

void from_json(const json & j, Application & x);
void to_json(json & j, const Application & x);

void from_json(const json & j, Cti & x);
void to_json(json & j, const Cti & x);

void from_json(const json & j, InsulationType & x);
void to_json(json & j, const InsulationType & x);

void from_json(const json & j, OvervoltageCategory & x);
void to_json(json & j, const OvervoltageCategory & x);

void from_json(const json & j, PollutionDegree & x);
void to_json(json & j, const PollutionDegree & x);

void from_json(const json & j, InsulationStandards & x);
void to_json(json & j, const InsulationStandards & x);

void from_json(const json & j, IsolationSide & x);
void to_json(json & j, const IsolationSide & x);

void from_json(const json & j, Market & x);
void to_json(json & j, const Market & x);

void from_json(const json & j, SubApplication & x);
void to_json(json & j, const SubApplication & x);

void from_json(const json & j, ConnectionType & x);
void to_json(json & j, const ConnectionType & x);

void from_json(const json & j, Topologies & x);
void to_json(json & j, const Topologies & x);

void from_json(const json & j, WiringTechnology & x);
void to_json(json & j, const WiringTechnology & x);

void from_json(const json & j, BobbinFamily & x);
void to_json(json & j, const BobbinFamily & x);

void from_json(const json & j, Status & x);
void to_json(json & j, const Status & x);

void from_json(const json & j, TemperatureClassEnum & x);
void to_json(json & j, const TemperatureClassEnum & x);

void from_json(const json & j, PinShape & x);
void to_json(json & j, const PinShape & x);

void from_json(const json & j, PinDescriptionType & x);
void to_json(json & j, const PinDescriptionType & x);

void from_json(const json & j, FunctionalDescriptionType & x);
void to_json(json & j, const FunctionalDescriptionType & x);

void from_json(const json & j, ColumnShape & x);
void to_json(json & j, const ColumnShape & x);

void from_json(const json & j, CoilAlignment & x);
void to_json(json & j, const CoilAlignment & x);

void from_json(const json & j, WindingOrientation & x);
void to_json(json & j, const WindingOrientation & x);

void from_json(const json & j, WindingWindowShape & x);
void to_json(json & j, const WindingWindowShape & x);

void from_json(const json & j, Direction & x);
void to_json(json & j, const Direction & x);

void from_json(const json & j, InsulationWireCoatingType & x);
void to_json(json & j, const InsulationWireCoatingType & x);

void from_json(const json & j, WireStandard & x);
void to_json(json & j, const WireStandard & x);

void from_json(const json & j, WireType & x);
void to_json(json & j, const WireType & x);

void from_json(const json & j, CoordinateSystem & x);
void to_json(json & j, const CoordinateSystem & x);

void from_json(const json & j, ElectricalType & x);
void to_json(json & j, const ElectricalType & x);

void from_json(const json & j, WindingStyle & x);
void to_json(json & j, const WindingStyle & x);

void from_json(const json & j, TurnCrossSectionalShape & x);
void to_json(json & j, const TurnCrossSectionalShape & x);

void from_json(const json & j, TurnOrientation & x);
void to_json(json & j, const TurnOrientation & x);

void from_json(const json & j, Coating & x);
void to_json(json & j, const Coating & x);

void from_json(const json & j, GapType & x);
void to_json(json & j, const GapType & x);

void from_json(const json & j, MassCoreLossesMethodType & x);
void to_json(json & j, const MassCoreLossesMethodType & x);

void from_json(const json & j, MaterialType & x);
void to_json(json & j, const MaterialType & x);

void from_json(const json & j, MaterialComposition & x);
void to_json(json & j, const MaterialComposition & x);

void from_json(const json & j, InitialPermeabilitModifierMethod & x);
void to_json(json & j, const InitialPermeabilitModifierMethod & x);

void from_json(const json & j, CoreMaterialType & x);
void to_json(json & j, const CoreMaterialType & x);

void from_json(const json & j, VolumetricCoreLossesMethodType & x);
void to_json(json & j, const VolumetricCoreLossesMethodType & x);

void from_json(const json & j, CoreShapeFamily & x);
void to_json(json & j, const CoreShapeFamily & x);

void from_json(const json & j, MagneticCircuit & x);
void to_json(json & j, const MagneticCircuit & x);

void from_json(const json & j, CoreType & x);
void to_json(json & j, const CoreType & x);

void from_json(const json & j, CoreGeometricalDescriptionElementType & x);
void to_json(json & j, const CoreGeometricalDescriptionElementType & x);

void from_json(const json & j, ColumnType & x);
void to_json(json & j, const ColumnType & x);

void from_json(const json & j, MasConformance & x);
void to_json(json & j, const MasConformance & x);

void from_json(const json & j, ResultOrigin & x);
void to_json(json & j, const ResultOrigin & x);

void from_json(const json & j, VoltageType & x);
void to_json(json & j, const VoltageType & x);
}
namespace nlohmann {
template <>
struct adl_serializer<std::variant<MAS::DimensionWithTolerance, double>> {
    static void from_json(const json & j, std::variant<MAS::DimensionWithTolerance, double> & x);
    static void to_json(json & j, const std::variant<MAS::DimensionWithTolerance, double> & x);
};

template <>
struct adl_serializer<std::variant<double, MAS::TemperatureClassEnum>> {
    static void from_json(const json & j, std::variant<double, MAS::TemperatureClassEnum> & x);
    static void to_json(json & j, const std::variant<double, MAS::TemperatureClassEnum> & x);
};

template <>
struct adl_serializer<std::variant<MAS::InsulationMaterial, std::string>> {
    static void from_json(const json & j, std::variant<MAS::InsulationMaterial, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::InsulationMaterial, std::string> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, double>> {
    static void from_json(const json & j, std::variant<std::vector<double>, double> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, double> & x);
};

template <>
struct adl_serializer<std::variant<MAS::Bobbin, std::string>> {
    static void from_json(const json & j, std::variant<MAS::Bobbin, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::Bobbin, std::string> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string>> {
    static void from_json(const json & j, std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string> & x);
    static void to_json(json & j, const std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string> & x);
};

template <>
struct adl_serializer<std::variant<MAS::InsulationWireCoating, std::string>> {
    static void from_json(const json & j, std::variant<MAS::InsulationWireCoating, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::InsulationWireCoating, std::string> & x);
};

template <>
struct adl_serializer<std::variant<MAS::WireMaterial, std::string>> {
    static void from_json(const json & j, std::variant<MAS::WireMaterial, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::WireMaterial, std::string> & x);
};

template <>
struct adl_serializer<std::variant<MAS::WireRound, std::string>> {
    static void from_json(const json & j, std::variant<MAS::WireRound, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::WireRound, std::string> & x);
};

template <>
struct adl_serializer<std::variant<MAS::Wire, std::string>> {
    static void from_json(const json & j, std::variant<MAS::Wire, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::Wire, std::string> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<double>, MAS::MarginInfo>> {
    static void from_json(const json & j, std::variant<std::vector<double>, MAS::MarginInfo> & x);
    static void to_json(json & j, const std::variant<std::vector<double>, MAS::MarginInfo> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData>> {
    static void from_json(const json & j, std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData> & x);
    static void to_json(json & j, const std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint>> {
    static void from_json(const json & j, std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint> & x);
    static void to_json(json & j, const std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint> & x);
};

template <>
struct adl_serializer<std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData>> {
    static void from_json(const json & j, std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData> & x);
    static void to_json(json & j, const std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData> & x);
};

template <>
struct adl_serializer<std::variant<MAS::CoreMaterial, std::string>> {
    static void from_json(const json & j, std::variant<MAS::CoreMaterial, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::CoreMaterial, std::string> & x);
};

template <>
struct adl_serializer<std::variant<MAS::CoreShape, std::string>> {
    static void from_json(const json & j, std::variant<MAS::CoreShape, std::string> & x);
    static void to_json(json & j, const std::variant<MAS::CoreShape, std::string> & x);
};
}
namespace MAS {
    inline void from_json(const json & j, DimensionWithTolerance& x) {
        x.set_exclude_maximum(get_stack_optional<bool>(j, "excludeMaximum"));
        x.set_exclude_minimum(get_stack_optional<bool>(j, "excludeMinimum"));
        x.set_maximum(get_stack_optional<double>(j, "maximum"));
        x.set_minimum(get_stack_optional<double>(j, "minimum"));
        x.set_nominal(get_stack_optional<double>(j, "nominal"));
        x.set_unit(get_stack_optional<std::string>(j, "unit"));
    }

    inline void to_json(json & j, const DimensionWithTolerance & x) {
        j = json::object();
        j["excludeMaximum"] = x.get_exclude_maximum();
        j["excludeMinimum"] = x.get_exclude_minimum();
        j["maximum"] = x.get_maximum();
        j["minimum"] = x.get_minimum();
        j["nominal"] = x.get_nominal();
        j["unit"] = x.get_unit();
    }

    inline void from_json(const json & j, TopologyExcitation& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_output_currents(j.at("outputCurrents").get<std::vector<double>>());
        x.set_output_currents_type(get_stack_optional<OutputSType>(j, "outputCurrentsType"));
        x.set_output_voltages(j.at("outputVoltages").get<std::vector<double>>());
        x.set_output_voltages_type(get_stack_optional<OutputSType>(j, "outputVoltagesType"));
        x.set_switching_frequency(j.at("switchingFrequency").get<double>());
    }

    inline void to_json(json & j, const TopologyExcitation & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["outputCurrents"] = x.get_output_currents();
        j["outputCurrentsType"] = x.get_output_currents_type();
        j["outputVoltages"] = x.get_output_voltages();
        j["outputVoltagesType"] = x.get_output_voltages_type();
        j["switchingFrequency"] = x.get_switching_frequency();
    }

    inline void from_json(const json & j, Boost& x) {
        x.set_current_ripple_ratio(get_stack_optional<double>(j, "currentRippleRatio"));
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const Boost & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, Buck& x) {
        x.set_current_ripple_ratio(get_stack_optional<double>(j, "currentRippleRatio"));
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const Buck & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, CllcOperatingPoint& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_output_currents(j.at("outputCurrents").get<std::vector<double>>());
        x.set_output_currents_type(get_stack_optional<OutputSType>(j, "outputCurrentsType"));
        x.set_output_voltages(j.at("outputVoltages").get<std::vector<double>>());
        x.set_output_voltages_type(get_stack_optional<OutputSType>(j, "outputVoltagesType"));
        x.set_switching_frequency(j.at("switchingFrequency").get<double>());
        x.set_power_flow(j.at("powerFlow").get<CllcPowerFlow>());
    }

    inline void to_json(json & j, const CllcOperatingPoint & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["outputCurrents"] = x.get_output_currents();
        j["outputCurrentsType"] = x.get_output_currents_type();
        j["outputVoltages"] = x.get_output_voltages();
        j["outputVoltagesType"] = x.get_output_voltages_type();
        j["switchingFrequency"] = x.get_switching_frequency();
        j["powerFlow"] = x.get_power_flow();
    }

    inline void from_json(const json & j, CllcResonant& x) {
        x.set_bidirectional(get_stack_optional<bool>(j, "bidirectional"));
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_max_switching_frequency(j.at("maxSwitchingFrequency").get<double>());
        x.set_min_switching_frequency(j.at("minSwitchingFrequency").get<double>());
        x.set_operating_points(j.at("operatingPoints").get<std::vector<CllcOperatingPoint>>());
        x.set_quality_factor(get_stack_optional<double>(j, "qualityFactor"));
        x.set_symmetric_design(get_stack_optional<bool>(j, "symmetricDesign"));
    }

    inline void to_json(json & j, const CllcResonant & x) {
        j = json::object();
        j["bidirectional"] = x.get_bidirectional();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maxSwitchingFrequency"] = x.get_max_switching_frequency();
        j["minSwitchingFrequency"] = x.get_min_switching_frequency();
        j["operatingPoints"] = x.get_operating_points();
        j["qualityFactor"] = x.get_quality_factor();
        j["symmetricDesign"] = x.get_symmetric_design();
    }

    inline void from_json(const json & j, CurrentTransformer& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_burden_resistor(j.at("burdenResistor").get<double>());
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_frequency(j.at("frequency").get<double>());
        x.set_maximum_duty_cycle(j.at("maximumDutyCycle").get<double>());
        x.set_maximum_primary_current_peak(j.at("maximumPrimaryCurrentPeak").get<double>());
        x.set_waveform_label(j.at("waveformLabel").get<WaveformLabel>());
    }

    inline void to_json(json & j, const CurrentTransformer & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["burdenResistor"] = x.get_burden_resistor();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["frequency"] = x.get_frequency();
        j["maximumDutyCycle"] = x.get_maximum_duty_cycle();
        j["maximumPrimaryCurrentPeak"] = x.get_maximum_primary_current_peak();
        j["waveformLabel"] = x.get_waveform_label();
    }

    inline void from_json(const json & j, DabOperatingPoint& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_output_currents(j.at("outputCurrents").get<std::vector<double>>());
        x.set_output_currents_type(get_stack_optional<OutputSType>(j, "outputCurrentsType"));
        x.set_output_voltages(j.at("outputVoltages").get<std::vector<double>>());
        x.set_output_voltages_type(get_stack_optional<OutputSType>(j, "outputVoltagesType"));
        x.set_switching_frequency(j.at("switchingFrequency").get<double>());
        x.set_inner_phase_shift1(get_stack_optional<double>(j, "innerPhaseShift1"));
        x.set_inner_phase_shift2(get_stack_optional<double>(j, "innerPhaseShift2"));
        x.set_inner_phase_shift3(get_stack_optional<double>(j, "innerPhaseShift3"));
        x.set_modulation_type(get_stack_optional<ModulationType>(j, "modulationType"));
    }

    inline void to_json(json & j, const DabOperatingPoint & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["outputCurrents"] = x.get_output_currents();
        j["outputCurrentsType"] = x.get_output_currents_type();
        j["outputVoltages"] = x.get_output_voltages();
        j["outputVoltagesType"] = x.get_output_voltages_type();
        j["switchingFrequency"] = x.get_switching_frequency();
        j["innerPhaseShift1"] = x.get_inner_phase_shift1();
        j["innerPhaseShift2"] = x.get_inner_phase_shift2();
        j["innerPhaseShift3"] = x.get_inner_phase_shift3();
        j["modulationType"] = x.get_modulation_type();
    }

    inline void from_json(const json & j, DualActiveBridge& x) {
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_operating_points(j.at("operatingPoints").get<std::vector<DabOperatingPoint>>());
        x.set_per_secondary_leakage(get_stack_optional<std::vector<double>>(j, "perSecondaryLeakage"));
        x.set_series_inductance(get_stack_optional<double>(j, "seriesInductance"));
        x.set_use_leakage_inductance(get_stack_optional<bool>(j, "useLeakageInductance"));
    }

    inline void to_json(json & j, const DualActiveBridge & x) {
        j = json::object();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["operatingPoints"] = x.get_operating_points();
        j["perSecondaryLeakage"] = x.get_per_secondary_leakage();
        j["seriesInductance"] = x.get_series_inductance();
        j["useLeakageInductance"] = x.get_use_leakage_inductance();
    }

    inline void from_json(const json & j, FlybackOperatingPoint& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_mode(get_stack_optional<FlybackModes>(j, "mode"));
        x.set_output_currents(j.at("outputCurrents").get<std::vector<double>>());
        x.set_output_currents_type(get_stack_optional<OutputSType>(j, "outputCurrentsType"));
        x.set_output_voltages(j.at("outputVoltages").get<std::vector<double>>());
        x.set_output_voltages_type(get_stack_optional<OutputSType>(j, "outputVoltagesType"));
        x.set_switching_frequency(get_stack_optional<double>(j, "switchingFrequency"));
    }

    inline void to_json(json & j, const FlybackOperatingPoint & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["mode"] = x.get_mode();
        j["outputCurrents"] = x.get_output_currents();
        j["outputCurrentsType"] = x.get_output_currents_type();
        j["outputVoltages"] = x.get_output_voltages();
        j["outputVoltagesType"] = x.get_output_voltages_type();
        j["switchingFrequency"] = x.get_switching_frequency();
    }

    inline void from_json(const json & j, Flyback& x) {
        x.set_current_ripple_ratio(j.at("currentRippleRatio").get<double>());
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_efficiency(j.at("efficiency").get<double>());
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_drain_source_voltage(get_stack_optional<double>(j, "maximumDrainSourceVoltage"));
        x.set_maximum_duty_cycle(get_stack_optional<double>(j, "maximumDutyCycle"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<FlybackOperatingPoint>>());
    }

    inline void to_json(json & j, const Flyback & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumDrainSourceVoltage"] = x.get_maximum_drain_source_voltage();
        j["maximumDutyCycle"] = x.get_maximum_duty_cycle();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, Forward& x) {
        x.set_current_ripple_ratio(j.at("currentRippleRatio").get<double>());
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_duty_cycle(get_stack_optional<double>(j, "dutyCycle"));
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const Forward & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["dutyCycle"] = x.get_duty_cycle();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, IsolatedBuck& x) {
        x.set_current_ripple_ratio(get_stack_optional<double>(j, "currentRippleRatio"));
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const IsolatedBuck & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, IsolatedBuckBoost& x) {
        x.set_current_ripple_ratio(get_stack_optional<double>(j, "currentRippleRatio"));
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const IsolatedBuckBoost & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, LlcResonant& x) {
        x.set_bridge_type(get_stack_optional<LlcBridgeType>(j, "bridgeType"));
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_inductance_ratio(get_stack_optional<double>(j, "inductanceRatio"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_integrated_resonant_inductor(get_stack_optional<bool>(j, "integratedResonantInductor"));
        x.set_max_switching_frequency(j.at("maxSwitchingFrequency").get<double>());
        x.set_min_switching_frequency(j.at("minSwitchingFrequency").get<double>());
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
        x.set_quality_factor(get_stack_optional<double>(j, "qualityFactor"));
        x.set_resonant_capacitance(get_stack_optional<double>(j, "resonantCapacitance"));
        x.set_resonant_frequency(get_stack_optional<double>(j, "resonantFrequency"));
        x.set_series_inductance(get_stack_optional<double>(j, "seriesInductance"));
    }

    inline void to_json(json & j, const LlcResonant & x) {
        j = json::object();
        j["bridgeType"] = x.get_bridge_type();
        j["efficiency"] = x.get_efficiency();
        j["inductanceRatio"] = x.get_inductance_ratio();
        j["inputVoltage"] = x.get_input_voltage();
        j["integratedResonantInductor"] = x.get_integrated_resonant_inductor();
        j["maxSwitchingFrequency"] = x.get_max_switching_frequency();
        j["minSwitchingFrequency"] = x.get_min_switching_frequency();
        j["operatingPoints"] = x.get_operating_points();
        j["qualityFactor"] = x.get_quality_factor();
        j["resonantCapacitance"] = x.get_resonant_capacitance();
        j["resonantFrequency"] = x.get_resonant_frequency();
        j["seriesInductance"] = x.get_series_inductance();
    }

    inline void from_json(const json & j, PsfbOperatingPoint& x) {
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_output_currents(j.at("outputCurrents").get<std::vector<double>>());
        x.set_output_currents_type(get_stack_optional<OutputSType>(j, "outputCurrentsType"));
        x.set_output_voltages(j.at("outputVoltages").get<std::vector<double>>());
        x.set_output_voltages_type(get_stack_optional<OutputSType>(j, "outputVoltagesType"));
        x.set_switching_frequency(j.at("switchingFrequency").get<double>());
        x.set_phase_shift(j.at("phaseShift").get<double>());
    }

    inline void to_json(json & j, const PsfbOperatingPoint & x) {
        j = json::object();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["outputCurrents"] = x.get_output_currents();
        j["outputCurrentsType"] = x.get_output_currents_type();
        j["outputVoltages"] = x.get_output_voltages();
        j["outputVoltagesType"] = x.get_output_voltages_type();
        j["switchingFrequency"] = x.get_switching_frequency();
        j["phaseShift"] = x.get_phase_shift();
    }

    inline void from_json(const json & j, PhaseShiftFullBridge& x) {
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_phase_shift(get_stack_optional<double>(j, "maximumPhaseShift"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<PsfbOperatingPoint>>());
        x.set_output_inductance(get_stack_optional<double>(j, "outputInductance"));
        x.set_rectifier_type(get_stack_optional<PsfbRectifierType>(j, "rectifierType"));
        x.set_series_inductance(get_stack_optional<double>(j, "seriesInductance"));
        x.set_use_leakage_inductance(get_stack_optional<bool>(j, "useLeakageInductance"));
    }

    inline void to_json(json & j, const PhaseShiftFullBridge & x) {
        j = json::object();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumPhaseShift"] = x.get_maximum_phase_shift();
        j["operatingPoints"] = x.get_operating_points();
        j["outputInductance"] = x.get_output_inductance();
        j["rectifierType"] = x.get_rectifier_type();
        j["seriesInductance"] = x.get_series_inductance();
        j["useLeakageInductance"] = x.get_use_leakage_inductance();
    }

    inline void from_json(const json & j, PushPull& x) {
        x.set_current_ripple_ratio(j.at("currentRippleRatio").get<double>());
        x.set_diode_voltage_drop(j.at("diodeVoltageDrop").get<double>());
        x.set_duty_cycle(get_stack_optional<double>(j, "dutyCycle"));
        x.set_efficiency(get_stack_optional<double>(j, "efficiency"));
        x.set_input_voltage(j.at("inputVoltage").get<DimensionWithTolerance>());
        x.set_maximum_drain_source_voltage(get_stack_optional<double>(j, "maximumDrainSourceVoltage"));
        x.set_maximum_switch_current(get_stack_optional<double>(j, "maximumSwitchCurrent"));
        x.set_operating_points(j.at("operatingPoints").get<std::vector<TopologyExcitation>>());
    }

    inline void to_json(json & j, const PushPull & x) {
        j = json::object();
        j["currentRippleRatio"] = x.get_current_ripple_ratio();
        j["diodeVoltageDrop"] = x.get_diode_voltage_drop();
        j["dutyCycle"] = x.get_duty_cycle();
        j["efficiency"] = x.get_efficiency();
        j["inputVoltage"] = x.get_input_voltage();
        j["maximumDrainSourceVoltage"] = x.get_maximum_drain_source_voltage();
        j["maximumSwitchCurrent"] = x.get_maximum_switch_current();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, SupportedTopologies& x) {
        x.set_boost(get_stack_optional<Boost>(j, "boost"));
        x.set_buck(get_stack_optional<Buck>(j, "buck"));
        x.set_cllc_resonant(get_stack_optional<CllcResonant>(j, "cllcResonant"));
        x.set_current_transformer(get_stack_optional<CurrentTransformer>(j, "currentTransformer"));
        x.set_dual_active_bridge(get_stack_optional<DualActiveBridge>(j, "dualActiveBridge"));
        x.set_flyback(get_stack_optional<Flyback>(j, "flyback"));
        x.set_forward(get_stack_optional<Forward>(j, "forward"));
        x.set_isolated_buck(get_stack_optional<IsolatedBuck>(j, "isolatedBuck"));
        x.set_isolated_buck_boost(get_stack_optional<IsolatedBuckBoost>(j, "isolatedBuckBoost"));
        x.set_llc_resonant(get_stack_optional<LlcResonant>(j, "llcResonant"));
        x.set_phase_shift_full_bridge(get_stack_optional<PhaseShiftFullBridge>(j, "phaseShiftFullBridge"));
        x.set_push_pull(get_stack_optional<PushPull>(j, "pushPull"));
    }

    inline void to_json(json & j, const SupportedTopologies & x) {
        j = json::object();
        j["boost"] = x.get_boost();
        j["buck"] = x.get_buck();
        j["cllcResonant"] = x.get_cllc_resonant();
        j["currentTransformer"] = x.get_current_transformer();
        j["dualActiveBridge"] = x.get_dual_active_bridge();
        j["flyback"] = x.get_flyback();
        j["forward"] = x.get_forward();
        j["isolatedBuck"] = x.get_isolated_buck();
        j["isolatedBuckBoost"] = x.get_isolated_buck_boost();
        j["llcResonant"] = x.get_llc_resonant();
        j["phaseShiftFullBridge"] = x.get_phase_shift_full_bridge();
        j["pushPull"] = x.get_push_pull();
    }

    inline void from_json(const json & j, ConverterInformation& x) {
        x.set_supported_topologies(get_stack_optional<SupportedTopologies>(j, "supportedTopologies"));
    }

    inline void to_json(json & j, const ConverterInformation & x) {
        j = json::object();
        j["supportedTopologies"] = x.get_supported_topologies();
    }

    inline void from_json(const json & j, InsulationRequirements& x) {
        x.set_altitude(get_stack_optional<DimensionWithTolerance>(j, "altitude"));
        x.set_cti(get_stack_optional<Cti>(j, "cti"));
        x.set_insulation_type(get_stack_optional<InsulationType>(j, "insulationType"));
        x.set_main_supply_voltage(get_stack_optional<DimensionWithTolerance>(j, "mainSupplyVoltage"));
        x.set_overvoltage_category(get_stack_optional<OvervoltageCategory>(j, "overvoltageCategory"));
        x.set_pollution_degree(get_stack_optional<PollutionDegree>(j, "pollutionDegree"));
        x.set_standards(get_stack_optional<std::vector<InsulationStandards>>(j, "standards"));
    }

    inline void to_json(json & j, const InsulationRequirements & x) {
        j = json::object();
        j["altitude"] = x.get_altitude();
        j["cti"] = x.get_cti();
        j["insulationType"] = x.get_insulation_type();
        j["mainSupplyVoltage"] = x.get_main_supply_voltage();
        j["overvoltageCategory"] = x.get_overvoltage_category();
        j["pollutionDegree"] = x.get_pollution_degree();
        j["standards"] = x.get_standards();
    }

    inline void from_json(const json & j, MaximumDimensions& x) {
        x.set_depth(get_stack_optional<double>(j, "depth"));
        x.set_height(get_stack_optional<double>(j, "height"));
        x.set_width(get_stack_optional<double>(j, "width"));
    }

    inline void to_json(json & j, const MaximumDimensions & x) {
        j = json::object();
        j["depth"] = x.get_depth();
        j["height"] = x.get_height();
        j["width"] = x.get_width();
    }

    inline void from_json(const json & j, ImpedancePoint& x) {
        x.set_imaginary_part(get_stack_optional<double>(j, "imaginaryPart"));
        x.set_magnitude(j.at("magnitude").get<double>());
        x.set_phase(get_stack_optional<double>(j, "phase"));
        x.set_real_part(get_stack_optional<double>(j, "realPart"));
    }

    inline void to_json(json & j, const ImpedancePoint & x) {
        j = json::object();
        j["imaginaryPart"] = x.get_imaginary_part();
        j["magnitude"] = x.get_magnitude();
        j["phase"] = x.get_phase();
        j["realPart"] = x.get_real_part();
    }

    inline void from_json(const json & j, ImpedanceAtFrequency& x) {
        x.set_frequency(j.at("frequency").get<double>());
        x.set_impedance(j.at("impedance").get<ImpedancePoint>());
    }

    inline void to_json(json & j, const ImpedanceAtFrequency & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["impedance"] = x.get_impedance();
    }

    inline void from_json(const json & j, DesignRequirements& x) {
        x.set_application(get_stack_optional<Application>(j, "application"));
        x.set_insulation(get_stack_optional<InsulationRequirements>(j, "insulation"));
        x.set_isolation_sides(get_stack_optional<std::vector<IsolationSide>>(j, "isolationSides"));
        x.set_leakage_inductance(get_stack_optional<std::vector<DimensionWithTolerance>>(j, "leakageInductance"));
        x.set_magnetizing_inductance(j.at("magnetizingInductance").get<DimensionWithTolerance>());
        x.set_market(get_stack_optional<Market>(j, "market"));
        x.set_maximum_dimensions(get_stack_optional<MaximumDimensions>(j, "maximumDimensions"));
        x.set_maximum_weight(get_stack_optional<double>(j, "maximumWeight"));
        x.set_minimum_impedance(get_stack_optional<std::vector<ImpedanceAtFrequency>>(j, "minimumImpedance"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_operating_temperature(get_stack_optional<DimensionWithTolerance>(j, "operatingTemperature"));
        x.set_stray_capacitance(get_stack_optional<std::vector<DimensionWithTolerance>>(j, "strayCapacitance"));
        x.set_sub_application(get_stack_optional<SubApplication>(j, "subApplication"));
        x.set_terminal_type(get_stack_optional<std::vector<ConnectionType>>(j, "terminalType"));
        x.set_topology(get_stack_optional<Topologies>(j, "topology"));
        x.set_turns_ratios(j.at("turnsRatios").get<std::vector<DimensionWithTolerance>>());
        x.set_wiring_technology(get_stack_optional<WiringTechnology>(j, "wiringTechnology"));
    }

    inline void to_json(json & j, const DesignRequirements & x) {
        j = json::object();
        j["application"] = x.get_application();
        j["insulation"] = x.get_insulation();
        j["isolationSides"] = x.get_isolation_sides();
        j["leakageInductance"] = x.get_leakage_inductance();
        j["magnetizingInductance"] = x.get_magnetizing_inductance();
        j["market"] = x.get_market();
        j["maximumDimensions"] = x.get_maximum_dimensions();
        j["maximumWeight"] = x.get_maximum_weight();
        j["minimumImpedance"] = x.get_minimum_impedance();
        j["name"] = x.get_name();
        j["operatingTemperature"] = x.get_operating_temperature();
        j["strayCapacitance"] = x.get_stray_capacitance();
        j["subApplication"] = x.get_sub_application();
        j["terminalType"] = x.get_terminal_type();
        j["topology"] = x.get_topology();
        j["turnsRatios"] = x.get_turns_ratios();
        j["wiringTechnology"] = x.get_wiring_technology();
    }

    inline void from_json(const json & j, Cooling& x) {
        x.set_fluid(get_stack_optional<std::string>(j, "fluid"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_flow_diameter(get_stack_optional<double>(j, "flowDiameter"));
        x.set_velocity(get_stack_optional<std::vector<double>>(j, "velocity"));
        x.set_dimensions(get_stack_optional<std::vector<double>>(j, "dimensions"));
        x.set_interface_thermal_resistance(get_stack_optional<double>(j, "interfaceThermalResistance"));
        x.set_interface_thickness(get_stack_optional<double>(j, "interfaceThickness"));
        x.set_thermal_resistance(get_stack_optional<double>(j, "thermalResistance"));
        x.set_maximum_temperature(get_stack_optional<double>(j, "maximumTemperature"));
    }

    inline void to_json(json & j, const Cooling & x) {
        j = json::object();
        j["fluid"] = x.get_fluid();
        j["temperature"] = x.get_temperature();
        j["flowDiameter"] = x.get_flow_diameter();
        j["velocity"] = x.get_velocity();
        j["dimensions"] = x.get_dimensions();
        j["interfaceThermalResistance"] = x.get_interface_thermal_resistance();
        j["interfaceThickness"] = x.get_interface_thickness();
        j["thermalResistance"] = x.get_thermal_resistance();
        j["maximumTemperature"] = x.get_maximum_temperature();
    }

    inline void from_json(const json & j, OperatingConditions& x) {
        x.set_ambient_relative_humidity(get_stack_optional<double>(j, "ambientRelativeHumidity"));
        x.set_ambient_temperature(j.at("ambientTemperature").get<double>());
        x.set_cooling(get_stack_optional<Cooling>(j, "cooling"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
    }

    inline void to_json(json & j, const OperatingConditions & x) {
        j = json::object();
        j["ambientRelativeHumidity"] = x.get_ambient_relative_humidity();
        j["ambientTemperature"] = x.get_ambient_temperature();
        j["cooling"] = x.get_cooling();
        j["name"] = x.get_name();
    }

    inline void from_json(const json & j, Harmonics& x) {
        x.set_amplitudes(j.at("amplitudes").get<std::vector<double>>());
        x.set_frequencies(j.at("frequencies").get<std::vector<double>>());
    }

    inline void to_json(json & j, const Harmonics & x) {
        j = json::object();
        j["amplitudes"] = x.get_amplitudes();
        j["frequencies"] = x.get_frequencies();
    }

    inline void from_json(const json & j, Processed& x) {
        x.set_ac_effective_frequency(get_stack_optional<double>(j, "acEffectiveFrequency"));
        x.set_average(get_stack_optional<double>(j, "average"));
        x.set_dead_time(get_stack_optional<double>(j, "deadTime"));
        x.set_duty_cycle(get_stack_optional<double>(j, "dutyCycle"));
        x.set_effective_frequency(get_stack_optional<double>(j, "effectiveFrequency"));
        x.set_label(j.at("label").get<WaveformLabel>());
        x.set_negative_peak(get_stack_optional<double>(j, "negativePeak"));
        x.set_offset(j.at("offset").get<double>());
        x.set_peak(get_stack_optional<double>(j, "peak"));
        x.set_peak_to_peak(get_stack_optional<double>(j, "peakToPeak"));
        x.set_phase(get_stack_optional<double>(j, "phase"));
        x.set_positive_peak(get_stack_optional<double>(j, "positivePeak"));
        x.set_rms(get_stack_optional<double>(j, "rms"));
        x.set_thd(get_stack_optional<double>(j, "thd"));
    }

    inline void to_json(json & j, const Processed & x) {
        j = json::object();
        j["acEffectiveFrequency"] = x.get_ac_effective_frequency();
        j["average"] = x.get_average();
        j["deadTime"] = x.get_dead_time();
        j["dutyCycle"] = x.get_duty_cycle();
        j["effectiveFrequency"] = x.get_effective_frequency();
        j["label"] = x.get_label();
        j["negativePeak"] = x.get_negative_peak();
        j["offset"] = x.get_offset();
        j["peak"] = x.get_peak();
        j["peakToPeak"] = x.get_peak_to_peak();
        j["phase"] = x.get_phase();
        j["positivePeak"] = x.get_positive_peak();
        j["rms"] = x.get_rms();
        j["thd"] = x.get_thd();
    }

    inline void from_json(const json & j, Waveform& x) {
        x.set_data(j.at("data").get<std::vector<double>>());
        x.set_number_periods(get_stack_optional<int64_t>(j, "numberPeriods"));
        x.set_ancillary_label(get_stack_optional<WaveformLabel>(j, "ancillaryLabel"));
        x.set_time(get_stack_optional<std::vector<double>>(j, "time"));
    }

    inline void to_json(json & j, const Waveform & x) {
        j = json::object();
        j["data"] = x.get_data();
        j["numberPeriods"] = x.get_number_periods();
        j["ancillaryLabel"] = x.get_ancillary_label();
        j["time"] = x.get_time();
    }

    inline void from_json(const json & j, SignalDescriptor& x) {
        x.set_harmonics(get_stack_optional<Harmonics>(j, "harmonics"));
        x.set_processed(get_stack_optional<Processed>(j, "processed"));
        x.set_waveform(get_stack_optional<Waveform>(j, "waveform"));
    }

    inline void to_json(json & j, const SignalDescriptor & x) {
        j = json::object();
        j["harmonics"] = x.get_harmonics();
        j["processed"] = x.get_processed();
        j["waveform"] = x.get_waveform();
    }

    inline void from_json(const json & j, OperatingPointExcitation& x) {
        x.set_current(get_stack_optional<SignalDescriptor>(j, "current"));
        x.set_frequency(j.at("frequency").get<double>());
        x.set_magnetic_field_strength(get_stack_optional<SignalDescriptor>(j, "magneticFieldStrength"));
        x.set_magnetic_flux_density(get_stack_optional<SignalDescriptor>(j, "magneticFluxDensity"));
        x.set_magnetizing_current(get_stack_optional<SignalDescriptor>(j, "magnetizingCurrent"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_voltage(get_stack_optional<SignalDescriptor>(j, "voltage"));
    }

    inline void to_json(json & j, const OperatingPointExcitation & x) {
        j = json::object();
        j["current"] = x.get_current();
        j["frequency"] = x.get_frequency();
        j["magneticFieldStrength"] = x.get_magnetic_field_strength();
        j["magneticFluxDensity"] = x.get_magnetic_flux_density();
        j["magnetizingCurrent"] = x.get_magnetizing_current();
        j["name"] = x.get_name();
        j["voltage"] = x.get_voltage();
    }

    inline void from_json(const json & j, OperatingPoint& x) {
        x.set_conditions(j.at("conditions").get<OperatingConditions>());
        x.set_excitations_per_winding(j.at("excitationsPerWinding").get<std::vector<OperatingPointExcitation>>());
        x.set_name(get_stack_optional<std::string>(j, "name"));
    }

    inline void to_json(json & j, const OperatingPoint & x) {
        j = json::object();
        j["conditions"] = x.get_conditions();
        j["excitationsPerWinding"] = x.get_excitations_per_winding();
        j["name"] = x.get_name();
    }

    inline void from_json(const json & j, Inputs& x) {
        x.set_converter_information(get_stack_optional<ConverterInformation>(j, "converterInformation"));
        x.set_design_requirements(j.at("designRequirements").get<DesignRequirements>());
        x.set_operating_points(j.at("operatingPoints").get<std::vector<OperatingPoint>>());
    }

    inline void to_json(json & j, const Inputs & x) {
        j = json::object();
        j["converterInformation"] = x.get_converter_information();
        j["designRequirements"] = x.get_design_requirements();
        j["operatingPoints"] = x.get_operating_points();
    }

    inline void from_json(const json & j, Cost& x) {
        x.set_currency(j.at("currency").get<std::string>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const Cost & x) {
        j = json::object();
        j["currency"] = x.get_currency();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, DistributorInfo& x) {
        x.set_cost(get_stack_optional<Cost>(j, "cost"));
        x.set_country(get_stack_optional<std::string>(j, "country"));
        x.set_distributed_area(get_stack_optional<std::string>(j, "distributedArea"));
        x.set_email(get_stack_optional<std::string>(j, "email"));
        x.set_link(get_stack_optional<std::string>(j, "link"));
        x.set_name(j.at("name").get<std::string>());
        x.set_phone(get_stack_optional<std::string>(j, "phone"));
        x.set_quantity(j.at("quantity").get<double>());
        x.set_reference(j.at("reference").get<std::string>());
        x.set_updated_at(get_stack_optional<std::string>(j, "updatedAt"));
    }

    inline void to_json(json & j, const DistributorInfo & x) {
        j = json::object();
        j["cost"] = x.get_cost();
        j["country"] = x.get_country();
        j["distributedArea"] = x.get_distributed_area();
        j["email"] = x.get_email();
        j["link"] = x.get_link();
        j["name"] = x.get_name();
        j["phone"] = x.get_phone();
        j["quantity"] = x.get_quantity();
        j["reference"] = x.get_reference();
        j["updatedAt"] = x.get_updated_at();
    }

    inline void from_json(const json & j, PinWindingConnection& x) {
        x.set_pin(get_stack_optional<std::string>(j, "pin"));
        x.set_winding(get_stack_optional<std::string>(j, "winding"));
    }

    inline void to_json(json & j, const PinWindingConnection & x) {
        j = json::object();
        j["pin"] = x.get_pin();
        j["winding"] = x.get_winding();
    }

    inline void from_json(const json & j, DielectricStrengthElement& x) {
        x.set_humidity(get_stack_optional<double>(j, "humidity"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_thickness(get_stack_optional<double>(j, "thickness"));
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const DielectricStrengthElement & x) {
        j = json::object();
        j["humidity"] = x.get_humidity();
        j["temperature"] = x.get_temperature();
        j["thickness"] = x.get_thickness();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, ManufacturerInfo& x) {
        x.set_cost(get_stack_optional<Cost>(j, "cost"));
        x.set_datasheet_url(get_stack_optional<std::string>(j, "datasheetUrl"));
        x.set_description(get_stack_optional<std::string>(j, "description"));
        x.set_family(get_stack_optional<std::string>(j, "family"));
        x.set_irdi(get_stack_optional<std::string>(j, "irdi"));
        x.set_name(j.at("name").get<std::string>());
        x.set_order_code(get_stack_optional<std::string>(j, "orderCode"));
        x.set_reference(get_stack_optional<std::string>(j, "reference"));
        x.set_status(get_stack_optional<Status>(j, "status"));
    }

    inline void to_json(json & j, const ManufacturerInfo & x) {
        j = json::object();
        j["cost"] = x.get_cost();
        j["datasheetUrl"] = x.get_datasheet_url();
        j["description"] = x.get_description();
        j["family"] = x.get_family();
        j["irdi"] = x.get_irdi();
        j["name"] = x.get_name();
        j["orderCode"] = x.get_order_code();
        j["reference"] = x.get_reference();
        j["status"] = x.get_status();
    }

    inline void from_json(const json & j, ResistivityPoint& x) {
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const ResistivityPoint & x) {
        j = json::object();
        j["temperature"] = x.get_temperature();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, InsulationMaterial& x) {
        x.set_aliases(get_stack_optional<std::vector<std::string>>(j, "aliases"));
        x.set_composition(get_stack_optional<std::string>(j, "composition"));
        x.set_dielectric_strength(j.at("dielectricStrength").get<std::vector<DielectricStrengthElement>>());
        x.set_manufacturer_info(get_stack_optional<ManufacturerInfo>(j, "manufacturerInfo"));
        x.set_melting_point(get_stack_optional<double>(j, "meltingPoint"));
        x.set_name(j.at("name").get<std::string>());
        x.set_relative_permittivity(get_stack_optional<double>(j, "relativePermittivity"));
        x.set_resistivity(get_stack_optional<std::vector<ResistivityPoint>>(j, "resistivity"));
        x.set_specific_heat(get_stack_optional<double>(j, "specificHeat"));
        x.set_surface_resistivity(get_stack_optional<std::vector<ResistivityPoint>>(j, "surfaceResistivity"));
        x.set_temperature_class(get_stack_optional<std::variant<double, TemperatureClassEnum>>(j, "temperatureClass"));
        x.set_thermal_conductivity(get_stack_optional<double>(j, "thermalConductivity"));
    }

    inline void to_json(json & j, const InsulationMaterial & x) {
        j = json::object();
        j["aliases"] = x.get_aliases();
        j["composition"] = x.get_composition();
        j["dielectricStrength"] = x.get_dielectric_strength();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["meltingPoint"] = x.get_melting_point();
        j["name"] = x.get_name();
        j["relativePermittivity"] = x.get_relative_permittivity();
        j["resistivity"] = x.get_resistivity();
        j["specificHeat"] = x.get_specific_heat();
        j["surfaceResistivity"] = x.get_surface_resistivity();
        j["temperatureClass"] = x.get_temperature_class();
        j["thermalConductivity"] = x.get_thermal_conductivity();
    }

    inline void from_json(const json & j, Pin& x) {
        x.set_coordinates(get_stack_optional<std::vector<double>>(j, "coordinates"));
        x.set_dimensions(j.at("dimensions").get<std::vector<double>>());
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_rotation(get_stack_optional<std::vector<double>>(j, "rotation"));
        x.set_shape(j.at("shape").get<PinShape>());
        x.set_type(j.at("type").get<PinDescriptionType>());
    }

    inline void to_json(json & j, const Pin & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["dimensions"] = x.get_dimensions();
        j["name"] = x.get_name();
        j["rotation"] = x.get_rotation();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, Pinout& x) {
        x.set_central_pitch(get_stack_optional<double>(j, "centralPitch"));
        x.set_number_pins(j.at("numberPins").get<int64_t>());
        x.set_number_pins_per_row(get_stack_optional<std::vector<int64_t>>(j, "numberPinsPerRow"));
        x.set_number_rows(get_stack_optional<int64_t>(j, "numberRows"));
        x.set_pin_description(j.at("pinDescription").get<Pin>());
        x.set_pitch(j.at("pitch").get<Pitch>());
        x.set_row_distance(j.at("rowDistance").get<double>());
    }

    inline void to_json(json & j, const Pinout & x) {
        j = json::object();
        j["centralPitch"] = x.get_central_pitch();
        j["numberPins"] = x.get_number_pins();
        j["numberPinsPerRow"] = x.get_number_pins_per_row();
        j["numberRows"] = x.get_number_rows();
        j["pinDescription"] = x.get_pin_description();
        j["pitch"] = x.get_pitch();
        j["rowDistance"] = x.get_row_distance();
    }

    inline void from_json(const json & j, BobbinFunctionalDescription& x) {
        x.set_connections(get_stack_optional<std::vector<PinWindingConnection>>(j, "connections"));
        x.set_dimensions(j.at("dimensions").get<std::map<std::string, Dimension>>());
        x.set_family(j.at("family").get<BobbinFamily>());
        x.set_family_subtype(get_stack_optional<std::string>(j, "familySubtype"));
        x.set_material(get_stack_optional<std::variant<InsulationMaterial, std::string>>(j, "material"));
        x.set_pinout(get_stack_optional<Pinout>(j, "pinout"));
        x.set_shape(j.at("shape").get<std::string>());
        x.set_type(j.at("type").get<FunctionalDescriptionType>());
    }

    inline void to_json(json & j, const BobbinFunctionalDescription & x) {
        j = json::object();
        j["connections"] = x.get_connections();
        j["dimensions"] = x.get_dimensions();
        j["family"] = x.get_family();
        j["familySubtype"] = x.get_family_subtype();
        j["material"] = x.get_material();
        j["pinout"] = x.get_pinout();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, WindingWindowElement& x) {
        x.set_area(get_stack_optional<double>(j, "area"));
        x.set_coordinates(get_stack_optional<std::vector<double>>(j, "coordinates"));
        x.set_height(get_stack_optional<double>(j, "height"));
        x.set_sections_alignment(get_stack_optional<CoilAlignment>(j, "sectionsAlignment"));
        x.set_sections_orientation(get_stack_optional<WindingOrientation>(j, "sectionsOrientation"));
        x.set_shape(get_stack_optional<WindingWindowShape>(j, "shape"));
        x.set_width(get_stack_optional<double>(j, "width"));
        x.set_angle(get_stack_optional<double>(j, "angle"));
        x.set_radial_height(get_stack_optional<double>(j, "radialHeight"));
    }

    inline void to_json(json & j, const WindingWindowElement & x) {
        j = json::object();
        j["area"] = x.get_area();
        j["coordinates"] = x.get_coordinates();
        j["height"] = x.get_height();
        j["sectionsAlignment"] = x.get_sections_alignment();
        j["sectionsOrientation"] = x.get_sections_orientation();
        j["shape"] = x.get_shape();
        j["width"] = x.get_width();
        j["angle"] = x.get_angle();
        j["radialHeight"] = x.get_radial_height();
    }

    inline void from_json(const json & j, CoreBobbinProcessedDescription& x) {
        x.set_column_depth(j.at("columnDepth").get<double>());
        x.set_column_shape(j.at("columnShape").get<ColumnShape>());
        x.set_column_thickness(j.at("columnThickness").get<double>());
        x.set_column_width(get_stack_optional<double>(j, "columnWidth"));
        x.set_coordinates(get_stack_optional<std::vector<double>>(j, "coordinates"));
        x.set_pins(get_stack_optional<std::vector<Pin>>(j, "pins"));
        x.set_wall_thickness(j.at("wallThickness").get<double>());
        x.set_winding_windows(j.at("windingWindows").get<std::vector<WindingWindowElement>>());
    }

    inline void to_json(json & j, const CoreBobbinProcessedDescription & x) {
        j = json::object();
        j["columnDepth"] = x.get_column_depth();
        j["columnShape"] = x.get_column_shape();
        j["columnThickness"] = x.get_column_thickness();
        j["columnWidth"] = x.get_column_width();
        j["coordinates"] = x.get_coordinates();
        j["pins"] = x.get_pins();
        j["wallThickness"] = x.get_wall_thickness();
        j["windingWindows"] = x.get_winding_windows();
    }

    inline void from_json(const json & j, Bobbin& x) {
        x.set_distributors_info(get_stack_optional<std::vector<DistributorInfo>>(j, "distributorsInfo"));
        x.set_functional_description(get_stack_optional<BobbinFunctionalDescription>(j, "functionalDescription"));
        x.set_manufacturer_info(get_stack_optional<ManufacturerInfo>(j, "manufacturerInfo"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_processed_description(get_stack_optional<CoreBobbinProcessedDescription>(j, "processedDescription"));
    }

    inline void to_json(json & j, const Bobbin & x) {
        j = json::object();
        j["distributorsInfo"] = x.get_distributors_info();
        j["functionalDescription"] = x.get_functional_description();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["name"] = x.get_name();
        j["processedDescription"] = x.get_processed_description();
    }

    inline void from_json(const json & j, ConnectionElement& x) {
        x.set_direction(get_stack_optional<Direction>(j, "direction"));
        x.set_length(get_stack_optional<double>(j, "length"));
        x.set_metric(get_stack_optional<int64_t>(j, "metric"));
        x.set_pin_name(get_stack_optional<std::string>(j, "pinName"));
        x.set_type(get_stack_optional<ConnectionType>(j, "type"));
    }

    inline void to_json(json & j, const ConnectionElement & x) {
        j = json::object();
        j["direction"] = x.get_direction();
        j["length"] = x.get_length();
        j["metric"] = x.get_metric();
        j["pinName"] = x.get_pin_name();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, InsulationWireCoating& x) {
        x.set_breakdown_voltage(get_stack_optional<double>(j, "breakdownVoltage"));
        x.set_grade(get_stack_optional<int64_t>(j, "grade"));
        x.set_material(get_stack_optional<std::variant<InsulationMaterial, std::string>>(j, "material"));
        x.set_number_layers(get_stack_optional<int64_t>(j, "numberLayers"));
        x.set_temperature_rating(get_stack_optional<double>(j, "temperatureRating"));
        x.set_thickness(get_stack_optional<DimensionWithTolerance>(j, "thickness"));
        x.set_thickness_layers(get_stack_optional<double>(j, "thicknessLayers"));
        x.set_type(get_stack_optional<InsulationWireCoatingType>(j, "type"));
    }

    inline void to_json(json & j, const InsulationWireCoating & x) {
        j = json::object();
        j["breakdownVoltage"] = x.get_breakdown_voltage();
        j["grade"] = x.get_grade();
        j["material"] = x.get_material();
        j["numberLayers"] = x.get_number_layers();
        j["temperatureRating"] = x.get_temperature_rating();
        j["thickness"] = x.get_thickness();
        j["thicknessLayers"] = x.get_thickness_layers();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, Resistivity& x) {
        x.set_reference_temperature(j.at("referenceTemperature").get<double>());
        x.set_reference_value(j.at("referenceValue").get<double>());
        x.set_temperature_coefficient(j.at("temperatureCoefficient").get<double>());
    }

    inline void to_json(json & j, const Resistivity & x) {
        j = json::object();
        j["referenceTemperature"] = x.get_reference_temperature();
        j["referenceValue"] = x.get_reference_value();
        j["temperatureCoefficient"] = x.get_temperature_coefficient();
    }

    inline void from_json(const json & j, ThermalConductivityElement& x) {
        x.set_temperature(j.at("temperature").get<double>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const ThermalConductivityElement & x) {
        j = json::object();
        j["temperature"] = x.get_temperature();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, WireMaterial& x) {
        x.set_name(j.at("name").get<std::string>());
        x.set_permeability(j.at("permeability").get<double>());
        x.set_resistivity(j.at("resistivity").get<Resistivity>());
        x.set_thermal_conductivity(get_stack_optional<std::vector<ThermalConductivityElement>>(j, "thermalConductivity"));
    }

    inline void to_json(json & j, const WireMaterial & x) {
        j = json::object();
        j["name"] = x.get_name();
        j["permeability"] = x.get_permeability();
        j["resistivity"] = x.get_resistivity();
        j["thermalConductivity"] = x.get_thermal_conductivity();
    }

    inline void from_json(const json & j, WireRound& x) {
        x.set_conducting_diameter(j.at("conductingDiameter").get<DimensionWithTolerance>());
        x.set_material(get_stack_optional<std::variant<WireMaterial, std::string>>(j, "material"));
        x.set_outer_diameter(get_stack_optional<DimensionWithTolerance>(j, "outerDiameter"));
        x.set_coating(get_stack_optional<std::variant<InsulationWireCoating, std::string>>(j, "coating"));
        x.set_conducting_area(get_stack_optional<DimensionWithTolerance>(j, "conductingArea"));
        x.set_manufacturer_info(get_stack_optional<ManufacturerInfo>(j, "manufacturerInfo"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_number_conductors(get_stack_optional<int64_t>(j, "numberConductors"));
        x.set_standard(get_stack_optional<WireStandard>(j, "standard"));
        x.set_standard_name(get_stack_optional<std::string>(j, "standardName"));
        x.set_type(j.at("type").get<WireType>());
    }

    inline void to_json(json & j, const WireRound & x) {
        j = json::object();
        j["conductingDiameter"] = x.get_conducting_diameter();
        j["material"] = x.get_material();
        j["outerDiameter"] = x.get_outer_diameter();
        j["coating"] = x.get_coating();
        j["conductingArea"] = x.get_conducting_area();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["name"] = x.get_name();
        j["numberConductors"] = x.get_number_conductors();
        j["standard"] = x.get_standard();
        j["standardName"] = x.get_standard_name();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, Wire& x) {
        x.set_conducting_diameter(get_stack_optional<DimensionWithTolerance>(j, "conductingDiameter"));
        x.set_material(get_stack_optional<std::variant<WireMaterial, std::string>>(j, "material"));
        x.set_outer_diameter(get_stack_optional<DimensionWithTolerance>(j, "outerDiameter"));
        x.set_coating(get_stack_optional<std::variant<InsulationWireCoating, std::string>>(j, "coating"));
        x.set_conducting_area(get_stack_optional<DimensionWithTolerance>(j, "conductingArea"));
        x.set_manufacturer_info(get_stack_optional<ManufacturerInfo>(j, "manufacturerInfo"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_number_conductors(get_stack_optional<int64_t>(j, "numberConductors"));
        x.set_standard(get_stack_optional<WireStandard>(j, "standard"));
        x.set_standard_name(get_stack_optional<std::string>(j, "standardName"));
        x.set_type(j.at("type").get<WireType>());
        x.set_conducting_height(get_stack_optional<DimensionWithTolerance>(j, "conductingHeight"));
        x.set_conducting_width(get_stack_optional<DimensionWithTolerance>(j, "conductingWidth"));
        x.set_outer_height(get_stack_optional<DimensionWithTolerance>(j, "outerHeight"));
        x.set_outer_width(get_stack_optional<DimensionWithTolerance>(j, "outerWidth"));
        x.set_edge_radius(get_stack_optional<DimensionWithTolerance>(j, "edgeRadius"));
        x.set_strand(get_stack_optional<std::variant<WireRound, std::string>>(j, "strand"));
        x.set_twist_pitch(get_stack_optional<DimensionWithTolerance>(j, "twistPitch"));
    }

    inline void to_json(json & j, const Wire & x) {
        j = json::object();
        j["conductingDiameter"] = x.get_conducting_diameter();
        j["material"] = x.get_material();
        j["outerDiameter"] = x.get_outer_diameter();
        j["coating"] = x.get_coating();
        j["conductingArea"] = x.get_conducting_area();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["name"] = x.get_name();
        j["numberConductors"] = x.get_number_conductors();
        j["standard"] = x.get_standard();
        j["standardName"] = x.get_standard_name();
        j["type"] = x.get_type();
        j["conductingHeight"] = x.get_conducting_height();
        j["conductingWidth"] = x.get_conducting_width();
        j["outerHeight"] = x.get_outer_height();
        j["outerWidth"] = x.get_outer_width();
        j["edgeRadius"] = x.get_edge_radius();
        j["strand"] = x.get_strand();
        j["twistPitch"] = x.get_twist_pitch();
    }

    inline void from_json(const json & j, CoilFunctionalDescription& x) {
        x.set_connections(get_stack_optional<std::vector<ConnectionElement>>(j, "connections"));
        x.set_isolation_side(j.at("isolationSide").get<IsolationSide>());
        x.set_name(j.at("name").get<std::string>());
        x.set_number_parallels(j.at("numberParallels").get<int64_t>());
        x.set_number_turns(j.at("numberTurns").get<int64_t>());
        x.set_wire(j.at("wire").get<WireDataOrNameUnion>());
        x.set_wound_with(get_stack_optional<std::vector<std::string>>(j, "woundWith"));
    }

    inline void to_json(json & j, const CoilFunctionalDescription & x) {
        j = json::object();
        j["connections"] = x.get_connections();
        j["isolationSide"] = x.get_isolation_side();
        j["name"] = x.get_name();
        j["numberParallels"] = x.get_number_parallels();
        j["numberTurns"] = x.get_number_turns();
        j["wire"] = x.get_wire();
        j["woundWith"] = x.get_wound_with();
    }

    inline void from_json(const json & j, PartialWinding& x) {
        x.set_connections(get_stack_optional<std::vector<ConnectionElement>>(j, "connections"));
        x.set_parallels_proportion(j.at("parallelsProportion").get<std::vector<double>>());
        x.set_winding(j.at("winding").get<std::string>());
    }

    inline void to_json(json & j, const PartialWinding & x) {
        j = json::object();
        j["connections"] = x.get_connections();
        j["parallelsProportion"] = x.get_parallels_proportion();
        j["winding"] = x.get_winding();
    }

    inline void from_json(const json & j, Group& x) {
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_coordinate_system(get_stack_optional<CoordinateSystem>(j, "coordinateSystem"));
        x.set_dimensions(j.at("dimensions").get<std::vector<double>>());
        x.set_name(j.at("name").get<std::string>());
        x.set_partial_windings(j.at("partialWindings").get<std::vector<PartialWinding>>());
        x.set_sections_orientation(j.at("sectionsOrientation").get<WindingOrientation>());
        x.set_type(j.at("type").get<WiringTechnology>());
    }

    inline void to_json(json & j, const Group & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["coordinateSystem"] = x.get_coordinate_system();
        j["dimensions"] = x.get_dimensions();
        j["name"] = x.get_name();
        j["partialWindings"] = x.get_partial_windings();
        j["sectionsOrientation"] = x.get_sections_orientation();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, Layer& x) {
        x.set_additional_coordinates(get_stack_optional<std::vector<std::vector<double>>>(j, "additionalCoordinates"));
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_coordinate_system(get_stack_optional<CoordinateSystem>(j, "coordinateSystem"));
        x.set_dimensions(j.at("dimensions").get<std::vector<double>>());
        x.set_filling_factor(get_stack_optional<double>(j, "fillingFactor"));
        x.set_insulation_material(get_stack_optional<std::variant<InsulationMaterial, std::string>>(j, "insulationMaterial"));
        x.set_name(j.at("name").get<std::string>());
        x.set_orientation(j.at("orientation").get<WindingOrientation>());
        x.set_partial_windings(j.at("partialWindings").get<std::vector<PartialWinding>>());
        x.set_section(get_stack_optional<std::string>(j, "section"));
        x.set_turns_alignment(get_stack_optional<CoilAlignment>(j, "turnsAlignment"));
        x.set_type(j.at("type").get<ElectricalType>());
        x.set_winding_style(get_stack_optional<WindingStyle>(j, "windingStyle"));
    }

    inline void to_json(json & j, const Layer & x) {
        j = json::object();
        j["additionalCoordinates"] = x.get_additional_coordinates();
        j["coordinates"] = x.get_coordinates();
        j["coordinateSystem"] = x.get_coordinate_system();
        j["dimensions"] = x.get_dimensions();
        j["fillingFactor"] = x.get_filling_factor();
        j["insulationMaterial"] = x.get_insulation_material();
        j["name"] = x.get_name();
        j["orientation"] = x.get_orientation();
        j["partialWindings"] = x.get_partial_windings();
        j["section"] = x.get_section();
        j["turnsAlignment"] = x.get_turns_alignment();
        j["type"] = x.get_type();
        j["windingStyle"] = x.get_winding_style();
    }

    inline void from_json(const json & j, MarginInfo& x) {
        x.set_bottom_or_right_width(j.at("bottomOrRightWidth").get<double>());
        x.set_insulation_material(get_stack_optional<std::variant<InsulationMaterial, std::string>>(j, "insulationMaterial"));
        x.set_layer_thickness(j.at("layerThickness").get<double>());
        x.set_number_layers(j.at("numberLayers").get<int64_t>());
        x.set_top_or_left_width(j.at("topOrLeftWidth").get<double>());
    }

    inline void to_json(json & j, const MarginInfo & x) {
        j = json::object();
        j["bottomOrRightWidth"] = x.get_bottom_or_right_width();
        j["insulationMaterial"] = x.get_insulation_material();
        j["layerThickness"] = x.get_layer_thickness();
        j["numberLayers"] = x.get_number_layers();
        j["topOrLeftWidth"] = x.get_top_or_left_width();
    }

    inline void from_json(const json & j, Section& x) {
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_coordinate_system(get_stack_optional<CoordinateSystem>(j, "coordinateSystem"));
        x.set_dimensions(j.at("dimensions").get<std::vector<double>>());
        x.set_filling_factor(get_stack_optional<double>(j, "fillingFactor"));
        x.set_group(get_stack_optional<std::string>(j, "group"));
        x.set_layers_alignment(get_stack_optional<CoilAlignment>(j, "layersAlignment"));
        x.set_layers_orientation(j.at("layersOrientation").get<WindingOrientation>());
        x.set_margin(get_stack_optional<std::variant<std::vector<double>, MarginInfo>>(j, "margin"));
        x.set_name(j.at("name").get<std::string>());
        x.set_number_layers(get_stack_optional<double>(j, "numberLayers"));
        x.set_partial_windings(j.at("partialWindings").get<std::vector<PartialWinding>>());
        x.set_type(j.at("type").get<ElectricalType>());
        x.set_winding_style(get_stack_optional<WindingStyle>(j, "windingStyle"));
    }

    inline void to_json(json & j, const Section & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["coordinateSystem"] = x.get_coordinate_system();
        j["dimensions"] = x.get_dimensions();
        j["fillingFactor"] = x.get_filling_factor();
        j["group"] = x.get_group();
        j["layersAlignment"] = x.get_layers_alignment();
        j["layersOrientation"] = x.get_layers_orientation();
        j["margin"] = x.get_margin();
        j["name"] = x.get_name();
        j["numberLayers"] = x.get_number_layers();
        j["partialWindings"] = x.get_partial_windings();
        j["type"] = x.get_type();
        j["windingStyle"] = x.get_winding_style();
    }

    inline void from_json(const json & j, Turn& x) {
        x.set_additional_coordinates(get_stack_optional<std::vector<std::vector<double>>>(j, "additionalCoordinates"));
        x.set_angle(get_stack_optional<double>(j, "angle"));
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_coordinate_system(get_stack_optional<CoordinateSystem>(j, "coordinateSystem"));
        x.set_cross_sectional_shape(get_stack_optional<TurnCrossSectionalShape>(j, "crossSectionalShape"));
        x.set_dimensions(get_stack_optional<std::vector<double>>(j, "dimensions"));
        x.set_layer(get_stack_optional<std::string>(j, "layer"));
        x.set_length(j.at("length").get<double>());
        x.set_name(j.at("name").get<std::string>());
        x.set_orientation(get_stack_optional<TurnOrientation>(j, "orientation"));
        x.set_parallel(j.at("parallel").get<int64_t>());
        x.set_rotation(get_stack_optional<double>(j, "rotation"));
        x.set_section(get_stack_optional<std::string>(j, "section"));
        x.set_winding(j.at("winding").get<std::string>());
    }

    inline void to_json(json & j, const Turn & x) {
        j = json::object();
        j["additionalCoordinates"] = x.get_additional_coordinates();
        j["angle"] = x.get_angle();
        j["coordinates"] = x.get_coordinates();
        j["coordinateSystem"] = x.get_coordinate_system();
        j["crossSectionalShape"] = x.get_cross_sectional_shape();
        j["dimensions"] = x.get_dimensions();
        j["layer"] = x.get_layer();
        j["length"] = x.get_length();
        j["name"] = x.get_name();
        j["orientation"] = x.get_orientation();
        j["parallel"] = x.get_parallel();
        j["rotation"] = x.get_rotation();
        j["section"] = x.get_section();
        j["winding"] = x.get_winding();
    }

    inline void from_json(const json & j, Coil& x) {
        x.set_bobbin(j.at("bobbin").get<BobbinDataOrNameUnion>());
        x.set_functional_description(j.at("functionalDescription").get<std::vector<CoilFunctionalDescription>>());
        x.set_groups_description(get_stack_optional<std::vector<Group>>(j, "groupsDescription"));
        x.set_layers_description(get_stack_optional<std::vector<Layer>>(j, "layersDescription"));
        x.set_sections_description(get_stack_optional<std::vector<Section>>(j, "sectionsDescription"));
        x.set_turns_description(get_stack_optional<std::vector<Turn>>(j, "turnsDescription"));
    }

    inline void to_json(json & j, const Coil & x) {
        j = json::object();
        j["bobbin"] = x.get_bobbin();
        j["functionalDescription"] = x.get_functional_description();
        j["groupsDescription"] = x.get_groups_description();
        j["layersDescription"] = x.get_layers_description();
        j["sectionsDescription"] = x.get_sections_description();
        j["turnsDescription"] = x.get_turns_description();
    }

    inline void from_json(const json & j, CoreGap& x) {
        x.set_area(get_stack_optional<double>(j, "area"));
        x.set_coordinates(get_stack_optional<std::vector<double>>(j, "coordinates"));
        x.set_distance_closest_normal_surface(get_stack_optional<double>(j, "distanceClosestNormalSurface"));
        x.set_distance_closest_parallel_surface(get_stack_optional<double>(j, "distanceClosestParallelSurface"));
        x.set_length(j.at("length").get<double>());
        x.set_section_dimensions(get_stack_optional<std::vector<double>>(j, "sectionDimensions"));
        x.set_shape(get_stack_optional<ColumnShape>(j, "shape"));
        x.set_type(j.at("type").get<GapType>());
    }

    inline void to_json(json & j, const CoreGap & x) {
        j = json::object();
        j["area"] = x.get_area();
        j["coordinates"] = x.get_coordinates();
        j["distanceClosestNormalSurface"] = x.get_distance_closest_normal_surface();
        j["distanceClosestParallelSurface"] = x.get_distance_closest_parallel_surface();
        j["length"] = x.get_length();
        j["sectionDimensions"] = x.get_section_dimensions();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, BhCycleElement& x) {
        x.set_magnetic_field(j.at("magneticField").get<double>());
        x.set_magnetic_flux_density(j.at("magneticFluxDensity").get<double>());
        x.set_temperature(j.at("temperature").get<double>());
    }

    inline void to_json(json & j, const BhCycleElement & x) {
        j = json::object();
        j["magneticField"] = x.get_magnetic_field();
        j["magneticFluxDensity"] = x.get_magnetic_flux_density();
        j["temperature"] = x.get_temperature();
    }

    inline void from_json(const json & j, MassLossesPoint& x) {
        x.set_magnetic_flux_density(j.at("magneticFluxDensity").get<OperatingPointExcitation>());
        x.set_origin(j.at("origin").get<std::string>());
        x.set_temperature(j.at("temperature").get<double>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const MassLossesPoint & x) {
        j = json::object();
        j["magneticFluxDensity"] = x.get_magnetic_flux_density();
        j["origin"] = x.get_origin();
        j["temperature"] = x.get_temperature();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, MagnetecCoreLossesMethodData& x) {
        x.set_method(j.at("method").get<MassCoreLossesMethodType>());
    }

    inline void to_json(json & j, const MagnetecCoreLossesMethodData & x) {
        j = json::object();
        j["method"] = x.get_method();
    }

    inline void from_json(const json & j, FrequencyFactor& x) {
        x.set_a(j.at("a").get<double>());
        x.set_b(j.at("b").get<double>());
        x.set_c(j.at("c").get<double>());
        x.set_d(j.at("d").get<double>());
        x.set_e(get_stack_optional<double>(j, "e"));
    }

    inline void to_json(json & j, const FrequencyFactor & x) {
        j = json::object();
        j["a"] = x.get_a();
        j["b"] = x.get_b();
        j["c"] = x.get_c();
        j["d"] = x.get_d();
        j["e"] = x.get_e();
    }

    inline void from_json(const json & j, MagneticFieldDcBiasFactor& x) {
        x.set_a(j.at("a").get<double>());
        x.set_b(j.at("b").get<double>());
        x.set_c(j.at("c").get<double>());
        x.set_d(get_stack_optional<double>(j, "d"));
    }

    inline void to_json(json & j, const MagneticFieldDcBiasFactor & x) {
        j = json::object();
        j["a"] = x.get_a();
        j["b"] = x.get_b();
        j["c"] = x.get_c();
        j["d"] = x.get_d();
    }

    inline void from_json(const json & j, MagneticFluxDensityFactor& x) {
        x.set_a(j.at("a").get<double>());
        x.set_b(j.at("b").get<double>());
        x.set_c(j.at("c").get<double>());
        x.set_d(j.at("d").get<double>());
        x.set_e(j.at("e").get<double>());
        x.set_f(j.at("f").get<double>());
    }

    inline void to_json(json & j, const MagneticFluxDensityFactor & x) {
        j = json::object();
        j["a"] = x.get_a();
        j["b"] = x.get_b();
        j["c"] = x.get_c();
        j["d"] = x.get_d();
        j["e"] = x.get_e();
        j["f"] = x.get_f();
    }

    inline void from_json(const json & j, TemperatureFactor& x) {
        x.set_a(j.at("a").get<double>());
        x.set_b(get_stack_optional<double>(j, "b"));
        x.set_c(get_stack_optional<double>(j, "c"));
        x.set_d(get_stack_optional<double>(j, "d"));
        x.set_e(get_stack_optional<double>(j, "e"));
    }

    inline void to_json(json & j, const TemperatureFactor & x) {
        j = json::object();
        j["a"] = x.get_a();
        j["b"] = x.get_b();
        j["c"] = x.get_c();
        j["d"] = x.get_d();
        j["e"] = x.get_e();
    }

    inline void from_json(const json & j, InitialPermeabilitModifier& x) {
        x.set_frequency_factor(get_stack_optional<FrequencyFactor>(j, "frequencyFactor"));
        x.set_magnetic_field_dc_bias_factor(get_stack_optional<MagneticFieldDcBiasFactor>(j, "magneticFieldDcBiasFactor"));
        x.set_method(get_stack_optional<InitialPermeabilitModifierMethod>(j, "method"));
        x.set_temperature_factor(get_stack_optional<TemperatureFactor>(j, "temperatureFactor"));
        x.set_magnetic_flux_density_factor(get_stack_optional<MagneticFluxDensityFactor>(j, "magneticFluxDensityFactor"));
    }

    inline void to_json(json & j, const InitialPermeabilitModifier & x) {
        j = json::object();
        j["frequencyFactor"] = x.get_frequency_factor();
        j["magneticFieldDcBiasFactor"] = x.get_magnetic_field_dc_bias_factor();
        j["method"] = x.get_method();
        j["temperatureFactor"] = x.get_temperature_factor();
        j["magneticFluxDensityFactor"] = x.get_magnetic_flux_density_factor();
    }

    inline void from_json(const json & j, PermeabilityPoint& x) {
        x.set_frequency(get_stack_optional<double>(j, "frequency"));
        x.set_magnetic_field_dc_bias(get_stack_optional<double>(j, "magneticFieldDcBias"));
        x.set_magnetic_flux_density_peak(get_stack_optional<double>(j, "magneticFluxDensityPeak"));
        x.set_modifiers(get_stack_optional<std::map<std::string, InitialPermeabilitModifier>>(j, "modifiers"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_tolerance(get_stack_optional<double>(j, "tolerance"));
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const PermeabilityPoint & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["magneticFieldDcBias"] = x.get_magnetic_field_dc_bias();
        j["magneticFluxDensityPeak"] = x.get_magnetic_flux_density_peak();
        j["modifiers"] = x.get_modifiers();
        j["temperature"] = x.get_temperature();
        j["tolerance"] = x.get_tolerance();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, ComplexPermeabilityData& x) {
        x.set_imaginary(j.at("imaginary").get<Permeability>());
        x.set_real(j.at("real").get<Permeability>());
    }

    inline void to_json(json & j, const ComplexPermeabilityData & x) {
        j = json::object();
        j["imaginary"] = x.get_imaginary();
        j["real"] = x.get_real();
    }

    inline void from_json(const json & j, Permeabilities& x) {
        x.set_amplitude(get_stack_optional<std::variant<std::vector<PermeabilityPoint>, PermeabilityPoint>>(j, "amplitude"));
        x.set_complex(get_stack_optional<ComplexPermeabilityData>(j, "complex"));
        x.set_incremental(get_stack_optional<std::variant<std::vector<PermeabilityPoint>, PermeabilityPoint>>(j, "incremental"));
        x.set_initial(j.at("initial").get<Permeability>());
        x.set_reversible(get_stack_optional<std::variant<std::vector<PermeabilityPoint>, PermeabilityPoint>>(j, "reversible"));
    }

    inline void to_json(json & j, const Permeabilities & x) {
        j = json::object();
        j["amplitude"] = x.get_amplitude();
        j["complex"] = x.get_complex();
        j["incremental"] = x.get_incremental();
        j["initial"] = x.get_initial();
        j["reversible"] = x.get_reversible();
    }

    inline void from_json(const json & j, CoreMaterialRecommendations& x) {
        x.set_maximum_frequency(get_stack_optional<double>(j, "maximumFrequency"));
        x.set_maximum_magnetic_flux_density(get_stack_optional<double>(j, "maximumMagneticFluxDensity"));
        x.set_maximum_operating_temperature(get_stack_optional<double>(j, "maximumOperatingTemperature"));
        x.set_minimum_frequency(get_stack_optional<double>(j, "minimumFrequency"));
        x.set_typical_applications(get_stack_optional<std::vector<std::string>>(j, "typicalApplications"));
        x.set_typical_topologies(get_stack_optional<std::vector<std::string>>(j, "typicalTopologies"));
    }

    inline void to_json(json & j, const CoreMaterialRecommendations & x) {
        j = json::object();
        j["maximumFrequency"] = x.get_maximum_frequency();
        j["maximumMagneticFluxDensity"] = x.get_maximum_magnetic_flux_density();
        j["maximumOperatingTemperature"] = x.get_maximum_operating_temperature();
        j["minimumFrequency"] = x.get_minimum_frequency();
        j["typicalApplications"] = x.get_typical_applications();
        j["typicalTopologies"] = x.get_typical_topologies();
    }

    inline void from_json(const json & j, VolumetricLossesPoint& x) {
        x.set_magnetic_flux_density(j.at("magneticFluxDensity").get<OperatingPointExcitation>());
        x.set_origin(j.at("origin").get<std::string>());
        x.set_temperature(j.at("temperature").get<double>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const VolumetricLossesPoint & x) {
        j = json::object();
        j["magneticFluxDensity"] = x.get_magnetic_flux_density();
        j["origin"] = x.get_origin();
        j["temperature"] = x.get_temperature();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, RoshenAdditionalCoefficients& x) {
        x.set_excess_losses_coefficient(j.at("excessLossesCoefficient").get<double>());
        x.set_resistivity_frequency_coefficient(j.at("resistivityFrequencyCoefficient").get<double>());
        x.set_resistivity_magnetic_flux_density_coefficient(j.at("resistivityMagneticFluxDensityCoefficient").get<double>());
        x.set_resistivity_offset(j.at("resistivityOffset").get<double>());
        x.set_resistivity_temperature_coefficient(j.at("resistivityTemperatureCoefficient").get<double>());
    }

    inline void to_json(json & j, const RoshenAdditionalCoefficients & x) {
        j = json::object();
        j["excessLossesCoefficient"] = x.get_excess_losses_coefficient();
        j["resistivityFrequencyCoefficient"] = x.get_resistivity_frequency_coefficient();
        j["resistivityMagneticFluxDensityCoefficient"] = x.get_resistivity_magnetic_flux_density_coefficient();
        j["resistivityOffset"] = x.get_resistivity_offset();
        j["resistivityTemperatureCoefficient"] = x.get_resistivity_temperature_coefficient();
    }

    inline void from_json(const json & j, LossFactorPoint& x) {
        x.set_frequency(get_stack_optional<double>(j, "frequency"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const LossFactorPoint & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["temperature"] = x.get_temperature();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, SteinmetzCoreLossesMethodRangeDatum& x) {
        x.set_alpha(j.at("alpha").get<double>());
        x.set_beta(j.at("beta").get<double>());
        x.set_ct0(get_stack_optional<double>(j, "ct0"));
        x.set_ct1(get_stack_optional<double>(j, "ct1"));
        x.set_ct2(get_stack_optional<double>(j, "ct2"));
        x.set_k(j.at("k").get<double>());
        x.set_maximum_frequency(get_stack_optional<double>(j, "maximumFrequency"));
        x.set_minimum_frequency(get_stack_optional<double>(j, "minimumFrequency"));
    }

    inline void to_json(json & j, const SteinmetzCoreLossesMethodRangeDatum & x) {
        j = json::object();
        j["alpha"] = x.get_alpha();
        j["beta"] = x.get_beta();
        j["ct0"] = x.get_ct0();
        j["ct1"] = x.get_ct1();
        j["ct2"] = x.get_ct2();
        j["k"] = x.get_k();
        j["maximumFrequency"] = x.get_maximum_frequency();
        j["minimumFrequency"] = x.get_minimum_frequency();
    }

    inline void from_json(const json & j, CoreLossesMethodData& x) {
        x.set_method(j.at("method").get<VolumetricCoreLossesMethodType>());
        x.set_ranges(get_stack_optional<std::vector<SteinmetzCoreLossesMethodRangeDatum>>(j, "ranges"));
        x.set_coefficients(get_stack_optional<RoshenAdditionalCoefficients>(j, "coefficients"));
        x.set_reference_volumetric_losses(get_stack_optional<std::vector<VolumetricLossesPoint>>(j, "referenceVolumetricLosses"));
        x.set_a(get_stack_optional<double>(j, "a"));
        x.set_b(get_stack_optional<double>(j, "b"));
        x.set_c(get_stack_optional<double>(j, "c"));
        x.set_d(get_stack_optional<double>(j, "d"));
        x.set_factors(get_stack_optional<std::vector<LossFactorPoint>>(j, "factors"));
    }

    inline void to_json(json & j, const CoreLossesMethodData & x) {
        j = json::object();
        j["method"] = x.get_method();
        j["ranges"] = x.get_ranges();
        j["coefficients"] = x.get_coefficients();
        j["referenceVolumetricLosses"] = x.get_reference_volumetric_losses();
        j["a"] = x.get_a();
        j["b"] = x.get_b();
        j["c"] = x.get_c();
        j["d"] = x.get_d();
        j["factors"] = x.get_factors();
    }

    inline void from_json(const json & j, CoreMaterial& x) {
        x.set_alternatives(get_stack_optional<std::vector<std::string>>(j, "alternatives"));
        x.set_application(get_stack_optional<std::vector<Application>>(j, "application"));
        x.set_bh_cycle(get_stack_optional<std::vector<BhCycleElement>>(j, "bhCycle"));
        x.set_coercive_force(get_stack_optional<std::vector<BhCycleElement>>(j, "coerciveForce"));
        x.set_commercial_name(get_stack_optional<std::string>(j, "commercialName"));
        x.set_curie_temperature(get_stack_optional<double>(j, "curieTemperature"));
        x.set_density(get_stack_optional<double>(j, "density"));
        x.set_family(get_stack_optional<std::string>(j, "family"));
        x.set_heat_capacity(get_stack_optional<DimensionWithTolerance>(j, "heatCapacity"));
        x.set_heat_conductivity(get_stack_optional<DimensionWithTolerance>(j, "heatConductivity"));
        x.set_manufacturer_info(j.at("manufacturerInfo").get<ManufacturerInfo>());
        x.set_mass_losses(get_stack_optional<std::map<std::string, std::vector<MassLossesMethod>>>(j, "massLosses"));
        x.set_material(j.at("material").get<MaterialType>());
        x.set_material_composition(get_stack_optional<MaterialComposition>(j, "materialComposition"));
        x.set_name(j.at("name").get<std::string>());
        x.set_permeability(j.at("permeability").get<Permeabilities>());
        x.set_recommendations(get_stack_optional<CoreMaterialRecommendations>(j, "recommendations"));
        x.set_remanence(get_stack_optional<std::vector<BhCycleElement>>(j, "remanence"));
        x.set_resistivity(j.at("resistivity").get<std::vector<ResistivityPoint>>());
        x.set_saturation(j.at("saturation").get<std::vector<BhCycleElement>>());
        x.set_type(j.at("type").get<CoreMaterialType>());
        x.set_volumetric_losses(j.at("volumetricLosses").get<std::map<std::string, std::vector<VolumetricLossesMethod>>>());
    }

    inline void to_json(json & j, const CoreMaterial & x) {
        j = json::object();
        j["alternatives"] = x.get_alternatives();
        j["application"] = x.get_application();
        j["bhCycle"] = x.get_bh_cycle();
        j["coerciveForce"] = x.get_coercive_force();
        j["commercialName"] = x.get_commercial_name();
        j["curieTemperature"] = x.get_curie_temperature();
        j["density"] = x.get_density();
        j["family"] = x.get_family();
        j["heatCapacity"] = x.get_heat_capacity();
        j["heatConductivity"] = x.get_heat_conductivity();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["massLosses"] = x.get_mass_losses();
        j["material"] = x.get_material();
        j["materialComposition"] = x.get_material_composition();
        j["name"] = x.get_name();
        j["permeability"] = x.get_permeability();
        j["recommendations"] = x.get_recommendations();
        j["remanence"] = x.get_remanence();
        j["resistivity"] = x.get_resistivity();
        j["saturation"] = x.get_saturation();
        j["type"] = x.get_type();
        j["volumetricLosses"] = x.get_volumetric_losses();
    }

    inline void from_json(const json & j, CoreShape& x) {
        x.set_aliases(get_stack_optional<std::vector<std::string>>(j, "aliases"));
        x.set_dimensions(get_stack_optional<std::map<std::string, Dimension>>(j, "dimensions"));
        x.set_family(j.at("family").get<CoreShapeFamily>());
        x.set_family_subtype(get_stack_optional<std::string>(j, "familySubtype"));
        x.set_magnetic_circuit(get_stack_optional<MagneticCircuit>(j, "magneticCircuit"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_type(j.at("type").get<FunctionalDescriptionType>());
    }

    inline void to_json(json & j, const CoreShape & x) {
        j = json::object();
        j["aliases"] = x.get_aliases();
        j["dimensions"] = x.get_dimensions();
        j["family"] = x.get_family();
        j["familySubtype"] = x.get_family_subtype();
        j["magneticCircuit"] = x.get_magnetic_circuit();
        j["name"] = x.get_name();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, CoreFunctionalDescription& x) {
        x.set_coating(get_stack_optional<Coating>(j, "coating"));
        x.set_gapping(j.at("gapping").get<std::vector<CoreGap>>());
        x.set_material(j.at("material").get<CoreMaterialDataOrNameUnion>());
        x.set_number_stacks(get_stack_optional<int64_t>(j, "numberStacks"));
        x.set_shape(j.at("shape").get<CoreShapeDataOrNameUnion>());
        x.set_type(j.at("type").get<CoreType>());
    }

    inline void to_json(json & j, const CoreFunctionalDescription & x) {
        j = json::object();
        j["coating"] = x.get_coating();
        j["gapping"] = x.get_gapping();
        j["material"] = x.get_material();
        j["numberStacks"] = x.get_number_stacks();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
    }

    inline void from_json(const json & j, Machining& x) {
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_length(j.at("length").get<double>());
    }

    inline void to_json(json & j, const Machining & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["length"] = x.get_length();
    }

    inline void from_json(const json & j, CoreGeometricalDescriptionElement& x) {
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_machining(get_stack_optional<std::vector<Machining>>(j, "machining"));
        x.set_material(get_stack_optional<std::variant<CoreMaterial, std::string>>(j, "material"));
        x.set_rotation(get_stack_optional<std::vector<double>>(j, "rotation"));
        x.set_shape(get_stack_optional<std::variant<CoreShape, std::string>>(j, "shape"));
        x.set_type(j.at("type").get<CoreGeometricalDescriptionElementType>());
        x.set_dimensions(get_stack_optional<std::vector<double>>(j, "dimensions"));
        x.set_insulation_material(get_stack_optional<std::variant<InsulationMaterial, std::string>>(j, "insulationMaterial"));
    }

    inline void to_json(json & j, const CoreGeometricalDescriptionElement & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["machining"] = x.get_machining();
        j["material"] = x.get_material();
        j["rotation"] = x.get_rotation();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
        j["dimensions"] = x.get_dimensions();
        j["insulationMaterial"] = x.get_insulation_material();
    }

    inline void from_json(const json & j, ColumnElement& x) {
        x.set_area(j.at("area").get<double>());
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_depth(j.at("depth").get<double>());
        x.set_height(j.at("height").get<double>());
        x.set_minimum_depth(get_stack_optional<double>(j, "minimumDepth"));
        x.set_minimum_width(get_stack_optional<double>(j, "minimumWidth"));
        x.set_shape(j.at("shape").get<ColumnShape>());
        x.set_type(j.at("type").get<ColumnType>());
        x.set_width(j.at("width").get<double>());
    }

    inline void to_json(json & j, const ColumnElement & x) {
        j = json::object();
        j["area"] = x.get_area();
        j["coordinates"] = x.get_coordinates();
        j["depth"] = x.get_depth();
        j["height"] = x.get_height();
        j["minimumDepth"] = x.get_minimum_depth();
        j["minimumWidth"] = x.get_minimum_width();
        j["shape"] = x.get_shape();
        j["type"] = x.get_type();
        j["width"] = x.get_width();
    }

    inline void from_json(const json & j, EffectiveParameters& x) {
        x.set_effective_area(j.at("effectiveArea").get<double>());
        x.set_effective_length(j.at("effectiveLength").get<double>());
        x.set_effective_volume(j.at("effectiveVolume").get<double>());
        x.set_minimum_area(j.at("minimumArea").get<double>());
    }

    inline void to_json(json & j, const EffectiveParameters & x) {
        j = json::object();
        j["effectiveArea"] = x.get_effective_area();
        j["effectiveLength"] = x.get_effective_length();
        j["effectiveVolume"] = x.get_effective_volume();
        j["minimumArea"] = x.get_minimum_area();
    }

    inline void from_json(const json & j, CoreProcessedDescription& x) {
        x.set_columns(j.at("columns").get<std::vector<ColumnElement>>());
        x.set_depth(j.at("depth").get<double>());
        x.set_effective_parameters(j.at("effectiveParameters").get<EffectiveParameters>());
        x.set_height(j.at("height").get<double>());
        x.set_thermal_resistance(get_stack_optional<double>(j, "thermalResistance"));
        x.set_width(j.at("width").get<double>());
        x.set_winding_windows(j.at("windingWindows").get<std::vector<WindingWindowElement>>());
    }

    inline void to_json(json & j, const CoreProcessedDescription & x) {
        j = json::object();
        j["columns"] = x.get_columns();
        j["depth"] = x.get_depth();
        j["effectiveParameters"] = x.get_effective_parameters();
        j["height"] = x.get_height();
        j["thermalResistance"] = x.get_thermal_resistance();
        j["width"] = x.get_width();
        j["windingWindows"] = x.get_winding_windows();
    }

    inline void from_json(const json & j, MagneticCore& x) {
        x.set_distributors_info(get_stack_optional<std::vector<DistributorInfo>>(j, "distributorsInfo"));
        x.set_functional_description(j.at("functionalDescription").get<CoreFunctionalDescription>());
        x.set_geometrical_description(get_stack_optional<std::vector<CoreGeometricalDescriptionElement>>(j, "geometricalDescription"));
        x.set_manufacturer_info(get_stack_optional<ManufacturerInfo>(j, "manufacturerInfo"));
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_processed_description(get_stack_optional<CoreProcessedDescription>(j, "processedDescription"));
    }

    inline void to_json(json & j, const MagneticCore & x) {
        j = json::object();
        j["distributorsInfo"] = x.get_distributors_info();
        j["functionalDescription"] = x.get_functional_description();
        j["geometricalDescription"] = x.get_geometrical_description();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["name"] = x.get_name();
        j["processedDescription"] = x.get_processed_description();
    }

    inline void from_json(const json & j, MagneticDatasheetApplication& x) {
        x.set_auxiliary_voltage(get_stack_optional<double>(j, "auxiliaryVoltage"));
        x.set_input_voltage(get_stack_optional<std::variant<DimensionWithTolerance, double>>(j, "inputVoltage"));
        x.set_output_currents(get_stack_optional<std::vector<double>>(j, "outputCurrents"));
        x.set_output_voltages(get_stack_optional<std::vector<double>>(j, "outputVoltages"));
        x.set_switching_frequency(get_stack_optional<double>(j, "switchingFrequency"));
    }

    inline void to_json(json & j, const MagneticDatasheetApplication & x) {
        j = json::object();
        j["auxiliaryVoltage"] = x.get_auxiliary_voltage();
        j["inputVoltage"] = x.get_input_voltage();
        j["outputCurrents"] = x.get_output_currents();
        j["outputVoltages"] = x.get_output_voltages();
        j["switchingFrequency"] = x.get_switching_frequency();
    }

    inline void from_json(const json & j, Business& x) {
        x.set_packaging(get_stack_optional<std::string>(j, "packaging"));
    }

    inline void to_json(json & j, const Business & x) {
        j = json::object();
        j["packaging"] = x.get_packaging();
    }

    inline void from_json(const json & j, CommonModeFilter& x) {
        x.set_attenuation(get_stack_optional<double>(j, "attenuation"));
        x.set_attenuation_test_condition(get_stack_optional<std::string>(j, "attenuationTestCondition"));
        x.set_cut_off_frequency(get_stack_optional<double>(j, "cutOffFrequency"));
    }

    inline void to_json(json & j, const CommonModeFilter & x) {
        j = json::object();
        j["attenuation"] = x.get_attenuation();
        j["attenuationTestCondition"] = x.get_attenuation_test_condition();
        j["cutOffFrequency"] = x.get_cut_off_frequency();
    }

    inline void from_json(const json & j, DcResistance& x) {
        x.set_resistance(j.at("resistance").get<double>());
        x.set_winding(j.at("winding").get<std::string>());
    }

    inline void to_json(json & j, const DcResistance & x) {
        j = json::object();
        j["resistance"] = x.get_resistance();
        j["winding"] = x.get_winding();
    }

    inline void from_json(const json & j, DatasheetImpedancePoint& x) {
        x.set_frequency(j.at("frequency").get<double>());
        x.set_impedance(j.at("impedance").get<ImpedancePoint>());
    }

    inline void to_json(json & j, const DatasheetImpedancePoint & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["impedance"] = x.get_impedance();
    }

    inline void from_json(const json & j, Electrical& x) {
        x.set_common_mode_filter(get_stack_optional<CommonModeFilter>(j, "commonModeFilter"));
        x.set_coupling_coefficient(get_stack_optional<double>(j, "couplingCoefficient"));
        x.set_dc_resistance(get_stack_optional<DimensionWithTolerance>(j, "dcResistance"));
        x.set_dc_resistances(get_stack_optional<std::vector<DcResistance>>(j, "dcResistances"));
        x.set_impedance_points(get_stack_optional<std::vector<DatasheetImpedancePoint>>(j, "impedancePoints"));
        x.set_inductance(get_stack_optional<DimensionWithTolerance>(j, "inductance"));
        x.set_insulation_resistance(get_stack_optional<double>(j, "insulationResistance"));
        x.set_insulation_test_voltage_ac(get_stack_optional<double>(j, "insulationTestVoltageAC"));
        x.set_leakage_inductance(get_stack_optional<DimensionWithTolerance>(j, "leakageInductance"));
        x.set_maximum_impedance(get_stack_optional<double>(j, "maximumImpedance"));
        x.set_rated_current(get_stack_optional<double>(j, "ratedCurrent"));
        x.set_rated_voltage_ac(get_stack_optional<double>(j, "ratedVoltageAC"));
        x.set_rated_voltage_dc(get_stack_optional<double>(j, "ratedVoltageDC"));
        x.set_saturation_current_peak(get_stack_optional<double>(j, "saturationCurrentPeak"));
        x.set_self_resonant_frequency(get_stack_optional<double>(j, "selfResonantFrequency"));
        x.set_turns_ratio(get_stack_optional<double>(j, "turnsRatio"));
    }

    inline void to_json(json & j, const Electrical & x) {
        j = json::object();
        j["commonModeFilter"] = x.get_common_mode_filter();
        j["couplingCoefficient"] = x.get_coupling_coefficient();
        j["dcResistance"] = x.get_dc_resistance();
        j["dcResistances"] = x.get_dc_resistances();
        j["impedancePoints"] = x.get_impedance_points();
        j["inductance"] = x.get_inductance();
        j["insulationResistance"] = x.get_insulation_resistance();
        j["insulationTestVoltageAC"] = x.get_insulation_test_voltage_ac();
        j["leakageInductance"] = x.get_leakage_inductance();
        j["maximumImpedance"] = x.get_maximum_impedance();
        j["ratedCurrent"] = x.get_rated_current();
        j["ratedVoltageAC"] = x.get_rated_voltage_ac();
        j["ratedVoltageDC"] = x.get_rated_voltage_dc();
        j["saturationCurrentPeak"] = x.get_saturation_current_peak();
        j["selfResonantFrequency"] = x.get_self_resonant_frequency();
        j["turnsRatio"] = x.get_turns_ratio();
    }

    inline void from_json(const json & j, Mechanical& x) {
        x.set_diameter(get_stack_optional<DimensionWithTolerance>(j, "diameter"));
        x.set_height(get_stack_optional<DimensionWithTolerance>(j, "height"));
        x.set_length(get_stack_optional<DimensionWithTolerance>(j, "length"));
        x.set_mounting(get_stack_optional<ConnectionType>(j, "mounting"));
        x.set_width(get_stack_optional<DimensionWithTolerance>(j, "width"));
    }

    inline void to_json(json & j, const Mechanical & x) {
        j = json::object();
        j["diameter"] = x.get_diameter();
        j["height"] = x.get_height();
        j["length"] = x.get_length();
        j["mounting"] = x.get_mounting();
        j["width"] = x.get_width();
    }

    inline void from_json(const json & j, Part& x) {
        x.set_automotive(get_stack_optional<bool>(j, "automotive"));
        x.set_case_code(get_stack_optional<std::string>(j, "caseCode"));
        x.set_description(get_stack_optional<std::string>(j, "description"));
        x.set_family(get_stack_optional<std::string>(j, "family"));
        x.set_insulation_grade(get_stack_optional<InsulationType>(j, "insulationGrade"));
        x.set_match_code(get_stack_optional<std::string>(j, "matchCode"));
        x.set_material(get_stack_optional<std::string>(j, "material"));
        x.set_number_of_windings(get_stack_optional<int64_t>(j, "numberOfWindings"));
        x.set_part_number(get_stack_optional<std::string>(j, "partNumber"));
        x.set_shielded(get_stack_optional<bool>(j, "shielded"));
        x.set_winding_style(get_stack_optional<std::string>(j, "windingStyle"));
    }

    inline void to_json(json & j, const Part & x) {
        j = json::object();
        j["automotive"] = x.get_automotive();
        j["caseCode"] = x.get_case_code();
        j["description"] = x.get_description();
        j["family"] = x.get_family();
        j["insulationGrade"] = x.get_insulation_grade();
        j["matchCode"] = x.get_match_code();
        j["material"] = x.get_material();
        j["numberOfWindings"] = x.get_number_of_windings();
        j["partNumber"] = x.get_part_number();
        j["shielded"] = x.get_shielded();
        j["windingStyle"] = x.get_winding_style();
    }

    inline void from_json(const json & j, Thermal& x) {
        x.set_operating_temperature(get_stack_optional<DimensionWithTolerance>(j, "operatingTemperature"));
        x.set_temperature_rise(get_stack_optional<double>(j, "temperatureRise"));
        x.set_thermal_resistance(get_stack_optional<double>(j, "thermalResistance"));
    }

    inline void to_json(json & j, const Thermal & x) {
        j = json::object();
        j["operatingTemperature"] = x.get_operating_temperature();
        j["temperatureRise"] = x.get_temperature_rise();
        j["thermalResistance"] = x.get_thermal_resistance();
    }

    inline void from_json(const json & j, DatasheetInfo& x) {
        x.set_application(get_stack_optional<MagneticDatasheetApplication>(j, "application"));
        x.set_business(get_stack_optional<Business>(j, "business"));
        x.set_electrical(get_stack_optional<Electrical>(j, "electrical"));
        x.set_mechanical(get_stack_optional<Mechanical>(j, "mechanical"));
        x.set_part(get_stack_optional<Part>(j, "part"));
        x.set_thermal(get_stack_optional<Thermal>(j, "thermal"));
    }

    inline void to_json(json & j, const DatasheetInfo & x) {
        j = json::object();
        j["application"] = x.get_application();
        j["business"] = x.get_business();
        j["electrical"] = x.get_electrical();
        j["mechanical"] = x.get_mechanical();
        j["part"] = x.get_part();
        j["thermal"] = x.get_thermal();
    }

    inline void from_json(const json & j, MagneticManufacturerInfo& x) {
        x.set_cost(get_stack_optional<Cost>(j, "cost"));
        x.set_datasheet_url(get_stack_optional<std::string>(j, "datasheetUrl"));
        x.set_description(get_stack_optional<std::string>(j, "description"));
        x.set_family(get_stack_optional<std::string>(j, "family"));
        x.set_irdi(get_stack_optional<std::string>(j, "irdi"));
        x.set_name(j.at("name").get<std::string>());
        x.set_order_code(get_stack_optional<std::string>(j, "orderCode"));
        x.set_reference(get_stack_optional<std::string>(j, "reference"));
        x.set_status(get_stack_optional<Status>(j, "status"));
        x.set_datasheet_info(get_stack_optional<DatasheetInfo>(j, "datasheetInfo"));
    }

    inline void to_json(json & j, const MagneticManufacturerInfo & x) {
        j = json::object();
        j["cost"] = x.get_cost();
        j["datasheetUrl"] = x.get_datasheet_url();
        j["description"] = x.get_description();
        j["family"] = x.get_family();
        j["irdi"] = x.get_irdi();
        j["name"] = x.get_name();
        j["orderCode"] = x.get_order_code();
        j["reference"] = x.get_reference();
        j["status"] = x.get_status();
        j["datasheetInfo"] = x.get_datasheet_info();
    }

    inline void from_json(const json & j, Magnetic& x) {
        x.set_coil(j.at("coil").get<Coil>());
        x.set_core(j.at("core").get<MagneticCore>());
        x.set_distributors_info(get_stack_optional<std::vector<DistributorInfo>>(j, "distributorsInfo"));
        x.set_manufacturer_info(get_stack_optional<MagneticManufacturerInfo>(j, "manufacturerInfo"));
        x.set_rotation(get_stack_optional<std::vector<double>>(j, "rotation"));
    }

    inline void to_json(json & j, const Magnetic & x) {
        j = json::object();
        j["coil"] = x.get_coil();
        j["core"] = x.get_core();
        j["distributorsInfo"] = x.get_distributors_info();
        j["manufacturerInfo"] = x.get_manufacturer_info();
        j["rotation"] = x.get_rotation();
    }

    inline void from_json(const json & j, CoreLossesOutput& x) {
        x.set_core_losses(j.at("coreLosses").get<double>());
        x.set_eddy_current_core_losses(get_stack_optional<double>(j, "eddyCurrentCoreLosses"));
        x.set_hysteresis_core_losses(get_stack_optional<double>(j, "hysteresisCoreLosses"));
        x.set_magnetic_flux_density(get_stack_optional<SignalDescriptor>(j, "magneticFluxDensity"));
        x.set_mass_losses(get_stack_optional<double>(j, "massLosses"));
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_temperature(j.at("temperature").get<double>());
        x.set_volumetric_losses(get_stack_optional<double>(j, "volumetricLosses"));
    }

    inline void to_json(json & j, const CoreLossesOutput & x) {
        j = json::object();
        j["coreLosses"] = x.get_core_losses();
        j["eddyCurrentCoreLosses"] = x.get_eddy_current_core_losses();
        j["hysteresisCoreLosses"] = x.get_hysteresis_core_losses();
        j["magneticFluxDensity"] = x.get_magnetic_flux_density();
        j["massLosses"] = x.get_mass_losses();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["temperature"] = x.get_temperature();
        j["volumetricLosses"] = x.get_volumetric_losses();
    }

    inline void from_json(const json & j, ComplexMatrixAtFrequency& x) {
        x.set_frequency(j.at("frequency").get<double>());
        x.set_magnitude(j.at("magnitude").get<std::map<std::string, std::map<std::string, DimensionWithTolerance>>>());
        x.set_phase(j.at("phase").get<std::map<std::string, std::map<std::string, DimensionWithTolerance>>>());
    }

    inline void to_json(json & j, const ComplexMatrixAtFrequency & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["magnitude"] = x.get_magnitude();
        j["phase"] = x.get_phase();
    }

    inline void from_json(const json & j, ScalarMatrixAtFrequency& x) {
        x.set_frequency(j.at("frequency").get<double>());
        x.set_magnitude(j.at("magnitude").get<std::map<std::string, std::map<std::string, DimensionWithTolerance>>>());
    }

    inline void to_json(json & j, const ScalarMatrixAtFrequency & x) {
        j = json::object();
        j["frequency"] = x.get_frequency();
        j["magnitude"] = x.get_magnitude();
    }

    inline void from_json(const json & j, ImpedanceOutput& x) {
        x.set_impedance_matrix(get_stack_optional<std::vector<ComplexMatrixAtFrequency>>(j, "impedanceMatrix"));
        x.set_inductance_matrix(j.at("inductanceMatrix").get<std::vector<ScalarMatrixAtFrequency>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_resistance_matrix(j.at("resistanceMatrix").get<std::vector<ScalarMatrixAtFrequency>>());
    }

    inline void to_json(json & j, const ImpedanceOutput & x) {
        j = json::object();
        j["impedanceMatrix"] = x.get_impedance_matrix();
        j["inductanceMatrix"] = x.get_inductance_matrix();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["resistanceMatrix"] = x.get_resistance_matrix();
    }

    inline void from_json(const json & j, LeakageInductanceOutput& x) {
        x.set_leakage_inductance_per_winding(j.at("leakageInductancePerWinding").get<std::vector<DimensionWithTolerance>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
    }

    inline void to_json(json & j, const LeakageInductanceOutput & x) {
        j = json::object();
        j["leakageInductancePerWinding"] = x.get_leakage_inductance_per_winding();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
    }

    inline void from_json(const json & j, InductanceMeasurementCondition& x) {
        x.set_current_rms(get_stack_optional<double>(j, "currentRms"));
        x.set_dc_bias_current(get_stack_optional<double>(j, "dcBiasCurrent"));
        x.set_frequency(get_stack_optional<double>(j, "frequency"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_voltage_rms(get_stack_optional<double>(j, "voltageRms"));
    }

    inline void to_json(json & j, const InductanceMeasurementCondition & x) {
        j = json::object();
        j["currentRms"] = x.get_current_rms();
        j["dcBiasCurrent"] = x.get_dc_bias_current();
        j["frequency"] = x.get_frequency();
        j["temperature"] = x.get_temperature();
        j["voltageRms"] = x.get_voltage_rms();
    }

    inline void from_json(const json & j, AirGapReluctanceOutput& x) {
        x.set_fringing_factor(j.at("fringingFactor").get<double>());
        x.set_maximum_storable_magnetic_energy(j.at("maximumStorableMagneticEnergy").get<double>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_reluctance(j.at("reluctance").get<double>());
    }

    inline void to_json(json & j, const AirGapReluctanceOutput & x) {
        j = json::object();
        j["fringingFactor"] = x.get_fringing_factor();
        j["maximumStorableMagneticEnergy"] = x.get_maximum_storable_magnetic_energy();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["reluctance"] = x.get_reluctance();
    }

    inline void from_json(const json & j, MagnetizingInductanceOutput& x) {
        x.set_core_reluctance(j.at("coreReluctance").get<double>());
        x.set_gapping_reluctance(get_stack_optional<double>(j, "gappingReluctance"));
        x.set_magnetizing_inductance(j.at("magnetizingInductance").get<DimensionWithTolerance>());
        x.set_maximum_fringing_factor(get_stack_optional<double>(j, "maximumFringingFactor"));
        x.set_maximum_magnetic_energy_core(get_stack_optional<double>(j, "maximumMagneticEnergyCore"));
        x.set_maximum_storable_magnetic_energy_gapping(get_stack_optional<double>(j, "maximumStorableMagneticEnergyGapping"));
        x.set_measurement_condition(get_stack_optional<InductanceMeasurementCondition>(j, "measurementCondition"));
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_reluctance_per_gap(get_stack_optional<std::vector<AirGapReluctanceOutput>>(j, "reluctancePerGap"));
        x.set_ungapped_core_reluctance(get_stack_optional<double>(j, "ungappedCoreReluctance"));
    }

    inline void to_json(json & j, const MagnetizingInductanceOutput & x) {
        j = json::object();
        j["coreReluctance"] = x.get_core_reluctance();
        j["gappingReluctance"] = x.get_gapping_reluctance();
        j["magnetizingInductance"] = x.get_magnetizing_inductance();
        j["maximumFringingFactor"] = x.get_maximum_fringing_factor();
        j["maximumMagneticEnergyCore"] = x.get_maximum_magnetic_energy_core();
        j["maximumStorableMagneticEnergyGapping"] = x.get_maximum_storable_magnetic_energy_gapping();
        j["measurementCondition"] = x.get_measurement_condition();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["reluctancePerGap"] = x.get_reluctance_per_gap();
        j["ungappedCoreReluctance"] = x.get_ungapped_core_reluctance();
    }

    inline void from_json(const json & j, InductanceOutput& x) {
        x.set_coupling_coefficients_matrix(get_stack_optional<std::vector<ScalarMatrixAtFrequency>>(j, "couplingCoefficientsMatrix"));
        x.set_inductance_matrix(get_stack_optional<std::vector<ScalarMatrixAtFrequency>>(j, "inductanceMatrix"));
        x.set_leakage_inductance(get_stack_optional<LeakageInductanceOutput>(j, "leakageInductance"));
        x.set_magnetizing_inductance(j.at("magnetizingInductance").get<MagnetizingInductanceOutput>());
    }

    inline void to_json(json & j, const InductanceOutput & x) {
        j = json::object();
        j["couplingCoefficientsMatrix"] = x.get_coupling_coefficients_matrix();
        j["inductanceMatrix"] = x.get_inductance_matrix();
        j["leakageInductance"] = x.get_leakage_inductance();
        j["magnetizingInductance"] = x.get_magnetizing_inductance();
    }

    inline void from_json(const json & j, DielectricVoltage& x) {
        x.set_duration(get_stack_optional<double>(j, "duration"));
        x.set_method_used(get_stack_optional<std::string>(j, "methodUsed"));
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_voltage(j.at("voltage").get<double>());
        x.set_voltage_type(j.at("voltageType").get<VoltageType>());
    }

    inline void to_json(json & j, const DielectricVoltage & x) {
        j = json::object();
        j["duration"] = x.get_duration();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["voltage"] = x.get_voltage();
        j["voltageType"] = x.get_voltage_type();
    }

    inline void from_json(const json & j, InsulationCoordinationOutput& x) {
        x.set_clearance(j.at("clearance").get<double>());
        x.set_creepage_distance(j.at("creepageDistance").get<double>());
        x.set_distance_through_insulation(j.at("distanceThroughInsulation").get<double>());
        x.set_withstand_voltage(j.at("withstandVoltage").get<double>());
        x.set_withstand_voltage_duration(get_stack_optional<double>(j, "withstandVoltageDuration"));
        x.set_withstand_voltage_type(get_stack_optional<VoltageType>(j, "withstandVoltageType"));
    }

    inline void to_json(json & j, const InsulationCoordinationOutput & x) {
        j = json::object();
        j["clearance"] = x.get_clearance();
        j["creepageDistance"] = x.get_creepage_distance();
        j["distanceThroughInsulation"] = x.get_distance_through_insulation();
        j["withstandVoltage"] = x.get_withstand_voltage();
        j["withstandVoltageDuration"] = x.get_withstand_voltage_duration();
        j["withstandVoltageType"] = x.get_withstand_voltage_type();
    }

    inline void from_json(const json & j, SixCapacitorNetworkPerWinding& x) {
        x.set_c1(j.at("C1").get<double>());
        x.set_c2(j.at("C2").get<double>());
        x.set_c3(j.at("C3").get<double>());
        x.set_c4(j.at("C4").get<double>());
        x.set_c5(j.at("C5").get<double>());
        x.set_c6(j.at("C6").get<double>());
    }

    inline void to_json(json & j, const SixCapacitorNetworkPerWinding & x) {
        j = json::object();
        j["C1"] = x.get_c1();
        j["C2"] = x.get_c2();
        j["C3"] = x.get_c3();
        j["C4"] = x.get_c4();
        j["C5"] = x.get_c5();
        j["C6"] = x.get_c6();
    }

    inline void from_json(const json & j, TripoleCapacitancePerWinding& x) {
        x.set_c1(j.at("C1").get<double>());
        x.set_c2(j.at("C2").get<double>());
        x.set_c3(j.at("C3").get<double>());
    }

    inline void to_json(json & j, const TripoleCapacitancePerWinding & x) {
        j = json::object();
        j["C1"] = x.get_c1();
        j["C2"] = x.get_c2();
        j["C3"] = x.get_c3();
    }

    inline void from_json(const json & j, StrayCapacitanceOutput& x) {
        x.set_capacitance_among_turns(get_stack_optional<std::map<std::string, std::map<std::string, double>>>(j, "capacitanceAmongTurns"));
        x.set_capacitance_among_windings(get_stack_optional<std::map<std::string, std::map<std::string, double>>>(j, "capacitanceAmongWindings"));
        x.set_capacitance_matrix(get_stack_optional<std::map<std::string, std::map<std::string, ScalarMatrixAtFrequency>>>(j, "capacitanceMatrix"));
        x.set_electric_energy_among_turns(get_stack_optional<std::map<std::string, std::map<std::string, double>>>(j, "electricEnergyAmongTurns"));
        x.set_maxwell_capacitance_matrix(get_stack_optional<std::vector<ScalarMatrixAtFrequency>>(j, "maxwellCapacitanceMatrix"));
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_six_capacitor_network_per_winding(get_stack_optional<std::map<std::string, std::map<std::string, SixCapacitorNetworkPerWinding>>>(j, "sixCapacitorNetworkPerWinding"));
        x.set_tripole_capacitance_per_winding(get_stack_optional<std::map<std::string, std::map<std::string, TripoleCapacitancePerWinding>>>(j, "tripoleCapacitancePerWinding"));
        x.set_voltage_divider_end_per_turn(get_stack_optional<std::vector<double>>(j, "voltageDividerEndPerTurn"));
        x.set_voltage_divider_start_per_turn(get_stack_optional<std::vector<double>>(j, "voltageDividerStartPerTurn"));
        x.set_voltage_drop_among_turns(get_stack_optional<std::map<std::string, std::map<std::string, double>>>(j, "voltageDropAmongTurns"));
        x.set_voltage_per_turn(get_stack_optional<std::vector<double>>(j, "voltagePerTurn"));
    }

    inline void to_json(json & j, const StrayCapacitanceOutput & x) {
        j = json::object();
        j["capacitanceAmongTurns"] = x.get_capacitance_among_turns();
        j["capacitanceAmongWindings"] = x.get_capacitance_among_windings();
        j["capacitanceMatrix"] = x.get_capacitance_matrix();
        j["electricEnergyAmongTurns"] = x.get_electric_energy_among_turns();
        j["maxwellCapacitanceMatrix"] = x.get_maxwell_capacitance_matrix();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["sixCapacitorNetworkPerWinding"] = x.get_six_capacitor_network_per_winding();
        j["tripoleCapacitancePerWinding"] = x.get_tripole_capacitance_per_winding();
        j["voltageDividerEndPerTurn"] = x.get_voltage_divider_end_per_turn();
        j["voltageDividerStartPerTurn"] = x.get_voltage_divider_start_per_turn();
        j["voltageDropAmongTurns"] = x.get_voltage_drop_among_turns();
        j["voltagePerTurn"] = x.get_voltage_per_turn();
    }

    inline void from_json(const json & j, TemperaturePoint& x) {
        x.set_coordinates(j.at("coordinates").get<std::vector<double>>());
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const TemperaturePoint & x) {
        j = json::object();
        j["coordinates"] = x.get_coordinates();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, TemperatureOutput& x) {
        x.set_bulk_thermal_resistance(get_stack_optional<double>(j, "bulkThermalResistance"));
        x.set_initial_temperature(get_stack_optional<double>(j, "initialTemperature"));
        x.set_maximum_temperature(j.at("maximumTemperature").get<double>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_temperature_point(get_stack_optional<TemperaturePoint>(j, "temperaturePoint"));
    }

    inline void to_json(json & j, const TemperatureOutput & x) {
        j = json::object();
        j["bulkThermalResistance"] = x.get_bulk_thermal_resistance();
        j["initialTemperature"] = x.get_initial_temperature();
        j["maximumTemperature"] = x.get_maximum_temperature();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["temperaturePoint"] = x.get_temperature_point();
    }

    inline void from_json(const json & j, OhmicLosses& x) {
        x.set_losses(j.at("losses").get<double>());
        x.set_method_used(get_stack_optional<std::string>(j, "methodUsed"));
        x.set_origin(j.at("origin").get<ResultOrigin>());
    }

    inline void to_json(json & j, const OhmicLosses & x) {
        j = json::object();
        j["losses"] = x.get_losses();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
    }

    inline void from_json(const json & j, WindingLossElement& x) {
        x.set_harmonic_frequencies(j.at("harmonicFrequencies").get<std::vector<double>>());
        x.set_losses_per_harmonic(j.at("lossesPerHarmonic").get<std::vector<double>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
    }

    inline void to_json(json & j, const WindingLossElement & x) {
        j = json::object();
        j["harmonicFrequencies"] = x.get_harmonic_frequencies();
        j["lossesPerHarmonic"] = x.get_losses_per_harmonic();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
    }

    inline void from_json(const json & j, WindingLossesPerElement& x) {
        x.set_name(get_stack_optional<std::string>(j, "name"));
        x.set_ohmic_losses(get_stack_optional<OhmicLosses>(j, "ohmicLosses"));
        x.set_proximity_effect_losses(get_stack_optional<WindingLossElement>(j, "proximityEffectLosses"));
        x.set_skin_effect_losses(get_stack_optional<WindingLossElement>(j, "skinEffectLosses"));
    }

    inline void to_json(json & j, const WindingLossesPerElement & x) {
        j = json::object();
        j["name"] = x.get_name();
        j["ohmicLosses"] = x.get_ohmic_losses();
        j["proximityEffectLosses"] = x.get_proximity_effect_losses();
        j["skinEffectLosses"] = x.get_skin_effect_losses();
    }

    inline void from_json(const json & j, WindingLossesOutput& x) {
        x.set_current_divider_per_turn(get_stack_optional<std::vector<double>>(j, "currentDividerPerTurn"));
        x.set_current_per_winding(get_stack_optional<OperatingPoint>(j, "currentPerWinding"));
        x.set_dc_resistance_per_turn(get_stack_optional<std::vector<double>>(j, "dcResistancePerTurn"));
        x.set_dc_resistance_per_winding(get_stack_optional<std::vector<double>>(j, "dcResistancePerWinding"));
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_resistance_matrix(get_stack_optional<std::vector<ScalarMatrixAtFrequency>>(j, "resistanceMatrix"));
        x.set_temperature(get_stack_optional<double>(j, "temperature"));
        x.set_winding_losses(j.at("windingLosses").get<double>());
        x.set_winding_losses_per_layer(get_stack_optional<std::vector<WindingLossesPerElement>>(j, "windingLossesPerLayer"));
        x.set_winding_losses_per_section(get_stack_optional<std::vector<WindingLossesPerElement>>(j, "windingLossesPerSection"));
        x.set_winding_losses_per_turn(get_stack_optional<std::vector<WindingLossesPerElement>>(j, "windingLossesPerTurn"));
        x.set_winding_losses_per_winding(get_stack_optional<std::vector<WindingLossesPerElement>>(j, "windingLossesPerWinding"));
    }

    inline void to_json(json & j, const WindingLossesOutput & x) {
        j = json::object();
        j["currentDividerPerTurn"] = x.get_current_divider_per_turn();
        j["currentPerWinding"] = x.get_current_per_winding();
        j["dcResistancePerTurn"] = x.get_dc_resistance_per_turn();
        j["dcResistancePerWinding"] = x.get_dc_resistance_per_winding();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["resistanceMatrix"] = x.get_resistance_matrix();
        j["temperature"] = x.get_temperature();
        j["windingLosses"] = x.get_winding_losses();
        j["windingLossesPerLayer"] = x.get_winding_losses_per_layer();
        j["windingLossesPerSection"] = x.get_winding_losses_per_section();
        j["windingLossesPerTurn"] = x.get_winding_losses_per_turn();
        j["windingLossesPerWinding"] = x.get_winding_losses_per_winding();
    }

    inline void from_json(const json & j, FieldPoint& x) {
        x.set_label(get_stack_optional<std::string>(j, "label"));
        x.set_point(j.at("point").get<std::vector<double>>());
        x.set_rotation(get_stack_optional<double>(j, "rotation"));
        x.set_turn_index(get_stack_optional<int64_t>(j, "turnIndex"));
        x.set_turn_length(get_stack_optional<double>(j, "turnLength"));
        x.set_value(j.at("value").get<double>());
    }

    inline void to_json(json & j, const FieldPoint & x) {
        j = json::object();
        j["label"] = x.get_label();
        j["point"] = x.get_point();
        j["rotation"] = x.get_rotation();
        j["turnIndex"] = x.get_turn_index();
        j["turnLength"] = x.get_turn_length();
        j["value"] = x.get_value();
    }

    inline void from_json(const json & j, Field& x) {
        x.set_data(j.at("data").get<std::vector<FieldPoint>>());
        x.set_frequency(j.at("frequency").get<double>());
    }

    inline void to_json(json & j, const Field & x) {
        j = json::object();
        j["data"] = x.get_data();
        j["frequency"] = x.get_frequency();
    }

    inline void from_json(const json & j, WindingWindowCurrentDensityFieldOutput& x) {
        x.set_field_per_frequency(j.at("fieldPerFrequency").get<std::vector<Field>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
        x.set_wires(j.at("wires").get<std::vector<WireDataOrNameUnion>>());
    }

    inline void to_json(json & j, const WindingWindowCurrentDensityFieldOutput & x) {
        j = json::object();
        j["fieldPerFrequency"] = x.get_field_per_frequency();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
        j["wires"] = x.get_wires();
    }

    inline void from_json(const json & j, WindingWindowCurrentFieldOutput& x) {
        x.set_field_per_frequency(j.at("fieldPerFrequency").get<std::vector<Field>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
    }

    inline void to_json(json & j, const WindingWindowCurrentFieldOutput & x) {
        j = json::object();
        j["fieldPerFrequency"] = x.get_field_per_frequency();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
    }

    inline void from_json(const json & j, ComplexFieldPoint& x) {
        x.set_imaginary(j.at("imaginary").get<double>());
        x.set_label(get_stack_optional<std::string>(j, "label"));
        x.set_point(j.at("point").get<std::vector<double>>());
        x.set_real(j.at("real").get<double>());
        x.set_turn_index(get_stack_optional<int64_t>(j, "turnIndex"));
        x.set_turn_length(get_stack_optional<double>(j, "turnLength"));
    }

    inline void to_json(json & j, const ComplexFieldPoint & x) {
        j = json::object();
        j["imaginary"] = x.get_imaginary();
        j["label"] = x.get_label();
        j["point"] = x.get_point();
        j["real"] = x.get_real();
        j["turnIndex"] = x.get_turn_index();
        j["turnLength"] = x.get_turn_length();
    }

    inline void from_json(const json & j, ComplexField& x) {
        x.set_data(j.at("data").get<std::vector<ComplexFieldPoint>>());
        x.set_frequency(j.at("frequency").get<double>());
    }

    inline void to_json(json & j, const ComplexField & x) {
        j = json::object();
        j["data"] = x.get_data();
        j["frequency"] = x.get_frequency();
    }

    inline void from_json(const json & j, WindingWindowMagneticStrengthFieldOutput& x) {
        x.set_field_per_frequency(j.at("fieldPerFrequency").get<std::vector<ComplexField>>());
        x.set_method_used(j.at("methodUsed").get<std::string>());
        x.set_origin(j.at("origin").get<ResultOrigin>());
    }

    inline void to_json(json & j, const WindingWindowMagneticStrengthFieldOutput & x) {
        j = json::object();
        j["fieldPerFrequency"] = x.get_field_per_frequency();
        j["methodUsed"] = x.get_method_used();
        j["origin"] = x.get_origin();
    }

    inline void from_json(const json & j, Outputs& x) {
        x.set_core_losses(get_stack_optional<CoreLossesOutput>(j, "coreLosses"));
        x.set_impedance(get_stack_optional<ImpedanceOutput>(j, "impedance"));
        x.set_inductance(get_stack_optional<InductanceOutput>(j, "inductance"));
        x.set_insulation(get_stack_optional<std::vector<DielectricVoltage>>(j, "insulation"));
        x.set_insulation_coordination(get_stack_optional<InsulationCoordinationOutput>(j, "insulationCoordination"));
        x.set_stray_capacitance(get_stack_optional<std::vector<StrayCapacitanceOutput>>(j, "strayCapacitance"));
        x.set_temperature(get_stack_optional<TemperatureOutput>(j, "temperature"));
        x.set_winding_losses(get_stack_optional<WindingLossesOutput>(j, "windingLosses"));
        x.set_winding_window_current_density_field(get_stack_optional<WindingWindowCurrentDensityFieldOutput>(j, "windingWindowCurrentDensityField"));
        x.set_winding_window_current_field(get_stack_optional<WindingWindowCurrentFieldOutput>(j, "windingWindowCurrentField"));
        x.set_winding_window_magnetic_strength_field(get_stack_optional<WindingWindowMagneticStrengthFieldOutput>(j, "windingWindowMagneticStrengthField"));
    }

    inline void to_json(json & j, const Outputs & x) {
        j = json::object();
        j["coreLosses"] = x.get_core_losses();
        j["impedance"] = x.get_impedance();
        j["inductance"] = x.get_inductance();
        j["insulation"] = x.get_insulation();
        j["insulationCoordination"] = x.get_insulation_coordination();
        j["strayCapacitance"] = x.get_stray_capacitance();
        j["temperature"] = x.get_temperature();
        j["windingLosses"] = x.get_winding_losses();
        j["windingWindowCurrentDensityField"] = x.get_winding_window_current_density_field();
        j["windingWindowCurrentField"] = x.get_winding_window_current_field();
        j["windingWindowMagneticStrengthField"] = x.get_winding_window_magnetic_strength_field();
    }

    inline void from_json(const json & j, Mas& x) {
        x.set_inputs(j.at("inputs").get<Inputs>());
        x.set_magnetic(j.at("magnetic").get<Magnetic>());
        x.set_mas_conformance(get_stack_optional<MasConformance>(j, "masConformance"));
        x.set_mas_version(get_stack_optional<std::string>(j, "masVersion"));
        x.set_outputs(j.at("outputs").get<std::vector<Outputs>>());
    }

    inline void to_json(json & j, const Mas & x) {
        j = json::object();
        j["inputs"] = x.get_inputs();
        j["magnetic"] = x.get_magnetic();
        j["masConformance"] = x.get_mas_conformance();
        j["masVersion"] = x.get_mas_version();
        j["outputs"] = x.get_outputs();
    }

    inline void from_json(const json & j, OutputSType & x) {
        if (j == "average") x = OutputSType::AVERAGE;
        else if (j == "dc") x = OutputSType::DC;
        else if (j == "peak") x = OutputSType::PEAK;
        else if (j == "peakToPeak") x = OutputSType::PEAK_TO_PEAK;
        else if (j == "rms") x = OutputSType::RMS;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const OutputSType & x) {
        switch (x) {
            case OutputSType::AVERAGE: j = "average"; break;
            case OutputSType::DC: j = "dc"; break;
            case OutputSType::PEAK: j = "peak"; break;
            case OutputSType::PEAK_TO_PEAK: j = "peakToPeak"; break;
            case OutputSType::RMS: j = "rms"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CllcPowerFlow & x) {
        if (j == "forward") x = CllcPowerFlow::FORWARD;
        else if (j == "reverse") x = CllcPowerFlow::REVERSE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CllcPowerFlow & x) {
        switch (x) {
            case CllcPowerFlow::FORWARD: j = "forward"; break;
            case CllcPowerFlow::REVERSE: j = "reverse"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WaveformLabel & x) {
        static std::unordered_map<std::string, WaveformLabel> enumValues {
            {"bipolarRectangular", WaveformLabel::BIPOLAR_RECTANGULAR},
            {"bipolarTriangular", WaveformLabel::BIPOLAR_TRIANGULAR},
            {"custom", WaveformLabel::CUSTOM},
            {"flybackPrimary", WaveformLabel::FLYBACK_PRIMARY},
            {"flybackSecondary", WaveformLabel::FLYBACK_SECONDARY},
            {"flybackSecondaryWithDeadtime", WaveformLabel::FLYBACK_SECONDARY_WITH_DEADTIME},
            {"rectangular", WaveformLabel::RECTANGULAR},
            {"rectangularDCM", WaveformLabel::RECTANGULAR_DCM},
            {"rectangularWithDeadtime", WaveformLabel::RECTANGULAR_WITH_DEADTIME},
            {"secondaryRectangular", WaveformLabel::SECONDARY_RECTANGULAR},
            {"secondaryRectangularWithDeadtime", WaveformLabel::SECONDARY_RECTANGULAR_WITH_DEADTIME},
            {"sinusoidal", WaveformLabel::SINUSOIDAL},
            {"triangular", WaveformLabel::TRIANGULAR},
            {"triangularWithDeadtime", WaveformLabel::TRIANGULAR_WITH_DEADTIME},
            {"unipolarRectangular", WaveformLabel::UNIPOLAR_RECTANGULAR},
            {"unipolarTriangular", WaveformLabel::UNIPOLAR_TRIANGULAR},
        };
        auto iter = enumValues.find(j.get<std::string>());
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const WaveformLabel & x) {
        switch (x) {
            case WaveformLabel::BIPOLAR_RECTANGULAR: j = "bipolarRectangular"; break;
            case WaveformLabel::BIPOLAR_TRIANGULAR: j = "bipolarTriangular"; break;
            case WaveformLabel::CUSTOM: j = "custom"; break;
            case WaveformLabel::FLYBACK_PRIMARY: j = "flybackPrimary"; break;
            case WaveformLabel::FLYBACK_SECONDARY: j = "flybackSecondary"; break;
            case WaveformLabel::FLYBACK_SECONDARY_WITH_DEADTIME: j = "flybackSecondaryWithDeadtime"; break;
            case WaveformLabel::RECTANGULAR: j = "rectangular"; break;
            case WaveformLabel::RECTANGULAR_DCM: j = "rectangularDCM"; break;
            case WaveformLabel::RECTANGULAR_WITH_DEADTIME: j = "rectangularWithDeadtime"; break;
            case WaveformLabel::SECONDARY_RECTANGULAR: j = "secondaryRectangular"; break;
            case WaveformLabel::SECONDARY_RECTANGULAR_WITH_DEADTIME: j = "secondaryRectangularWithDeadtime"; break;
            case WaveformLabel::SINUSOIDAL: j = "sinusoidal"; break;
            case WaveformLabel::TRIANGULAR: j = "triangular"; break;
            case WaveformLabel::TRIANGULAR_WITH_DEADTIME: j = "triangularWithDeadtime"; break;
            case WaveformLabel::UNIPOLAR_RECTANGULAR: j = "unipolarRectangular"; break;
            case WaveformLabel::UNIPOLAR_TRIANGULAR: j = "unipolarTriangular"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ModulationType & x) {
        if (j == "DPS") x = ModulationType::DPS;
        else if (j == "EPS") x = ModulationType::EPS;
        else if (j == "SPS") x = ModulationType::SPS;
        else if (j == "TPS") x = ModulationType::TPS;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ModulationType & x) {
        switch (x) {
            case ModulationType::DPS: j = "DPS"; break;
            case ModulationType::EPS: j = "EPS"; break;
            case ModulationType::SPS: j = "SPS"; break;
            case ModulationType::TPS: j = "TPS"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, FlybackModes & x) {
        if (j == "boundaryModeOperation") x = FlybackModes::BOUNDARY_MODE_OPERATION;
        else if (j == "continuousConductionMode") x = FlybackModes::CONTINUOUS_CONDUCTION_MODE;
        else if (j == "discontinuousConductionMode") x = FlybackModes::DISCONTINUOUS_CONDUCTION_MODE;
        else if (j == "quasiResonantMode") x = FlybackModes::QUASI_RESONANT_MODE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const FlybackModes & x) {
        switch (x) {
            case FlybackModes::BOUNDARY_MODE_OPERATION: j = "boundaryModeOperation"; break;
            case FlybackModes::CONTINUOUS_CONDUCTION_MODE: j = "continuousConductionMode"; break;
            case FlybackModes::DISCONTINUOUS_CONDUCTION_MODE: j = "discontinuousConductionMode"; break;
            case FlybackModes::QUASI_RESONANT_MODE: j = "quasiResonantMode"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, LlcBridgeType & x) {
        if (j == "fullBridge") x = LlcBridgeType::FULL_BRIDGE;
        else if (j == "halfBridge") x = LlcBridgeType::HALF_BRIDGE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const LlcBridgeType & x) {
        switch (x) {
            case LlcBridgeType::FULL_BRIDGE: j = "fullBridge"; break;
            case LlcBridgeType::HALF_BRIDGE: j = "halfBridge"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PsfbRectifierType & x) {
        if (j == "centerTapped") x = PsfbRectifierType::CENTER_TAPPED;
        else if (j == "currentDoubler") x = PsfbRectifierType::CURRENT_DOUBLER;
        else if (j == "fullBridge") x = PsfbRectifierType::FULL_BRIDGE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const PsfbRectifierType & x) {
        switch (x) {
            case PsfbRectifierType::CENTER_TAPPED: j = "centerTapped"; break;
            case PsfbRectifierType::CURRENT_DOUBLER: j = "currentDoubler"; break;
            case PsfbRectifierType::FULL_BRIDGE: j = "fullBridge"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Application & x) {
        if (j == "interferenceSuppression") x = Application::INTERFERENCE_SUPPRESSION;
        else if (j == "power") x = Application::POWER;
        else if (j == "signalProcessing") x = Application::SIGNAL_PROCESSING;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Application & x) {
        switch (x) {
            case Application::INTERFERENCE_SUPPRESSION: j = "interferenceSuppression"; break;
            case Application::POWER: j = "power"; break;
            case Application::SIGNAL_PROCESSING: j = "signalProcessing"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Cti & x) {
        if (j == "groupI") x = Cti::GROUP_I;
        else if (j == "groupII") x = Cti::GROUP_II;
        else if (j == "groupIIIA") x = Cti::GROUP_IIIA;
        else if (j == "groupIIIB") x = Cti::GROUP_IIIB;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Cti & x) {
        switch (x) {
            case Cti::GROUP_I: j = "groupI"; break;
            case Cti::GROUP_II: j = "groupII"; break;
            case Cti::GROUP_IIIA: j = "groupIIIA"; break;
            case Cti::GROUP_IIIB: j = "groupIIIB"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, InsulationType & x) {
        if (j == "basic") x = InsulationType::BASIC;
        else if (j == "double") x = InsulationType::DOUBLE;
        else if (j == "functional") x = InsulationType::FUNCTIONAL;
        else if (j == "reinforced") x = InsulationType::REINFORCED;
        else if (j == "supplementary") x = InsulationType::SUPPLEMENTARY;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const InsulationType & x) {
        switch (x) {
            case InsulationType::BASIC: j = "basic"; break;
            case InsulationType::DOUBLE: j = "double"; break;
            case InsulationType::FUNCTIONAL: j = "functional"; break;
            case InsulationType::REINFORCED: j = "reinforced"; break;
            case InsulationType::SUPPLEMENTARY: j = "supplementary"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, OvervoltageCategory & x) {
        if (j == "I") x = OvervoltageCategory::I;
        else if (j == "II") x = OvervoltageCategory::II;
        else if (j == "III") x = OvervoltageCategory::III;
        else if (j == "IV") x = OvervoltageCategory::IV;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const OvervoltageCategory & x) {
        switch (x) {
            case OvervoltageCategory::I: j = "I"; break;
            case OvervoltageCategory::II: j = "II"; break;
            case OvervoltageCategory::III: j = "III"; break;
            case OvervoltageCategory::IV: j = "IV"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PollutionDegree & x) {
        if (j == "PD1") x = PollutionDegree::PD1;
        else if (j == "PD2") x = PollutionDegree::PD2;
        else if (j == "PD3") x = PollutionDegree::PD3;
        else if (j == "PD4") x = PollutionDegree::PD4;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const PollutionDegree & x) {
        switch (x) {
            case PollutionDegree::PD1: j = "PD1"; break;
            case PollutionDegree::PD2: j = "PD2"; break;
            case PollutionDegree::PD3: j = "PD3"; break;
            case PollutionDegree::PD4: j = "PD4"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, InsulationStandards & x) {
        if (j == "IEC 60335-1") x = InsulationStandards::IEC_603351;
        else if (j == "IEC 60664-1") x = InsulationStandards::IEC_606641;
        else if (j == "IEC 61558-1") x = InsulationStandards::IEC_615581;
        else if (j == "IEC 62368-1") x = InsulationStandards::IEC_623681;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const InsulationStandards & x) {
        switch (x) {
            case InsulationStandards::IEC_603351: j = "IEC 60335-1"; break;
            case InsulationStandards::IEC_606641: j = "IEC 60664-1"; break;
            case InsulationStandards::IEC_615581: j = "IEC 61558-1"; break;
            case InsulationStandards::IEC_623681: j = "IEC 62368-1"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, IsolationSide & x) {
        if (j == "denary") x = IsolationSide::DENARY;
        else if (j == "duodenary") x = IsolationSide::DUODENARY;
        else if (j == "nonary") x = IsolationSide::NONARY;
        else if (j == "octonary") x = IsolationSide::OCTONARY;
        else if (j == "primary") x = IsolationSide::PRIMARY;
        else if (j == "quaternary") x = IsolationSide::QUATERNARY;
        else if (j == "quinary") x = IsolationSide::QUINARY;
        else if (j == "secondary") x = IsolationSide::SECONDARY;
        else if (j == "senary") x = IsolationSide::SENARY;
        else if (j == "septenary") x = IsolationSide::SEPTENARY;
        else if (j == "tertiary") x = IsolationSide::TERTIARY;
        else if (j == "undenary") x = IsolationSide::UNDENARY;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const IsolationSide & x) {
        switch (x) {
            case IsolationSide::DENARY: j = "denary"; break;
            case IsolationSide::DUODENARY: j = "duodenary"; break;
            case IsolationSide::NONARY: j = "nonary"; break;
            case IsolationSide::OCTONARY: j = "octonary"; break;
            case IsolationSide::PRIMARY: j = "primary"; break;
            case IsolationSide::QUATERNARY: j = "quaternary"; break;
            case IsolationSide::QUINARY: j = "quinary"; break;
            case IsolationSide::SECONDARY: j = "secondary"; break;
            case IsolationSide::SENARY: j = "senary"; break;
            case IsolationSide::SEPTENARY: j = "septenary"; break;
            case IsolationSide::TERTIARY: j = "tertiary"; break;
            case IsolationSide::UNDENARY: j = "undenary"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Market & x) {
        if (j == "commercial") x = Market::COMMERCIAL;
        else if (j == "industrial") x = Market::INDUSTRIAL;
        else if (j == "medical") x = Market::MEDICAL;
        else if (j == "military") x = Market::MILITARY;
        else if (j == "space") x = Market::SPACE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Market & x) {
        switch (x) {
            case Market::COMMERCIAL: j = "commercial"; break;
            case Market::INDUSTRIAL: j = "industrial"; break;
            case Market::MEDICAL: j = "medical"; break;
            case Market::MILITARY: j = "military"; break;
            case Market::SPACE: j = "space"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, SubApplication & x) {
        if (j == "commonModeNoiseFiltering") x = SubApplication::COMMON_MODE_NOISE_FILTERING;
        else if (j == "differentialModeNoiseFiltering") x = SubApplication::DIFFERENTIAL_MODE_NOISE_FILTERING;
        else if (j == "isolation") x = SubApplication::ISOLATION;
        else if (j == "powerFiltering") x = SubApplication::POWER_FILTERING;
        else if (j == "transforming") x = SubApplication::TRANSFORMING;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const SubApplication & x) {
        switch (x) {
            case SubApplication::COMMON_MODE_NOISE_FILTERING: j = "commonModeNoiseFiltering"; break;
            case SubApplication::DIFFERENTIAL_MODE_NOISE_FILTERING: j = "differentialModeNoiseFiltering"; break;
            case SubApplication::ISOLATION: j = "isolation"; break;
            case SubApplication::POWER_FILTERING: j = "powerFiltering"; break;
            case SubApplication::TRANSFORMING: j = "transforming"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ConnectionType & x) {
        if (j == "flyingLead") x = ConnectionType::FLYING_LEAD;
        else if (j == "pcbPad") x = ConnectionType::PCB_PAD;
        else if (j == "pin") x = ConnectionType::PIN;
        else if (j == "screw") x = ConnectionType::SCREW;
        else if (j == "smt") x = ConnectionType::SMT;
        else if (j == "tht") x = ConnectionType::THT;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ConnectionType & x) {
        switch (x) {
            case ConnectionType::FLYING_LEAD: j = "flyingLead"; break;
            case ConnectionType::PCB_PAD: j = "pcbPad"; break;
            case ConnectionType::PIN: j = "pin"; break;
            case ConnectionType::SCREW: j = "screw"; break;
            case ConnectionType::SMT: j = "smt"; break;
            case ConnectionType::THT: j = "tht"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Topologies & x) {
        static std::unordered_map<std::string, Topologies> enumValues {
            {"activeClampForwardConverter", Topologies::ACTIVE_CLAMP_FORWARD_CONVERTER},
            {"boostConverter", Topologies::BOOST_CONVERTER},
            {"buckConverter", Topologies::BUCK_CONVERTER},
            {"cllcResonantConverter", Topologies::CLLC_RESONANT_CONVERTER},
            {"commonModeChoke", Topologies::COMMON_MODE_CHOKE},
            {"cukConverter", Topologies::CUK_CONVERTER},
            {"currentTransformer", Topologies::CURRENT_TRANSFORMER},
            {"differentialModeChoke", Topologies::DIFFERENTIAL_MODE_CHOKE},
            {"dualActiveBridgeConverter", Topologies::DUAL_ACTIVE_BRIDGE_CONVERTER},
            {"flybackConverter", Topologies::FLYBACK_CONVERTER},
            {"fullBridgeConverter", Topologies::FULL_BRIDGE_CONVERTER},
            {"halfBridgeConverter", Topologies::HALF_BRIDGE_CONVERTER},
            {"invertingBuckBoostConverter", Topologies::INVERTING_BUCK_BOOST_CONVERTER},
            {"isolatedBuckBoostConverter", Topologies::ISOLATED_BUCK_BOOST_CONVERTER},
            {"isolatedBuckConverter", Topologies::ISOLATED_BUCK_CONVERTER},
            {"llcResonantConverter", Topologies::LLC_RESONANT_CONVERTER},
            {"phaseShiftedFullBridgeConverter", Topologies::PHASE_SHIFTED_FULL_BRIDGE_CONVERTER},
            {"phaseShiftedHalfBridgeConverter", Topologies::PHASE_SHIFTED_HALF_BRIDGE_CONVERTER},
            {"powerFactorCorrection", Topologies::POWER_FACTOR_CORRECTION},
            {"pushPullConverter", Topologies::PUSH_PULL_CONVERTER},
            {"SEPIC", Topologies::SEPIC},
            {"singleSwitchForwardConverter", Topologies::SINGLE_SWITCH_FORWARD_CONVERTER},
            {"twoSwitchForwardConverter", Topologies::TWO_SWITCH_FORWARD_CONVERTER},
            {"weinbergConverter", Topologies::WEINBERG_CONVERTER},
            {"zetaConverter", Topologies::ZETA_CONVERTER},
        };
        auto iter = enumValues.find(j.get<std::string>());
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const Topologies & x) {
        switch (x) {
            case Topologies::ACTIVE_CLAMP_FORWARD_CONVERTER: j = "activeClampForwardConverter"; break;
            case Topologies::BOOST_CONVERTER: j = "boostConverter"; break;
            case Topologies::BUCK_CONVERTER: j = "buckConverter"; break;
            case Topologies::CLLC_RESONANT_CONVERTER: j = "cllcResonantConverter"; break;
            case Topologies::COMMON_MODE_CHOKE: j = "commonModeChoke"; break;
            case Topologies::CUK_CONVERTER: j = "cukConverter"; break;
            case Topologies::CURRENT_TRANSFORMER: j = "currentTransformer"; break;
            case Topologies::DIFFERENTIAL_MODE_CHOKE: j = "differentialModeChoke"; break;
            case Topologies::DUAL_ACTIVE_BRIDGE_CONVERTER: j = "dualActiveBridgeConverter"; break;
            case Topologies::FLYBACK_CONVERTER: j = "flybackConverter"; break;
            case Topologies::FULL_BRIDGE_CONVERTER: j = "fullBridgeConverter"; break;
            case Topologies::HALF_BRIDGE_CONVERTER: j = "halfBridgeConverter"; break;
            case Topologies::INVERTING_BUCK_BOOST_CONVERTER: j = "invertingBuckBoostConverter"; break;
            case Topologies::ISOLATED_BUCK_BOOST_CONVERTER: j = "isolatedBuckBoostConverter"; break;
            case Topologies::ISOLATED_BUCK_CONVERTER: j = "isolatedBuckConverter"; break;
            case Topologies::LLC_RESONANT_CONVERTER: j = "llcResonantConverter"; break;
            case Topologies::PHASE_SHIFTED_FULL_BRIDGE_CONVERTER: j = "phaseShiftedFullBridgeConverter"; break;
            case Topologies::PHASE_SHIFTED_HALF_BRIDGE_CONVERTER: j = "phaseShiftedHalfBridgeConverter"; break;
            case Topologies::POWER_FACTOR_CORRECTION: j = "powerFactorCorrection"; break;
            case Topologies::PUSH_PULL_CONVERTER: j = "pushPullConverter"; break;
            case Topologies::SEPIC: j = "SEPIC"; break;
            case Topologies::SINGLE_SWITCH_FORWARD_CONVERTER: j = "singleSwitchForwardConverter"; break;
            case Topologies::TWO_SWITCH_FORWARD_CONVERTER: j = "twoSwitchForwardConverter"; break;
            case Topologies::WEINBERG_CONVERTER: j = "weinbergConverter"; break;
            case Topologies::ZETA_CONVERTER: j = "zetaConverter"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WiringTechnology & x) {
        if (j == "deposition") x = WiringTechnology::DEPOSITION;
        else if (j == "printed") x = WiringTechnology::PRINTED;
        else if (j == "stamped") x = WiringTechnology::STAMPED;
        else if (j == "wound") x = WiringTechnology::WOUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WiringTechnology & x) {
        switch (x) {
            case WiringTechnology::DEPOSITION: j = "deposition"; break;
            case WiringTechnology::PRINTED: j = "printed"; break;
            case WiringTechnology::STAMPED: j = "stamped"; break;
            case WiringTechnology::WOUND: j = "wound"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, BobbinFamily & x) {
        if (j == "e") x = BobbinFamily::E;
        else if (j == "ec") x = BobbinFamily::EC;
        else if (j == "efd") x = BobbinFamily::EFD;
        else if (j == "el") x = BobbinFamily::EL;
        else if (j == "ep") x = BobbinFamily::EP;
        else if (j == "er") x = BobbinFamily::ER;
        else if (j == "etd") x = BobbinFamily::ETD;
        else if (j == "p") x = BobbinFamily::P;
        else if (j == "pm") x = BobbinFamily::PM;
        else if (j == "pq") x = BobbinFamily::PQ;
        else if (j == "rm") x = BobbinFamily::RM;
        else if (j == "t") x = BobbinFamily::T;
        else if (j == "u") x = BobbinFamily::U;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const BobbinFamily & x) {
        switch (x) {
            case BobbinFamily::E: j = "e"; break;
            case BobbinFamily::EC: j = "ec"; break;
            case BobbinFamily::EFD: j = "efd"; break;
            case BobbinFamily::EL: j = "el"; break;
            case BobbinFamily::EP: j = "ep"; break;
            case BobbinFamily::ER: j = "er"; break;
            case BobbinFamily::ETD: j = "etd"; break;
            case BobbinFamily::P: j = "p"; break;
            case BobbinFamily::PM: j = "pm"; break;
            case BobbinFamily::PQ: j = "pq"; break;
            case BobbinFamily::RM: j = "rm"; break;
            case BobbinFamily::T: j = "t"; break;
            case BobbinFamily::U: j = "u"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Status & x) {
        if (j == "obsolete") x = Status::OBSOLETE;
        else if (j == "production") x = Status::PRODUCTION;
        else if (j == "prototype") x = Status::PROTOTYPE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Status & x) {
        switch (x) {
            case Status::OBSOLETE: j = "obsolete"; break;
            case Status::PRODUCTION: j = "production"; break;
            case Status::PROTOTYPE: j = "prototype"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, TemperatureClassEnum & x) {
        if (j == "A") x = TemperatureClassEnum::A;
        else if (j == "B") x = TemperatureClassEnum::B;
        else if (j == "E") x = TemperatureClassEnum::E;
        else if (j == "F") x = TemperatureClassEnum::F;
        else if (j == "H") x = TemperatureClassEnum::H;
        else if (j == "N") x = TemperatureClassEnum::N;
        else if (j == "R") x = TemperatureClassEnum::R;
        else if (j == "200") x = TemperatureClassEnum::THE_200;
        else if (j == "220") x = TemperatureClassEnum::THE_220;
        else if (j == "250") x = TemperatureClassEnum::THE_250;
        else if (j == "Y") x = TemperatureClassEnum::Y;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TemperatureClassEnum & x) {
        switch (x) {
            case TemperatureClassEnum::A: j = "A"; break;
            case TemperatureClassEnum::B: j = "B"; break;
            case TemperatureClassEnum::E: j = "E"; break;
            case TemperatureClassEnum::F: j = "F"; break;
            case TemperatureClassEnum::H: j = "H"; break;
            case TemperatureClassEnum::N: j = "N"; break;
            case TemperatureClassEnum::R: j = "R"; break;
            case TemperatureClassEnum::THE_200: j = "200"; break;
            case TemperatureClassEnum::THE_220: j = "220"; break;
            case TemperatureClassEnum::THE_250: j = "250"; break;
            case TemperatureClassEnum::Y: j = "Y"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PinShape & x) {
        if (j == "irregular") x = PinShape::IRREGULAR;
        else if (j == "rectangular") x = PinShape::RECTANGULAR;
        else if (j == "round") x = PinShape::ROUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const PinShape & x) {
        switch (x) {
            case PinShape::IRREGULAR: j = "irregular"; break;
            case PinShape::RECTANGULAR: j = "rectangular"; break;
            case PinShape::ROUND: j = "round"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, PinDescriptionType & x) {
        if (j == "smd") x = PinDescriptionType::SMD;
        else if (j == "tht") x = PinDescriptionType::THT;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const PinDescriptionType & x) {
        switch (x) {
            case PinDescriptionType::SMD: j = "smd"; break;
            case PinDescriptionType::THT: j = "tht"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, FunctionalDescriptionType & x) {
        if (j == "custom") x = FunctionalDescriptionType::CUSTOM;
        else if (j == "standard") x = FunctionalDescriptionType::STANDARD;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const FunctionalDescriptionType & x) {
        switch (x) {
            case FunctionalDescriptionType::CUSTOM: j = "custom"; break;
            case FunctionalDescriptionType::STANDARD: j = "standard"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ColumnShape & x) {
        if (j == "irregular") x = ColumnShape::IRREGULAR;
        else if (j == "oblong") x = ColumnShape::OBLONG;
        else if (j == "rectangular") x = ColumnShape::RECTANGULAR;
        else if (j == "round") x = ColumnShape::ROUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ColumnShape & x) {
        switch (x) {
            case ColumnShape::IRREGULAR: j = "irregular"; break;
            case ColumnShape::OBLONG: j = "oblong"; break;
            case ColumnShape::RECTANGULAR: j = "rectangular"; break;
            case ColumnShape::ROUND: j = "round"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoilAlignment & x) {
        if (j == "centered") x = CoilAlignment::CENTERED;
        else if (j == "innerOrTop") x = CoilAlignment::INNER_OR_TOP;
        else if (j == "outerOrBottom") x = CoilAlignment::OUTER_OR_BOTTOM;
        else if (j == "spread") x = CoilAlignment::SPREAD;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CoilAlignment & x) {
        switch (x) {
            case CoilAlignment::CENTERED: j = "centered"; break;
            case CoilAlignment::INNER_OR_TOP: j = "innerOrTop"; break;
            case CoilAlignment::OUTER_OR_BOTTOM: j = "outerOrBottom"; break;
            case CoilAlignment::SPREAD: j = "spread"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WindingOrientation & x) {
        if (j == "contiguous") x = WindingOrientation::CONTIGUOUS;
        else if (j == "overlapping") x = WindingOrientation::OVERLAPPING;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WindingOrientation & x) {
        switch (x) {
            case WindingOrientation::CONTIGUOUS: j = "contiguous"; break;
            case WindingOrientation::OVERLAPPING: j = "overlapping"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WindingWindowShape & x) {
        if (j == "rectangular") x = WindingWindowShape::RECTANGULAR;
        else if (j == "round") x = WindingWindowShape::ROUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WindingWindowShape & x) {
        switch (x) {
            case WindingWindowShape::RECTANGULAR: j = "rectangular"; break;
            case WindingWindowShape::ROUND: j = "round"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Direction & x) {
        if (j == "input") x = Direction::INPUT;
        else if (j == "output") x = Direction::OUTPUT;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Direction & x) {
        switch (x) {
            case Direction::INPUT: j = "input"; break;
            case Direction::OUTPUT: j = "output"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, InsulationWireCoatingType & x) {
        if (j == "bare") x = InsulationWireCoatingType::BARE;
        else if (j == "enamelled") x = InsulationWireCoatingType::ENAMELLED;
        else if (j == "extruded") x = InsulationWireCoatingType::EXTRUDED;
        else if (j == "insulated") x = InsulationWireCoatingType::INSULATED;
        else if (j == "served") x = InsulationWireCoatingType::SERVED;
        else if (j == "taped") x = InsulationWireCoatingType::TAPED;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const InsulationWireCoatingType & x) {
        switch (x) {
            case InsulationWireCoatingType::BARE: j = "bare"; break;
            case InsulationWireCoatingType::ENAMELLED: j = "enamelled"; break;
            case InsulationWireCoatingType::EXTRUDED: j = "extruded"; break;
            case InsulationWireCoatingType::INSULATED: j = "insulated"; break;
            case InsulationWireCoatingType::SERVED: j = "served"; break;
            case InsulationWireCoatingType::TAPED: j = "taped"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WireStandard & x) {
        if (j == "IEC 60317") x = WireStandard::IEC_60317;
        else if (j == "IPC-6012") x = WireStandard::IPC_6012;
        else if (j == "NEMA MW 1000 C") x = WireStandard::NEMA_MW_1000_C;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WireStandard & x) {
        switch (x) {
            case WireStandard::IEC_60317: j = "IEC 60317"; break;
            case WireStandard::IPC_6012: j = "IPC-6012"; break;
            case WireStandard::NEMA_MW_1000_C: j = "NEMA MW 1000 C"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WireType & x) {
        if (j == "foil") x = WireType::FOIL;
        else if (j == "litz") x = WireType::LITZ;
        else if (j == "planar") x = WireType::PLANAR;
        else if (j == "rectangular") x = WireType::RECTANGULAR;
        else if (j == "round") x = WireType::ROUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WireType & x) {
        switch (x) {
            case WireType::FOIL: j = "foil"; break;
            case WireType::LITZ: j = "litz"; break;
            case WireType::PLANAR: j = "planar"; break;
            case WireType::RECTANGULAR: j = "rectangular"; break;
            case WireType::ROUND: j = "round"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoordinateSystem & x) {
        if (j == "cartesian") x = CoordinateSystem::CARTESIAN;
        else if (j == "cylindrical") x = CoordinateSystem::CYLINDRICAL;
        else if (j == "polar") x = CoordinateSystem::POLAR;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CoordinateSystem & x) {
        switch (x) {
            case CoordinateSystem::CARTESIAN: j = "cartesian"; break;
            case CoordinateSystem::CYLINDRICAL: j = "cylindrical"; break;
            case CoordinateSystem::POLAR: j = "polar"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ElectricalType & x) {
        if (j == "conduction") x = ElectricalType::CONDUCTION;
        else if (j == "insulation") x = ElectricalType::INSULATION;
        else if (j == "shielding") x = ElectricalType::SHIELDING;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ElectricalType & x) {
        switch (x) {
            case ElectricalType::CONDUCTION: j = "conduction"; break;
            case ElectricalType::INSULATION: j = "insulation"; break;
            case ElectricalType::SHIELDING: j = "shielding"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, WindingStyle & x) {
        if (j == "windByConsecutiveParallels") x = WindingStyle::WIND_BY_CONSECUTIVE_PARALLELS;
        else if (j == "windByConsecutiveTurns") x = WindingStyle::WIND_BY_CONSECUTIVE_TURNS;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const WindingStyle & x) {
        switch (x) {
            case WindingStyle::WIND_BY_CONSECUTIVE_PARALLELS: j = "windByConsecutiveParallels"; break;
            case WindingStyle::WIND_BY_CONSECUTIVE_TURNS: j = "windByConsecutiveTurns"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, TurnCrossSectionalShape & x) {
        if (j == "oval") x = TurnCrossSectionalShape::OVAL;
        else if (j == "rectangular") x = TurnCrossSectionalShape::RECTANGULAR;
        else if (j == "round") x = TurnCrossSectionalShape::ROUND;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TurnCrossSectionalShape & x) {
        switch (x) {
            case TurnCrossSectionalShape::OVAL: j = "oval"; break;
            case TurnCrossSectionalShape::RECTANGULAR: j = "rectangular"; break;
            case TurnCrossSectionalShape::ROUND: j = "round"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, TurnOrientation & x) {
        if (j == "clockwise") x = TurnOrientation::CLOCKWISE;
        else if (j == "counterClockwise") x = TurnOrientation::COUNTER_CLOCKWISE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const TurnOrientation & x) {
        switch (x) {
            case TurnOrientation::CLOCKWISE: j = "clockwise"; break;
            case TurnOrientation::COUNTER_CLOCKWISE: j = "counterClockwise"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, Coating & x) {
        if (j == "epoxy") x = Coating::EPOXY;
        else if (j == "parylene") x = Coating::PARYLENE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const Coating & x) {
        switch (x) {
            case Coating::EPOXY: j = "epoxy"; break;
            case Coating::PARYLENE: j = "parylene"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, GapType & x) {
        if (j == "additive") x = GapType::ADDITIVE;
        else if (j == "residual") x = GapType::RESIDUAL;
        else if (j == "subtractive") x = GapType::SUBTRACTIVE;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const GapType & x) {
        switch (x) {
            case GapType::ADDITIVE: j = "additive"; break;
            case GapType::RESIDUAL: j = "residual"; break;
            case GapType::SUBTRACTIVE: j = "subtractive"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MassCoreLossesMethodType & x) {
        if (j == "magnetec") x = MassCoreLossesMethodType::MAGNETEC;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MassCoreLossesMethodType & x) {
        switch (x) {
            case MassCoreLossesMethodType::MAGNETEC: j = "magnetec"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MaterialType & x) {
        if (j == "amorphous") x = MaterialType::AMORPHOUS;
        else if (j == "electricalSteel") x = MaterialType::ELECTRICAL_STEEL;
        else if (j == "ferrite") x = MaterialType::FERRITE;
        else if (j == "nanocrystalline") x = MaterialType::NANOCRYSTALLINE;
        else if (j == "powder") x = MaterialType::POWDER;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MaterialType & x) {
        switch (x) {
            case MaterialType::AMORPHOUS: j = "amorphous"; break;
            case MaterialType::ELECTRICAL_STEEL: j = "electricalSteel"; break;
            case MaterialType::FERRITE: j = "ferrite"; break;
            case MaterialType::NANOCRYSTALLINE: j = "nanocrystalline"; break;
            case MaterialType::POWDER: j = "powder"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MaterialComposition & x) {
        if (j == "carbonylIron") x = MaterialComposition::CARBONYL_IRON;
        else if (j == "FeMo") x = MaterialComposition::FE_MO;
        else if (j == "FeNi") x = MaterialComposition::FE_NI;
        else if (j == "FeNiMo") x = MaterialComposition::FE_NI_MO;
        else if (j == "FeSi") x = MaterialComposition::FE_SI;
        else if (j == "FeSiAl") x = MaterialComposition::FE_SI_AL;
        else if (j == "iron") x = MaterialComposition::IRON;
        else if (j == "MgZn") x = MaterialComposition::MG_ZN;
        else if (j == "MnZn") x = MaterialComposition::MN_ZN;
        else if (j == "NiZn") x = MaterialComposition::NI_ZN;
        else if (j == "proprietary") x = MaterialComposition::PROPRIETARY;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MaterialComposition & x) {
        switch (x) {
            case MaterialComposition::CARBONYL_IRON: j = "carbonylIron"; break;
            case MaterialComposition::FE_MO: j = "FeMo"; break;
            case MaterialComposition::FE_NI: j = "FeNi"; break;
            case MaterialComposition::FE_NI_MO: j = "FeNiMo"; break;
            case MaterialComposition::FE_SI: j = "FeSi"; break;
            case MaterialComposition::FE_SI_AL: j = "FeSiAl"; break;
            case MaterialComposition::IRON: j = "iron"; break;
            case MaterialComposition::MG_ZN: j = "MgZn"; break;
            case MaterialComposition::MN_ZN: j = "MnZn"; break;
            case MaterialComposition::NI_ZN: j = "NiZn"; break;
            case MaterialComposition::PROPRIETARY: j = "proprietary"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, InitialPermeabilitModifierMethod & x) {
        if (j == "fair-rite") x = InitialPermeabilitModifierMethod::FAIR_RITE;
        else if (j == "magnetics") x = InitialPermeabilitModifierMethod::MAGNETICS;
        else if (j == "micrometals") x = InitialPermeabilitModifierMethod::MICROMETALS;
        else if (j == "poco") x = InitialPermeabilitModifierMethod::POCO;
        else if (j == "tdg") x = InitialPermeabilitModifierMethod::TDG;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const InitialPermeabilitModifierMethod & x) {
        switch (x) {
            case InitialPermeabilitModifierMethod::FAIR_RITE: j = "fair-rite"; break;
            case InitialPermeabilitModifierMethod::MAGNETICS: j = "magnetics"; break;
            case InitialPermeabilitModifierMethod::MICROMETALS: j = "micrometals"; break;
            case InitialPermeabilitModifierMethod::POCO: j = "poco"; break;
            case InitialPermeabilitModifierMethod::TDG: j = "tdg"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoreMaterialType & x) {
        if (j == "commercial") x = CoreMaterialType::COMMERCIAL;
        else if (j == "custom") x = CoreMaterialType::CUSTOM;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CoreMaterialType & x) {
        switch (x) {
            case CoreMaterialType::COMMERCIAL: j = "commercial"; break;
            case CoreMaterialType::CUSTOM: j = "custom"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, VolumetricCoreLossesMethodType & x) {
        if (j == "lossFactor") x = VolumetricCoreLossesMethodType::LOSS_FACTOR;
        else if (j == "magnetics") x = VolumetricCoreLossesMethodType::MAGNETICS;
        else if (j == "micrometals") x = VolumetricCoreLossesMethodType::MICROMETALS;
        else if (j == "poco") x = VolumetricCoreLossesMethodType::POCO;
        else if (j == "roshen") x = VolumetricCoreLossesMethodType::ROSHEN;
        else if (j == "steinmetz") x = VolumetricCoreLossesMethodType::STEINMETZ;
        else if (j == "tdg") x = VolumetricCoreLossesMethodType::TDG;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const VolumetricCoreLossesMethodType & x) {
        switch (x) {
            case VolumetricCoreLossesMethodType::LOSS_FACTOR: j = "lossFactor"; break;
            case VolumetricCoreLossesMethodType::MAGNETICS: j = "magnetics"; break;
            case VolumetricCoreLossesMethodType::MICROMETALS: j = "micrometals"; break;
            case VolumetricCoreLossesMethodType::POCO: j = "poco"; break;
            case VolumetricCoreLossesMethodType::ROSHEN: j = "roshen"; break;
            case VolumetricCoreLossesMethodType::STEINMETZ: j = "steinmetz"; break;
            case VolumetricCoreLossesMethodType::TDG: j = "tdg"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoreShapeFamily & x) {
        static std::unordered_map<std::string, CoreShapeFamily> enumValues {
            {"c", CoreShapeFamily::C},
            {"drum", CoreShapeFamily::DRUM},
            {"e", CoreShapeFamily::E},
            {"ec", CoreShapeFamily::EC},
            {"efd", CoreShapeFamily::EFD},
            {"ei", CoreShapeFamily::EI},
            {"el", CoreShapeFamily::EL},
            {"elp", CoreShapeFamily::ELP},
            {"ep", CoreShapeFamily::EP},
            {"epx", CoreShapeFamily::EPX},
            {"eq", CoreShapeFamily::EQ},
            {"er", CoreShapeFamily::ER},
            {"etd", CoreShapeFamily::ETD},
            {"h", CoreShapeFamily::H},
            {"lp", CoreShapeFamily::LP},
            {"p", CoreShapeFamily::P},
            {"planarE", CoreShapeFamily::PLANAR_E},
            {"planarEL", CoreShapeFamily::PLANAR_EL},
            {"planarER", CoreShapeFamily::PLANAR_ER},
            {"pm", CoreShapeFamily::PM},
            {"pq", CoreShapeFamily::PQ},
            {"pqi", CoreShapeFamily::PQI},
            {"rm", CoreShapeFamily::RM},
            {"rod", CoreShapeFamily::ROD},
            {"t", CoreShapeFamily::T},
            {"u", CoreShapeFamily::U},
            {"ui", CoreShapeFamily::UI},
            {"ur", CoreShapeFamily::UR},
            {"ut", CoreShapeFamily::UT},
        };
        auto iter = enumValues.find(j.get<std::string>());
        if (iter != enumValues.end()) {
            x = iter->second;
        }
    }

    inline void to_json(json & j, const CoreShapeFamily & x) {
        switch (x) {
            case CoreShapeFamily::C: j = "c"; break;
            case CoreShapeFamily::DRUM: j = "drum"; break;
            case CoreShapeFamily::E: j = "e"; break;
            case CoreShapeFamily::EC: j = "ec"; break;
            case CoreShapeFamily::EFD: j = "efd"; break;
            case CoreShapeFamily::EI: j = "ei"; break;
            case CoreShapeFamily::EL: j = "el"; break;
            case CoreShapeFamily::ELP: j = "elp"; break;
            case CoreShapeFamily::EP: j = "ep"; break;
            case CoreShapeFamily::EPX: j = "epx"; break;
            case CoreShapeFamily::EQ: j = "eq"; break;
            case CoreShapeFamily::ER: j = "er"; break;
            case CoreShapeFamily::ETD: j = "etd"; break;
            case CoreShapeFamily::H: j = "h"; break;
            case CoreShapeFamily::LP: j = "lp"; break;
            case CoreShapeFamily::P: j = "p"; break;
            case CoreShapeFamily::PLANAR_E: j = "planarE"; break;
            case CoreShapeFamily::PLANAR_EL: j = "planarEL"; break;
            case CoreShapeFamily::PLANAR_ER: j = "planarER"; break;
            case CoreShapeFamily::PM: j = "pm"; break;
            case CoreShapeFamily::PQ: j = "pq"; break;
            case CoreShapeFamily::PQI: j = "pqi"; break;
            case CoreShapeFamily::RM: j = "rm"; break;
            case CoreShapeFamily::ROD: j = "rod"; break;
            case CoreShapeFamily::T: j = "t"; break;
            case CoreShapeFamily::U: j = "u"; break;
            case CoreShapeFamily::UI: j = "ui"; break;
            case CoreShapeFamily::UR: j = "ur"; break;
            case CoreShapeFamily::UT: j = "ut"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MagneticCircuit & x) {
        if (j == "closed") x = MagneticCircuit::CLOSED;
        else if (j == "open") x = MagneticCircuit::OPEN;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MagneticCircuit & x) {
        switch (x) {
            case MagneticCircuit::CLOSED: j = "closed"; break;
            case MagneticCircuit::OPEN: j = "open"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoreType & x) {
        if (j == "closedShape") x = CoreType::CLOSED_SHAPE;
        else if (j == "pieceAndPlate") x = CoreType::PIECE_AND_PLATE;
        else if (j == "toroidal") x = CoreType::TOROIDAL;
        else if (j == "twoPieceSet") x = CoreType::TWO_PIECE_SET;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CoreType & x) {
        switch (x) {
            case CoreType::CLOSED_SHAPE: j = "closedShape"; break;
            case CoreType::PIECE_AND_PLATE: j = "pieceAndPlate"; break;
            case CoreType::TOROIDAL: j = "toroidal"; break;
            case CoreType::TWO_PIECE_SET: j = "twoPieceSet"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, CoreGeometricalDescriptionElementType & x) {
        if (j == "closed") x = CoreGeometricalDescriptionElementType::CLOSED;
        else if (j == "halfSet") x = CoreGeometricalDescriptionElementType::HALF_SET;
        else if (j == "plate") x = CoreGeometricalDescriptionElementType::PLATE;
        else if (j == "sheet") x = CoreGeometricalDescriptionElementType::SHEET;
        else if (j == "spacer") x = CoreGeometricalDescriptionElementType::SPACER;
        else if (j == "toroidal") x = CoreGeometricalDescriptionElementType::TOROIDAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const CoreGeometricalDescriptionElementType & x) {
        switch (x) {
            case CoreGeometricalDescriptionElementType::CLOSED: j = "closed"; break;
            case CoreGeometricalDescriptionElementType::HALF_SET: j = "halfSet"; break;
            case CoreGeometricalDescriptionElementType::PLATE: j = "plate"; break;
            case CoreGeometricalDescriptionElementType::SHEET: j = "sheet"; break;
            case CoreGeometricalDescriptionElementType::SPACER: j = "spacer"; break;
            case CoreGeometricalDescriptionElementType::TOROIDAL: j = "toroidal"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ColumnType & x) {
        if (j == "central") x = ColumnType::CENTRAL;
        else if (j == "lateral") x = ColumnType::LATERAL;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ColumnType & x) {
        switch (x) {
            case ColumnType::CENTRAL: j = "central"; break;
            case ColumnType::LATERAL: j = "lateral"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, MasConformance & x) {
        if (j == "A") x = MasConformance::A;
        else if (j == "B") x = MasConformance::B;
        else if (j == "C") x = MasConformance::C;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const MasConformance & x) {
        switch (x) {
            case MasConformance::A: j = "A"; break;
            case MasConformance::B: j = "B"; break;
            case MasConformance::C: j = "C"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, ResultOrigin & x) {
        if (j == "manufacturer") x = ResultOrigin::MANUFACTURER;
        else if (j == "measurement") x = ResultOrigin::MEASUREMENT;
        else if (j == "simulation") x = ResultOrigin::SIMULATION;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const ResultOrigin & x) {
        switch (x) {
            case ResultOrigin::MANUFACTURER: j = "manufacturer"; break;
            case ResultOrigin::MEASUREMENT: j = "measurement"; break;
            case ResultOrigin::SIMULATION: j = "simulation"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }

    inline void from_json(const json & j, VoltageType & x) {
        if (j == "AC") x = VoltageType::AC;
        else if (j == "DC") x = VoltageType::DC;
        else { throw std::runtime_error("Input JSON does not conform to schema!"); }
    }

    inline void to_json(json & j, const VoltageType & x) {
        switch (x) {
            case VoltageType::AC: j = "AC"; break;
            case VoltageType::DC: j = "DC"; break;
            default: throw std::runtime_error("Unexpected value in enumeration \"[object Object]\": " + std::to_string(static_cast<int>(x)));
        }
    }
}
namespace nlohmann {
    inline void adl_serializer<std::variant<MAS::DimensionWithTolerance, double>>::from_json(const json & j, std::variant<MAS::DimensionWithTolerance, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_object())
            x = j.get<MAS::DimensionWithTolerance>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::DimensionWithTolerance, double>>::to_json(json & j, const std::variant<MAS::DimensionWithTolerance, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::DimensionWithTolerance>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<double, MAS::TemperatureClassEnum>>::from_json(const json & j, std::variant<double, MAS::TemperatureClassEnum> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_string())
            x = j.get<MAS::TemperatureClassEnum>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<double, MAS::TemperatureClassEnum>>::to_json(json & j, const std::variant<double, MAS::TemperatureClassEnum> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<double>(x);
                break;
            case 1:
                j = std::get<MAS::TemperatureClassEnum>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::InsulationMaterial, std::string>>::from_json(const json & j, std::variant<MAS::InsulationMaterial, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::InsulationMaterial>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::InsulationMaterial, std::string>>::to_json(json & j, const std::variant<MAS::InsulationMaterial, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::InsulationMaterial>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<double>, double>>::from_json(const json & j, std::variant<std::vector<double>, double> & x) {
        if (j.is_number())
            x = j.get<double>();
        else if (j.is_array())
            x = j.get<std::vector<double>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<double>, double>>::to_json(json & j, const std::variant<std::vector<double>, double> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<double>>(x);
                break;
            case 1:
                j = std::get<double>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::Bobbin, std::string>>::from_json(const json & j, std::variant<MAS::Bobbin, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::Bobbin>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::Bobbin, std::string>>::to_json(json & j, const std::variant<MAS::Bobbin, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::Bobbin>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string>>::from_json(const json & j, std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::Bobbin>();
        else if (j.is_array())
            x = j.get<std::vector<MAS::BobbinDataOrNameUnionElement>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string>>::to_json(json & j, const std::variant<std::vector<MAS::BobbinDataOrNameUnionElement>, MAS::Bobbin, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<MAS::BobbinDataOrNameUnionElement>>(x);
                break;
            case 1:
                j = std::get<MAS::Bobbin>(x);
                break;
            case 2:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::InsulationWireCoating, std::string>>::from_json(const json & j, std::variant<MAS::InsulationWireCoating, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::InsulationWireCoating>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::InsulationWireCoating, std::string>>::to_json(json & j, const std::variant<MAS::InsulationWireCoating, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::InsulationWireCoating>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::WireMaterial, std::string>>::from_json(const json & j, std::variant<MAS::WireMaterial, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::WireMaterial>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::WireMaterial, std::string>>::to_json(json & j, const std::variant<MAS::WireMaterial, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::WireMaterial>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::WireRound, std::string>>::from_json(const json & j, std::variant<MAS::WireRound, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::WireRound>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::WireRound, std::string>>::to_json(json & j, const std::variant<MAS::WireRound, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::WireRound>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::Wire, std::string>>::from_json(const json & j, std::variant<MAS::Wire, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::Wire>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::Wire, std::string>>::to_json(json & j, const std::variant<MAS::Wire, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::Wire>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<double>, MAS::MarginInfo>>::from_json(const json & j, std::variant<std::vector<double>, MAS::MarginInfo> & x) {
        if (j.is_object())
            x = j.get<MAS::MarginInfo>();
        else if (j.is_array())
            x = j.get<std::vector<double>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<double>, MAS::MarginInfo>>::to_json(json & j, const std::variant<std::vector<double>, MAS::MarginInfo> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<double>>(x);
                break;
            case 1:
                j = std::get<MAS::MarginInfo>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData>>::from_json(const json & j, std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData> & x) {
        if (j.is_object())
            x = j.get<MAS::MagnetecCoreLossesMethodData>();
        else if (j.is_array())
            x = j.get<std::vector<MAS::MassLossesPoint>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData>>::to_json(json & j, const std::variant<std::vector<MAS::MassLossesPoint>, MAS::MagnetecCoreLossesMethodData> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<MAS::MassLossesPoint>>(x);
                break;
            case 1:
                j = std::get<MAS::MagnetecCoreLossesMethodData>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint>>::from_json(const json & j, std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint> & x) {
        if (j.is_object())
            x = j.get<MAS::PermeabilityPoint>();
        else if (j.is_array())
            x = j.get<std::vector<MAS::PermeabilityPoint>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint>>::to_json(json & j, const std::variant<std::vector<MAS::PermeabilityPoint>, MAS::PermeabilityPoint> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<MAS::PermeabilityPoint>>(x);
                break;
            case 1:
                j = std::get<MAS::PermeabilityPoint>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData>>::from_json(const json & j, std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData> & x) {
        if (j.is_object())
            x = j.get<MAS::CoreLossesMethodData>();
        else if (j.is_array())
            x = j.get<std::vector<MAS::VolumetricLossesPoint>>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData>>::to_json(json & j, const std::variant<std::vector<MAS::VolumetricLossesPoint>, MAS::CoreLossesMethodData> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<std::vector<MAS::VolumetricLossesPoint>>(x);
                break;
            case 1:
                j = std::get<MAS::CoreLossesMethodData>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::CoreMaterial, std::string>>::from_json(const json & j, std::variant<MAS::CoreMaterial, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::CoreMaterial>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::CoreMaterial, std::string>>::to_json(json & j, const std::variant<MAS::CoreMaterial, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::CoreMaterial>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }

    inline void adl_serializer<std::variant<MAS::CoreShape, std::string>>::from_json(const json & j, std::variant<MAS::CoreShape, std::string> & x) {
        if (j.is_string())
            x = j.get<std::string>();
        else if (j.is_object())
            x = j.get<MAS::CoreShape>();
        else throw std::runtime_error("Could not deserialise!");
    }

    inline void adl_serializer<std::variant<MAS::CoreShape, std::string>>::to_json(json & j, const std::variant<MAS::CoreShape, std::string> & x) {
        switch (x.index()) {
            case 0:
                j = std::get<MAS::CoreShape>(x);
                break;
            case 1:
                j = std::get<std::string>(x);
                break;
            default: throw std::runtime_error("Input JSON does not conform to schema!");
        }
    }
}
