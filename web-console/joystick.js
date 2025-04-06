import { sendCmd } from "./websockets.js";

const joystickContainer = document.getElementById('joystick-container');
const joystick = document.getElementById('joystick');
const containerSize = 250;
const joystickSize = 50;
const maxDistance = (containerSize - joystickSize) / 2;

let isDragging = false;
let lastX = 0; // Last known X position relative to container
let lastY = 0; // Last known Y position relative to container
let animationFrameId = null;

// Define the initial (center) position
function getCenterPosition() {
    const rect = joystickContainer.getBoundingClientRect();
    return {
        x: rect.width / 2,
        y: rect.height / 2
    };
}

function moveJoystick(x, y) {
    const center = getCenterPosition();
    let dx = x - center.x;
    let dy = y - center.y;
    const distance = Math.sqrt(dx * dx + dy * dy);

    if (distance > maxDistance) {
        dx = dx * maxDistance / distance;
        dy = dy * maxDistance / distance;
    }

    joystick.style.left = `${center.x + dx}px`;
    joystick.style.top = `${center.y + dy}px`;

    // Determine command based on angle
    const angle = Math.atan2(dy, dx) * 180 / Math.PI;

    let command = 'stop';

    if (distance > 20) { // Dead zone
        if (angle >= -45 && angle < 45) command = 'right';
        else if (angle >= 45 && angle < 135) command = 'back';
        else if (angle >= 135 || angle < -135) command = 'left';
        else if (angle >= -135 && angle < -45) command = 'go';
    }

    sendCmd(command);
}

function updateJoystick() {
    if (isDragging) {
        moveJoystick(lastX, lastY);
        animationFrameId = requestAnimationFrame(updateJoystick);
    }
}

// Mouse Events
joystickContainer.addEventListener('mousedown', (e) => {
    isDragging = true;
    joystick.classList.add('active');
    const rect = joystickContainer.getBoundingClientRect();
    lastX = e.clientX - rect.left;
    lastY = e.clientY - rect.top;
    moveJoystick(lastX, lastY);
    if (!animationFrameId) {
        animationFrameId = requestAnimationFrame(updateJoystick);
    }
});

joystickContainer.addEventListener('mousemove', (e) => {
    if (isDragging) {
        const rect = joystickContainer.getBoundingClientRect();
        lastX = e.clientX - rect.left;
        lastY = e.clientY - rect.top;
    }
});

document.addEventListener('mouseup', () => {
    if (isDragging) {
        isDragging = false;
        joystick.classList.remove('active');
        if (animationFrameId) {
            cancelAnimationFrame(animationFrameId);
            animationFrameId = null;
        }

        // Reset to center
        const center = getCenterPosition();
        moveJoystick(center.x, center.y);
    }
});

// Touch Events
joystickContainer.addEventListener('touchstart', (e) => {
    isDragging = true;
    joystick.classList.add('active');
    const rect = joystickContainer.getBoundingClientRect();
    const touch = e.touches[0]; // Use the first touch point
    lastX = touch.clientX - rect.left;
    lastY = touch.clientY - rect.top;
    moveJoystick(lastX, lastY);
    if (!animationFrameId) {
        animationFrameId = requestAnimationFrame(updateJoystick);
    }
    e.preventDefault(); // Prevent scrolling or other default touch behaviors
});

joystickContainer.addEventListener('touchmove', (e) => {
    if (isDragging) {
        const rect = joystickContainer.getBoundingClientRect();
        const touch = e.touches[0]; // Track the first touch point
        lastX = touch.clientX - rect.left;
        lastY = touch.clientY - rect.top;
        e.preventDefault(); // Prevent scrolling
    }
});

document.addEventListener('touchend', (e) => {
    if (isDragging) {
        isDragging = false;
        joystick.classList.remove('active');
        if (animationFrameId) {
            cancelAnimationFrame(animationFrameId);
            animationFrameId = null;
        }
        // Reset to center
        const center = getCenterPosition();
        moveJoystick(center.x, center.y);
    }
});

// Optional: Handle touchcancel (e.g., interrupted touch)
document.addEventListener('touchcancel', () => {
    if (isDragging) {
        isDragging = false;
        joystick.classList.remove('active');
        if (animationFrameId) {
            cancelAnimationFrame(animationFrameId);
            animationFrameId = null;
        }
        // Reset to center
        const center = getCenterPosition();
        moveJoystick(center.x, center.y);
    }
});
