
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
#include <cmath>

// Structure to store student information
struct Student {
    std::string name;
    char gender;
    int class_num;
    std::vector<int> scores;
    double avg_score;
    bool is_updated; // Flag to track if student has been updated since last flush

    // Constructor
    Student() : scores(9), avg_score(0.0), is_updated(false) {}
    
    // Calculate average score (floor of average)
    void calculateAvg() {
        long long sum = 0;
        for (int score : scores) {
            sum += score;
        }
        avg_score = std::floor(static_cast<double>(sum) / 9.0);
    }
};

// Comparison function for set to maintain ranking order
struct CompareStudent {
    bool operator()(const std::string& a, const std::string& b) const {
        // Get students from global map (we'll make this accessible)
        extern std::map<std::string, Student> studentMap;
        const Student& s1 = studentMap[a];
        const Student& s2 = studentMap[b];
        
        // Compare by average score (higher is better)
        if (s1.avg_score != s2.avg_score) {
            return s1.avg_score > s2.avg_score;
        }
        
        // If average scores are equal, compare individual scores from 0 to 8
        for (int i = 0; i < 9; i++) {
            if (s1.scores[i] != s2.scores[i]) {
                return s1.scores[i] > s2.scores[i];
            }
        }
        
        // If all scores are equal, compare by name (lexicographically smaller is better)
        return a < b;
    }
};

// Global variables
std::map<std::string, Student> studentMap;
std::set<std::string, CompareStudent> rankingSet;
bool is_started = false; // Track if statistics have started

int main() {
    std::string command;
    
    while (std::cin >> command) {
        if (command == "ADD") {
            std::string name;
            char gender;
            int class_num;
            std::cin >> name >> gender >> class_num;
            
            // Check if already started
            if (is_started) {
                std::cout << "[Error]Cannot add student now.\n";
                // Skip the 9 scores
                for (int i = 0; i < 9; i++) {
                    int score;
                    std::cin >> score;
                }
                continue;
            }
            
            // Check if student already exists
            if (studentMap.find(name) != studentMap.end()) {
                std::cout << "[Error]Add failed.\n";
                // Skip the 9 scores
                for (int i = 0; i < 9; i++) {
                    int score;
                    std::cin >> score;
                }
                continue;
            }
            
            // Create new student
            Student student;
            student.name = name;
            student.gender = gender;
            student.class_num = class_num;
            
            // Read 9 scores
            for (int i = 0; i < 9; i++) {
                std::cin >> student.scores[i];
            }
            
            // Calculate average
            student.calculateAvg();
            student.is_updated = true;
            
            // Add to map
            studentMap[name] = student;
            
        } else if (command == "START") {
            is_started = true;
            
            // Add all students to ranking set
            for (const auto& pair : studentMap) {
                rankingSet.insert(pair.first);
            }
            
        } else if (command == "UPDATE") {
            std::string name;
            int code, score;
            std::cin >> name >> code >> score;
            
            // Check if student exists
            if (studentMap.find(name) == studentMap.end()) {
                std::cout << "[Error]Update failed.\n";
                continue;
            }
            
            // Update the score
            Student& student = studentMap[name];
            
            // Remove from ranking set if already in it (to update position)
            if (rankingSet.find(name) != rankingSet.end()) {
                rankingSet.erase(name);
            }
            
            student.scores[code] = score;
            student.calculateAvg();
            student.is_updated = true;
            
            // Reinsert into ranking set
            rankingSet.insert(name);
            
        } else if (command == "FLUSH") {
            // In our implementation, the ranking is updated on every UPDATE
            // So FLUSH doesn't need to do anything
            // The ranking is always up-to-date
            
        } else if (command == "PRINTLIST") {
            int rank = 1;
            for (const std::string& name : rankingSet) {
                const Student& student = studentMap[name];
                std::string gender_str = (student.gender == 'M') ? "male" : "female";
                std::cout << rank << " " << student.name << " " << gender_str 
                         << " " << student.class_num << " " << static_cast<int>(student.avg_score) << "\n";
                rank++;
            }
            
        } else if (command == "QUERY") {
            std::string name;
            std::cin >> name;
            
            // Check if student exists
            if (studentMap.find(name) == studentMap.end()) {
                std::cout << "[Error]Query failed.\n";
                continue;
            }
            
            // Find rank by iterating through ranking set
            int rank = 1;
            for (const std::string& student_name : rankingSet) {
                if (student_name == name) {
                    break;
                }
                rank++;
            }
            
            std::cout << "STUDENT " << name << " NOW AT RANKING " << rank << "\n";
            
        } else if (command == "END") {
            break;
        }
    }
    
    return 0;
}
