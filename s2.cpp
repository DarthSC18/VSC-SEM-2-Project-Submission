#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <ctime>
#include <cctype>  // For toupper()
#include <algorithm>

using namespace std;

// Class for storing events
class Event {
public:
    int day;
    int month;
    int year;
    string description;
    bool recurring;  // Flag to mark if the event is recurring every year

    Event(int d, int m, int y, const string& desc, bool recur = false) 
        : day(d), month(m), year(y), description(desc), recurring(recur) {}
};

// Class for storing tasks
class Task {
public:
    int day;
    int month;
    int year;
    string description;

    Task(int d, int m, int y, const string& desc) : day(d), month(m), year(y), description(desc) {}
};

// Function to convert month number to name (uppercase)
string monthW(int month) {
    const string months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    string month_name = months[month - 1];
    for (char &c : month_name) {
        c = toupper(c);  // Convert the month name to uppercase
    }
    return month_name;
}

// Function to display the calendar of a month with highlighted dates
void displayCal(int month, int year, const vector<Event>& events, const vector<Task>& tasks) {
    cout << "\nDisplaying calendar for " << monthW(month) << " " << year << ":\n";
    const int days_in_month[] = {31, (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Print header
    cout << "Sun Mon Tue Wed Thu Fri Sat\n";
    
    // Find the first day of the month
    struct tm time_in = {0};
    time_in.tm_mday = 1;
    time_in.tm_mon = month - 1;
    time_in.tm_year = year - 1900;
    mktime(&time_in);  // Normalize time and get the first weekday of the month
    
    int start_day = time_in.tm_wday;  // Day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
    
    // Print spaces for the first row
    for (int i = 0; i < start_day; ++i) {
        cout << "    ";  // Print 4 spaces for alignment
    }

    // Print the days of the month, and check if the day has an event or task
    for (int i = 1; i <= days_in_month[month - 1]; ++i) {
        bool event_found = false;
        bool task_found = false;

        // Check if there are events on this day
        for (const auto& event : events) {
            if ((event.day == i && event.month == month && event.year == year) || 
                (event.recurring && event.month == month && event.day == i)) {
                event_found = true;
                break;
            }
        }

        // Check if there are tasks on this day
        for (const auto& task : tasks) {
            if (task.day == i && task.month == month && task.year == year) {
                task_found = true;
                break;
            }
        }

        // Print the day with an asterisk if there is an event or task
        if (event_found) {
            cout << setw(2) << i << "* ";  // Ensure uniform width (2 digits for single-digit days)
        }
        else if (task_found) {
            cout << setw(2) << i << "# ";  // Ensure uniform width (2 digits for single-digit days)
        }
        else {
            cout << setw(2) << i << "  ";  // Ensure uniform width (2 digits for single-digit days)
        }

        // Move to the next line after Saturday
        if ((start_day + i) % 7 == 0) {
            cout << "\n";
        }
    }

    // Ensure that the last week of days ends with a newline if necessary
    cout << "\n";
}

// Function to add an event
void addEvent(vector<Event>& events) {
    int day, month, year;
    string description;
    char recurring_choice;
    cout << "Enter event date (day month year): ";
    cin >> day >> month >> year;
    cin.ignore();  // Ignore leftover newline character
    cout << "Enter event description: ";
    getline(cin, description);
    cout << "Is this a recurring event (y/n)? ";
    cin >> recurring_choice;
    bool recurring = (recurring_choice == 'y' || recurring_choice == 'Y');
    
    Event event(day, month, year, description, recurring);
    events.push_back(event);
}

// Function to add a task
void addTask(vector<Task>& tasks) {
    int day, month, year;
    string description;
    cout << "Enter task date (day month year): ";
    cin >> day >> month >> year;
    cin.ignore();  // Ignore leftover newline character
    cout << "Enter task description: ";
    getline(cin, description);

    Task task(day, month, year, description);
    tasks.push_back(task);
}

// Function to remove an event
void removeEvent(vector<Event>& events) {
    int day, month, year;
    cout << "Enter event date to remove (day month year): ";
    cin >> day >> month >> year;

    auto it = remove_if(events.begin(), events.end(), [=](const Event& event) {
        return event.day == day && event.month == month && event.year == year;
    });

    if (it != events.end()) {
        events.erase(it, events.end());
        cout << "Event removed successfully!\n";
    } else {
        cout << "No event found on that date.\n";
    }
}

// Function to remove a task
void removeTask(vector<Task>& tasks) {
    int day, month, year;
    cout << "Enter task date to remove (day month year): ";
    cin >> day >> month >> year;

    auto it = remove_if(tasks.begin(), tasks.end(), [=](const Task& task) {
        return task.day == day && task.month == month && task.year == year;
    });

    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        cout << "Task removed successfully!\n";
    } else {
        cout << "No task found on that date.\n";
    }
}

// Function to display events for a given month and year
void displayEvents(const vector<Event>& events, int month, int year) {
    cout << "\nEvents for " << monthW(month) << " " << year << ":\n";
    bool found = false;
    for (const auto& event : events) {
        if (event.month == month && event.year == year) {
            cout << "Date: " << event.day << "/" << event.month << "/" << event.year << " - " << event.description << "\n";
            found = true;
        } else if (event.recurring && event.month == month) {
            // Display recurring events for the month (without the year)
            cout << "Date: " << event.day << "/" << event.month << " (Recurring every year) - " << event.description << "\n";
            found = true;
        }
    }
    if (!found) {
        cout << "No events for this month.\n";
    }
}

// Function to display tasks for a given month and year
void displayTasks(const vector<Task>& tasks, int month, int year) {
    cout << "\nTasks for " << monthW(month) << " " << year << ":\n";
    bool found = false;
    for (const auto& task : tasks) {
        if (task.month == month && task.year == year) {
            cout << "Date: " << task.day << "/" << task.month << "/" << task.year << " - " << task.description << "\n";
            found = true;
        }
    }
    if (!found) {
        cout << "No tasks for this month.\n";
    }
}

// Function to save events to a file
void saveEvents(const vector<Event>& events) {
    ofstream outFile("events.txt");
    if (outFile.is_open()) {
        for (const auto& event : events) {
            outFile << event.day << " " << event.month << " " << event.year << " " << event.description << " " << event.recurring << "\n";
        }
        outFile.close();
        cout << "Events saved successfully!\n";
    } else {
        cout << "Error saving events!\n";
    }
}

// Function to save tasks to a file
void saveTasks(const vector<Task>& tasks) {
    ofstream outFile("tasks.txt");
    if (outFile.is_open()) {
        for (const auto& task : tasks) {
            outFile << task.day << " " << task.month << " " << task.year << " " << task.description << "\n";
        }
        outFile.close();
        cout << "Tasks saved successfully!\n";
    } else {
        cout << "Error saving tasks!\n";
    }
}

// Function to load events from a file
void loadEvents(vector<Event>& events) {
    ifstream inFile("events.txt");
    if (inFile.is_open()) {
        int day, month, year;
        string description;
        bool recurring;
        while (inFile >> day >> month >> year) {
            inFile.ignore();  // Ignore space after year
            getline(inFile, description);
            inFile >> recurring;
            events.push_back(Event(day, month, year, description, recurring));
        }
        inFile.close();
    }
}

// Function to load tasks from a file
void loadTasks(vector<Task>& tasks) {
    ifstream inFile("tasks.txt");
    if (inFile.is_open()) {
        int day, month, year;
        string description;
        while (inFile >> day >> month >> year) {
            inFile.ignore();  // Ignore space after year
            getline(inFile, description);
            tasks.push_back(Task(day, month, year, description));
        }
        inFile.close();
    }
}

int main() {
    vector<Event> events;
    vector<Task> tasks;
    loadEvents(events);  // Load saved events from the file
    loadTasks(tasks);    // Load saved tasks from the file

    while (true) {
        int choice;
        cout << "\n1. View Calendar\n";
        cout << "2. Add Event\n";
        cout << "3. Add Task\n";
        cout << "4. View Events\n";
        cout << "5. View Tasks\n";
        cout << "6. Remove Event\n";
        cout << "7. Remove Task\n";
        cout << "8. Save Events\n";
        cout << "9. Save Tasks\n";
        cout << "10. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            int month, year;
            cout << "Enter month and year (month year): ";
            cin >> month >> year;
            displayCal(month, year, events, tasks);  // Pass events and tasks to the calendar display function
        } 
        else if (choice == 2) {
            addEvent(events);
        } 
        else if (choice == 3) {
            addTask(tasks);
        }
        else if (choice == 4) {
            int month, year;
            cout << "Enter month and year to view events (month year): ";
            cin >> month >> year;
            displayEvents(events, month, year);
        } 
        else if (choice == 5) {
            int month, year;
            cout << "Enter month and year to view tasks (month year): ";
            cin >> month >> year;
            displayTasks(tasks, month, year);
        } 
        else if (choice == 6) {
            removeEvent(events);
        } 
        else if (choice == 7) {
            removeTask(tasks);
        } 
        else if (choice == 8) {
            saveEvents(events);
        } 
        else if (choice == 9) {
            saveTasks(tasks);
        } 
        else if (choice == 10) {
            break;
        } 
        else {
            cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}
