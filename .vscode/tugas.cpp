#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

class AbstractTask {
public:
    virtual void display() const = 0;
    virtual void edit(const std::string& newName, int newDuration) = 0;
    virtual void remove() = 0;
    virtual ~AbstractTask() {}
};

class Subtask : public AbstractTask {
private:
    std::string name;
    int duration;

public:
    Subtask(const std::string& n, int d) : name(n), duration(d) {}

    void display() const override {
        std::cout << "    Subtask: " << name << " (" << duration << " days)" << std::endl;
    }

    void edit(const std::string& newName, int newDuration) override {
        name = newName;
        duration = newDuration;
    }

    void remove() override {
        // Nothing special to do for removal of a Subtask   
    }

    std::string getName() const {
        return name;
    }
};

class TaskManager; // Forward declaration

class Task : public AbstractTask {
private:
    std::string name;
    int duration;
    std::vector<std::unique_ptr<Subtask>> subtasks;
    TaskManager& taskManager; // Reference to TaskManager

public:
    Task(TaskManager& manager, const std::string& n, int d) : taskManager(manager), name(n), duration(d) {}

    void display() const override {
        std::cout << "Task: " << name << ", Duration: " << duration << " days" << std::endl;
        for (const auto& subtask : subtasks) {
            subtask->display();
        }
    }

    void addSubtask(std::unique_ptr<Subtask> subtask) {
        subtasks.push_back(std::move(subtask));
    }

    void edit(const std::string& newName, int newDuration) override {
        name = newName;
        duration = newDuration;
    }

    void remove() override {
        subtasks.clear();  // Clearing the vector deletes the Subtask objects
        std::cout << "Task removed successfully.\n";
    }

    std::string getName() const {
        return name;
    }

    Subtask* findSubtask(const std::string& subtaskName) const {
        auto it = std::find_if(subtasks.begin(), subtasks.end(), [&subtaskName](const std::unique_ptr<Subtask>& subtask) {
            return subtask->getName() == subtaskName;
        });

        return (it != subtasks.end()) ? it->get() : nullptr;
    }

    void removeSubtask(const std::string& subtaskName);

};

class TaskManager {
private:
    std::vector<std::unique_ptr<Task>> tasks;
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;

public:
    void addTask(std::unique_ptr<Task> task) {
        tasks.push_back(std::move(task));
    }

    void addEdge(const std::string& source, const std::string& destination) {
        adjacencyList[source].push_back(destination);
    }

    void displayTasks() const {
        for (const auto& task : tasks) {
            task->display();
        }
    }

    Task* findTask(const std::string& taskName) const {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&taskName](const std::unique_ptr<Task>& task) {
            return task->getName() == taskName;
        });

        return (it != tasks.end()) ? it->get() : nullptr;
    }

    void removeTask(const std::string& taskName) {
        auto it = std::remove_if(tasks.begin(), tasks.end(), [&taskName](const std::unique_ptr<Task>& task) {
            return task->getName() == taskName;
        });

        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            adjacencyList.erase(taskName);  // Remove the corresponding entry in the adjacency list
            std::cout << "Task removed successfully.\n";
        } else {
            std::cout << "Task not found.\n";
        }
    }

    void displayGraph() const {
        for (const auto& entry : adjacencyList) {
            std::cout << entry.first << " -> ";
            for (const auto& neighbor : entry.second) {
                std::cout << neighbor << " ";
            }
            std::cout << std::endl;
        }
    }

    void removeEdge(const std::string& source, const std::string& destination) {
        auto& neighbors = adjacencyList[source];
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), destination), neighbors.end());
    }
};

void Task::removeSubtask(const std::string& subtaskName) {
    auto it = std::remove_if(subtasks.begin(), subtasks.end(), [&subtaskName](const std::unique_ptr<Subtask>& subtask) {
        return subtask->getName() == subtaskName;
    });

    if (it != subtasks.end()) {
        // Remove the corresponding edge from the adjacency list
        taskManager.removeEdge(name, subtaskName);

        subtasks.erase(it, subtasks.end());
        std::cout << "Subtask removed successfully.\n";
    } else {
        std::cout << "Subtask not found.\n";
    }
}

