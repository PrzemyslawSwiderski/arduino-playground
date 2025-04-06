
const img = document.getElementById('stream');
const lightOnBtn = document.getElementById('light-on');
const lightOffBtn = document.getElementById('light-off');
const addressSelect = document.getElementById('websocket-address');
const errorBanner = document.getElementById('error-banner');
const connectBtn = document.getElementById('connect-btn');

var wsUrl = resolveAddress();
var ws = connectWebSocket();

function resolveAddress() {
    const address = addressSelect.value;
    return `ws://${address}:443/ws`;
}

function connectWebSocket() {
    wsUrl = resolveAddress();
    ws = new WebSocket(wsUrl);

    ws.binaryType = 'arraybuffer';

    ws.onopen = () => {
        console.log(`Connected to WebSocket at ${wsUrl}`);
        ws.send('Hello! FROM the client');
        errorBanner.classList.remove('show');
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
        // setTimeout(() => ws = connectWebSocket(), 1000); // Reconnect after 1 second
    };

    ws.onerror = (error) => {
        console.error('WebSocket error:', error);
        errorBanner.classList.add('show');
    };
    return ws;
}

connectBtn.addEventListener('click', () => ws = connectWebSocket());

lightOnBtn.addEventListener('mousedown', (e) => {
    sendCmd('ledon');
});

lightOffBtn.addEventListener('mousedown', (e) => {
    sendCmd('ledoff');
});

export function sendCmd(arg) {
    console.log(arg);
    ws.send(arg);
}

