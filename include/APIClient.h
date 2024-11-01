#ifndef APICLIENT_H
#define APICLIENT_H

#include <string>
#include <thread>
#include <atomic>

class APIClient {
public:
    APIClient();
    ~APIClient();

    void startFetching();
    void stopFetching();
    void readWeatherData();

protected:
    void fetchData();
    std::string makeApiRequest(const std::string& url);

    void setupDatabase();
    void storeWeatherData(const std::string& city, const std::string& weather, const std::string& description,
                          double temperature, double feelsLike, int humidity, double windSpeed,
                          int windDirection, double rainVolume);

    std::thread fetchThread;
    std::atomic<bool> isRunning;

    void logToFile(const std::string& message);  // Logging function
};

#endif  // APICLIENT_H