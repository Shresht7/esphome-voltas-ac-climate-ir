# Library
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

# Automatically load the climate_ir component base
AUTO_LOAD = ['climate_ir']

# Define our namespace and class
voltas_ac_climate_ir_ns = cg.esphome_ns.namespace('voltas_ac_climate_ir')
VoltasACClimateIR = voltas_ac_climate_ir_ns.class_('VoltasACClimateIR', climate_ir.ClimateIR)

# Inherit the standard climate_ir configuration schema and extend it with our custom options
CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(VoltasACClimateIR).extend({
    cv.GenerateID(): cv.declare_id(VoltasACClimateIR),
})

async def to_code(config):
    var = await climate_ir.new_climate_ir(config)
