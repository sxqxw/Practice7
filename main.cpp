#include <chrono>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// Адрес светофора и координаты точки на карте.
struct Address
{
    std::string country;
    std::string city;
    std::string street;
    double latitude;
    double longitude;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Address, country, city, street, latitude, longitude)

enum class TrafficLightsState { RED, YELLOW, GREEN };

// Красивый вывод перечисления, как в примере практической работы.
std::ostream& operator<<(std::ostream& out, TrafficLightsState state)
{
    switch (state)
    {
    case TrafficLightsState::RED:    return out << "КРАСНЫЙ";
    case TrafficLightsState::YELLOW: return out << "ЖЁЛТЫЙ";
    case TrafficLightsState::GREEN:  return out << "ЗЕЛЁНЫЙ";
    }
    return out;
}

class TrafficLight
{
private:
    Address address;

public:
    explicit TrafficLight(const Address& address) : address(address) {}

    TrafficLightsState GetCurrentState() const
    {
        const auto now = std::chrono::system_clock::now();
        const auto now_sec = std::chrono::floor<std::chrono::seconds>(now).time_since_epoch();
        const int seconds = static_cast<int>((now_sec % 60).count());

        // Учебный цикл продолжительностью одну минуту.
        if (seconds <= 5 || seconds >= 33 && seconds <= 35)
            return TrafficLightsState::YELLOW;
        if (seconds <= 32)
            return TrafficLightsState::GREEN;
        return TrafficLightsState::RED;
    }

    void GetInfo() const
    {
        const TrafficLightsState state = GetCurrentState();
        std::cout << "------------------------------\n"
                  << "Страна: " << address.country << '\n'
                  << "Город: " << address.city << '\n'
                  << "Улица: " << address.street << '\n'
                  << "Координаты: " << std::fixed << std::setprecision(6)
                  << address.latitude << ", " << address.longitude << '\n'
                  << "Текущий сигнал: " << state << "\n" << std::endl;
    }
};

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::vector<TrafficLight> trafficLights;
    try
    {
        for (const std::string fileName : {"traffic_light1.json", "traffic_light2.json", "traffic_light3.json"})
        {
            std::ifstream file(fileName);
            if (!file) throw std::runtime_error("Не удалось открыть " + fileName);
            nlohmann::json data;
            file >> data;
            trafficLights.emplace_back(data.get<Address>());
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "Ошибка чтения данных: " << error.what() << std::endl;
        return 1;
    }
    using namespace std::chrono_literals;
    while (true)
    {
        for (const auto& trafficLight : trafficLights) trafficLight.GetInfo();
        std::this_thread::sleep_for(23000ms);
    }
}

