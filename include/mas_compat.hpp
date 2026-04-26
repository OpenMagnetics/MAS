// MAS — backwards-compatibility shim for loading pre-1.0 documents
//
// The 1.0 release renamed many enum values (RFC 0007 camelCase sweep,
// RFC 0008 IEC 60664 spelling). Documents written against 0.x carry
// the old spellings and would otherwise fail to deserialize through
// the quicktype-generated MAS.hpp.
//
// This header provides:
//   - mas_compat::MAPPING                — old -> new enum string table
//   - mas_compat::migrate(nlohmann::json&) — rewrite an arbitrary MAS
//     document in place to 1.0 spellings
//   - mas_compat::parse(std::string_view) — convenience: json::parse +
//     migrate
//
// The mapping is the C++ port of scripts/migrate-to-1.0.py and is the
// single source of truth on the C++ side. If you update the mapping
// in one place, update the other.
//
// Header-only. Requires nlohmann::json.
//
// Includes a small set of typedef aliases for topology operating-point
// classes that quicktype merged into BaseOperatingPoint at 1.0 (because
// their schema $defs are pure $refs to baseOperatingPoint with no extras).
// Pre-1.0 consumers referenced these by name; the typedefs keep that
// source compatible. To activate them, also include MAS.hpp before
// using any *OperatingPoint type:
//
//     #include <MAS.hpp>
//     #include <mas_compat.hpp>
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

// Topology operating-point typedefs. Six topologies use $ref baseOperatingPoint
// with no extras after RFC 0006; quicktype emits only BaseOperatingPoint for
// them. These typedefs preserve the pre-1.0 names so source code that referenced
// them keeps compiling.
//
// MAS.hpp must be included before this header for these typedefs to expand.
#if defined(QUICKTYPE_MAS_HPP) || __has_include(<MAS.hpp>)
namespace MAS {
    using ForwardOperatingPoint           = BaseOperatingPoint;
    using PushPullOperatingPoint          = BaseOperatingPoint;
    using LlcOperatingPoint               = BaseOperatingPoint;
    using IsolatedBuckOperatingPoint      = BaseOperatingPoint;
    using IsolatedBuckBoostOperatingPoint = BaseOperatingPoint;
    using FlybuckOperatingPoint           = BaseOperatingPoint;
}
#endif

