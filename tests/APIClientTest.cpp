#include "APIClient.h"
#include <gtest/gtest.h>

// Derived test class to access protected members of APIClient
class APIClientTestHelper : public APIClient {
public:
    using APIClient::makeApiRequest;    // Expose protected method for testing
    using APIClient::setupDatabase;     // Expose protected method for testing
    using APIClient::storeWeatherData;  // Expose protected method for testing
};

// Test that APIClient can make an API request and receive a non-empty response
TEST(APIClientTest, TestApiRequest) {
    APIClientTestHelper client;
    std::string url = "https://api.openweathermap.org/data/2.5/weather?q=Kuala+Lumpur&appid=a9c43513578f728eaf03841d46ca8b16&units=metric";
    std::string response = client.makeApiRequest(url);
    EXPECT_FALSE(response.empty());  // Expect a non-empty response
}

// Test that the database and table setup completes without error
TEST(APIClientTest, TestDatabaseSetup) {
    APIClientTestHelper client;
    ASSERT_NO_THROW(client.setupDatabase());
}

// Test that data can be stored without error in the database
TEST(APIClientTest, TestStoreWeatherData) {
    APIClientTestHelper client;
    client.setupDatabase();  // Ensure the database is set up

    // Attempt to store sample data
    ASSERT_NO_THROW(client.storeWeatherData("Kuala Lumpur", "Clear", "clear sky", 30.0, 33.0, 60, 5.0, 270, 0.0));
}