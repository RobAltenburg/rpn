//
//  conversions.cpp
//  rpn
//
//  Created by Robert Altenburg on 2/3/25.
//

#include "conversions.hpp"

double convert_units(DoubleVector& dv, const std::string& from, const std::string& to) {
    // Define conversion factors
    std::unordered_map<std::string, double> conversion_factors = {
        {"kg_to_lb", 2.20462},
        {"m_to_ft", 3.28084},
        {"m_to_cm", 100},
        {"m_to_mm", 1000},
        {"l_to_gal", 0.264172},
        {"ft_to_in", 12},
        //{"in_to_mm", 25.4},
        //{"in_to_cm", 2.54},
        {"in_to_m", 0.0254},
        {"mil_to_mm", 0.0254},
        {"mi_to_nm", 0.8689762}, // miles to nautical miles
        {"mi_to_km", 1.609344}, // miles to kilometers
        {"deg_to_rad", 0.01745329},
        // Add more conversions as needed
    };

    // Construct the key for the conversion map
    std::string key = from + "_to_" + to;

    // Check if the direct conversion exists
    if (conversion_factors.find(key) != conversion_factors.end()) {
        return conversion_factors[key] * dv.pop();
    } else {
        // Check for the reciprocal conversion
        std::string reciprocal_key = to + "_to_" + from;
        if (conversion_factors.find(reciprocal_key) != conversion_factors.end()) {
            return (1.0 / conversion_factors[reciprocal_key]) * dv.pop();
        }

        // Check for multi-step conversion
        for (const auto& pair : conversion_factors) {
            std::string intermediate_from = pair.first.substr(0, pair.first.find("_to_"));
            std::string intermediate_to = pair.first.substr(pair.first.find("_to_") + 4);

            if (from == intermediate_from) {
                std::string intermediate_key = intermediate_to + "_to_" + to;
                if (conversion_factors.find(intermediate_key) != conversion_factors.end()) {
                    double factor = pair.second * conversion_factors[intermediate_key];
                    return factor * dv.pop();
                } else {
                    // Check for reciprocal of the intermediate step
                    std::string reciprocal_intermediate_key = to + "_to_" + intermediate_to;
                    if (conversion_factors.find(reciprocal_intermediate_key) != conversion_factors.end()) {
                        double factor = pair.second / conversion_factors[reciprocal_intermediate_key];
                        return factor * dv.pop();
                    }
                }
            }
        }

        // If no conversion is found, return the last element of the vector
        return dv.pop();
    }
}
