
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

export function sendCmd(arg) {
    ws.send(arg);
}

