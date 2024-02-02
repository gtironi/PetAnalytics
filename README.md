# PetAnalytics

Code of the PetAnalytics project's IoT device, handling the microcontroller and the sensors connected to a dog harness.

---

## Table of Contents

- [Installation & Setup](#installation--setup)
- [Modules](#modules)
- [Configuration & Variables](#configuration--variables)
- [Future Improvements](#future-improvements)
- [Contact](#contact)

---

## Installation & Setup

1. **GitHub Repository Clone**: Clone or download the repository.
2. **Install Arduino IDE 2.0**: Download and install the latest version of the Arduino IDE 2.0 from [here](https://www.arduino.cc/en/software).
3. **Install ESP32 boards dependencies**: Open the Arduino IDE 2.0 `Preferences` and add the following link to the `Additional Boards Manager URLs` field: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`.
3. **Install ESP32 Board on Arduino IDE**: Open the Arduino IDE 2.0 and go to `Tools > Board > Boards Manager`. Search for `esp32` and install the latest version of the board.
4. **Install the necessary libraries**: Open the Arduino IDE 2.0 and go to `Tools > Manage Libraries`. Search and install the following libraries:
    - ` SparkFun ICM-20948`
5. **Open the sketch and configure the code**: Open the `main.ino` file in the Arduino IDE 2.0 and configure the code (see [Configuration & Variables](#configuration--variables)).
6. **Connect the microcontroller to the computer**: Connect the microcontroller to the computer using a USB cable. Also select the correct COM port in the Arduino IDE 2.0 (`Tools > Port`) and the correct board (`Tools > Board > esp32 Arduino > SparkFun ESP32 Thing Plus C`).
7. **Upload the code to the microcontroller**: Open the `main.ino` file in the Arduino IDE 2.0 and upload the code to the microcontroller (`Sketch > Upload` / `Ctrl+U`). Wait until the code is uploaded and the microcontroller is ready to use.

## Modules

| Module Name | Description |
|-------------|-------------|
| `IMU` | Sets the IMU sample rate.|

## Configuration & Variables

This section highlights the key variables in the project which can be changed for customization.

| Variable Name | Module | Description | Default Value |
|---------------|---------------|-------------|---------------|
| `SAMPLE_RATE`  | `IMU` | Sample rate of the IMU, in hertz (Hz) | `100` |

## Future Improvements

- **Connect to a database**: Enable integration with a database, ensuring efficient storage and retrieval of data. This enhances overall data management, supporting comprehensive analytics and user-specific insights.
- **Wifi communication**: Incorporate wireless capabilities to enhance data transfer speed and reliability, ensuring a more responsive user experience.

## Acknowledgements

Thank you to the team members, our Professor Rafael de Pinho André and FGV EMAp for their contributions, support, or resources provided to this project.

## Contact

For any queries or feedback, please contact us at: gustavotironi100@gmail.com
