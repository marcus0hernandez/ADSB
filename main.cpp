/*
 * Author: Marcus Hernandez
 * ADSB flight tracker & map
 */

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <ctime> // used to make epoch time more readable

/*
 * Callback function for libcurl to call in the code
 * to feed data as the download happens
 *
 * Parameters:
 * data: bytes recieved from the network
 * size: size of each member in bytes (usually 1 byte)
 * numMembers: number of members (real byte count)
 * out: comes from curl_easy_setopt(curl, CURL_OPT_WRITEDATA, &response)
 */
static size_t writeCallback(void *data, size_t size, size_t numMembers,
                            std::string *out)
{
    // Compute number of bytes received
    size_t totalSize = size * numMembers;

    // append the data onto the existing string
    out->append((char *)data, totalSize);
    return totalSize;
}

// Function to get a web address with the url input
std::string getHttp(const std::string &url)
{
    CURL *curl = curl_easy_init(); // function returns curl handle
    std::string response;

    if (curl)
    {
        // Address
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Callback for recieved data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        // passed to callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // 1 = yes follow redirect, Long literal
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // 15 secs max, Long literal
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

        // Perform blocking network transfer
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        }

        // Free the easy handle
        curl_easy_cleanup(curl);
    }
    return response;
}

#include <ctime>

// Converts an epoch timestamp into a readable UTC date/time string
std::string epochToString(long epochTime)
{
    if (epochTime == 0)
    {
        return "unknown";
    }

    time_t rawTime = static_cast<time_t>(epochTime);
    std::tm *timeInfo = std::gmtime(&rawTime); // UTC time (OpenSky uses UTC)

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", timeInfo); // year-month-day-hr-min-sec

    return std::string(buffer);
}

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // This url is for flights arriving at KBOS beginnng and ending in epoch times
    std::string url = "https://opensky-network.org/api/flights/arrival?airport=KBOS&begin=1785337200&end=1785350940";
    std::string raw = getHttp(url);

    // Print flight information
    try
    {
        nlohmann::json data = nlohmann::json::parse(raw);

        std::cout << "Flights found: " << data.size() << "\n\n";

        for (auto &flight : data)
        {
            std::string icao24 = flight.value("icao24", "");
            long firstSeen = flight.value("firstSeen", 0);
            long lastSeen = flight.value("lastSeen", 0);

            std::string estDepartureAirport = flight.value("estDepartureAirport", "unknown");

            std::cout << "ICAO24: " << icao24
                      << " | From: " << estDepartureAirport
                      << " | Landed at: " << epochToString(lastSeen)
                      << "\n";
        }
    }
    catch (nlohmann::json::exception &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        std::cerr << "Rae response: " << raw << std::endl;
    }

    curl_global_cleanup();
    return 0;
}