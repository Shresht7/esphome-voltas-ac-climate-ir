#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        class VoltasACClimateIR : public esphome::sensor::Sensor, public esphome::PollingComponent
        {
        public:
            void setup() override;
            void loop() override;
            void update() override;
            void dump_config() override;
        };

    } // namespace voltas_ac_climate_ir

} // namespace esphome
