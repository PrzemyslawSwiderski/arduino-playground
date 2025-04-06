import { sendCmd } from "./websockets.js";

const joystickContainer = document.getElementById('joystick-container');
const joystick = document.getElementById('joystick');
const containerSize = 250;
const joystickSize = 50;
const maxDistance = (containerSize - joystickSize) / 2;
let isDragging = false;
let command = 'stop';

function moveJoystick(x, y) {
    const centerX = containerSize / 2;
    const centerY = containerSize / 2;
    let dx = x - centerX;
    let dy = y - centerY;
    const distance = Math.sqrt(dx * dx + dy * dy);

    if (distance > maxDistance) {
        dx = dx * maxDistance / distance;
        dy = dy * maxDistance / distance;
    }

    joystick.style.left = `${centerX + dx}px`;
    joystick.style.top = `${centerY + dy}px`;

    // Determine command based on angle
    const angle = Math.atan2(dy, dx) * 180 / Math.PI;

    if (distance > 20) { // Dead zone
        if (angle >= -45 && angle < 45) command = 'right';
        else if (angle >= 45 && angle < 135) command = 'back';
        else if (angle >= 135 || angle < -135) command = 'left';
        else if (angle >= -135 && angle < -45) command = 'go';
    }

    sendCmd(command);
}

function resetJoystick() {
    isDragging = false;
    joystick.classList.remove('active');
    joystick.style.left = '50%';
    joystick.style.top = '50%';
    sendCmd('stop');
}

joystickContainer.addEventListener('mousedown', (e) => {
    isDragging = true;
    joystick.classList.add('active');
    const rect = joystickContainer.getBoundingClientRect();
    moveJoystick(e.clientX - rect.left, e.clientY - rect.top);
});

joystickContainer.addEventListener('mouseover', (e) => {
    if (isDragging) {
        const rect = joystickContainer.getBoundingClientRect();
        moveJoystick(e.clientX - rect.left, e.clientY - rect.top);
    }
});

document.addEventListener('mouseup', () => {
    if (isDragging) resetJoystick();
});

// Touch support
joystickContainer.addEventListener('touchstart', (e) => {
    e.preventDefault();
    isDragging = true;
    joystick.classList.add('active');
    const touch = e.touches[0];
    const rect = joystickContainer.getBoundingClientRect();
    moveJoystick(touch.clientX - rect.left, touch.clientY - rect.top);
});

joystickContainer.addEventListener('touchmove', (e) => {
    e.preventDefault();
    if (isDragging) {
        const touch = e.touches[0];
        const rect = joystickContainer.getBoundingClientRect();
        moveJoystick(touch.clientX - rect.left, touch.clientY - rect.top);
    }
});

document.addEventListener('touchend', () => {
    if (isDragging) resetJoystick();
});
