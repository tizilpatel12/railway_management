  C++ Railway Management System
A comprehensive, console-based Railway Management System built using modern C++. This project demonstrates Object-Oriented Programming (OOP) principles, efficient data handling with STL containers, and the use of standard algorithms for a responsive user experience.

## Key Features
Dual User Roles: Separate, password-protected interfaces for Admin and User.

Secure Authentication: A robust login and registration system for user accounts.

Admin Dashboard:

Add new train routes to the system.

Modify existing train details like fare and seat capacity.

View a complete list of all tickets booked across the system.

User Dashboard:

Book tickets for multiple passengers.

View personal booking history.

Cancel tickets with automatic seat recalculation.

Efficient Data Management:

Uses std::vector to manage the list of trains.

Uses std::map for fast, PNR-based searching, viewing, and cancellation of tickets (O(
logn)).

Dynamic Train Schedules:

View all available trains with real-time seat availability.

Uses std::sort with custom comparators (lambda functions) to organize train schedules by number, fare, or name.

## Technology Stack
Language: C++

Core Concepts: Object-Oriented Programming (OOP)

Encapsulation: Data and functions are bundled into classes like RailwayManager, Train, and Ticket.

Abstraction: Complex operations are hidden behind a simple interface.

Standard Template Library (STL):

Containers: std::vector, std::map, std::string

Algorithms: std::sort, std::find_if
