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
        taskManager.removeEdge(name, "");
        name = newName;
        duration = newDuration;
        taskManager.addEdge(newName, "");
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