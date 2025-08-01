#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <memory>

// Forward declarations for circular dependencies
class Ticket;
class User;

// =====================================================================
// UTILITY FUNCTIONS
// =====================================================================

// Clears the console screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Pauses execution and waits for the user to press Enter
void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Prints a styled header for different sections of the UI
void printHeader(const std::string& title) {
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw((80 + title.length()) / 2) << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

// =====================================================================
// CORE CLASSES
// =====================================================================

/**
 * @class Passenger
 * @brief Represents a single passenger's information.
 * Used within the Ticket class to store details of the traveler.
 */
class Passenger {
public:
    std::string name;
    int age;
    char gender;

    void getDetails();
    void displayDetails() const;
};

void Passenger::getDetails() {
    std::cout << "      Enter Passenger Name: ";
    std::getline(std::cin >> std::ws, name);
    
    std::cout << "      Enter Age: ";
    std::cin >> age;
    while(std::cin.fail()) {
        std::cout << "      Invalid input. Please enter a numerical age: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> age;
    }
    
    std::cout << "      Enter Gender (M/F/O): ";
    std::cin >> gender;
}

void Passenger::displayDetails() const {
    std::cout << "      Name: " << std::left << std::setw(20) << name
              << "Age: " << std::setw(5) << age
              << "Gender: " << gender << std::endl;
}

/**
 * @class Train
 * @brief Represents a train, its route, schedule, and seat availability.
 * This class is fundamental to the system, managed by the Admin.
 */
class Train {
public:
    int trainNumber;
    std::string trainName;
    std::string source;
    std::string destination;
    double fare;
    int totalSeats;
    int availableSeats;

    Train(int num, std::string name, std::string src, std::string dest, double f, int seats);

    void display(bool showSeats = false) const;
    bool bookSeats(int numSeats);
    void cancelSeats(int numSeats);
};

Train::Train(int num, std::string name, std::string src, std::string dest, double f, int seats)
    : trainNumber(num), trainName(name), source(src), destination(dest), fare(f), 
      totalSeats(seats), availableSeats(seats) {}

void Train::display(bool showSeats) const {
    std::cout << std::left << std::setw(10) << trainNumber
              << std::setw(25) << trainName
              << std::setw(20) << source
              << std::setw(20) << destination
              << "Rs. " << std::setw(10) << std::fixed << std::setprecision(2) << fare;
    if (showSeats) {
        std::cout << "Seats: " << availableSeats << "/" << totalSeats;
    }
    std::cout << std::endl;
}

bool Train::bookSeats(int numSeats) {
    if (availableSeats >= numSeats) {
        availableSeats -= numSeats;
        return true;
    }
    return false;
}

void Train::cancelSeats(int numSeats) {
    availableSeats += numSeats;
    if (availableSeats > totalSeats) {
        availableSeats = totalSeats; // Failsafe
    }
}

/**
 * @class Ticket
 * @brief Represents a booked ticket, connecting Passengers to a Train.
 * Stored in a map for efficient PNR-based searching.
 */
class Ticket {
public:
    int pnr;
    Train trainDetails;
    std::vector<Passenger> passengers;
    std::string bookedByUsername;

    Ticket(int pnrNum, const Train& train, const std::string& username);
    void addPassenger(const Passenger& passenger);
    void display() const;
};

Ticket::Ticket(int pnrNum, const Train& train, const std::string& username)
    : pnr(pnrNum), trainDetails(train), bookedByUsername(username) {}

void Ticket::addPassenger(const Passenger& passenger) {
    passengers.push_back(passenger);
}

void Ticket::display() const {
    printHeader("TICKET DETAILS");
    std::cout << "  PNR Number: " << pnr << std::endl;
    std::cout << "  Booked By: " << bookedByUsername << std::endl;
    std::cout << "  Train No:   " << trainDetails.trainNumber << " (" << trainDetails.trainName << ")" << std::endl;
    std::cout << "  Route:      " << trainDetails.source << " -> " << trainDetails.destination << std::endl;
    std::cout << "  Total Fare: Rs. " << std::fixed << std::setprecision(2) << trainDetails.fare * passengers.size() << std::endl;
    std::cout << "\n--- Passengers (" << passengers.size() << ") ---" << std::endl;
    for (const auto& p : passengers) {
        p.displayDetails();
    }
    std::cout << std::string(80, '-') << std::endl;
}

/**
 * @class User
 * @brief Represents a user account in the system.
 */
class User {
public:
    std::string username;
    std::string password;
    bool isAdmin;

    User(std::string uname, std::string pwd, bool admin = false)
        : username(uname), password(pwd), isAdmin(admin) {}
};

// =====================================================================
// RAILWAY MANAGEMENT SYSTEM
// =====================================================================

/**
 * @class RailwayManager
 * @brief Main class to manage all railway operations and user interactions.
 * This class demonstrates Object-Oriented Programming principles by
 * encapsulating all data and functionalities.
 */
class RailwayManager {
private:
    std::vector<Train> trains;
    std::map<int, Ticket> bookedTickets; // Map for efficient PNR-based searching.
    std::map<std::string, User> users;   // Map for efficient username-based lookup.
    User* currentUser = nullptr;

    int generatePNR();
    void seedData();

    // Admin functionalities
    void adminDashboard();
    void addNewTrain();
    void viewAllSystemTickets();
    void modifyTrain();

    // User functionalities
    void userDashboard();
    void viewAndSortTrains();
    void bookTicket();
    void viewMyTickets();
    void cancelTicket();

public:
    RailwayManager();
    void run(); // Main application loop
    bool login();
    void registerUser();
};

// --- Constructor & Initializer ---
RailwayManager::RailwayManager() {
    srand(static_cast<unsigned int>(time(nullptr)));
    seedData();
}

void RailwayManager::seedData() {
    // Using STL vector to store train objects
    trains.push_back(Train(12049, "Shatabdi Express", "New Delhi", "Kanpur", 1500.00, 100));
    trains.push_back(Train(12951, "Rajdhani Express", "Mumbai", "New Delhi", 2870.00, 72));
    trains.push_back(Train(22439, "Vande Bharat", "New Delhi", "Katra", 1800.50, 80));
    trains.push_back(Train(12301, "Howrah Rajdhani", "Kolkata", "New Delhi", 2950.00, 72));
    trains.push_back(Train(15027, "Maurya Express", "Gorakhpur", "Hatia", 750.00, 200));

    // Using STL map to store user objects
    users.emplace("admin", User("admin", "admin123", true));
    users.emplace("user", User("user", "user123", false));
}

int RailwayManager::generatePNR() {
    // Keep generating until a unique PNR is found
    while (true) {
        int pnr = 100000 + (rand() % 900000);
        if (bookedTickets.find(pnr) == bookedTickets.end()) {
            return pnr;
        }
    }
}

// --- Login and Registration ---
bool RailwayManager::login() {
    printHeader("LOGIN");
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    auto userIt = users.find(username);
    if (userIt != users.end() && userIt->second.password == password) {
        currentUser = &userIt->second;
        std::cout << "\n✅ Login successful! Welcome, " << currentUser->username << "." << std::endl;
        return true;
    }

    std::cout << "\n❌ Invalid username or password." << std::endl;
    return false;
}

void RailwayManager::registerUser() {
    printHeader("REGISTER NEW USER");
    std::string username, password;
    std::cout << "Enter new username: ";
    std::cin >> username;

    if (users.count(username)) {
        std::cout << "\n❌ Username already exists. Please try another." << std::endl;
        return;
    }

    std::cout << "Enter new password: ";
    std::cin >> password;
    users.emplace(username, User(username, password));
    std::cout << "\n✅ User '" << username << "' registered successfully. Please login." << std::endl;
}

// --- Main Application Loop ---
void RailwayManager::run() {
    while (true) {
        clearScreen();
        printHeader("RAILWAY MANAGEMENT SYSTEM");
        std::cout << "1. Login\n";
        std::cout << "2. Register\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = 0;
        }

        switch (choice) {
            case 1:
                if (login()) {
                    pressEnterToContinue();
                    if (currentUser->isAdmin) {
                        adminDashboard();
                    } else {
                        userDashboard();
                    }
                }
                pressEnterToContinue();
                break;
            case 2:
                registerUser();
                pressEnterToContinue();
                break;
            case 3:
                std::cout << "\nThank you for using the system. Goodbye! 👋\n";
                return;
            default:
                std::cout << "\nInvalid choice. Please try again." << std::endl;
                pressEnterToContinue();
                break;
        }
    }
}

