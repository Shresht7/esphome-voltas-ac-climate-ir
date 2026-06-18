#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        // Inherit from ClimateIR
        class VoltasACClimateIR : public esphome::climate_ir::ClimateIR
        {
        public:
            // The constructor configures the capabilities shown in Home Assistant, such as supported modes, temperature range, etc.
            VoltasACClimateIR() : esphome::climate_ir::ClimateIR(
                                      16.0f,                                                                  // Minimum Temperature
                                      30.0f,                                                                  // Maximum Temperature
                                      1.0f,                                                                   // Temperature Step
                                      false,                                                                  // Supports Dry Mode? (Hardcoded to false for now)
                                      false,                                                                  // Supports Fan Only Mode? (Hardcoded to false for now)
                                      {esphome::climate::CLIMATE_FAN_LOW, esphome::climate::CLIMATE_FAN_HIGH} // Supported Fan Speeds
                                  )
            {
            }

            // Override traits to specify the capabilities of this climate controller
            esphome::climate::ClimateTraits traits() override;

            // Override the transmit method - called wheneer Home Assistant changes a setting
            void transmit_state() override;
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
