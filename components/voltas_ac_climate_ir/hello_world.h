#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace hello_world
    {

        class HelloWorld : public esphome::sensor::Sensor, public esphome::PollingComponent
        {
        public:
            void setup() override;
            void loop() override;
            void update() override;
            void dump_config() override;
        };

    } // namespace hello_world

} // namespace esphome
