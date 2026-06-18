#include "esphome/core/log.h"
#include "hello_world.h"

namespace esphome
{
    namespace hello_world
    {

        static const char *TAG = "hello_world.sensor";

        void HelloWorld::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up Hello World Sensor...");
        }

        void HelloWorld::loop()
        {
            // This method will be called in every loop iteration.
        }

        void HelloWorld::update()
        {
            // This method will be called every time the sensor is updated.
            // You can use this method to publish new sensor values.
            // Publish random number for demonstration purposes
            this->publish_state(42.0);
        }

        void HelloWorld::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Hello World Sensor:");
        }

    } // namespace hello_world
} // namespace esphome
