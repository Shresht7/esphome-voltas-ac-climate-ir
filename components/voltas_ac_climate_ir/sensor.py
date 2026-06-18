import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor

hello_world_ns = cg.esphome_ns.namespace('hello_world')
HelloWorldSensor = hello_world_ns.class_('HelloWorld', cg.PollingComponent)

CONFIG_SCHEMA = sensor.sensor_schema(
    HelloWorldSensor,
    unit_of_measurement='°C',
    accuracy_decimals=1,
    icon='mdi:thermometer',
    device_class=sensor.DEVICE_CLASS_TEMPERATURE
).extend(cv.polling_component_schema('5s'))

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