namespace mas_compat {

// Old-spelling -> new-spelling enum mapping. Mirror of
// scripts/migrate-to-1.0.py MAPPING.
inline const std::unordered_map<std::string, std::string>& mapping() {
    static const std::unordered_map<std::string, std::string> m = {
        // insulationType
        {"Functional", "functional"},
        {"Basic", "basic"},
        {"Supplementary", "supplementary"},
        {"Double", "double"},
        {"Reinforced", "reinforced"},
        // wiringTechnology
        {"Wound", "wound"},
        {"Printed", "printed"},
        {"Stamped", "stamped"},
        {"Deposition", "deposition"},
        // application
        {"Power", "power"},
        {"Signal Processing", "signalProcessing"},
        {"Interference Suppression", "interferenceSuppression"},
        // subApplication
        {"Power Filtering", "powerFiltering"},
        {"Transforming", "transforming"},
        {"Isolation", "isolation"},
        {"Common Mode Noise Filtering", "commonModeNoiseFiltering"},
        {"Differential Mode Noise Filtering", "differentialModeNoiseFiltering"},
        // connectionType
        {"Pin", "pin"},
        {"Screw", "screw"},
        {"SMT", "smt"},
        {"Flying Lead", "flyingLead"},
        {"THT", "tht"},
        {"PCB Pad", "pcbPad"},
        // coilAlignment
        {"inner or top", "innerOrTop"},
        {"outer or bottom", "outerOrBottom"},
        // coreType
        {"two-piece set", "twoPieceSet"},
        {"piece and plate", "pieceAndPlate"},
        {"closed shape", "closedShape"},
        // piece type
        {"half set", "halfSet"},
        // DMC configuration
        {"SINGLE_PHASE", "singlePhase"},
        {"THREE_PHASE", "threePhase"},
        {"THREE_PHASE_WITH_NEUTRAL", "threePhaseWithNeutral"},
        // waveformLabel
        {"Custom", "custom"},
        {"Triangular", "triangular"},
        {"Sinusoidal", "sinusoidal"},
        {"Rectangular", "rectangular"},
        {"Unipolar Rectangular", "unipolarRectangular"},
        {"Unipolar Triangular", "unipolarTriangular"},
        {"Bipolar Rectangular", "bipolarRectangular"},
        {"Bipolar Triangular", "bipolarTriangular"},
        {"Flyback Primary", "flybackPrimary"},
        {"Flyback Secondary", "flybackSecondary"},
        {"Rectangular With Deadtime", "rectangularWithDeadtime"},
        {"Flyback Secondary With Deadtime", "flybackSecondaryWithDeadtime"},
        {"Rectangular DCM", "rectangularDCM"},
        {"Secondary Rectangular", "secondaryRectangular"},
        {"Secondary Rectangular With Deadtime", "secondaryRectangularWithDeadtime"},
        {"Triangular With Deadtime", "triangularWithDeadtime"},
        // topology
        {"Buck Converter", "buckConverter"},
        {"Boost Converter", "boostConverter"},
        {"Inverting Buck-Boost Converter", "invertingBuckBoostConverter"},
        {"Cuk Converter", "cukConverter"},
        {"Zeta Converter", "zetaConverter"},
        {"Flyback Converter", "flybackConverter"},
        {"Active Clamp Forward Converter", "activeClampForwardConverter"},
        {"Single Switch Forward Converter", "singleSwitchForwardConverter"},
        {"Two Switch Forward Converter", "twoSwitchForwardConverter"},
        {"Push-Pull Converter", "pushPullConverter"},
        {"Weinberg Converter", "weinbergConverter"},
        {"Half-Bridge Converter", "halfBridgeConverter"},
        {"Full-Bridge Converter", "fullBridgeConverter"},
        {"Phase-Shifted Full-Bridge Converter", "phaseShiftedFullBridgeConverter"},
        {"Phase-Shifted Half-Bridge Converter", "phaseShiftedHalfBridgeConverter"},
        {"Current Transformer", "currentTransformer"},
        {"Isolated Buck Converter", "isolatedBuckConverter"},
        {"Isolated Buck-Boost Converter", "isolatedBuckBoostConverter"},
        {"Dual Active Bridge Converter", "dualActiveBridgeConverter"},
        {"LLC Resonant Converter", "llcResonantConverter"},
        {"CLLC Resonant Converter", "cllcResonantConverter"},
        {"Common Mode Choke", "commonModeChoke"},
        {"Differential Mode Choke", "differentialModeChoke"},
        {"Power Factor Correction", "powerFactorCorrection"},
        // flybackModes
        {"Continuous Conduction Mode", "continuousConductionMode"},
        {"Discontinuous Conduction Mode", "discontinuousConductionMode"},
        {"Quasi Resonant Mode", "quasiResonantMode"},
        {"Boundary Mode Operation", "boundaryModeOperation"},
        // pfcModes
        {"Critical Conduction Mode", "criticalConductionMode"},
        {"Transition Mode", "transitionMode"},
        // bridge types
        {"Half Bridge", "halfBridge"},
        {"Full Bridge", "fullBridge"},
        {"Center Tapped", "centerTapped"},
        {"Current Doubler", "currentDoubler"},
        // cllcPowerFlow
        {"Forward", "forward"},
        {"Reverse", "reverse"},
        // market
        {"Medical", "medical"},
        {"Commercial", "commercial"},
        {"Industrial", "industrial"},
        {"Military", "military"},
        {"Space", "space"},
        // materialComposition
        {"Carbonyl Iron", "carbonylIron"},
        {"Iron", "iron"},
        {"Proprietary", "proprietary"},
        // cti
        {"Group I", "groupI"},
        {"Group II", "groupII"},
        {"Group IIIA", "groupIIIA"},
        {"Group IIIB", "groupIIIB"},
        // pollutionDegree (RFC 0008)
        {"P1", "PD1"},
        {"P2", "PD2"},
        {"P3", "PD3"},
        // overvoltageCategory (RFC 0008)
        {"OVC-I", "I"},
        {"OVC-II", "II"},
        {"OVC-III", "III"},
        {"OVC-IV", "IV"},
        // shape family
        {"planar e", "planarE"},
        {"planar er", "planarER"},
        {"planar el", "planarEL"},
    };
    return m;
}

// Rewrite a MAS document in place: any string value that matches a key
// in the mapping is replaced with the corresponding new value. Walks
// arrays and objects recursively. Object keys are NOT rewritten (only
// values).
inline void migrate(nlohmann::json& j) {
    if (j.is_string()) {
        const auto& m = mapping();
        const std::string s = j.get<std::string>();
        auto it = m.find(s);
        if (it != m.end()) {
            j = it->second;
        }
    } else if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            migrate(it.value());
        }
    } else if (j.is_array()) {
        for (auto& e : j) {
            migrate(e);
        }
    }
}

// Parse a JSON string and migrate the result to 1.0 spellings. Use
// this in place of nlohmann::json::parse anywhere a pre-1.0 MAS
// document might be loaded.
inline nlohmann::json parse(std::string_view s) {
    auto j = nlohmann::json::parse(s);
    migrate(j);
    return j;
}

}  // namespace mas_compat
