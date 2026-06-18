# ESPHome Voltas AC Climate IR

ESPHome External Component for a Voltas Window AC with Infrared (IR).

---

## 📖 Usage

To import this component into your ESPHome project, add the following to your YAML configuration file:

```yaml
external_components:
  - source:
        type: git
        url: https://github.com/Shresht7/esphome-voltas-ac-climate-ir
        ref: main
    components: [voltas_ac_climate_ir]
```

and then use the `voltas_ac_climate_ir` platform in your climate configuration:

```yaml
climate:
  - platform: voltas_ac_climate_ir
    name: "Voltas AC"
```

---

## 🚧 Development

### Setup IDE Environment

To setup autocomplete and intellisense, we need to do a bit of setup by providing VS Code the Python modules and C++ header files to reference.

#### Python Intellisense

1. Open the terminal in the root folder.
2. Create a virtual environment using `python -m venv .venv` or `uv venv`
3. Install `esphome` in the virtual environment using `pip install esphome` or `uv pip install esphome`
4. Active the virtual environment using `source .venv/bin/activate` or the VS Code command palette `Python: Select Interpreter` and select the `.venv` interpreter in the current workspace.

Python files should now have intellisense for the ESPHome modules.

#### C++ Intellisense

This is slightly more tricky as we don't have the ESPHome header files in this project or its dependencies. We could sparse-checkout the ESPHome repository but that still doesn't give us all the header files (like the transitive dependencies). There's probably a better way to do this, but I am a C++ noob so I am going to do this the ***hacky way***.

We can compile a _"fake.yaml"_ file with the ESPHome CLI to generate the source files in the `build` folder. We can then point VS Code to that folder for intellisense to reference all the relevant header files.

1. Create a `fake.yaml` file in the root folder with the following content:

```yaml
esphome:
  name: esphome-test

esp32:
  board: esp32dev
  framework:
    type: esp-idf
```

2. Compile the `fake.yaml` configuration file using `esphome compile fake.yaml`
3. Create a `c_cpp_properties.json` file in the `.vscode` folder with the following content:

```json
{
    "configurations": [
        {
            "name": "ESPHome",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/.esphome/**",
                "~/.platformio/packages/toolchain-xtensa-esp32/**",
                "~/.platformio/packages/toolchain-riscv32-esp/**"
            ],
            "intelliSenseMode": "...",
            "compilerPath": "...",
            "cStandard": "c17",
            "cppStandard": "c++17"
        }
    ],
    "version": 4
}
```
I only added the `"name"` and `"includePath"` fields and the C++ extension was able to auto-populate the rest. We point it to our workspace folder, the `.esphome` folder where the generated source code files are located, and the PlatformIO toolchain folders where the ESPHome dependencies are located.

4. Reload VS Code and the C++ files should now have intellisense for the ESPHome header files.
5. You can now remove the `fake.yaml` file as it was only needed to generate the source code files for intellisense.

---

## 📕 References

- [ESPHome](https://esphome.io/)
- [ESPHome - IR Remote Climate](https://esphome.io/components/climate/climate_ir/)
- [ESPHome - External Components](https://esphome.io/components/external_components/)
- [ESPHome - Component Architecture](https://developers.esphome.io/architecture/components/)
- [GitHub - ESPHome Starter Components](https://github.com/esphome/starter-components)
- [Github - ESPHome Repository](https://github.com/esphome/esphome)

---

## 📄 License

MIT License. See [LICENSE](LICENSE) for more information.
