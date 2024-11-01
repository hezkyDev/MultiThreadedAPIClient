#MultiThreadedAPIClient

MultiThreadedAPIClient is a C++ application that fetches real-time weather data for Kuala Lumpur using the OpenWeatherMap API. The program stores data locally in an SQLite database, supports multi-threaded data fetching, and includes CLI commands to control data fetching, view stored records, and monitor errors through detailed logging.

##Table of Contents

	•	Features
	•	Prerequisites
	•	Installation
	•	Configuration
	•	Usage
	•	Database Commands
	•	Error Handling and Logging
	•	Project Structure

##Features

	•	Real-Time Weather Data: Fetches data from OpenWeatherMap’s API, including temperature, humidity, wind speed, and more.
	•	Multi-Threaded Data Fetching: Uses a dedicated thread for continuous data fetching.
	•	SQLite Database Storage: Saves weather data in an SQLite database for historical record-keeping.
	•	Command-Line Interface (CLI): Provides commands to start, stop, view data, and exit the application.
	•	Enhanced Logging: Logs errors and important events to program_log.txt for troubleshooting and monitoring.
	•	Detailed Error Handling: Captures network and database errors with specific feedback messages.

##Prerequisites
Ensure the following dependencies are installed:

•	libcurl (for making HTTP requests)
•	SQLite3 (for database storage)
•	nlohmann/json (for JSON parsing)
•	Google Test (for testing, optional)

##Installation

1.	Clone the Repository:

git clone https://github.com/your-username/MultiThreadedAPIClient.git
cd MultiThreadedAPIClient


2.	Build the Project:

mkdir build
cd build
cmake ..
make


3.	Verify Dependencies:
Ensure libcurl and SQLite3 are correctly linked. Adjust paths in CMakeLists.txt if necessary.

##Configuration

1.	API Key: Replace YOUR_API_KEY in APIClient.cpp with your OpenWeatherMap API key.
2.	Database: The program will automatically create weather_data.db and the necessary tables upon the first run.

##Usage

Run the program using the following command:

./MultiThreadedAPIClient

##CLI Commands

	•	start: Begin continuous data fetching in a background thread.
	•	stop: Stop data fetching.
	•	view: Display stored weather data from the database.
	•	exit: Stop data fetching (if active) and exit the program.

##Example

Welcome to the Real-Time Weather Fetcher!
Commands:
- 'start' to begin fetching data
- 'stop' to stop fetching
- 'view' to view stored data
- 'exit' to quit the program

Enter command: start
Fetching started in a new thread.

Enter command: view
[Displays stored weather data]

Enter command: stop
Fetching stopped.

Enter command: exit
Exiting the program. Goodbye!

##Database Commands

For advanced users, you can query the SQLite database directly using sqlite3:

sqlite3 weather_data.db

####Some useful SQL commands:

.tables                              -- List all tables
PRAGMA table_info(Weather);          -- View table structure
SELECT * FROM Weather;               -- View all records
DELETE FROM Weather WHERE Timestamp < '2023-01-01';  -- Delete older records
.exit                                -- Exit SQLite

####Error Handling and Logging

	•	Errors (network, database, or other issues) are logged to program_log.txt.
	•	Detailed Feedback: Any errors in data fetching or storage operations are also displayed in the terminal for immediate feedback.

##Project Structure

MultiThreadedAPIClient/
│
├── src/
│   ├── APIClient.cpp               # Core logic for data fetching, database handling, and logging
│   └── main.cpp                    # CLI and main control loop
│
├── include/
│   └── APIClient.h                 # Class definition for APIClient
│
├── tests/
│   └── APIClientTest.cpp           # Unit tests for core functions using Google Test
│
├── CMakeLists.txt                  # Build configuration
├── README.md                       # Project documentation
└── program_log.txt                 # Error and event log (generated at runtime)

##Future Improvements

	•	Enhanced Filtering: Add more CLI commands to filter database records by date or conditions.
	•	More Data Sources: Expand to include other APIs or support more cities.
	•	Config File: Use a configuration file for managing API keys, database names, and fetch intervals.

##License

This project is licensed under the MIT License. See LICENSE for details.