int main() {
    TaskManager taskManager;

    int choice;

    do {
        std::cout << "Menu:\n";
        std::cout << "1. Add Task\n";
        std::cout << "2. Display Tasks\n";
        std::cout << "3. Edit Task\n";
        std::cout << "4. Edit Subtask\n";
        std::cout << "5. Remove Task\n";
        std::cout << "6. Remove Subtask\n";
        std::cout << "7. Display Graph\n";
        std::cout << "8. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string name;
                int duration;

                std::cout << "Enter task name: ";
                std::cin >> name;

                std::cout << "Enter task duration (in days): ";
                std::cin >> duration;

                auto userTask = std::make_unique<Task>(taskManager, name, duration);

                int numSubtasks;
                std::cout << "Enter the number of subtasks for task " << userTask->getName() << ": ";
                std::cin >> numSubtasks;

                for (int j = 0; j < numSubtasks; ++j) {
                    std::string subtaskName;
                    int subtaskDuration;

                    std::cout << "Enter subtask name: ";
                    std::cin >> subtaskName;

                    std::cout << "Enter subtask duration (in days): ";
                    std::cin >> subtaskDuration;

                    auto userSubtask = std::make_unique<Subtask>(subtaskName, subtaskDuration);
                    userTask->addSubtask(std::move(userSubtask));

                    // Add an edge in the graph
                    taskManager.addEdge(userTask->getName(), subtaskName);
                }

                taskManager.addTask(std::move(userTask));
                break;
            }
            case 2:
                taskManager.displayTasks();
                break;
            case 3: {
                std::string taskName;
                std::cout << "Enter the name of the task to edit: ";
                std::cin >> taskName;

                Task* taskToEdit = taskManager.findTask(taskName);
                if (taskToEdit) {
                    std::string newName;
                    int newDuration;

                    std::cout << "Enter the new name for the task: ";
                    std::cin >> newName;

                    std::cout << "Enter the new duration for the task (in days): ";
                    std::cin >> newDuration;

                    taskToEdit->edit(newName, newDuration);
                    std::cout << "Task edited successfully.\n";
                } else {
                    std::cout << "Task not found.\n";
                }
                break;
            }
            case 4: {
                std::string taskName;
                std::cout << "Enter the name of the task containing the subtask to edit: ";
                std::cin >> taskName;

                Task* taskContainingSubtask = taskManager.findTask(taskName);
                if (taskContainingSubtask) {
                    std::string subtaskName;
                    std::cout << "Enter the name of the subtask to edit: ";
                    std::cin >> subtaskName;

                    Subtask* subtaskToEdit = taskContainingSubtask->findSubtask(subtaskName);
                    if (subtaskToEdit) {
                        std::string newSubtaskName;
                        int newSubtaskDuration;

                        std::cout << "Enter the new name for the subtask: ";
                        std::cin >> newSubtaskName;

                        std::cout << "Enter the new duration for the subtask (in days): ";
                        std::cin >> newSubtaskDuration;

                        subtaskToEdit->edit(newSubtaskName, newSubtaskDuration);
                        std::cout << "Subtask edited successfully.\n";
                    } else {
                        std::cout << "Subtask not found.\n";
                    }
                } else {
                    std::cout << "Task not found.\n";
                }
                break;
            }
            case 5: {
                std::string taskName;
                std::cout << "Enter the name of the task to remove: ";
                std::cin >> taskName;

                taskManager.removeTask(taskName);
                break;
            }
            case 6: {
                std::string taskName;
                std::cout << "Enter the name of the task containing the subtask to remove: ";
                std::cin >> taskName;

                Task* taskContainingSubtask = taskManager.findTask(taskName);
                if (taskContainingSubtask) {
                    std::string subtaskName;
                    std::cout << "Enter the name of the subtask to remove: ";
                    std::cin >> subtaskName;

                    taskContainingSubtask->removeSubtask(subtaskName);
                } else {
                    std::cout << "Task not found.\n";
                }
                break;
            }
            case 7:
                taskManager.displayGraph();
                break;
            case 8:
                std::cout << "Exiting the program.\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);

    return 0;
}
