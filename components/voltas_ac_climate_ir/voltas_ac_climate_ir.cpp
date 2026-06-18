#include "esphome/core/log.h"

#include "voltas_ac_climate_ir.h"

namespace esphome
{
    namespace voltas_ac_climate_ir
    {

        static const char *TAG = "voltas_ac_climate_ir.sensor";

        void VoltasACClimateIR::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Voltas AC Climate IR Sensor...");
        }

        void VoltasACClimateIR::loop()
        {
            // This method will be called in every loop iteration.
        }

        void VoltasACClimateIR::update()
        {
            // This method will be called every time the polling interval triggers (5s).
            // Publish our dummy value for demonstration purposes
            this->publish_state(42.0);
        }

        void VoltasACClimateIR::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Voltas AC Climate IR Sensor:");
        }

    } // namespace voltas_ac_climate_ir
} // namespace esphome
