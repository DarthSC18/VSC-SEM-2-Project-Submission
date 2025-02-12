#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;  // This allows you to use standard library features without std:: prefix

// Define a Task structure so we cam use the variables later on in the code
struct Task {
    int id; //Name to identify the task
    int duration; // How long the task should run (in time slots)
    int priority; // Priority (higher value means higher priority)
    bool isPaused; // If the task is paused or not

    Task(int id, int duration, int priority)
        : id(id), duration(duration), priority(priority), isPaused(false) {}

    // Comparator to prioritize tasks in a priority queue based on priority
    bool operator<(const Task& other) const {
        return priority > other.priority; // Smaller the number, higher is the priority //Operator< is connected to std::priority queue
    }
};

// Scheduler class to manage the tasks
class MicroScheduler {
public:
    MicroScheduler(int timeSlotLength) : timeSlotLength(timeSlotLength) 
    {}
    void addTask(const Task& task) // Task is structure and task is pass by refrence
     {
        taskQueue.push(task);
    }

    void Execute() {
        // Execute only unpaused tasks
        cout << "Executing unpaused tasks...\n";
        vector<Task> remainingTasks;
        while (!taskQueue.empty()) {
            Task currentTask = taskQueue.top();//.top is the print the top value
            taskQueue.pop();//.pop is to remove the top value
            if (!currentTask.isPaused) {
                // Execute task in a separate thread
                thread t(&MicroScheduler::executeTask, this, ref(currentTask));
                t.join();  // Wait for the task to finish
            } else {
                // Store paused tasks to be resumed later
                remainingTasks.push_back(currentTask);
            }
        }

        // Re-add remaining tasks (paused ones) back to the queue for future execution
        for (const auto& task : remainingTasks) {
            taskQueue.push(task);
        }
    }

    void resumeTasks() {
        // Resume all paused tasks and reinsert them into the queue
        vector<Task> tempQueue;
        while (!taskQueue.empty()) {
            Task task = taskQueue.top();
            taskQueue.pop();
            if (task.isPaused) {
                task.isPaused = false; // Mark as resumed
            }
            tempQueue.push_back(task);
        }

        // Re-insert all tasks back into the queue
        for (const auto& task : tempQueue) {
            taskQueue.push(task);
        }
    }

    void pauseTask(int taskId) {
        // Find task and mark it as paused
        priority_queue<Task> tempQueue;
        while (!taskQueue.empty()) {
            Task task = taskQueue.top();
            taskQueue.pop();
            if (task.id == taskId) {
                task.isPaused = true;
            }
            tempQueue.push(task);
        }
        taskQueue = tempQueue;
    }

    void printQueue() {
        cout << "Task Queue (Priority Order):\n";
        priority_queue<Task> tempQueue = taskQueue;
        if(!tempQueue.empty()){
        while (!tempQueue.empty()) {
            Task task = tempQueue.top();
            tempQueue.pop();
            cout << "Task ID: " << task.id << ", Priority: " << task.priority << ", Duration: " << task.duration << " slots\n";
        }}
        else
        cout<<" Queue is Currently Empty";
    }

private:
    int timeSlotLength; // Duration of each time slot
    priority_queue<Task> taskQueue;
    mutex mtx;

    void executeTask(Task& task) {
        lock_guard<mutex> lock(mtx); // Ensure thread-safety for shared resource
        int timeRemaining = task.duration;
        cout << "Executing Task " << task.id << " (Priority: " << task.priority << ")\n";
        while (timeRemaining > 0) {
            this_thread::sleep_for(chrono::milliseconds(timeSlotLength));  // Simulate time slot
            timeRemaining--;
            cout << "Task " << task.id << " executed for 1 time slot. Remaining: " << timeRemaining << "\n";
        }
    }
};

int main() {
    // Create a scheduler with a time slot duration of 500ms
    int timeSlotLength = 500;  // Duration of each time slot in milliseconds
    MicroScheduler scheduler(timeSlotLength);

    bool running = true;
    while (running) {
        cout << "\nChoose an option:\n";
        cout << "1. Add Task\n";
        cout << "2. Execute Tasks\n";
        cout << "3. Pause Task\n";
        cout << "4. Resume Paused Task(s)\n";
        cout << "5. Print Task Queue\n";
        cout << "6. Exit\n";

        int c;
        cin >> c;

        switch (c) {
            case 1: {
                // Add a new task
                int id, duration, priority;
                cout << "Enter Task ID: ";
                cin >> id;
                cout << "Enter Task Duration (in time slots): ";
                cin >> duration;
                cout << "Enter Task Priority: ";
                cin >> priority;

                // Add task to the scheduler
                scheduler.addTask(Task(id, duration, priority));
                break;
            }

            case 2: {
                // Execute only unpaused tasks
                scheduler.Execute();
                break;
            }

            case 3: {
                // Pause a task
                int taskId;
                cout << "Enter Task ID to pause: ";
                cin >> taskId;
                scheduler.pauseTask(taskId);
                cout << "Task " << taskId << " is now paused.\n";
                break;
            }

            case 4: {
                // Resume all paused tasks
                scheduler.resumeTasks();
                cout << "All paused tasks are now resumed.\n";
                break;
            }

            case 5: {
                // Print the task queue in priority order
                scheduler.printQueue();
                break;
            }

            case 6: {
                // Exit the program
                running = false;
                cout << "Exiting...\n";
                break;
            }

            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
    }

    return 0;
}
