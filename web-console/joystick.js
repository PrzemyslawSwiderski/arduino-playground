import { sendCmd } from "./websockets.js";

const joystickContainer = document.getElementById('joystick-container');
const joystick = document.getElementById('joystick');
const controlSwitch = document.getElementById('controlSwitch');

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

let lastSentTime = 0;
const THROTTLE_DELAY = 50; // throttle period in ms

function moveJoystick(x, y) {
    const center = getCenterPosition();
    let dx = x - center.x;
    let dy = y - center.y;

    joystick.style.left = `${center.x + dx}px`;
    joystick.style.top = `${center.y + dy}px`;

    const now = Date.now();
    if (now - lastSentTime < THROTTLE_DELAY) {
        // Too soon to send the same command, skip it
        return;
    }

    // Determine command based on angle
    const angle = Math.atan2(dy, dx) * 180 / Math.PI;
    const distance = Math.sqrt(dx * dx + dy * dy);
    const maxDistance = 150;
    const distanceScaled = Math.min(Number(((distance / maxDistance) * 100).toFixed(0)), 100);

    const headControl = controlSwitch.checked;
    let command =  headControl ? 'rotate-stop' : 'stop';
    const rightCmd = headControl ? 'rotate-right' : 'right';
    const backCmd = headControl ? 'rotate-down' : 'back';
    const leftCmd = headControl ? 'rotate-left' : 'left';
    const goCmd = headControl ? 'rotate-up' : 'go';

    if (angle >= -45 && angle < 45) command = `${rightCmd}:${distanceScaled}`;
    else if (angle >= 45 && angle < 135) command = `${backCmd}:${distanceScaled}`;
    else if (angle >= 135 || angle < -135) command = `${leftCmd}:${distanceScaled}`;
    else if (angle >= -135 && angle < -45) command = `${goCmd}:${distanceScaled}`;

    sendCmd(command);
    lastSentTime = now;
}

function resetJoystick() {
    const center = getCenterPosition();

    joystick.style.left = `${center.x}px`;
    joystick.style.top = `${center.y}px`;

    const headControl = controlSwitch.checked;
    let stopCommand =  headControl ? 'rotate-stop' : 'stop';
    sendCmd(stopCommand);
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
        resetJoystick();
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
        resetJoystick();
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
        resetJoystick();
    }
});
