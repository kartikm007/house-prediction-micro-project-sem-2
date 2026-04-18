 #include "httplib.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace httplib;

// This is the class you will explain to your evaluator
class HousePredictor {
private:
    std::map<std::string, double> basePsf = {
        {"Indore", 5800},
        {"Bhopal", 4850},
        {"Jabalpur", 4200},
        {"Gwalior", 5000}
    };

public:
    // Core algorithm for estimation
    double calculatePrice(std::string city, double sqft, int bhk, int bath) {
        // 1. Base area calculation
        double price = basePsf[city] * sqft;

        // 2. Feature weights 
        price += (bhk - 2) * 380000;   // Premium for extra bedrooms
        price += (bath - 1) * 220000;  // Premium for extra bathrooms

        // 3. Minimum price threshold
        return std::max(price, 500000.0); 
    }
};

int main() {
    Server svr;
    HousePredictor predictor;

    // Route 1: Serve your HTML website
    svr.set_mount_point("/", "./");

    // Route 2: The API Endpoint that your website talks to
    svr.Get("/predict", [&](const Request& req, Response& res) {
        std::string city = req.get_param_value("city");
        double sqft = std::stod(req.get_param_value("sqft"));
        int bhk = std::stoi(req.get_param_value("bhk"));
        int bath = std::stoi(req.get_param_value("bath"));

        double finalPrice = predictor.calculatePrice(city, sqft, bhk, bath);
        double priceInLakhs = finalPrice / 100000.0;
        
        res.set_content(std::to_string(priceInLakhs), "text/plain");
    });

     std::cout << "========================================" << std::endl;
    std::cout << "🚀 C++ Server running at http://localhost:8081" << std::endl;
    std::cout << "Open your browser and test the prediction!" << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;
    std::cout << "========================================" << std::endl;
    
    // We added an error check, and changed localhost->0.0.0.0 and port to 8081
    if (!svr.listen("0.0.0.0", 8081)) {
        std::cout << "❌ ERROR: Could not start server. Port 8081 is blocked!" << std::endl;
    }
    
    return 0;
}