// --- Admin Dashboard & Functions ---
void RailwayManager::adminDashboard() {
    int choice;
    do {
        clearScreen();
        printHeader("ADMIN DASHBOARD");
        std::cout << "1. Add New Train\n";
        std::cout << "2. Modify Existing Train\n";
        std::cout << "3. View All Booked Tickets\n";
        std::cout << "4. Logout\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: addNewTrain(); break;
            case 2: modifyTrain(); break;
            case 3: viewAllSystemTickets(); break;
            case 4: 
                currentUser = nullptr; 
                std::cout << "\nLogging out..." << std::endl;
                break;
            default: std::cout << "\nInvalid choice." << std::endl; break;
        }
        if (choice != 4) pressEnterToContinue();
    } while (choice != 4);
}

void RailwayManager::addNewTrain() {
    printHeader("ADD NEW TRAIN");
    int num, seats;
    std::string name, src, dest;
    double fare;

    std::cout << "Enter Train Number: "; std::cin >> num;
    std::cout << "Enter Train Name: "; std::getline(std::cin >> std::ws, name);
    std::cout << "Enter Source: "; std::getline(std::cin >> std::ws, src);
    std::cout << "Enter Destination: "; std::getline(std::cin >> std::ws, dest);
    std::cout << "Enter Fare: "; std::cin >> fare;
    std::cout << "Enter Total Seats: "; std::cin >> seats;

    trains.emplace_back(num, name, src, dest, fare, seats);
    std::cout << "\n✅ Train '" << name << "' added successfully." << std::endl;
}

