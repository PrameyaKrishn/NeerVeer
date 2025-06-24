// This file contains the JavaScript code for the website. It handles dynamic interactions, such as updating the statistics displayed on the main page, fetching data, and managing user interactions.

document.addEventListener('DOMContentLoaded', function() {
    // Sample data for demonstration purposes
    const farmStats = {
        waterLevel: '75%',
        date: new Date().toLocaleDateString(),
        temperature: '22°C',
        expectedWeather: 'Sunny',
        soilMoisture: '30%',
        soilPh: '6.5',
        alerts: 'No alerts',
        soilTemperature: '18°C'
    };

    // Function to update the statistics on the page
    function updateStats() {
        document.getElementById('water-level').textContent = farmStats.waterLevel;
        document.getElementById('date').textContent = farmStats.date;
        document.getElementById('temperature').textContent = farmStats.temperature;
        document.getElementById('expected-weather').textContent = farmStats.expectedWeather;
        document.getElementById('soil-moisture').textContent = farmStats.soilMoisture;
        document.getElementById('soil-ph').textContent = farmStats.soilPh;
        document.getElementById('alerts').textContent = farmStats.alerts;
        document.getElementById('soil-temperature').textContent = farmStats.soilTemperature;
    }

    // Initial call to update the stats on page load
    updateStats();

    // Example of how to refresh data (this could be replaced with actual data fetching)
    setInterval(() => {
        // Here you would typically fetch new data from an API or database
        // For this example, we will just log a message
        console.log('Fetching new data...');
        // Update the stats again (in a real scenario, you would update farmStats with new data)
        updateStats();
    }, 60000); // Refresh every minute
});