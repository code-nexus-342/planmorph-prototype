#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>
#include <cstdlib>  // For system()

class WallDetector {
private:
    struct Wall {
        int x1, y1, x2, y2; 
        double length;
        double angle;
        Wall(int x1_, int y1_, int x2_, int y2_) : x1(x1_), y1(y1_), x2(x2_), y2(y2_) {
            length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
            angle = atan2(y2 - y1, x2 - x1) * 180 / M_PI;
        }
    };

    std::vector<Wall> walls;
    const double MIN_LENGTH = 100.0;
    const double ANGLE_TOLERANCE = 10.0;

    void clearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

public:
    bool addWall(int x1, int y1, int x2, int y2) {
        if (x1 == x2 && y1 == y2) {
            std::cout << "Error: Same points (" << x1 << "," << y1 << ")\n";
            return false;
        }
        Wall w(x1, y1, x2, y2);
        std::cout << "Added wall: (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 
                  << "), length = " << w.length << "mm, angle = " << w.angle << "°\n";
        walls.push_back(w);
        return true;
    }

    std::vector<Wall> getValidWalls() const {
        std::vector<Wall> valid_walls;
        for (const auto& w : walls) {
            bool is_horizontal = std::abs(w.angle) < ANGLE_TOLERANCE || 
                                 std::abs(w.angle - 180) < ANGLE_TOLERANCE ||
                                 std::abs(w.angle + 180) < ANGLE_TOLERANCE;
            bool is_vertical = std::abs(w.angle - 90) < ANGLE_TOLERANCE ||
                               std::abs(w.angle + 90) < ANGLE_TOLERANCE;
            if (w.length > MIN_LENGTH && (is_horizontal || is_vertical)) {
                valid_walls.push_back(w);
            }
        }
        return valid_walls;
    }

    void saveToJson(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open " << filename << "\n";
            return;
        }
        auto valid_walls = getValidWalls();
        file << "[\n";
        for (size_t i = 0; i < valid_walls.size(); ++i) {
            const auto& w = valid_walls[i];
            file << "  {\"x1\": " << w.x1 << ", \"y1\": " << w.y1 
                 << ", \"x2\": " << w.x2 << ", \"y2\": " << w.y2 
                 << ", \"length\": " << w.length 
                 << ", \"angle\": " << w.angle << "}";
            if (i < valid_walls.size() - 1) file << ",";
            file << "\n";
        }
        file << "]\n";
        file.close();
        std::cout << "Saved " << valid_walls.size() << " walls to " << filename << "\n";
    }

    void inputWalls() {
        std::string choice;
        while (true) {
            std::cout << "\nEnter wall coordinates (x1 y1 x2 y2) or 'q' to quit: ";
            std::getline(std::cin, choice);
            if (choice == "q" || choice == "Q") break;

            int x1, y1, x2, y2;
            std::istringstream iss(choice);
            if (iss >> x1 >> y1 >> x2 >> y2 && iss.eof()) {
                addWall(x1, y1, x2, y2);
            } else {
                std::cout << "Invalid input. Use format: x1 y1 x2 y2 (e.g., 0 0 300 0)\n";
            }
            clearInputBuffer();
        }
    }
};

int main() {
    WallDetector detector;

    std::cout << "PlanMorph Wall Detector\n";
    detector.inputWalls();

    const std::string jsonFile = "walls.json";
    detector.saveToJson(jsonFile);

    auto valid_walls = detector.getValidWalls();
    std::cout << "\nValid walls:\n";
    if (valid_walls.empty()) {
        std::cout << "No valid walls found.\n";
    } else {
        for (const auto& w : valid_walls) {
            std::cout << "Wall: (" << w.x1 << "," << w.y1 << ") to (" 
                      << w.x2 << "," << w.y2 << "), length = " << w.length 
                      << "mm, angle = " << w.angle << "°\n";
        }
    }

    // Automate running the Python drawing script
    std::cout << "\nCalling Python script to draw walls...\n";
    int ret = std::system("python draw_walls.py");
    if (ret != 0) {
        std::cout << "Warning: Python script execution failed or python not found.\n";
    }

    return 0;
}
