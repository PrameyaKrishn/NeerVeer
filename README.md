# 🌾 NeerVeer – Smart Irrigation System

*A Project from IIT Delhi's "Change Makers 2025"*

NeerVeer is a compact, modular smart irrigation system built during the Change Makers 2025 workshop hosted by IIT Delhi. Developed using Seeed Studio XIAO ESP32C6 boards, this project is designed to promote sustainable agriculture through low-cost automation, real-time environmental sensing, and completely offline access to a live dashboard.

The central ESP32C6 board acts as a Wi-Fi Access Point named 'रामबाण' (password: 12345678). When powered on, it starts a server accessible at the IP address displayed in the Arduino Serial Monitor. Upon connecting to the Wi-Fi and visiting that IP in a browser, a live dashboard appears, displaying real-time sensor data.

The board reads environmental parameters using a BME280 sensor (temperature, humidity, pressure, altitude), and stores all values in a file called `data.json` using the LittleFS filesystem. Users or additional ESP nodes can POST sensor data (like moisture, pH, water level, etc.) directly to the board. All files—HTML, CSS, JS, and JSON—are hosted from the onboard memory using the LittleFS library, and can be uploaded via the Arduino IDE using the "Upload LittleFS to ESP32" command from the command palette.

The `data.json` file acts as both a data store and communication layer. Clients can fetch it using HTTP GET requests to display sensor values, or send new values using HTTP POST in raw JSON format. A typical POST body might look like:
{ "moisture1": "20%", "moisture2": "15%", "moisture3": "25%", "moistureAvg": "20", "water": "80%", "ph": "6.5", "soilTemp": "23°C", "temp": "28°C", "alerts": "No alerts" }

The server then supplements this JSON file by periodically adding BME280 readings (air temperature, pressure, humidity, altitude) and an estimated weather forecast, which is derived from those values. The forecast logic uses a simple rule-based method: for example, if pressure is high and humidity is low, the forecast might say "Clear or Sunny", while low pressure with high humidity might mean "Rain Likely".

On the frontend, the index.html file shows all these parameters in a clean, card-based UI. It fetches `/data.json` every few seconds using JavaScript's `fetch()` function, parses the JSON response, and updates the DOM. There is also a small form in the dashboard that lets the user manually send updated values (useful for emulating sensor nodes or testing). The CSS styles are lightweight and designed to be mobile-friendly. You’ll find files like `styles.css` and `script.js` in the same folder as `index.html`, and they are all uploaded to the ESP using LittleFS.

File structure on the ESP32C6 (under LittleFS) should look like this:
/index.html – the frontend HTML file
/styles.css – basic styling
/script.js – handles fetching and sending data
/data.json – sensor data and state
You can update these files using Arduino IDE's LittleFS uploader.

Several important intricacies are managed in the system. The server handles JSON parse failures gracefully, avoiding crashes. If the `data.json` file doesn't exist, it creates a fallback one with default values. The weather forecasting logic compensates for changes in altitude by adjusting pressure thresholds, making it more accurate at different elevations. The architecture is modular and allows for expanding to a mesh of sensor nodes sending updates to the central hub.

In the future, features like real-time charting, pump control automation, solar-powered operation, or MQTT/LoRa integration could be added easily.

NeerVeer is a working demonstration of how simple, locally networked IoT systems can empower farmers and communities. Built under real-world constraints by young innovators, it is a small step towards democratizing smart agriculture.

Feel free to fork the project, suggest enhancements, or adapt it for your own field or garden.
