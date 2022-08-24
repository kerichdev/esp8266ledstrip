# ESP8266 Blynk-based LED Strip
This is a small project of mine, an LED strip powered by NodeMCU 1.0 (ESP-12E Module) board, and using Blynk as a backend.

## Usage
**Make sure to fill out src/secrets.h, as well as all the LED strip configurations in src/main.cpp if you want to use this with your own strip!**
### Software
**With Visual Studio Code**
- Open Visual Studio Code (other code editors may not work properly)
- Install the PlatformIO extension from the marketplace
- Once installed, restart VS Code, and find the PlatformIO icon on the sidebar
- Find `Miscellanceous` > `Clone Git Project`
- Clone the project into your local workspace
- Open a terminal window in the cloned repository root
- Do `git submodule update --init`
- Make sure lib/Blynk is not empty
- Fine-tune the `main.cpp and secrets.h` to your own preferences and hardware
- Install the dependencies in the PlatformIO Library Manager
- Build the project or upload it if you have a ESP8266 board connected

**With Arduino IDE**
- Make sure you have installed the necessary tools for ESP8266
- Clone the repository locally with the `--recurse-submodules` flag: `git clone https://github.com/kerichdev/esp8266ledstrip --recurse-submodules`
- Open `main.cpp` and `secrets.h`
- In Tools > Board: Choose `NodeMCU 1.0 (ESP-12E Module)`; Upload Speed: `115200`; CPU Frequency: `80 MHz`
- Using the Arduino IDE Library Manager, install the FastLED lubrary. Latest version should work, but in case of issues, fallback to `FastLED@3.5.0`
- Fine-tune the `main.cpp` and `secrets.h` to your own preferences and hardware
- Build or Upload the sketch to your board

### Hardware
- The project is intended for 3-pin addressable LED strips: `5V`, `GND` and `DI` (Data In, but name may vary)
- Make sure the LED strip has its own power supply, and is **not** connected to ESP8266's 3.3V/VIN pin
- Fill out the correct hardware info about your strip, like the amount of pins

### Blynk
- Create a project with the [following template](https://blynk.cloud/dashboard/26595/product/49552/info)
- Copy the token and paste it in the secrets.h file, as well as the template ID

### IFTTT Assistant Integration
- You can make whatever integration you like, in fact, all you need to do to connect the strip, is use a webhook/web request as an action, with the following settings:
    - URL: `https://(server_region).blynk.cloud/external/api/update?token=(your_blynk_token)&(virtual_pin)=(value)`
    - Method: `GET`
    - Content type: `(Please select, aka don't change)`
    - Additional Headers: `(leave empty)`
    - Body: `(leave empty)`

You can find your server region in the right bottom corner of your blynk dashboard. The virtual pins and their possible values are all present in the template.
