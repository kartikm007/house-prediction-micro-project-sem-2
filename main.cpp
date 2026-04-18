  #include "httplib.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace httplib;

// The Core Prediction Engine
class HousePredictor {
private:
    // Base Price per Square Foot by City
    std::map<std::string, double> basePsf = {
        {"Indore", 5800},
        {"Bhopal", 4850},
        {"Jabalpur", 4200},
        {"Gwalior", 5000}
    };

    // Location multipliers (Posh, Moderate, Outer)
    std::map<std::string, double> locationMultipliers = {
        {"Posh", 1.25},     // 25% premium for prime areas
        {"Moderate", 1.00}, // Standard base price
        {"Outer", 0.85}     // 15% discount for outer city areas
    };

public:
    double calculatePrice(std::string city, std::string location, double sqft, int bhk, int bath) {
        // 1. Fetch base rate and multiplier (with fallbacks)
        double baseRate = basePsf.count(city) ? basePsf[city] : 4500.0;
        double locMult = locationMultipliers.count(location) ? locationMultipliers[location] : 1.0;

        // 2. Base area calculation adjusted for location type
        double price = (baseRate * locMult) * sqft;

        // 3. Feature weights 
        price += (bhk - 2) * 380000;   // Premium for extra bedrooms
        price += (bath - 1) * 220000;  // Premium for extra bathrooms

        // 4. Minimum price threshold
        return std::max(price, 500000.0); 
    }
};

int main() {
    Server svr;
    HousePredictor predictor;

    // Route 1: Serve your HTML website files from the current folder
    svr.set_mount_point("/", "./");

    // Route 2: The API Endpoint that your website Javascript talks to
    svr.Get("/predict", [&](const Request& req, Response& res) {
        // Extract parameters from the URL
        std::string city = req.get_param_value("city");
        std::string location = req.get_param_value("location");
        
        // Prevent crashes if inputs are missing
        if (city.empty() || location.empty() || !req.has_param("sqft")) {
            res.set_content("Error: Missing parameters", "text/plain");
            return;
        }

        double sqft = std::stod(req.get_param_value("sqft"));
        int bhk = std::stoi(req.get_param_value("bhk"));
        int bath = std::stoi(req.get_param_value("bath"));

        // Calculate and format price
        double finalPrice = predictor.calculatePrice(city, location, sqft, bhk, bath);
        double priceInLakhs = finalPrice / 100000.0;
        
        // Send response back to the browser
        res.set_content(std::to_string(priceInLakhs), "text/plain");
    });

    std::cout << "========================================" << std::endl;
    std::cout << "🚀 C++ Server running at http://localhost:8081" << std::endl;
    std::cout << "Open your browser and test the prediction!" << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Start listening on port 8081
    if (!svr.listen("0.0.0.0", 8081)) {
        std::cout << "❌ ERROR: Could not start server. Port 8081 is blocked!" << std::endl;
    }
    
    return 0;
}
