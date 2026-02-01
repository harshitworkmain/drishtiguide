// DrishtiGuide Web Interface JavaScript

class DrishtiGuideInterface {
    constructor() {
        this.apiBase = window.location.origin;
        this.isConnected = false;
        this.updateInterval = null;
        this.alerts = [];
        this.startTime = Date.now();
        
        this.init();
    }
    
    init() {
        this.bindEvents();
        this.startDataUpdates();
        this.initializeMap();
        this.updateUptime();
    }
    
    bindEvents() {
        // Window events
        window.addEventListener('online', () => this.handleConnectionChange(true));
        window.addEventListener('offline', () => this.handleConnectionChange(false));
        
        // Modal events
        document.addEventListener('keydown', (e) => {
            if (e.key === 'Escape') {
                this.closeEmergencyModal();
            }
        });
    }
    
    // API Communication
    async fetchData(endpoint) {
        try {
            const response = await fetch(`${this.apiBase}${endpoint}`);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return await response.json();
        } catch (error) {
            console.error(`Error fetching ${endpoint}:`, error);
            this.addAlert('error', `Failed to fetch data: ${error.message}`);
            return null;
        }
    }
    
    // Data Updates
    async startDataUpdates() {
        // Initial data load
        await this.updateAllData();
        
        // Set up periodic updates
        this.updateInterval = setInterval(() => {
            this.updateAllData();
        }, 5000); // Update every 5 seconds
    }
    
    async updateAllData() {
        await Promise.all([
            this.updateGPSData(),
            this.updateSystemStatus(),
            this.updateSensorData()
        ]);
    }
    
    async updateGPSData() {
        const data = await this.fetchData('/gps');
        if (data) {
            this.updateLocationDisplay(data);
            this.updateMap(data.latitude, data.longitude);
        }
    }
    
    async updateSystemStatus() {
        const data = await this.fetchData('/status');
        if (data) {
            this.updateSystemDisplay(data);
            this.updateConnectionStatus(true);
        }
    }
    
    async updateSensorData() {
        // For now, we'll simulate sensor data
        // In real implementation, this would come from /sensors endpoint
        this.updateSensorDisplay({
            distance: Math.floor(Math.random() * 400) + 10,
            motion: Math.random() > 0.3 ? 'Active' : 'Inactive'
        });
    }
    
    // Display Updates
    updateLocationDisplay(data) {
        const elements = {
            latitude: document.getElementById('latitude'),
            longitude: document.getElementById('longitude'),
            lastFallTime: document.getElementById('lastFallTime')
        };
        
        if (elements.latitude) elements.latitude.textContent = data.latitude || '--';
        if (elements.longitude) elements.longitude.textContent = data.longitude || '--';
        if (elements.lastFallTime) {
            elements.lastFallTime.textContent = data.fallTime && data.fallTime !== 'N/A' 
                ? data.fallTime 
                : 'No falls detected';
        }
    }
    
    updateSystemDisplay(data) {
        // Update status badges
        this.updateStatusBadge('fallStatus', data.sensors?.fallDetection || 'active');
        this.updateStatusBadge('gpsStatus', data.sensors?.gps || 'active');
        this.updateStatusBadge('wifiStatus', data.sensors?.wifi || 'active');
        
        // Update battery level
        const batteryElement = document.getElementById('batteryLevel');
        if (batteryElement && data.batteryLevel) {
            batteryElement.textContent = `${data.batteryLevel}%`;
        }
    }
    
    updateSensorDisplay(data) {
        // Update distance
        const distanceElement = document.getElementById('distance');
        const distanceBar = document.getElementById('distanceBar');
        
        if (distanceElement && data.distance !== undefined) {
            distanceElement.textContent = `${data.distance} cm`;
            if (distanceBar) {
                const percentage = Math.min(100, (data.distance / 400) * 100);
                distanceBar.style.width = `${percentage}%`;
            }
        }
        
        // Update motion
        const motionElement = document.getElementById('motion');
        const motionDot = document.getElementById('motionDot');
        
        if (motionElement && data.motion) {
            motionElement.textContent = data.motion;
            if (motionDot) {
                motionDot.className = `motion-dot ${data.motion === 'Active' ? 'active' : ''}`;
            }
        }
    }
    
    updateStatusBadge(elementId, status) {
        const element = document.getElementById(elementId);
        if (element) {
            element.className = 'status-badge';
            
            switch (status.toLowerCase()) {
                case 'active':
                case 'normal':
                    element.classList.add('normal');
                    element.textContent = status.charAt(0).toUpperCase() + status.slice(1);
                    break;
                case 'warning':
                    element.classList.add('warning');
                    element.textContent = 'Warning';
                    break;
                case 'error':
                case 'critical':
                    element.classList.add('danger');
                    element.textContent = 'Error';
                    break;
                default:
                    element.classList.add('normal');
                    element.textContent = 'Unknown';
            }
        }
    }
    
    updateConnectionStatus(connected) {
        const statusDot = document.getElementById('statusDot');
        const statusText = document.getElementById('statusText');
        
        if (connected) {
            statusDot.className = 'status-dot connected';
            statusText.textContent = 'Connected';
            this.isConnected = true;
        } else {
            statusDot.className = 'status-dot error';
            statusText.textContent = 'Disconnected';
            this.isConnected = false;
            this.addAlert('error', 'Lost connection to device');
        }
    }
    
    // Map Integration
    initializeMap() {
        const mapElement = document.getElementById('map');
        if (mapElement) {
            // In a real implementation, this would integrate with a mapping service
            // like OpenStreetMap, Google Maps, or Mapbox
            this.mapInitialized = true;
        }
    }
    