void RailwayManager::modifyTrain() {
    printHeader("MODIFY TRAIN DETAILS");
    std::cout << "Enter Train Number to modify: ";
    int trainNum;
    std::cin >> trainNum;

    // Searching using a standard loop with an iterator
    auto it = std::find_if(trains.begin(), trains.end(), 
        [trainNum](const Train& t) { return t.trainNumber == trainNum; });
    
    if (it == trains.end()) {
        std::cout << "\n❌ Train not found." << std::endl;
        return;
    }
    
    std::cout << "\nFound Train: ";
    it->display(true);
    std::cout << "\nEnter new fare (or -1 to keep current): ";
    double newFare;
    std::cin >> newFare;
    if (newFare != -1) {
        it->fare = newFare;
        std::cout << "Fare updated." << std::endl;
    }

    std::cout << "Enter new total seats (or -1 to keep current): ";
    int newSeats;
    std::cin >> newSeats;
    if (newSeats != -1) {
        it->totalSeats = newSeats;
        it->availableSeats = newSeats; // Reset available seats
        std::cout << "Seat capacity updated." << std::endl;
    }
    
    std::cout << "\n✅ Train details modified." << std::endl;
}

void RailwayManager::viewAllSystemTickets() {
    printHeader("ALL BOOKED TICKETS");
    if (bookedTickets.empty()) {
        std::cout << "No tickets have been booked in the system yet." << std::endl;
        return;
    }
    for (const auto& pair : bookedTickets) {
        pair.second.display();
    }
}

// --- User Dashboard & Functions ---
void RailwayManager::userDashboard() {
    int choice;
    do {
        clearScreen();
        printHeader("USER DASHBOARD");
        std::cout << "Welcome, " << currentUser->username << "!\n\n";
        std::cout << "1. View and Sort Available Trains\n";
        std::cout << "2. Book a Ticket\n";
        std::cout << "3. View My Tickets\n";
        std::cout << "4. Cancel a Ticket\n";
        std::cout << "5. Logout\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: viewAndSortTrains(); break;
            case 2: bookTicket(); break;
            case 3: viewMyTickets(); break;
            case 4: cancelTicket(); break;
            case 5: 
                currentUser = nullptr;
                std::cout << "\nLogging out..." << std::endl;
                break;
            default: std::cout << "\nInvalid choice." << std::endl; break;
        }
        if (choice != 5) pressEnterToContinue();
    } while (choice != 5);
}

