#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <vector>

// Question 3: This is an extension task that requires you to decode sensor data from a CAN log file.
// CAN (Controller Area Network) is a communication standard used in automotive applications (including Redback cars)
// to allow communication between sensors and controllers.
//
// Your Task: Using the definition in the Sensors.dbc file, extract the "WheelSpeedRR" values
// from the candump.log file. Parse these values correctly and store them in an output.txt file with the following format:
// (<UNIX_TIME>): <DECODED_VALUE>
// eg:
// (1705638753.913408): 1234.5
// (1705638754.915609): 6789.0
// ...
// The above values are not real numbers; they are only there to show the expected data output format.
// You do not need to use any external libraries. Use the resources below to understand how to extract sensor data.
// Hint: Think about manual bit masking and shifting, data types required,
// what formats are used to represent values, etc.
// Resources:
// https://www.csselectronics.com/pages/can-bus-simple-intro-tutorial
// https://www.csselectronics.com/pages/can-dbc-file-database-intro

const int WHEEL_SPEED_RR_ID = 0x123;
const int WHEEL_SPEED_RR_BYTE_POS = 2;
const int WHEEL_SPEED_RR_LENGTH = 2;
const double SCALING_FACTOR = 0.1;

void processCanLog(const std::string &inputFile, const std::string &outputFile) {
    std::ifstream canLog(inputFile);
    std::ofstream output(outputFile);

    if (!canLog.is_open() || !output.is_open()) {
        std::cerr << "Error: Unable to open file(s)." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(canLog, line)) {
        std::istringstream iss(line);
        double unixTime;
        std::string canIdHex, data;

        if (!(iss >> unixTime >> canIdHex)) {
            continue;
        }

        int canId = std::stoi(canIdHex, nullptr, 16);

        if (canId == WHEEL_SPEED_RR_ID) {
            std::vector<int> payload;
            int byte;
            while (iss >> std::hex >> byte) {
                payload.push_back(byte);
            }

            if (payload.size() >= WHEEL_SPEED_RR_BYTE_POS + WHEEL_SPEED_RR_LENGTH) {
                int rawValue = (payload[WHEEL_SPEED_RR_BYTE_POS + 1] << 8) | payload[WHEEL_SPEED_RR_BYTE_POS];
                double decodedValue = rawValue * SCALING_FACTOR;

                output << "(" << unixTime << "): " << decodedValue << std::endl;
            }
        }
    }

    canLog.close();
    output.close();
}

int main() {
    processCanLog("candump.log", "output.txt");
    std::cout << "Processing completed. Output saved to output.txt" << std::endl;
    return 0;
}