    updateMap(latitude, longitude) {
        const mapElement = document.getElementById('map');
        if (mapElement && latitude && longitude) {
            // Update map with new coordinates
            // This is a placeholder implementation
            mapElement.innerHTML = `
                <div style="text-align: center; padding: 2rem;">
                    <i class="fas fa-map-marker-alt" style="font-size: 2rem; color: var(--primary-color); margin-bottom: 1rem;"></i>
                    <p style="margin: 0; font-weight: 600;">Location Tracked</p>
                    <p style="margin: 0; color: var(--text-secondary); font-size: 0.875rem;">
                        ${latitude}, ${longitude}
                    </p>
                </div>
            `;
        }
    }
    
    // Alert Management
    addAlert(type, message) {
        const alert = {
            id: Date.now(),
            type: type, // 'info', 'warning', 'error', 'success'
            message: message,
            timestamp: new Date()
        };
        
        this.alerts.unshift(alert);
        if (this.alerts.length > 10) {
            this.alerts.pop(); // Keep only last 10 alerts
        }
        
        this.displayAlerts();
        
        // Show emergency modal for critical alerts
        if (type === 'error' && message.toLowerCase().includes('fall')) {
            this.showEmergencyModal(alert);
        }
    }
    
    displayAlerts() {
        const container = document.getElementById('alertsContainer');
        if (!container) return;
        
        container.innerHTML = this.alerts.map(alert => `
            <div class="alert-item ${alert.type}">
                <i class="fas ${this.getAlertIcon(alert.type)}"></i>
                <div class="alert-content">
                    <p class="alert-message">${alert.message}</p>
                    <span class="alert-time">${this.formatTime(alert.timestamp)}</span>
                </div>
            </div>
        `).join('');
    }
    
    getAlertIcon(type) {
        const icons = {
            'info': 'fa-info-circle',
            'warning': 'fa-exclamation-triangle',
            'error': 'fa-times-circle',
            'success': 'fa-check-circle'
        };
        return icons[type] || 'fa-info-circle';
    }
    
    formatTime(timestamp) {
        const now = new Date();
        const diff = now - timestamp;
        
        if (diff < 60000) {
            return 'Just now';
        } else if (diff < 3600000) {
            return `${Math.floor(diff / 60000)} minutes ago`;
        } else if (diff < 86400000) {
            return `${Math.floor(diff / 3600000)} hours ago`;
        } else {
            return timestamp.toLocaleDateString();
        }
    }
    
    // Emergency Modal
    showEmergencyModal(alert) {
        const modal = document.getElementById('emergencyModal');
        const locationElement = document.getElementById('emergencyLocation');
        const timeElement = document.getElementById('emergencyTime');
        
        if (locationElement) {
            locationElement.textContent = 'Getting location...';
            // Update with actual GPS coordinates
            this.updateGPSData().then(data => {
                if (data && data.latitude && data.longitude) {
                    locationElement.textContent = `${data.latitude}, ${data.longitude}`;
                }
            });
        }
        
        if (timeElement) {
            timeElement.textContent = alert.timestamp.toLocaleTimeString();
        }
        
        if (modal) {
            modal.classList.remove('hidden');
        }
    }
    
    closeEmergencyModal() {
        const modal = document.getElementById('emergencyModal');
        if (modal) {
            modal.classList.add('hidden');
        }
    }
    
    callEmergency() {
        // In a real implementation, this would initiate an emergency call
        if (confirm('Call emergency services?')) {
            window.location.href = 'tel:911'; // or appropriate emergency number
        }
    }
    
    // Utility Functions
    updateUptime() {
        setInterval(() => {
            const uptimeElement = document.getElementById('uptime');
            if (uptimeElement) {
                const uptime = Date.now() - this.startTime;
                uptimeElement.textContent = this.formatUptime(uptime);
            }
        }, 1000);
    }
    
    formatUptime(milliseconds) {
        const seconds = Math.floor(milliseconds / 1000);
        const minutes = Math.floor(seconds / 60);
        const hours = Math.floor(minutes / 60);
        const days = Math.floor(hours / 24);
        
        const time = [];
        if (days > 0) time.push(`${days}d`);
        time.push(`${hours % 24}h`);
        time.push(`${minutes % 60}m`);
        time.push(`${seconds % 60}s`);
        
        return time.join(':');
    }
    
    handleConnectionChange(online) {
        if (online && !this.isConnected) {
            this.addAlert('info', 'Connection restored');
            this.startDataUpdates();
        } else if (!online) {
            this.updateConnectionStatus(false);
            if (this.updateInterval) {
                clearInterval(this.updateInterval);
            }
        }
    }
    
    // Public API
    destroy() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
        }
        window.removeEventListener('online', this.handleConnectionChange);
        window.removeEventListener('offline', this.handleConnectionChange);
    }
}

// Global functions for HTML onclick handlers
function closeEmergencyModal() {
    window.drishtiGuideInterface.closeEmergencyModal();
}

function callEmergency() {
    window.drishtiGuideInterface.callEmergency();
}

// Initialize the application when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.drishtiGuideInterface = new DrishtiGuideInterface();
    
    // Add initial welcome message
    window.drishtiGuideInterface.addAlert('info', 'System initialized successfully');
    
    // Simulate some initial activity
    setTimeout(() => {
        window.drishtiGuideInterface.addAlert('warning', 'Low battery warning: 15% remaining');
    }, 10000);
    
    // Simulate motion detection
    setTimeout(() => {
        window.drishtiGuideInterface.addAlert('info', 'Motion detected: User walking');
    }, 15000);
});

// Cleanup on page unload
window.addEventListener('beforeunload', () => {
    if (window.drishtiGuideInterface) {
        window.drishtiGuideInterface.destroy();
    }
});