#define CURL_STATICLIB
#define _USE_MATH_DEFINES

#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "details.h"
#include <iostream>
#include <string>
#include <cmath>

#define R 6371

using namespace nlohmann;

static size_t WriteCallBack(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

double Gaversinus(double lat1, double lon1, double lat2, double lon2)
{
    double lat = (lat2 - lat1) * (M_PI / 180);
    double lon = (lon2 - lon1) * (M_PI / 180);
    double rez = sin(lat / 2) * sin(lat / 2) + cos(lat1 * (M_PI / 180)) * cos(lat2 * (M_PI / 180)) * sin(lon / 2) * sin(lon / 2);
    double d = 2 * R * atan2(sqrt(rez), sqrt(1 - rez));
    return d;
}

int main()
{
    std::string city1, city2;
    std::cout << "Enter 2 cities: ";
    std::cin >> city1 >> city2;

    std::string url = "https://api.api-ninjas.com/v1/city?name=";
    std::string read_buffer;

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, details::X_Api_Key);

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, (url + city1).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

    curl_easy_perform(curl);

    if (read_buffer.substr(1, read_buffer.size() - 2).empty())
    {
        std::cout << "Incorrect city" << std::endl;
        system("pause");
        return 1;
    }

    json city1_json = json::parse(read_buffer.substr(1, read_buffer.size() - 2));

    read_buffer = "";

    curl_easy_setopt(curl, CURLOPT_URL, (url + city2).c_str());
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (read_buffer.substr(1, read_buffer.size() - 2).empty())
    {
        std::cout << "Incorrect city" << std::endl;
        system("pause");
        return 1;
    }

    json city2_json = json::parse(read_buffer.substr(1, read_buffer.size() - 2));

    std::cout << "Distance: " << Gaversinus(city1_json["latitude"].get<double>(), city1_json["longitude"].get<double>(), city2_json["latitude"].get<double>(), city2_json["longitude"].get<double>()) << " km" << std::endl;

    system("pause");

	return 0;
}