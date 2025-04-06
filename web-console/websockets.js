
const img = document.getElementById('stream');
const lightOnBtn = document.getElementById('light-on');
const lightOffBtn = document.getElementById('light-off');
const addressSelect = document.getElementById('websocket-address');
const errorBanner = document.getElementById('error-banner');
const connectBtn = document.getElementById('connect-btn');

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
        connectBtn.textContent = "Connected"
    };

    ws.onmessage = (event) => {
        if (event.data instanceof ArrayBuffer) {
            // Handle video frame
            const blob = new Blob([event.data], { type: 'image/jpeg' });
            img.src = URL.createObjectURL(blob);
        } else {
            // Handle text (e.g., heartbeat)
            console.log('Received:', event.data);
        }
    };

    ws.onclose = (event) => {
        console.log('WebSocket disconnected, code:', event.code);
        connectBtn.textContent = "Disconnected"
    };

    ws.onerror = (error) => {
        console.error('WebSocket error:', error);
        errorBanner.classList.add('show');
        connectBtn.textContent = "Try Reconnect"
    };

}

// Initial connection
connectWebSocket();

connectBtn.addEventListener('click', () => connectWebSocket());

lightOnBtn.addEventListener('mousedown', (e) => {
    sendCmd('ledon');
});

lightOffBtn.addEventListener('mousedown', (e) => {
    sendCmd('ledoff');
});


// State to track the last command and its timestamp
let lastCmd = null;
let lastSentTime = 0;
const THROTTLE_DELAY = 200; // 200ms throttle period

export function sendCmd(arg) {
    const now = Date.now();
    // Log the argument (unchanged)
    console.log(arg);
    // If the command is the same as the last one
    if (lastCmd === arg) {
        // Check if 200ms have passed since the last send
        if (now - lastSentTime < THROTTLE_DELAY) {
            // Too soon to send the same command, skip it
            return;
        }
        // 200ms have passed, allow sending
    }
    // Send the command
    ws.send(arg);
    // Update tracking state
    lastCmd = arg;
    lastSentTime = now;
}

