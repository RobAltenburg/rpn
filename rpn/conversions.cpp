//
//  conversions.cpp
//  rpn
//
//  Created by Robert Altenburg on 2/3/25.
//

#include "conversions.hpp"
double find_conversion_factor(const std::unordered_map<std::string, double>& conversion_factors, const std::string& from, const std::string& to) {
    std::string key = from + "_to_" + to;
    if (conversion_factors.find(key) != conversion_factors.end()) {
        return conversion_factors.at(key);
    }
    std::string reciprocal_key = to + "_to_" + from;
    if (conversion_factors.find(reciprocal_key) != conversion_factors.end()) {
        return 1.0 / conversion_factors.at(reciprocal_key);
    }
    return 0.0;
}

double multi_step_conversion(const std::unordered_map<std::string, double>& conversion_factors, const std::string& from, const std::string& to) {
    for (const auto& pair : conversion_factors) {
        std::string intermediate_from = pair.first.substr(0, pair.first.find("_to_"));
        std::string intermediate_to = pair.first.substr(pair.first.find("_to_") + 4);

        if (from == intermediate_from) {
            double first_factor = pair.second;
            double second_factor = find_conversion_factor(conversion_factors, intermediate_to, to);
            if (second_factor != 0.0) {
                return first_factor * second_factor;
            }
        }
    }
    return 0.0;
}


double convert_units(DoubleVector& dv, const std::string& from, const std::string& to) {
    // Define conversion factors
    std::unordered_map<std::string, double> conversion_factors = {
        {"kg_to_lb", 2.20462},
        {"slug_to_kg", 14.593903},
        {"m_to_ft", 3.28084},
        {"ft_to_mi", 5280},
        {"m_to_cm", 100},
        {"m_to_mm", 1000},
        {"km_to_m", 1000},
        {"mm_to_in", 25.4},
        {"fpm_to_mps", 0.00508},
        {"mph_to_mps", 0.44704},
        {"kts_to_mps", 0.514},
        {"l_to_gal", 0.264172},
        {"l_to_ml", 1000},
        {"gal_to_qt", 4},
        {"qt_to_pt", 2},
        {"pt_to_tbsp", 32},
        {"tbsp_to_tsp", 3},
        {"ft_to_in", 12},
        {"mil_to_mm", 0.0254},
        {"mi_to_nm", 0.8689762}, // miles to nautical miles
        {"mi_to_km", 1.609344}, // miles to kilometers
        {"deg_to_rad", 0.01745329},
        // Add more conversions as needed
    };

    // Find the conversion factor
    double factor = find_conversion_factor(conversion_factors, from, to);
    if (factor == 0.0) {
        factor = multi_step_conversion(conversion_factors, from, to);
    }

    // Perform the conversion
    if (factor != 0.0) {
        return factor * dv.pop();
    } else {
        // If no conversion is found, return the last element of the vector
        return dv.pop();
    }
}
