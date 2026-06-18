#include "esphome/core/log.h"
#include "hello_world.h"

namespace esphome
{
    namespace hello_world
    {

        static const char *TAG = "hello_world.sensor";

        class HelloWorld : public esphome::sensor::Sensor, public esphome::PollingComponent
        {

            void setup() override
            {
                ESP_LOGCONFIG(TAG, "Setting up Hello World Sensor...");
            }

            void loop() override
            {
                // This method will be called in every loop iteration.
            }

            void update() override
            {
                // This method will be called every time the sensor is updated.
                // You can use this method to publish new sensor values.
                // Publish random number for demonstration purposes
                this->publish_state(42.0);
            }

            void dump_config() override
            {
                ESP_LOGCONFIG(TAG, "Hello World Sensor:");
            }
        };

    } // namespace hello_world
} // namespace esphome