void RailwayManager::viewAndSortTrains() {
    printHeader("AVAILABLE TRAINS");
    std::cout << "Sort by: 1. Train Number (default) 2. Fare 3. Train Name\nEnter choice: ";
    int sortChoice;
    std::cin >> sortChoice;

    // Using STL algorithm (sort) with different lambda functions for schedule optimization/viewing
    std::sort(trains.begin(), trains.end(), [sortChoice](const Train& a, const Train& b) {
        if (sortChoice == 2) return a.fare < b.fare;
        if (sortChoice == 3) return a.trainName < b.trainName;
        return a.trainNumber < b.trainNumber; // Default
    });

    std::cout << "\n" << std::left << std::setw(10) << "Train No."
              << std::setw(25) << "Train Name"
              << std::setw(20) << "Source"
              << std::setw(20) << "Destination"
              << std::setw(15) << "Fare"
              << "Seats Available" << std::endl;
    std::cout << std::string(110, '-') << std::endl;

    for (const auto& train : trains) {
        train.display(true);
    }
}

void RailwayManager::bookTicket() {
    printHeader("BOOK TICKET");
    std::cout << "Enter Train Number to book: ";
    int trainNum;
    std::cin >> trainNum;

    // Using STL algorithm (find_if) for searching
    auto it = std::find_if(trains.begin(), trains.end(), 
        [trainNum](const Train& t) { return t.trainNumber == trainNum; });

    if (it == trains.end()) {
        std::cout << "\n❌ Invalid Train Number." << std::endl;
        return;
    }

    std::cout << "Enter number of passengers: ";
    int numPassengers;
    std::cin >> numPassengers;
    
    if (!it->bookSeats(numPassengers)) {
        std::cout << "\n❌ Not enough seats available. Only " << it->availableSeats << " left." << std::endl;
        return;
    }

    int pnr = generatePNR();
    Ticket newTicket(pnr, *it, currentUser->username);
    for (int i = 0; i < numPassengers; ++i) {
        std::cout << "\nEnter details for Passenger " << i + 1 << ":" << std::endl;
        Passenger p;
        p.getDetails();
        newTicket.addPassenger(p);
    }

    bookedTickets.emplace(pnr, newTicket);
    std::cout << "\n✅ Ticket booked successfully!" << std::endl;
    newTicket.display();
}

void RailwayManager::viewMyTickets() {
    printHeader("MY BOOKED TICKETS");
    bool found = false;
    for (const auto& pair : bookedTickets) {
        if (pair.second.bookedByUsername == currentUser->username) {
            pair.second.display();
            found = true;
        }
    }
    if (!found) {
        std::cout << "You have not booked any tickets yet." << std::endl;
    }
}

void RailwayManager::cancelTicket() {
    printHeader("CANCEL TICKET");
    std::cout << "Enter PNR Number to cancel: ";
    int pnr;
    std::cin >> pnr;

    // Using std::map's efficient find method
    auto it = bookedTickets.find(pnr);
    if (it == bookedTickets.end() || it->second.bookedByUsername != currentUser->username) {
        std::cout << "\n❌ Invalid PNR or you are not authorized to cancel this ticket." << std::endl;
        return;
    }
    
    int trainNumToUpdate = it->second.trainDetails.trainNumber;
    int seatsToReturn = it->second.passengers.size();
    
    bookedTickets.erase(it);

    // Find the original train and restore the seat count
    auto trainIt = std::find_if(trains.begin(), trains.end(), 
        [trainNumToUpdate](const Train& t) { return t.trainNumber == trainNumToUpdate; });
    
    if (trainIt != trains.end()) {
        trainIt->cancelSeats(seatsToReturn);
    }

    std::cout << "\n✅ Ticket with PNR " << pnr << " has been successfully cancelled." << std::endl;
}

// =====================================================================
// MAIN FUNCTION
// =====================================================================
int main() {
    RailwayManager app;
    app.run();
    return 0;
}
