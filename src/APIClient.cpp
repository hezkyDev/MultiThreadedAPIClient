#include "APIClient.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <chrono>
#include <cstdlib>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

APIClient::APIClient() : isRunning(false) {}

APIClient::~APIClient() {
    stopFetching();
    if (fetchThread.joinable()) {
        fetchThread.join();
    }
}

void APIClient::startFetching() {
    if (!isRunning) {
        isRunning = true;
        setupDatabase();
        fetchThread = std::thread(&APIClient::fetchData, this);
        std::cout << "Fetching started in a new thread." << std::endl;
    } else {
        std::cout << "Fetching is already running." << std::endl;
    }
}

void APIClient::stopFetching() {
    if (isRunning) {
        isRunning = false;
        if (fetchThread.joinable()) {
            fetchThread.join();
            std::cout << "Fetching stopped." << std::endl;
        }
    } else {
        std::cout << "Fetching is not running." << std::endl;
    }
}

std::string APIClient::makeApiRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::string errorMsg = "cURL request failed: " + std::string(curl_easy_strerror(res));
            std::cerr << errorMsg << std::endl;
            logToFile(errorMsg);
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

void APIClient::fetchData() {
    // Retrieve the API key from the environment variable
    const char* apiKey = getenv("OPENWEATHERMAP_API_KEY");
    if (!apiKey) {
        std::cerr << "Error: OPENWEATHERMAP_API_KEY environment variable not set." << std::endl;
        logToFile("Error: OPENWEATHERMAP_API_KEY environment variable not set.");
        return;
    }

    std::string baseUrl = "https://api.openweathermap.org/data/2.5/weather?q=Kuala+Lumpur&appid=";
    std::string url = baseUrl + apiKey + "&units=metric";

    while (isRunning) {
        std::string response = makeApiRequest(url);

        try {
            auto jsonResponse = nlohmann::json::parse(response);

            if (jsonResponse.contains("main") && jsonResponse.contains("wind") && jsonResponse.contains("weather")) {
                double temperature = jsonResponse["main"]["temp"];
                double feelsLike = jsonResponse["main"]["feels_like"];
                int humidity = jsonResponse["main"]["humidity"];
                double windSpeed = jsonResponse["wind"]["speed"];
                int windDirection = jsonResponse["wind"]["deg"];
                std::string weatherMain = jsonResponse["weather"][0]["main"];
                std::string weatherDescription = jsonResponse["weather"][0]["description"];
                std::string cityName = jsonResponse["name"];
                
                double rainVolume = jsonResponse.contains("rain") && jsonResponse["rain"].contains("1h") 
                                    ? jsonResponse["rain"]["1h"].get<double>() : 0.0;

                std::cout << "Current Weather in " << cityName << ":" << std::endl;
                std::cout << "Weather: " << weatherMain << " (" << weatherDescription << ")" << std::endl;
                std::cout << "Temperature: " << temperature << " °C" << std::endl;

                storeWeatherData(cityName, weatherMain, weatherDescription, temperature, feelsLike, humidity, windSpeed, windDirection, rainVolume);
            } else {
                std::cerr << "Error: Weather data not available in the response." << std::endl;
                logToFile("Error: Weather data not available in the response.");
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::string errorMsg = "Failed to parse JSON: " + std::string(e.what());
            std::cerr << errorMsg << std::endl;
            logToFile(errorMsg);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void APIClient::setupDatabase() {
    sqlite3* db;
    char* errorMessage = 0;

    int exit = sqlite3_open("weather_data.db", &db);
    if (exit) {
        std::string errorMsg = "Error opening SQLite database: " + std::string(sqlite3_errmsg(db));
        std::cerr << errorMsg << std::endl;
        logToFile(errorMsg);
        return;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS Weather ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "City TEXT NOT NULL, "
                      "Weather TEXT, "
                      "Description TEXT, "
                      "Temperature REAL, "
                      "FeelsLike REAL, "
                      "Humidity INTEGER, "
                      "WindSpeed REAL, "
                      "WindDirection INTEGER, "
                      "RainVolume REAL, "
                      "Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (exit != SQLITE_OK) {
        std::string errorMsg = "Error creating table: " + std::string(errorMessage);
        std::cerr << errorMsg << std::endl;
        logToFile(errorMsg);
        sqlite3_free(errorMessage);
    }

    sqlite3_close(db);
}

void APIClient::storeWeatherData(const std::string& city, const std::string& weather, const std::string& description,
                                 double temperature, double feelsLike, int humidity, double windSpeed, 
                                 int windDirection, double rainVolume) {
    sqlite3* db;
    sqlite3_open("weather_data.db", &db);

    std::string sql = "INSERT INTO Weather (City, Weather, Description, Temperature, FeelsLike, Humidity, WindSpeed, WindDirection, RainVolume) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, weather.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, temperature);
    sqlite3_bind_double(stmt, 5, feelsLike);
    sqlite3_bind_int(stmt, 6, humidity);
    sqlite3_bind_double(stmt, 7, windSpeed);
    sqlite3_bind_int(stmt, 8, windDirection);
    sqlite3_bind_double(stmt, 9, rainVolume);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string errorMsg = "Failed to insert data: " + std::string(sqlite3_errmsg(db));
        std::cerr << errorMsg << std::endl;
        logToFile(errorMsg);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void APIClient::readWeatherData() {
    sqlite3* db;
    sqlite3_open("weather_data.db", &db);
    
    std::string sql = "SELECT * FROM Weather";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string weather = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            double temperature = sqlite3_column_double(stmt, 4);
            double feelsLike = sqlite3_column_double(stmt, 5);
            int humidity = sqlite3_column_int(stmt, 6);
            double windSpeed = sqlite3_column_double(stmt, 7);
            int windDirection = sqlite3_column_int(stmt, 8);
            double rainVolume = sqlite3_column_double(stmt, 9);
            std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));

            std::cout << "Timestamp: " << timestamp << "\n"
                      << "City: " << city << "\n"
                      << "Weather: " << weather << " (" << description << ")\n"
                      << "Temperature: " << temperature << " °C, Feels Like: " << feelsLike << " °C\n"
                      << "Humidity: " << humidity << "%, Wind Speed: " << windSpeed << " m/s, Direction: " << windDirection << "°\n"
                      << "Rain Volume (last 1h): " << rainVolume << " mm\n"
                      << "-----------------------------" << std::endl;
        }
    } else {
        std::cerr << "Error reading data: " << sqlite3_errmsg(db) << std::endl;
        logToFile("Error reading data: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void APIClient::logToFile(const std::string& message) {
    std::ofstream logFile("program_log.txt", std::ios::app);
    logFile << message << std::endl;
}