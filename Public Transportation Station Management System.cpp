/*
Name: NGUYEN QUOC BAO KHANG
Class: 24110FIE4
ID: 24110099
DOB: 25/09/2006
*/
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Vehicle;
struct Station;
struct Passenger;

struct Vehicle 
{
    string route;
    int capacity;
    string status;
    string assignedStation;
    int occupied;

    Vehicle(const string& route_, int capacity_, const string& status_ = "On-time")
        : route(route_), capacity(capacity_), status(status_), assignedStation(""), occupied(0) {}

    virtual ~Vehicle() = default;

    virtual double travelTime(double distanceKm) const 
    {
        const double avgSpeed = 40.0;
        return distanceKm / avgSpeed;
    }

    virtual void displayInfo() const 
    {
        cout << "Vehicle (Route: " << route << ", Capacity: " << capacity
            << ", Occupied: " << occupied << ", Status: " << status;
        if (!assignedStation.empty()) cout << ", Assigned: " << assignedStation;
        cout << ")\n";
    }

    void assignStation(const string& stationName) 
    {
        assignedStation = stationName;
    }

    bool bookSeat() 
    {
        if (occupied < capacity) 
        {
            ++occupied;
            return true;
        }
        return false;
    }

    bool cancelSeat() 
    {
        if (occupied > 0) 
        {
            --occupied;
            return true;
        }
        return false;
    }
    string getRoute() const { return route; }
};

struct Bus : public Vehicle 
{
    Bus(const string& route_, int capacity_, const string& status_ = "On-time")
        : Vehicle(route_, capacity_, status_) {}
};
struct ExpressBus : public Bus 
{
    double speedKmH;        
    vector<string> fewerStops;

    ExpressBus(const string& route_, int capacity_, double speedKmH_ = 60.0,
        const vector<string>& stops_ = {}, const string& status_ = "On-time")
        : Bus(route_, capacity_, status_), speedKmH(speedKmH_), fewerStops(stops_) {}

    double travelTime(double distanceKm) const override
    {
       
        double baseTime = distanceKm / speedKmH;
        return baseTime * 0.8;
    }

    void displayInfo() const override {
        cout << "ExpressBus (Route: " << route << ", Speed: " << speedKmH << " km/h"
            << ", Capacity: " << capacity << ", Occupied: " << occupied
            << ", Status: " << status;
        if (!assignedStation.empty()) cout << ", Assigned: " << assignedStation;
        cout << ", Stops: [";
        for (size_t i = 0; i < fewerStops.size(); ++i) {
            cout << fewerStops[i] << (i + 1 < fewerStops.size() ? ", " : "");
        }
        cout << "])\n";
    }
};
struct Station {
    string name;
    string location;
    string type;
    vector<pair<shared_ptr<Vehicle>, string>> schedules;

    Station(const string& name_, const string& location_, const string& type_)
        : name(name_), location(location_), type(type_) {}

    void addSchedule(shared_ptr<Vehicle> vehicle, const string& time) {
        schedules.emplace_back(vehicle, time);
    }

    void removeScheduleByRoute(const string& route) {
        schedules.erase(remove_if(schedules.begin(), schedules.end(),
            [&](const pair<shared_ptr<Vehicle>, string>& p) {
                return p.first->getRoute() == route;
            }), schedules.end());
    }

    void displayInfo() const {
        cout << "Station: " << name << " (" << type << ") at " << location << "\n";
        if (schedules.empty()) {
            cout << "  No schedules.\n";
        }
        else {
            cout << "  Schedules:\n";
            for (const auto& p : schedules) {
                cout << "   - " << p.first->getRoute() << " at " << p.second
                    << " (" << p.first->status << ")\n";
            }
        }
    }
};


struct Passenger 
{
    string name;
    int id;
    vector<shared_ptr<Vehicle>> tickets; 

    Passenger(const string& name_, int id_) : name(name_), id(id_) {}

    bool bookTicket(shared_ptr<Vehicle> vehicle) 
    {
        if (vehicle->bookSeat()) 
        {
            tickets.push_back(vehicle);
            cout << name << " booked ticket on " << vehicle->getRoute() << "\n";
            return true;
        }
        else {
            cout << "Booking failed for " << name << " on " << vehicle->getRoute()
                << " (Vehicle full)\n";
            return false;
        }
    }

    bool cancelTicket(const string& route) 
    {
        auto it = find_if(tickets.begin(), tickets.end(),
            [&](const shared_ptr<Vehicle>& v) { return v->getRoute() == route; });
        if (it != tickets.end()) 
        {
            if ((*it)->cancelSeat()) 
            {
                cout << name << " canceled ticket on " << route << "\n";
            }
            else 
            {
                cout << "Warning: couldn't cancel seat count for " << route << "\n";
            }
            tickets.erase(it);
            return true;
        }
        else 
        {
            cout << "No ticket found for " << name << " on route " << route << "\n";
            return false;
        }
    }

    void displayInfo() const 
    {
        cout << "Passenger: " << name << " (ID: " << id << ")\n";
        if (tickets.empty()) 
        {
            cout << "  No tickets.\n";
        }
        else {
            cout << "  Tickets:\n";
            for (const auto& v : tickets) 
            {
                cout << "   - " << v->getRoute() << " (" << v->status << ")\n";
            }
        }
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto central = make_shared<Station>("Central", "Downtown", "Train");
    auto mainBus = make_shared<Station>("Main Bus Stop", "City Center", "Bus");

    auto bus101 = make_shared<Bus>("Route 101", 50);
    auto expressX = make_shared<ExpressBus>("Route X", 30, 70.0,
        vector<string>{"Stop A", "Stop D", "Stop Z"});

    bus101->assignStation(mainBus->name);
    expressX->assignStation(mainBus->name);

    mainBus->addSchedule(bus101, "10:00");
    mainBus->addSchedule(expressX, "10:30");

    Passenger alice("Alice", 1);
    Passenger bob("Bob", 2);

    alice.bookTicket(bus101);    
    alice.bookTicket(expressX); 
    bob.bookTicket(expressX);    

    cout << "=== Stations ===\n";
    central->displayInfo();
    cout << "\n";
    mainBus->displayInfo();

    cout << "\n=== Vehicles ===\n";
    bus101->displayInfo();
    expressX->displayInfo();

    cout << "\n=== Passengers ===\n";
    alice.displayInfo();
    bob.displayInfo();

    double distance = 100.0;
    cout << fixed << setprecision(2);
    cout << "\nTravel time for " << distance << " km:\n";
    cout << "  Bus (" << bus101->getRoute() << "): " << bus101->travelTime(distance) << " hours\n";
    cout << "  Express (" << expressX->getRoute() << "): " << expressX->travelTime(distance) << " hours\n";

    alice.cancelTicket("Route 101");
    cout << "\nAfter cancellation:\n";
    alice.displayInfo();
    bus101->displayInfo();

    mainBus->removeScheduleByRoute("Route 101");
    cout << "\nAfter removing Route 101 schedule from Main Bus Stop:\n";
    mainBus->displayInfo();

    return 0;
}
