
const img = document.getElementById('stream');
const saveBtn = document.getElementById('save-frame-btn');
const addressSelect = document.getElementById('websocket-address');
const errorBanner = document.getElementById('error-banner');
const connectBtn = document.getElementById('connect-btn');
const consoleBox = document.getElementById('console-box');

let ws = null;

function resolveAddress() {
    const address = addressSelect.value;
    return `ws://${address}:443/ws`;
}

function connectWebSocket() {
    // Check if socket exists and is not already closed
    if (ws && ws.readyState !== WebSocket.CLOSED) {
        ws.close(); // Close the existing connection
        // Wait briefly to ensure the close completes
        setTimeout(() => {
            establishConnection();
        }, 100); // 100ms delay
    } else {
        establishConnection(); // No existing connection, proceed directly
    }
}

function establishConnection() {
    let wsUrl = resolveAddress();

    ws = new WebSocket(wsUrl);

    ws.binaryType = 'arraybuffer';

    ws.onopen = () => {
        console.log(`Connected to WebSocket at ${wsUrl}`);
        ws.send('Hello! FROM the client');
        errorBanner.classList.remove('show');
        connectBtn.textContent = "Connected";
    };

    ws.onmessage = (event) => {
        if (event.data instanceof ArrayBuffer) {
            const blob = new Blob([event.data], { type: 'image/jpeg' });
            let frameUrl = URL.createObjectURL(blob);
            img.src = frameUrl;
            saveBtn.href = frameUrl;
        } else {
            console.log('Received:', event.data);
            consoleBox.innerHTML += event.data + '<br>';
            consoleBox.scrollTop = consoleBox.scrollHeight;
        }
    };

    ws.onclose = (event) => {
        console.log('WebSocket disconnected, code: ', event.code);
        connectBtn.textContent = "Reconnect";
        setTimeout(() => {
            establishConnection();
        }, 100);
    };

    ws.onerror = (error) => {
        console.warn('WebSocket error, retrying...', error);
        errorBanner.classList.add('show');
        connectBtn.textContent = "Try Reconnect";
        setTimeout(() => {
            establishConnection();
        }, 100);
    };

}

connectWebSocket();

setInterval(() => {
    if (ws.readyState === WebSocket.CLOSED || ws.readyState === WebSocket.CLOSING) {
        console.log('Retrying connection...');
        establishConnection();
    }
}, 50);

connectBtn.addEventListener('click', () => connectWebSocket());

const fpsSlider = document.getElementById('fps-slider');
const fpsValue = document.getElementById('fps-value');

// FPS Regulator

let intervalId = null;
let currentFPS = 5;

// Start or update the interval based on FPS
function updateInterval(fps) {
    if (intervalId) {
        clearInterval(intervalId); // Clear existing interval
    }
    currentFPS = fps;
    fpsValue.textContent = fps; // Update displayed FPS
    fpsSlider.value = fps;
    if (fps === 0) {
        return;
    }
    const intervalMs = Math.round(1000 / fps); // Convert FPS to milliseconds
    intervalId = setInterval(() => sendCmd('vid'), intervalMs);
}

// Initial FPS set
updateInterval(currentFPS);

// Slider event listener
fpsSlider.addEventListener('input', (e) => {
    const newFPS = parseInt(e.target.value, 10);
    updateInterval(newFPS);
});


export function sendCmd(arg) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(arg);
    }
}
